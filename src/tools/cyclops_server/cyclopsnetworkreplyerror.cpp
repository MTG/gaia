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

// Based on QNetworkReply-NetworkError.cpp Grist.
// see: http://doc.qt.io/qt-5/qnetworkreply.html
void CyclopsMaster::ReplyError(QNetworkReply::NetworkError code)
{
//switch(request->error()) {
    switch (code) {
        case QNetworkReply::ConnectionRefusedError: // the remote server refused the connection (the server is not accepting requests)
        case QNetworkReply::RemoteHostClosedError: // the remote server closed the connection prematurely, before the entire reply was received and processed
        case QNetworkReply::HostNotFoundError: // the remote host name was not found (invalid hostname)
        case QNetworkReply::TimeoutError: // the connection to the remote server timed out
        case QNetworkReply::OperationCanceledError: // the operation was canceled via calls to abort() or close() before it was finished.
        case QNetworkReply::SslHandshakeFailedError: // the SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted.
        case QNetworkReply::TemporaryNetworkFailureError: // the connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established.
        case QNetworkReply::NetworkSessionFailedError: // the connection was broken due to disconnection from the network or failure to start the network.
        case QNetworkReply::BackgroundRequestNotAllowedError: // the background request is not currently allowed due to platform policy.
        case QNetworkReply::TooManyRedirectsError: // while following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)
        case QNetworkReply::InsecureRedirectError: // while following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)
        case QNetworkReply::ProxyConnectionRefusedError: // the connection to the proxy server was refused (the proxy server is not accepting requests)
        case QNetworkReply::ProxyConnectionClosedError: // the proxy server closed the connection prematurely, before the entire reply was received and processed
        case QNetworkReply::ProxyNotFoundError: // the proxy host name was not found (invalid proxy hostname)
        case QNetworkReply::ProxyTimeoutError: // the connection to the proxy timed out or the proxy did not reply in time to the request sent
        case QNetworkReply::ProxyAuthenticationRequiredError: // the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)
        case QNetworkReply::ContentAccessDenied: // the access to the remote content was denied (similar to HTTP error 401)
        case QNetworkReply::ContentOperationNotPermittedError: // the operation requested on the remote content is not permitted
        case QNetworkReply::ContentNotFoundError: // the remote content was not found at the server (similar to HTTP error 404)
        case QNetworkReply::AuthenticationRequiredError: // the remote server requires authentication to serve the content but the credentials provided were not accepted (if any)
        case QNetworkReply::ContentReSendError: // the request needed to be sent again, but this failed for example because the upload data could not be read a second time.
        case QNetworkReply::ContentConflictError: // the request could not be completed due to a conflict with the current state of the resource.
        case QNetworkReply::ContentGoneError: // the requested resource is no longer available at the server.
        case QNetworkReply::InternalServerError: // the server encountered an unexpected condition which prevented it from fulfilling the request.
        case QNetworkReply::OperationNotImplementedError: // the server does not support the functionality required to fulfill the request.
        case QNetworkReply::ServiceUnavailableError: // the server is unable to handle the request at this time.
        case QNetworkReply::ProtocolUnknownError: // the Network Access API cannot honor the request because the protocol is not known
        case QNetworkReply::ProtocolInvalidOperationError: // the requested operation is invalid for this protocol
        case QNetworkReply::UnknownNetworkError: // an unknown network-related error was detected
        case QNetworkReply::UnknownProxyError: // an unknown proxy-related error was detected
        case QNetworkReply::UnknownContentError: // an unknown error related to the remote content was detected
        case QNetworkReply::ProtocolFailure: // a breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)
        case QNetworkReply::UnknownServerError: // an unknown error related to the server response was detected
        default:
            qDebug() << "QNetworkReply::NetworkError" << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(request->error());
    }
}
