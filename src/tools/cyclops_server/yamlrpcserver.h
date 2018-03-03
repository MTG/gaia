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

#ifndef GAIA_YAMLRPCSERVER_H
#define GAIA_YAMLRPCSERVER_H

#include <QTcpServer>
#include <QDateTime>
#include "../../yamlcpp.h"

GAIA_DEFINE_EXCEPTION(NotImplementedError);
GAIA_DEFINE_EXCEPTION(InvalidParams);

class YamlProxy {
 public:
  virtual gaia2::yaml::Node process(const gaia2::yaml::Mapping& query) = 0;
  virtual ~YamlProxy() {}
};

/**
 * Return a string describing the time elapsed between 2 QDateTimes.
 */
QString timeElapsed(const QDateTime& from, const QDateTime& to);

/**
 * Return a Yaml object representing an error.
 */
gaia2::yaml::Mapping rpcError(int code, const QString& msg);

/**
 * Synchronously read the contents of an HTTP message from a socket.
 */
QByteArray readHTTPRequest(QTcpSocket* socket);


/**
 * Synchronously read a Yaml-RPC request from a socket.
 */
gaia2::yaml::Mapping readYamlRPCRequest(QTcpSocket* socket);


class YamlRPCServer : public QTcpServer {
Q_OBJECT

 public:

  YamlRPCServer(quint16 port, YamlProxy* proxy);

  void incomingConnection(int socket);

  gaia2::yaml::Mapping processRequest(QTcpSocket* socket);

  int numberConnectedClients() const;

 signals:
  void quit();

 protected slots:
  virtual void readClient();
  void discardClient();

 protected:
  YamlProxy* _proxy;
  QDateTime _startedOn;

  QMap<QTcpSocket*, QByteArray> _socketIDs;
};


#endif // GAIA_YAMLRPCSERVER_H
