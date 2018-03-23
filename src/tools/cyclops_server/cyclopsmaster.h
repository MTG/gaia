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

//#ifndef CYCLOPS_NORMAL
#ifndef GAIA_CYCLOPSMASTER_H
#define GAIA_CYCLOPSMASTER_H

#include <QObject>
#include <QNetworkReply>
#include <QHostAddress>
#include <QBuffer>
#include "yamlrpcserver.h"
#include "cyclops.h"

class PendingRequest {
 public:

  gaia2::yaml::Mapping request;
  QList<gaia2::yaml::Mapping> replies;

  // for requests that need multiple roundtrips (ie: nnSearchById), this is used to keep
  // a state for the request being processed (ie: progress state)
  int state;
  // and this can be used as a container for additional info (kind of a void*, but cleaner)
  QVariant info;

  PendingRequest(const gaia2::yaml::Mapping& request = gaia2::yaml::Mapping()) : request(request), state(0) {}

};

class CyclopsMaster : public YamlRPCServer {
#ifdef WAF
Q_OBJECT
#endif
 public:

  CyclopsMaster();
  virtual ~CyclopsMaster();

  void setupClients(const QString& configFilename);

 protected:
  /**
   * Maintains a list of connected slaves, as pairs (IP, port).
   */
  QList<QPair<QHostAddress, int> > _slaves;

  /**
   * Maintains a list of QHttp connection ids to client socket that originated the
   * spawning of this QHttp.
   */
  QMap<int, QTcpSocket*> _slaveToClient;

  /**
   * Maintains a list of QHttp connections incoming data.
   */
  QMap<int, QBuffer*> _slaveReplies;

  /**
   * Maintains the list of slave replies already obtained for a client query. When
   * this list grows up to a size that is equal to the number of slaves, we should merge
   * the results and send the result back to the client.
   */
  QMap<QTcpSocket*, PendingRequest> _pendingRequests;

  virtual void readClient();

  /**
   * Send a reply to the client if all slaves have replied.
   */
  void sendReadyReply(QTcpSocket* client);
#ifdef GAIA_QT5
  void requestFinished(QNetworkReply* pReply);
  void ReplyFinished();
  void ReplyError(QNetworkReply::NetworkError code);
  bool m_replyGotError;
  void ReplySSLError(const QList<QSslError> & errors);
#endif
  /**
   * Sends the specified message to client.
   */
  void sendReplyToClient(QTcpSocket* client, const gaia2::yaml::Mapping& reply);

  // need to rewrite this to be called only once and return all the split queries at once
  gaia2::yaml::Mapping split(const gaia2::yaml::Mapping& query, int idx, int total, PendingRequest& preq);
  gaia2::yaml::Mapping reduce(const PendingRequest& results);

  void distributeClientRequest(QTcpSocket* client, const gaia2::yaml::Mapping& q);
  int sendSlaveRequest(const QPair<QHostAddress, int>& slave, const gaia2::yaml::Mapping& request);

  void setup(const QString& filename);

 protected slots:
  void replyFromSlave(int id, bool error);

};

#endif // GAIA_CYCLOPSMASTER_H
//#endif
