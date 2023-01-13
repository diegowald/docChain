#include "authorfacade.h"
#include <QJsonObject>


AuthorFacade::AuthorFacade(QObject *parent)
    : QObject{parent}
{

}


const QJsonObject AuthorFacade::toJson() const
{
    QJsonObject jsonObj;

    jsonObj["name"] = _name;
    jsonObj["email"] = _email;
    jsonObj["password"] = _password;

    return jsonObj;
}

void AuthorFacade::setName(const QString &value)
{
    _name = value;
}

void AuthorFacade::setEmail(const QString &value)
{
    _email = value;
}

void AuthorFacade::setPassword(const QString &value)
{
    _password = value;
}

const QString AuthorFacade::name() const
{
    return _name;
}

const QString AuthorFacade::email() const
{
    return _email;
}

const QString AuthorFacade::password() const
{
    return _password;
}
