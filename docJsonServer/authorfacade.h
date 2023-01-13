#ifndef AUTHORFACADE_H
#define AUTHORFACADE_H

#include <QObject>

class AuthorFacade : public QObject
{
    Q_OBJECT
public:
    explicit AuthorFacade(QObject *parent = nullptr);

    const QJsonObject toJson() const;
    void setName(const QString &value);
    void setEmail(const QString &value);
    void setPassword(const QString &value);

    const QString name() const;
    const QString email() const;
    const QString password() const;
signals:
private:
    QString _name;
    QString _email;
    QString _password;
};

#endif // AUTHORFACADE_H
