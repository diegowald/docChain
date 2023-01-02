#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

class HttpHandler : public QObject
{

    Q_OBJECT
public:
    explicit HttpHandler(QString url, QObject *parent = nullptr);
    virtual ~HttpHandler();

signals:
    void receivedData(QString label, QJsonDocument response, int repsonseCode);


private slots:
    void dataReadyRead();
    void dataReadFinished();
    //void test(QJsonDocument response);

private:
    QNetworkAccessManager* _netManager;
    QNetworkReply* _netReply;
    QByteArray* _dataBuffer;
    QUrl _url;

protected: //functions
    QUrl mergeUrl(QString endpoint);
    void requestPOST(QString label, QString endpoint, QJsonDocument payload);
    void requestGET(QString label, QString endpoint, QJsonDocument payload);

private:
    QHash<QString, QString> _endpointsSignals;
};

#endif // HTTPHANDLER_H
