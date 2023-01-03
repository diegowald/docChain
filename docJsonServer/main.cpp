#include <QtCore>
#include <QtHttpServer>

#include "signingmanager.h"

#define API_KEY "SecretKey"

static bool checkApiKeyHeader(const QList<QPair<QByteArray, QByteArray>> &headers)
{
    for (const auto &[key, value] : headers) {
        if (key == "api_key" && value == API_KEY) {
            return true;
        }
    }
    return false;
}

static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &arr)
{
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isObject())
        return std::nullopt;
    return json.object();
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SigningManager signingMgr;

    //QMap<qint64, ContactEntry> contacts;
    // Setup QHttpServer
    QHttpServer httpServer;

    httpServer.route("/v1/createSignature", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("author") || !json->contains("payload")){
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }

        QByteArray author = QByteArray::fromBase64(json->value("author").toString().toUtf8());
        QByteArray payload = QByteArray::fromBase64(json->value("payload").toString().toUtf8());

        QByteArray result = signingMgr.createSignature(author, payload);

        QJsonObject jsonResult;
        jsonResult.insert("signature", QJsonValue(QString(result.toBase64())));

        return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Created);
    });

    httpServer.route("/v1/isValidSignature", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("signature") || !json->contains("payload")){
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }

        QByteArray signature = QByteArray::fromBase64(json->value("signature").toString().toUtf8());
        QByteArray payload = QByteArray::fromBase64(json->value("payload").toString().toUtf8());

        SigningManager::SignatureValidation result = signingMgr.isValidSignature(payload, signature);

        QJsonObject jsonResult;
        jsonResult.insert("result", QJsonValue((short)result));

        return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Accepted);
    });

    const auto port = httpServer.listen(QHostAddress::Any, 8080);
    if (!port) {
        qDebug() << QCoreApplication::translate("QHttpServerExample",
                                                "Server failed to listen on a port.");
        return 0;
    }

    qDebug() << QCoreApplication::translate(
                        "QHttpServerExample",
                        "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)")
                        .arg(port);

    return app.exec();
}
