#include "signinghandler.h"

SigningHandler::SigningHandler(const QString &url, QObject *parent)
    : HttpHandler(url, parent)
{
    connect(this, &HttpHandler::receivedData, this, &SigningHandler::onReceivedData);
}

SigningHandler::~SigningHandler()
{}

void SigningHandler::createSignature(const QByteArray &author, const QByteArray &payload)
{
    QJsonObject jsonObj;
    jsonObj.insert("author", QJsonValue::fromVariant(author.toBase64()));
    jsonObj.insert("payload", QJsonValue::fromVariant(payload.toBase64()));
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    requestPOST("createSignature", "v1/createSignature", jsonDoc);
}

void SigningHandler::validateSignature(const QByteArray& payload, const QByteArray &signature)
{
    QJsonObject jsonObj;
    jsonObj.insert("signature", QJsonValue::fromVariant(signature.toBase64()));
    jsonObj.insert("payload", QJsonValue::fromVariant(payload.toBase64()));
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    requestPOST("validateSignature", "v1/isValidSignature", jsonDoc);
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
    }
}

