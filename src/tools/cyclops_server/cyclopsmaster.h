#ifndef GAIA_CYCLOPSMASTER_H
#define GAIA_CYCLOPSMASTER_H

#include <QNetworkReply>
#include <QHostAddress>
#include <QBuffer>
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
Q_OBJECT
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
