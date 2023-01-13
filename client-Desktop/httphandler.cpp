#include "httphandler.h"

HttpHandler::HttpHandler(QString url, QObject *parent) :
    QObject(parent),
    _netManager(new QNetworkAccessManager(this)),
    _netReply(nullptr),
    _dataBuffer(new QByteArray),
    _url(QUrl(url))
{

    //qDebug() << "HttpHandler::HttpHandler";
    _netManager->setProxy(QNetworkProxy::NoProxy);

}

HttpHandler::~HttpHandler()
{}


QUrl HttpHandler::mergeUrl(const QString &endpoint)
{
    QString temp = _url.toString();
    temp = temp + "/" + endpoint;
    qDebug() << QUrl(temp);
    return QUrl(temp);
}

void HttpHandler::requestPOST(const QString& label, const QString& endpoint, const QMap<QString, QString>& headers, const QJsonDocument& payload)
{
    qDebug() << "POST request";
    QNetworkRequest request;

    request.setUrl(mergeUrl(endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    for(QString& k: headers.keys()) {
        request.setRawHeader(k.toUtf8(), headers[k].toUtf8());
    }
    //adding payload and send request
    QByteArray data = payload.toJson();
    qDebug() << "Performing POST now";
    qDebug() << "data: " << data;
    _netReply = _netManager->post(request, data);

    connect(_netReply, &QIODevice::readyRead, this, &HttpHandler::dataReadyRead);
    connect(_netReply, &QNetworkReply::finished, this, &HttpHandler::dataReadFinished);
    _endpointsSignals[request.url().toString()] = label;
}

void HttpHandler::requestGET(const QString &label, const QString &endpoint, const QMap<QString, QString>& headers, const QJsonDocument &payload)
{
    qDebug() << "GET request";
    QNetworkRequest request;
    request.setUrl(mergeUrl(endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    for(QString k: headers.keys()) {
        request.setRawHeader(k.toUtf8(), headers[k].toUtf8());
    }
    _netReply = _netManager->get(request);

    connect(_netReply, &QIODevice::readyRead, this, &HttpHandler::dataReadyRead);
    connect(_netReply, &QNetworkReply::finished, this, &HttpHandler::dataReadFinished);
    _endpointsSignals[request.url().toString()] = label;
}

void HttpHandler::dataReadyRead()
{
    _dataBuffer->append(_netReply->readAll());
}

void HttpHandler::dataReadFinished()
{
    QVariant status_code = _netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int responseCode = status_code.toString().toInt();
    qDebug() << "Status Code: " << responseCode;

    if(_netReply->error()) {
        qInfo() << "An error in HTTP request occured: " << _netReply->errorString();
    } else {
        QJsonParseError jsonError;
        //qDebug()<< "Incoming data";
        QJsonDocument response = QJsonDocument::fromJson(*_dataBuffer, &jsonError);
        qDebug() << *_dataBuffer;
        if(jsonError.error != QJsonParseError::NoError) {
            qDebug()<< jsonError.errorString();
        }
        //qDebug().noquote() << response.toJson();
        QString label = _endpointsSignals[_netReply->url().toString()];
        _netReply->deleteLater();
        emit receivedData(label, response, responseCode);
    }
    _dataBuffer->clear();
}




