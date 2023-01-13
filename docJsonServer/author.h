#ifndef AUTHOR_H
#define AUTHOR_H

#include <QObject>

class Author : public QObject
{
    Q_OBJECT
public:
    explicit Author(QObject *parent = nullptr);
    virtual ~Author();
    const QJsonObject toJson() const;
    const QJsonObject toJsonResponse() const;

    void setName(const QString &value);
    void setEmail(const QString &value);
    void setPassword(const QString &value);

    const QString name() const;
    const QString email() const;
    const QString password() const;

    const QString hash() const;
    const QString challenge(const QDateTime &seed) const;
    const QString calculateChallengeResponse(const QString &challenge);
    const bool validateChallenge(const QDateTime &seed, const QString &challengeResponse);
    const QString createToken(const QDateTime &seed) const;
signals:

private:
    QString _name;
    QString _email;
    QString _password;
};

#endif // AUTHOR_H
