#ifndef IAUTHORS_H
#define IAUTHORS_H

#include <QObject>
class Author;
class AuthorFacade;

class IAuthors : public QObject
{
    Q_OBJECT
public:
    explicit IAuthors(QObject *parent = nullptr);
    virtual ~IAuthors();
    virtual QPair<bool, QString> challenge(const QString &email) = 0;
    virtual QPair<bool, QString> validateChallenge(const QString &email, const QString &challengeResponse) = 0;
    virtual const bool isValidToken(const QString &token) = 0;
    virtual bool addAuthor(QSharedPointer<AuthorFacade> author) = 0;
    virtual QVector<QSharedPointer<Author>> authors() = 0;

    virtual const bool hasAuthors() const = 0;
    virtual QSharedPointer<Author> findByToken(const QByteArray& token) = 0;
signals:

};

#endif // IAUTHORS_H
