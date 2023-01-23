#include "author.h"
#include <QCryptographicHash>
#include <QJsonObject>

Author::Author(QObject *parent)
    : QObject{parent}
{}


Author::~Author()
{}

Author::Author(const QJsonObject &jsonObj, QObject *parent): QObject(parent)
{
    _name = jsonObj["name"].toString();
    _email = jsonObj["email"].toString();
    _password = jsonObj["password"].toString();
}

const QJsonObject Author::toJson() const
{
    QJsonObject jsonObj;

    jsonObj["name"] = _name;
    jsonObj["email"] = _email;
    jsonObj["password"] = _password;

    return jsonObj;
}

const QJsonObject Author::toJsonResponse() const
{
    QJsonObject jsonObj;

    jsonObj["name"] = _name;
    jsonObj["email"] = _email;

    return jsonObj;
}

void Author::setName(const QString &value)
{
    _name = value;
}

void Author::setEmail(const QString &value)
{
    _email = value;
}

void Author::setPassword(const QString &value)
{
    _password = value;
}

const QString Author::name() const
{
    return _name;
}

const QString Author::email() const
{
    return _email;
}

const QString Author::password() const
{
    return _password;
}


const QString Author::hash() const
{
    return QCryptographicHash::hash(_email.toUtf8(), QCryptographicHash::Algorithm::Md5);
}

const QString Author::challenge(const QDateTime& seed) const
{
    QString s = "%1-%2";
    s = s.arg("challenge").arg(seed.toMSecsSinceEpoch());
    return QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Algorithm::Md5);
}

const QString Author::calculateChallengeResponse(const QString &challenge)
{
    QString s = "%1-%2";
    s = s.arg(_password).arg(challenge);
    return QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Algorithm::Md5);
}

const bool Author::validateChallenge(const QDateTime &seed, const QString &challengeResponse)
{
    QString calculatedChallengeResponse = calculateChallengeResponse(challenge(seed));
    return calculatedChallengeResponse == challengeResponse;
}

const QString Author::createToken(const QDateTime &seed) const
{
    QString s = "%1-%2-%3";
    s = s.arg(_password).arg(seed.toMSecsSinceEpoch());
    return QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Algorithm::Md5);
}
