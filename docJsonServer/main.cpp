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

static QString token(const QList<QPair<QByteArray, QByteArray>> &headers)
{
    for (const auto &[key, value] : headers) {
        if (key == "token") {
            return value;
        }
    }
    return "";
}

static bool isLoggedIn(const QList<QPair<QByteArray, QByteArray>> &headers, SigningManager &signingManager)
{
    return signingManager.isValidToken(token(headers));
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
        qDebug() << "createSignature";
        if (!checkApiKeyHeader(request.headers())) {
            qDebug() << "unauthorized -> no api key";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        if (!isLoggedIn(request.headers(), signingMgr)) {
            qDebug() << "Unauthorized -> no valid token";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("payload")){
            qDebug() << "Bad Request -> No payload found";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }

        QByteArray payload = QByteArray::fromBase64(json->value("payload").toString().toUtf8());

        QByteArray result = signingMgr.createSignature(token(request.headers()).toUtf8(), payload);

        QJsonObject jsonResult;
        jsonResult.insert("signature", QJsonValue(QString(result.toBase64())));

        return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Created);
    });

    httpServer.route("/v1/isValidSignature", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        qDebug() << "IsValidSignature";
        if (!checkApiKeyHeader(request.headers())) {
            qDebug() << "unauthorized -> no api key";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        if (!isLoggedIn(request.headers(), signingMgr)) {
            qDebug() << "Unauthorized -> no valid token";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("signature") || !json->contains("payload")){
            qDebug() << "Bad Request -> No payload or signature found";
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
        qDebug() << "login1";
        if (!checkApiKeyHeader(request.headers())) {
            qDebug() << "unauthorized -> no api key";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email")){
            qDebug() << "Bad Request -> No email found";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
        QString email = json->value("email").toString();
        auto result = signingMgr.challenge(email);
        if (result.first) {
            QJsonObject jsonResult;
            jsonResult.insert("challenge", QJsonValue(result.second));
            return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Accepted);
        }
        qDebug() << "Unauthorized -> bad Challenge";
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
    });

    httpServer.route("/v1/login2", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        qDebug() << "Login2";
        if (!checkApiKeyHeader(request.headers())) {
            qDebug() << "unauthorized -> no api key";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }
        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email") || !json->contains("challengeResult")){
            qDebug() << "Bad Request -> No email or challenge result found";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
        }
        QString email = json->value("email").toString();
        QString challengeResult = json->value("challengeResult").toString();
        auto result = signingMgr.validateChallenge(email, challengeResult);
        if (result.first) {
            QJsonObject jsonResult;
            jsonResult.insert("response", result.second);
            return QHttpServerResponse(jsonResult, QHttpServerResponder::StatusCode::Accepted);
        }
        qDebug() << "Unaithorized -> bad challenge response";
        return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
    });

    httpServer.route("/v1/addUser", QHttpServerRequest::Method::Post,
                     [&signingMgr](const QHttpServerRequest &request) {
        qDebug() << "addUser";
        if (!checkApiKeyHeader(request.headers())) {
            qDebug() << "unauthorized -> no api key";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }

        if (signingMgr.hasAuthors() && !isLoggedIn(request.headers(), signingMgr)) {
            qDebug() << "unauthorized -> no token";
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
        }

        const auto json = byteArrayToJsonObject(request.body());
        if (!json || !json->contains("email") || !json->contains("password")){
            qDebug() << "Bad request -> No email or password";
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
