/* 
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 * 
 * Gaia is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU Affero General Public License as published by the Free 
 * Software Foundation (FSF), either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QFile>
#include <QBuffer>
#include <QUrl>
#include <QNetworkRequest>
#include <QTcpSocket>
#ifndef GAIA_QT5
#include <QHttp>
#else
#include "qhttp.h"
#endif
#include <QHostInfo>
#include "yamlrpcserver.h"
#include "cyclopsproxy.h"
#include "cyclopsmaster.h"
#include "../../view.h"
#include "logging.h"
#include "../../utils.h"
using namespace gaia2;


CyclopsMaster::CyclopsMaster() : YamlRPCServer(0, 0) {
  clog() << "A cyclops is born: it is a cyclops master!!";
}

CyclopsMaster::~CyclopsMaster() {
  clog() << "The old master retired...";
}


void CyclopsMaster::setupClients(const QString& configFilename) {
  QFile file(configFilename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open config file: ", configFilename);
  }

  yaml::Mapping config = yaml::load(file.readAll()).mapping();

  bool ok;
  quint16 port = config["port"].scalar().toUInt(&ok);

  if (!ok) throw InvalidParams("Port value should be a valid port number: ", config["port"]);

  foreach (const yaml::Node& slave, config["slaves"].sequence()) {
    QHostAddress host;

    // look only for IPv4 addresses (damn us techno-luddites...)
    foreach (const QHostAddress& addr, QHostInfo::fromName(slave["host"]).addresses()) {
      if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        host = addr;
        break;
      }
    }
    if (host.isNull()) {
      throw GaiaException("Could not find IPv4 address for slave: ", slave["host"]);
    }

    int sport = slave["port"].scalar().toUInt(&ok);
    if (!ok) throw InvalidParams("Port value should be a valid port number ", slave["port"]);

    _slaves.append(qMakePair(host, sport));
  }

  setup(configFilename);

  if (!listen(QHostAddress::Any, port)) {
    throw GaiaException("Cannot listen on port ", port);
  }
}

int CyclopsMaster::sendSlaveRequest(const QPair<QHostAddress, int>& slave, const yaml::Mapping& request) {
  QHttp* slaveRequest = new QHttp(slave.first.toString(), slave.second, this);

  // connect the reply from this slave to the replyFromSlave method
  connect(slaveRequest, SIGNAL(requestFinished(int,bool)),
          this, SLOT(replyFromSlave(int,bool)));

  QBuffer* replyData = new QBuffer();
  int connid = slaveRequest->post("/", QByteArray("q=") + yaml::dump(request), replyData);

  // make sure this ID doesn't already exist, otherwise we're toast...
  if (_slaveReplies.contains(connid)) {
    throw GaiaException("2 slaves connections have the same ID, impossible to go on like that...");
  }

  _slaveReplies.insert(connid, replyData);
  return connid;
}

void CyclopsMaster::readClient() {
  QTcpSocket* socket = (QTcpSocket*)sender();
  /*
  socket = new QTcpSocket(NULL);
  socket->setSocketDescriptor(sock_ptr);
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()), Qt::DirectConnection);
  connect(socket, SIGNAL(readyRead()), this, SLOT(readData()), Qt::DirectConnection);
  connect(socket, SIGNAL(disconnected()), this, SLOT(quit()), Qt::DirectConnection);
  const QHostAddress &connected = socket->peerAddress();
  */

  yaml::Mapping reply;
  reply.insert("id", "gloubi-boulga");

  QByteArray requestCopy;
  try {
    QByteArray httpRequest = readHTTPRequest(socket);
    requestCopy = httpRequest;

    // check request is well-formed, send a yaml error message otherwise
    QString params = QUrl::fromPercentEncoding(httpRequest).mid(2); // remove the leading "q="
    yaml::Mapping q = yaml::load(params).mapping();

    // you can't call the setup() method directly on the Cyclops master
    if (q["method"] == "setup") {
      reply.insert("id", q["id"]);
      reply.insert("error", yaml::Mapping());
      reply["error"].mapping().insert("code", QString::number(-32601));
      reply["error"].mapping().insert("message", "You can't call the setup() method on a Cyclops master server");

      return sendReplyToClient(socket, reply);
    }
    else if (q["method"] == "status") {
      yaml::Mapping status;
      status.insert("Uptime", timeElapsed(_startedOn, QDateTime::currentDateTime()));
      status.insert("VmSize", QString::number(memoryUsage("VmSize")));
      status.insert("VmRSS", QString::number(memoryUsage("VmRSS")));

      reply.insert("result", status);
      return sendReplyToClient(socket, reply);
    }


    // forward request to all slaves, changing the message if required (ie: load dataset)
    //connect(tcpSocket, &QIODevice::readyRead, this, &Client::readFortune);
    //connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
    distributeClientRequest(socket, q);

  }
  catch (GaiaException& e) {
    reply.insert("error", rpcError(23, e.msg()));
    return sendReplyToClient(socket, reply);
  }
  catch (std::exception& e) {
    clog() << "OOPS: wrongly caught exception...";
    clog() << " msg:" << e.what();
    clog() << " req: \"" << requestCopy.constData() << "\"";
    reply.insert("error", rpcError(23, e.what()));
    return sendReplyToClient(socket, reply);
  }
  catch (...) {
    clog() << "OOPS: wrongly caught exception, not even std::exception...";
    clog() << " req: \"" << requestCopy.constData() << "\"";
    reply.insert("error", rpcError(23, "Caught some unknown excepction"));
    return sendReplyToClient(socket, reply);
  }
}


void CyclopsMaster::distributeClientRequest(QTcpSocket* client, const yaml::Mapping& q) {
  // forward request to all slaves, changing the message if required (ie: load dataset)
  int nslaves = _slaves.size();
  _pendingRequests.insert(client, PendingRequest(q));

  for (int i=0; i<nslaves; i++) {
    // WARNING: This is SUPER inefficient, as we call split for each slave, when a single
    //          call to split which would return all parts at once would be much better
    int connid = sendSlaveRequest(_slaves[i], split(q, i, nslaves, _pendingRequests[client]));
    _slaveToClient.insert(connid, client);
  }
}


yaml::Mapping replyForMissingPoints(const yaml::Sequence& askedFor, const DataSet* ds) {
  QSet<QString> asked;
  foreach (const yaml::Node& n, askedFor) asked << n.scalar();
  QSet<QString> found = QSet<QString>::fromList(ds->pointNames());
  QStringList notfound = (asked - found).toList();

  yaml::Mapping error;
  error.insert("code", QString::number(23));
  error.insert("message", QString("Following points could not be found: ") + notfound.join(", "));

  yaml::Mapping result;
  result.insert("error", error);
  return result;
}


// from a chainedSearch containing both q-by-id and q-by-example, return a chainedSearch
// containing only q-by-examples. Missing points are fetched from the given dataset
yaml::Sequence chainedSearchByExample(const yaml::Sequence& origQueries, const DataSet& points) {
  yaml::Sequence result(origQueries);

  for (int i=0; i<result.size(); i++) {
    QString methodName = result[i][0].scalar();
    if (methodName.startsWith("nnSearchById")) {
      const QString& id = result[i][1][0].scalar();
      // replace methodName and point ID with the actual point data
      result[i][0] = methodName.replace(8, 4, "ByExample");
      result[i][1][0] = points.point(id)->toBase64();
    }
    // else do nothing
  }

  return result;
}



void CyclopsMaster::replyFromSlave(int id, bool error) {
  // should identify which request we're replying to
  QTcpSocket* client = _slaveToClient.take(id);
  QBuffer* slaveBuffer = _slaveReplies.take(id);
  QByteArray slaveReply = slaveBuffer->data();

  // we can now clear all the connection setup for this (slave, request)
  delete slaveBuffer;
#ifndef GAIA_QT5
  QHttp* slaveRequest = (QHttp*)sender();
#else
  QNetworkAccessManager* slaveRequest = (QNetworkAccessManager*)sender();
#endif
  slaveRequest->deleteLater();

  if (error) {
    // should get the error string from the QHttp or QNetworkAccessManager pointer...
    clog() << "There seems to be problem with one of the slaves... Exiting...";

    emit quit();

    return;
  }

  _pendingRequests[client].replies.append(yaml::load(slaveReply).mapping());

  // if we're the last of the slaves to respond, we need to send back the answer to the client
  sendReadyReply(client);
}

// returns and yields ownership over the new dataset
DataSet* reducePointsToDataSet(const QList<yaml::Mapping>& replies) {
  DataSet* resultds = new DataSet();
  for (int i=0; i<replies.size(); i++) {
    // all replies should contain a result, even if only an empty dataset
    DataSet ds;
    ds.fromBase64(replies[i]["result"].scalar().toLatin1());
    resultds->appendDataSet(&ds);
  }

  return resultds;
}

void CyclopsMaster::sendReadyReply(QTcpSocket* client) {
  int nslaves = _slaves.size();
  PendingRequest& results = _pendingRequests[client];

  // only send a reply if all slaves have answered
  if (results.replies.size() < nslaves) return;

  // in case we did a chainedSearch that needed some getPoints before, we have to resend the
  // query with all points in a q-by-example to the slaves
  if (results.request["method"] == "chainedSearch" && results.state == 0) {
    // enclose all of this inside a try/catch block, so that if any error occurs (such as
    // invalid arguments) we just fall through to the normal reduce/sendReply which will
    // send the correct error message back to the client
    // FIXME: this is just a temporary hack, a correct solution would imply a more in-depth
    //        refactoring of how arguments are checked
    try {

    results.state = 1;
    DataSet* pds = reducePointsToDataSet(results.replies);

    // clean the results from the first part of the query to be able to store the results
    // from the second part (the actual chainedSearch query)
    results.replies.clear();

    // need to make sure we got all our points here, otherwise throw a Yaml-RPC exception
    if (results.info.toList().size() > pds->size()) {
      QList<QVariant> asked = results.info.toList();
      yaml::Sequence askedFor;
      for (int i=0; i<asked.size(); i++) askedFor << asked[i].toString();

      yaml::Mapping reply = replyForMissingPoints(askedFor, pds);
      reply.insert("id", results.request["id"]);
      delete pds;

      _pendingRequests.remove(client);
      return sendReplyToClient(client, reply);
    }

    yaml::Mapping subquery(results.request);
    subquery["params"][0] = chainedSearchByExample(results.request["params"][0].sequence(), *pds);
    delete pds;

    // we need to change (n, offset) as we're distributing to clients. We don't know where the
    // results are actually going to come from, so we need to ask (n+offset, 0) to each client.
    int n = subquery["params"][2].scalar().toInt();
    int offset = subquery["params"][3].scalar().toInt();
    subquery["params"][2] = QString::number(n+offset);
    subquery["params"][3] = "0";

    // send to all the slaves
    for (int i=0; i<nslaves; i++) {
      int connid = sendSlaveRequest(_slaves[i], subquery);
      _slaveToClient.insert(connid, client);
    }

    // return without doing anything, we will actually respond to the client when those
    // subrequests complete
    return;
    }
    catch (...) {}
  }

  // if we asked for an nnSearchById, we first sent a getPoint request to all the slaves
  // if you just got that back, we need to resend a nnSearchByExample to the slaves before
  // receiving the final results we can send back to the client
  if (results.request["method"].scalar().startsWith("nnSearchById") && results.state == 0) {
    results.state = 1;
    QString point64;
    bool found = false;
    for (int i=0; i<nslaves; i++) {
      if (results.replies[i].contains("result")) {
        point64 = results.replies[i]["result"].scalar();
        found = true;
      }
    }
    if (!found) return sendReadyReply(client); // this works because we now have state = 1

    // we need to clean our mess from the first part of the query, and launch the second part
    results.replies.clear();

    // the query we need to send is the original query where we changed "ById" with "ByExample"
    // and in the params, the point id with the point in base64
    yaml::Mapping subquery(results.request);
    if (subquery["method"] == "nnSearchById") subquery["method"] = "nnSearchByExample";
    else subquery["method"] = "nnSearchByExampleWithFilter";
    subquery["params"][0] = point64;

    // send to all the slaves
    for (int i=0; i<nslaves; i++) {
      int connid = sendSlaveRequest(_slaves[i], subquery);
      _slaveToClient.insert(connid, client);
    }

    // return without doing anything, we will actually respond to the client when those
    // subrequests complete
    return;
  }

  // if asked to quit, emit the quit() signal (which will make use exit after having replied to the client)
  if (results.request["method"].scalar() == "quit") {
    emit quit();
  }

  yaml::Mapping reply = reduce(_pendingRequests.take(client));

  sendReplyToClient(client, reply);
}


void CyclopsMaster::sendReplyToClient(QTcpSocket* client, const yaml::Mapping& reply) {
  if (!client) {
    // not responding to any client, so probably this is the result of calling the setup()
    // method; if there was an error we need to throw an exception
    if (reply.contains("error")) {
      throw GaiaException("Error while doing initial setup: ", reply["error"]["message"]);
    }

    // otherwise, everything went fine, just return without sending any response to anyone
    return;
  }

  // before trying to send a response, make sure the client didn't disconnect in the meantime
  if (!_socketIDs.contains(client)) {
    clog() << "Client disconnected before we could reply...";
    return;
  }

  client->write("HTTP/1.0 200 Ok\r\n"
                "Content-Type: application/x-yaml\r\n"
                "\r\n");
  client->write(yaml::dump(reply));
  client->write("\n");
  client->close();

  if      (reply.contains("result")) {
    clog() << _socketIDs.value(client).constData() << "Sent successful result to client";
  }
  else if (reply.contains("error")) {
    clog() << _socketIDs.value(client).constData() << "Sent error message to client" << reply["error"]["message"];
  }
  else {
    clog() << _socketIDs.value(client).constData() << "Sent weird response to client...";
  }


  if (client->state() == QTcpSocket::UnconnectedState) {
    delete client;
    clog() << _socketIDs.take(client).constData() << "Connection closed right away";
  }

}


yaml::Mapping CyclopsMaster::split(const yaml::Mapping& query, int idx, int total, PendingRequest& preq) {
  if (query["method"] == "load" && query["params"].sequence().size() == 2) {
    // we need to only load a part of the dataset
    yaml::Mapping result(query);
    result["method"] = "loadNthPart";
    result["params"].sequence().append(QString::number(idx));
    result["params"].sequence().append(QString::number(total));

    return result;
  }

  if (query["method"] == "setupFromYaml") {
    // we need to create a new yaml setup, where we only load the given part of the specified
    // datasets
    if (!query["params"][0].mapping().contains("datasets")) return query;

    yaml::Mapping result(query);
    yaml::Mapping& datasets = result["params"][0]["datasets"].mapping();
    yaml::Mapping::iterator ds = datasets.begin();
    for (; ds != datasets.end(); ++ds) {
      ds.value().mapping().insert("idx", QString::number(idx));
      ds.value().mapping().insert("total", QString::number(total));
    }

    return result;
  }

  if (query["method"] == "chainedSearch") {
    // enclose all of this inside a try/catch block, so that if any error occurs (such as
    // invalid arguments) we just fall through to the normal reduce/sendReply which will
    // send the correct error message back to the client
    // FIXME: this is just a temporary hack, a correct solution would imply a more in-depth
    //        refactoring of how arguments are checked
    try {

    // find all the points we asked by ID, and issue a getPoints request on them

    const yaml::Sequence& queries = query["params"][0].sequence();

    yaml::Sequence params;
    //DataSet qpoints;
    QSet<QString> pointIDs;
    for (int i=0; i<queries.size(); i++) {
      const QString& methodName = queries[i][0]; // TODO: no temp objects here, right?
      const yaml::Sequence& args = queries[i][1].sequence();
      if (methodName.startsWith("nnSearchById")) {
        // TODO: we have unnecessary points copies here, maybe there's sth to optimize
        //Point q;
        //q.fromBase64(args[0].scalar().toLatin1());
        //qpoints.addPoint(q); // NB: this should be grouped in a single addPoints at the end
        pointIDs << args[0].scalar();
      }
    }

    if (pointIDs.isEmpty()) {
      // if qpoints is empty, we can do the query directly, no need to call a getPoints
      // before. We then need to set the state of the query directly to 1.
      preq.state = 1;

      // we need to change (n, offset) as we're distributing to clients. We don't know where the
      // results are actually going to come from, so we need to ask (n+offset, 0) to each client.
      yaml::Mapping subquery(query);
      int n = subquery["params"][2].scalar().toInt();
      int offset = subquery["params"][3].scalar().toInt();
      subquery["params"][2] = QString::number(n+offset);
      subquery["params"][3] = "0";

      return subquery;
    }
    else {
      QList<QVariant> pids;
      yaml::Sequence ypids;
      foreach (const QString& pid, pointIDs) {
        pids << pid;
        ypids << pid;
      }
      preq.info = pids;
      // we need to first send a getPoints for all the asked q-by-example
      yaml::Mapping result(query);
      result["method"] = "getPoints";
      // WARNING: this assumes all point lookups are from the same dataset
      const QString& datasetName = queries[0][1][1].scalar();
      result["params"] = yaml::Sequence() << datasetName << yaml::Node(ypids) << "false";

      return result;
    }

    }
    catch (...) {}
  }

  // TODO: remove me, as the nnSearch methods are now redundant with the proxied ones...
  if (query["method"].scalar().startsWith("nnSearchById")) {
    // we need to convert this to a getPoint, then do a nnSearchByExample (because the point in
    // question can only be found in a single part of the dataset, so it'll fail for all the other
    // ones otherwise)
    yaml::Mapping result(query);
    result["method"] = "getPoint";
    yaml::Sequence params;
    params << query["params"][1]; // the dataset name
    params << query["params"][0]; // the point ID
    result["params"] = params;

    return result;
  }

  if (query["method"] == "getPoints") {
    // need to be more lenient than usual if some points are not found
    yaml::Mapping result(query);
    result["params"].sequence().append("false");
    return result;
  }

  return query;
}

bool cmpSearchResults(const Result& r1, const Result& r2) {
  return r1.second < r2.second;
}


// when received all the replies from the slaves to a getPoints request, merge those into
// a single dataset and return the reply. If not all points were returned, prepare an error
// reply containing the names of the points that could not be found.
// This assumes that we called the getPoints method with failOnError == false
yaml::Mapping reduceGetPoints(const QList<yaml::Mapping>& replies, const yaml::Sequence& askedFor) {
  DataSet* resultds = reducePointsToDataSet(replies);
  /*
  for (int i=0; i<replies.size(); i++) {
    // all replies should contain a result, even if only an empty dataset
    DataSet ds;
    ds.fromBase64(replies[i]["result"].scalar().toLatin1());
    resultds.appendDataSet(&ds);
  }
  */

  if (resultds->size() == askedFor.size()) {
    yaml::Mapping result(replies[0]);
    result["result"] = resultds->toBase64();
    delete resultds;
    return result;
  }

  // need to find the missing points
  yaml::Mapping result = replyForMissingPoints(askedFor, resultds);
  result.insert("id", replies[0]["id"]);
  delete resultds;

  return result;
}

yaml::Mapping CyclopsMaster::reduce(const PendingRequest& results) {
  // for getPoint, we should get errors from all the dataset parts except the one that
  // actually contained it
  if (results.request["method"] == "getPoint") {
    for (int i=0; i<results.replies.size(); i++) {
      if (results.replies[i].contains("result")) {
        return results.replies[i];
      }
    }
  }

  // for getPoints, we need to get all the datasets we got back (if any), and merge
  // them together before returning the result. If the total number of points is different
  // from the number of points we asked, we need to get which are the missing ones and return
  // an error message with that
  if (results.request["method"] == "getPoints") {
    // enclose all of this inside a try/catch block, so that if any error occurs (such as
    // invalid arguments) we just fall through to the normal reduce/sendReply which will
    // send the correct error message back to the client
    // FIXME: this is just a temporary hack, a correct solution would imply a more in-depth
    //        refactoring of how arguments are checked
    try {
      if (results.request["params"].sequence().size() < 2) throw InvalidParams();
      return reduceGetPoints(results.replies, results.request["params"][1].sequence());
    }
    catch (...) {}
    /*
    DataSet resultds;
    for (int i=0; i<results.replies.size(); i++) {
      // all replies should contain result, even if only an empty dataset
      DataSet ds;
      ds.fromBase64(results.replies[i]["result"].scalar().toLatin1());
      resultds.appendDataSet(&ds);
    }
    if (resultds.size() == results.request["params"][1].sequence().size()) {
      yaml::Mapping result(results.replies[0]);
      result["result"] = resultds.toBase64();
      return result;
    }

    // need to find the missing points
    QSet<QString> asked;
    foreach (const yaml::Node& n, results.request["params"][1].sequence()) asked << n.scalar();
    QSet<QString> found = QSet<QString>::fromList(resultds.pointNames());
    QStringList notfound = (asked - found).toList();

    yaml::Mapping result;
    result.insert("id", results.request["id"]);
    result.insert("error", QString("Following points could not be found: ") + notfound.join(", "));
    return result;
    */
  }

  // first check there has been no errors, otherwise return that as a common answer
  for (int i=0; i<results.replies.size(); i++) {
    if (results.replies[i].contains("error")) {
      return results.replies[i];
    }
  }

  // for size queries, we need to sum up the size of all parts
  if (results.request["method"] == "size") {
    int total = 0;
    for (int i=0; i<results.replies.size(); i++) {
      total += results.replies[i]["result"].scalar().toInt();
    }

    yaml::Mapping result;
    result.insert("id", results.request["id"]);
    result.insert("result", QString::number(total));

    return result;
  }

  // for chainedSearch queries, we need to merge the results now, only keeping the best ones
  if (results.request["method"] == "chainedSearch") {
    /*
    SearchResults allResults;
    for (int i=0; i<results.replies.size(); i++) {
      const yaml::Sequence& result = results.replies[i]["result"].sequence();
      for (int j=0; j<result.size(); j++) {
        allResults << Result(result[j][0].scalar(), result[j][1].scalar().toFloat());
      }
    }

    sort(allResults.begin(), allResults.end(), cmpSearchResults);
    */
    CyclopsResults allResults;

    for (int i=0; i<results.replies.size(); i++) {
      const yaml::Sequence& result = results.replies[i]["result"]["results"].sequence();
      const yaml::Sequence& values = results.replies[i]["result"]["values"].sequence();

      for (int j=0; j<result.size(); j++) {
        QList<QVariant> vals;
        int vsize = values[j].sequence().size();
        for (int k=0; k<vsize; k++) vals << values[j][k].scalar();

        allResults.append(Result(result[j][0].scalar(), result[j][1].scalar().toFloat()),
                          vals);
      }
    }
    allResults.sort();

    int nresults = results.request["params"][2].scalar().toInt();
    int offset   = results.request["params"][3].scalar().toInt();
    allResults.mid(offset, qMin(allResults.size()-offset, nresults));

    yaml::Mapping result = results.replies[0];
    result["result"] = toYaml(allResults);
    // add the header only now, to avoid a conversion yaml -> QVariant -> yaml
    result["result"]["header"] = results.replies[0]["result"]["header"];

    return result;
  }

  // for nnSearch queries, we need to merge the results now, only keeping the best ones
  if (results.request["method"].scalar().startsWith("nnSearch")) {
    SearchResults allResults;
    for (int i=0; i<results.replies.size(); i++) {
      const yaml::Sequence& result = results.replies[i]["result"].sequence();
      for (int j=0; j<result.size(); j++) {
        allResults << Result(result[j][0].scalar(), result[j][1].scalar().toFloat());
      }
    }

    sort(allResults.begin(), allResults.end(), cmpSearchResults);
    int npos = 3;
    if (results.request["method"].scalar().endsWith("Filter")) npos = 4;
    int nresults = results.request["params"][npos].scalar().toInt();
    allResults = allResults.mid(0, qMin(allResults.size(), nresults));

    yaml::Mapping result = results.replies[0];
    result["result"] = toYaml(allResults);

    return result;
  }

  // for queries that return None, we can return the first one
  // for datasetNames, distanceNames and layout, we assume all results are the same and return the first one
  return results.replies[0];

}

void CyclopsMaster::setup(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    throw GaiaException("Could not open file: ", filename);
  }

  yaml::Mapping config = yaml::load(file.readAll()).mapping();

  yaml::Mapping setupRequest;
  setupRequest.insert("id", "glouba-boulgui");
  setupRequest.insert("method", "setupFromYaml");
  setupRequest.insert("params", yaml::Sequence() << config);

  distributeClientRequest(0, setupRequest);
}



#include "cyclopsmaster.moc"
