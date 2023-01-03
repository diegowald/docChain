#include "link.h"
#include <QJsonValue>
#include <QVariant>

Link::Link(const QByteArray &author, const QByteArray &signature, QObject *parent)
    : QObject{parent}, _id{0}, _author{author}, _signature{signature}
{
}

Link::Link(const QJsonObject& jsonObj)
{
    _id = jsonObj["id"].toInt();
    _author = QByteArray::fromBase64(jsonObj["author"].toString().toUtf8());
    _signature = QByteArray::fromBase64(jsonObj["signature"].toString().toUtf8());
    _creation = QDateTime::fromMSecsSinceEpoch(jsonObj["creation"].toInteger());
}

Link::~Link()
{}

QJsonObject Link::toJson()
{
    QJsonObject result;

    result["id"] = _id;
    result["author"] = QString(_author.toBase64());
    result["signature"] = QString(_signature.toBase64());
    result["creation"] = _creation.toMSecsSinceEpoch();

    return result;
}


uint64_t Link::id() const
{
    return _id;
}

void Link::setId(const uint64_t &id) {
    _id = id;
}

QDateTime Link::creation() const
{
    return _creation;
}

void Link::setCreation(const QDateTime &moment)
{
    _creation = moment;
}

QByteArray Link::author() const
{
    return _author;
}

QByteArray Link::signature() const
{
    return _signature;
}
