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

static bool isLoggedIn(const QList<QPair<QByteArray, QByteArray>> &headers, SigningManager &signingManager)
{
    for (const auto &[key, value] : headers) {
        if (key == "token") {
            return signingManager.isValidToken(value);
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

    // Setup QHttpServer
    QHttpServer httpServer;

    httpServer.route("/v1/createSignature", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        if (!isLoggedIn(request.headers(), signingMgr)) {
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
        if (!isLoggedIn(request.headers(), signingMgr)) {
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

    httpServer.route("/v1/login1", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email")){
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
        QString email = json->value("email").toString();
        auto result = signingMgr.challenge(email);
        if (result.first) {
            QJsonObject jsonResult;
            jsonResult.insert("challenge", QJsonValue(result.second));
            return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Accepted);
        }
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
    });

    httpServer.route("/v1/login2", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email") || !json->contains("challengeResult")){
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
        QString email = json->value("email").toString();
        QString challengeResult = json->value("challengeResult").toString();
        auto result = signingMgr.validateChallenge(email, challengeResult);
        if (result.first) {
            QJsonObject jsonResult;
            jsonResult.insert("response", result.second);
            return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Accepted);
        } else {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
    });

    httpServer.route("/v1/addUser", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        if (!checkApiKeyHeader(request.headers())) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }

        if (signingMgr.hasAuthors() && !isLoggedIn(request.headers(), signingMgr)) {
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }

        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email") || !json->contains("password")){
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
        QString email = json->value("email").toString();
        QString password = json->value("password").toString();
        signingMgr.addUser(email, password);
        return QHttpServerResponse(QJsonObject(), QHttpServerResponder::StatusCode::Accepted);
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
