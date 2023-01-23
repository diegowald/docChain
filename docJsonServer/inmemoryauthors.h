#ifndef INMEMORYAUTHORS_H
#define INMEMORYAUTHORS_H

#include <QObject>
#include "iauthors.h"
#include <QDateTime>
#include <QHash>


struct ChallengeCreation {
    QString challenge;
    QDateTime moment;
};

struct TokenCreation {
    QString email;
    QDateTime moment;
};

class InMemoryAuthors : public IAuthors
{
    Q_OBJECT
public:
    explicit InMemoryAuthors(QObject *parent = nullptr);
    virtual ~InMemoryAuthors();

    // IAuthors interface
    virtual QPair<bool, QString> challenge(const QString &email) override;
    virtual QPair<bool, QString> validateChallenge(const QString &email, const QString &challengeResponse) override;
    virtual const bool isValidToken(const QString &token) override;
    virtual bool addAuthor(QSharedPointer<AuthorFacade> authorFacade) override;
    virtual QVector<QSharedPointer<Author> > authors() override;
    virtual QSharedPointer<Author> findByToken(const QByteArray &token) override;
    virtual const bool hasAuthors() const override;

private:
    void loadData();
    void saveData();
private:
    QHash<QString, QSharedPointer<Author>> _authors;
    QHash<QString, QString> _idxAuthorsByEmail;
    QHash<QString, ChallengeCreation> _challenges;
    QHash<QString, TokenCreation> _tokens;
};

#endif // INMEMORYAUTHORS_H
