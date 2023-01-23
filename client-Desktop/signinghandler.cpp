#include "signinghandler.h"
#include <QCryptographicHash>

SigningHandler::SigningHandler(const QString &url, QObject *parent)
    : HttpHandler(url, parent)
{
    connect(this, &HttpHandler::receivedData, this, &SigningHandler::onReceivedData);
    _email = "";
    _password = "";
    _token = "";
}

SigningHandler::~SigningHandler()
{}

void SigningHandler::createSignature(const QByteArray &author, const QByteArray &payload)
{
    QByteArray payload2Send;
    if (payload.length() > 2048) {
        payload2Send = QCryptographicHash::hash(payload, QCryptographicHash::Algorithm::Md5);
    } else {
        payload2Send = payload;
    }
    QJsonObject jsonObj;
    jsonObj.insert("author", QJsonValue::fromVariant(author.toBase64()));
    jsonObj.insert("payload", QJsonValue::fromVariant(payload2Send.toBase64()));
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QMap<QString, QString> headers;
    headers["api_key"] = "SecretKey";
    headers["token"] = _token;
    requestPOST("createSignature", "v1/createSignature", headers, jsonDoc);
}

void SigningHandler::validateSignature(const QByteArray& payload, const QByteArray &signature)
{
    QJsonObject jsonObj;
    jsonObj.insert("signature", QJsonValue::fromVariant(signature.toBase64()));
    QByteArray payload2Send;
    if (payload.length() > 2048) {
        payload2Send = QCryptographicHash::hash(payload, QCryptographicHash::Algorithm::Md5);
    } else {
        payload2Send = payload;
    }
    jsonObj.insert("payload", QJsonValue::fromVariant(payload2Send.toBase64()));
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QMap<QString, QString> headers;
    headers["api_key"] = "SecretKey";
    headers["token"] = _token;
    requestPOST("validateSignature", "v1/isValidSignature", headers, jsonDoc);
}

void SigningHandler::login(const QString &email, const QString &password)
{
    _email = email;
    _password = password;
    _token.clear();

    QJsonObject jsonObj;
    jsonObj.insert("email", email);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QMap<QString, QString> headers;
    headers["api_key"] = "SecretKey";
    requestPOST("login", "v1/login1", headers, jsonDoc);
}

void SigningHandler::login2(const QString &email, const QString &challengeResponse)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", email);
    jsonObj.insert("challengeResult", challengeResponse);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QMap<QString, QString> headers;
    headers["api_key"] = "SecretKey";
    requestPOST("login2", "v1/login2", headers, jsonDoc);
}

void SigningHandler::calculateResponse(const QString &challenge)
{
    QString s = "%1-%2";
    s = s.arg(_password).arg(challenge);
    QString challengeResponse = QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Algorithm::Md5);
    login2(_email, challengeResponse);
}


void SigningHandler::addUser(const QString &email, const QString &password)
{
    _token.clear();
    QJsonObject jsonObj;
    jsonObj.insert("email", email);
    jsonObj.insert("password", password);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QMap<QString, QString> headers;
    headers["api_key"] = "SecretKey";
    requestPOST("addUser", "v1/addUser", headers, jsonDoc);
}

void SigningHandler::onReceivedData(QString label, QJsonDocument response, int repsonseCode)
{
    QJsonObject jsonResponse = response.object();
    if (label == "createSignature") {
        QByteArray signature = QByteArray::fromBase64(jsonResponse["signature"].toString().toUtf8());
        emit signatureCreated(signature);
    } else if (label == "validateSignature") {
        SignatureValidation result = (SignatureValidation)jsonResponse["result"].toInt();
        emit validateSignatureResult(result);
    } else if (label == "login") {
        QString challenge = jsonResponse["challenge"].toString();
        calculateResponse(challenge);
    } else if (label == "login2") {
        _token = jsonResponse["response"].toString();
    }
}

