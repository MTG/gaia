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

#include <QTcpSocket>
#include <QStringList>
#include <QHttpRequestHeader>
#include <QUrl>
#include <QDateTime>
#include "yamlrpcserver.h"
#include "gaiaexception.h"
#include "yamlcpp.h"
#include "logging.h"
#include "utils.h"
using namespace gaia2;


inline QByteArray sid(const QTcpSocket* socket) {
  return QString("%1:%2 :").arg(socket->peerAddress().toString()).arg(int(socket->peerPort())).toUtf8();
}

int YamlRPCServer::numberConnectedClients() const {
  return _socketIDs.size();
}


YamlRPCServer::YamlRPCServer(quint16 port, YamlProxy* proxy) : QTcpServer(), _proxy(proxy) {
  // only listen to a port if given a valid port, otherwise wait to be told explicitly
  // to listen to a specific port
  _startedOn = QDateTime::currentDateTime();
  if (port != 0) {
    bool ok = listen(QHostAddress::Any, port);
    if (!ok) {
      clog() << "Could not start listening on port" << port << "...";
      clog() << "Exiting...";
      exit(1);
    }
  }
}

void YamlRPCServer::incomingConnection(int socket) {
  // When a new client connects, the server constructs a QTcpSocket and all
  // communication with the client is done over this QTcpSocket. QTcpSocket
  // works asynchronously, this means that all the communication is done
  // in the two slots readClient() and discardClient().
  QTcpSocket* s = new QTcpSocket(this);
  connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
  connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
  s->setSocketDescriptor(socket);

  _socketIDs.insert(s, sid(s));
  clog() << _socketIDs.value(s).constData() << "New connection";
}


QByteArray readHTTPRequest(QTcpSocket* socket) {
  bool done = false;

  QStringList headers;
  QHttpRequestHeader httpHeader;

  // if asked for a GET on /status, this should return the equivalent of a well-formed Yaml-RPC call
  bool wantStatus = false;

  while (!done && (socket->bytesAvailable() > 0 || socket->waitForReadyRead())) {
    QString line = socket->readLine();

    if (line.trimmed().isEmpty())  {
      httpHeader = QHttpRequestHeader(headers.join(""));
      done = true;
    }
    else {
      headers << line;
      if (line.startsWith("GET /status")) {
        wantStatus = true;
      }
    }
  }

  QByteArray msgbytes;
  int messageLength = httpHeader.contentLength();
  msgbytes.reserve(messageLength + 1); // +1 for final '\0' (necessary?)

  while ((msgbytes.size() < messageLength) && (socket->bytesAvailable() > 0 || socket->waitForReadyRead())) {
    // TODO: need to make sure the socket is still valid, as the next call might return because of an error
    msgbytes.append(socket->read(messageLength - msgbytes.size()));
  }

  if (wantStatus) {
    return QByteArray("q=id%3A gloubi-boulga%0Amethod%3A status%0Aparams%3A %5B%5D%0A");
  }

  return msgbytes;
}

yaml::Mapping readYamlRPCRequest(QTcpSocket* socket) {
  QString params = QUrl::fromPercentEncoding(readHTTPRequest(socket)).mid(2); // remove the leading "q="
  yaml::Mapping q = yaml::load(params).mapping();

  return q;
}

yaml::Mapping rpcError(int code, const QString& msg) {
  yaml::Mapping error;
  error.insert("code", QString::number(code));
  error.insert("message", msg);
  return error;
}

yaml::Mapping rpcError(int code, const QStringList& msg) {
  return rpcError(code, msg.join(""));
}

QString timeElapsed(const QDateTime& from, const QDateTime& to) {
  QString result;
  int nsecs = from.secsTo(to);
  int n;
  n = nsecs % 60; if (n) result = QString::number(n) + " secs"; nsecs /= 60;
  n = nsecs % 60; if (n) result = QString::number(n) + " mins " + result; nsecs /= 60;
  n = nsecs % 24; if (n) result = QString::number(n) + " hours " + result; nsecs /= 24;
  n = nsecs;      if (n) result = QString::number(n) + " days " + result;

  return result.trimmed();
}

yaml::Mapping YamlRPCServer::processRequest(QTcpSocket* socket) {
  // Any exception that happens here should not crash the server, but be forwarded
  // to the client, so wrap the whole processing in a big try/catch block

  yaml::Mapping request, response;
  response.insert("id", "null");

  try {

    request = readYamlRPCRequest(socket);
    clog() << _socketIDs.value(socket).constData() << "asked for method:" << request["method"];

    response.insert("id", request.value("id"));

    // try to catch methods which are specific to a Yaml-RPC server and do not depend on the wrapped object
    if (request["method"].scalar() == "quit") {
      emit quit();

      response.insert("result", "null");
      return response;
    }
    else if (request["method"].scalar() == "ping") {
      response.insert("result", "pong");
      return response;
    }
    else if (request["method"].scalar() == "status") {
      yaml::Mapping status;
      status.insert("Uptime", timeElapsed(_startedOn, QDateTime::currentDateTime()));
      status.insert("VmSize", QString::number(memoryUsage("VmSize")));
      status.insert("VmRSS", QString::number(memoryUsage("VmRSS")));
      response.insert("result", status);
      return response;
    }

    yaml::Node result = _proxy->process(request);

    response.insert("result", result);
  }
  catch (NotImplementedError&) {
    response.insert("error", rpcError(-32601, QStringList()
                                      << "The method '" << request.value("method")
                                      << "' doesn't exist"));
  }
  catch (InvalidParams& e) {
    response.insert("error", rpcError(-32602, QStringList()
                                      << "invalid params for call to method '"
                                      << request.value("method") << "': "
                                      << e.what()));
  }
  catch (GaiaException& e) {
    response.insert("error", rpcError(23, e.msg()));
  }
  catch (std::exception& e) {
    response.insert("error", rpcError(23, QStringList() << "Unexpected error: " << e.what()));
  }

  return response;
}

void YamlRPCServer::readClient() {
  QTcpSocket* socket = (QTcpSocket*)sender();

  yaml::Mapping response = processRequest(socket);

  QTextStream os(socket);
  os.setAutoDetectUnicode(true);
  os << "HTTP/1.0 200 Ok\r\n"
    "Content-Type: application/x-yaml\r\n"
    "\r\n"
     << yaml::dump(response) << "\n";
  socket->close();

  if      (response.contains("result")) clog() << _socketIDs.value(socket).constData() << "Sent successful result to client";
  else if (response.contains("error"))  clog() << _socketIDs.value(socket).constData() << "Sent error message to client";
  else                                  clog() << _socketIDs.value(socket).constData() << "Sent weird response to client...";

  if (socket->state() == QTcpSocket::UnconnectedState) {
    delete socket;
    clog() << _socketIDs.take(socket).constData() << "Connection closed right away";
  }
}


void YamlRPCServer::discardClient() {
  QTcpSocket* socket = (QTcpSocket*)sender();
  socket->deleteLater();

  clog() << _socketIDs.take(socket).constData() << "Connection closed";

  //qDebug() << "clients connected: " << numberConnectedClients();
}


#include "yamlrpcserver.moc"
