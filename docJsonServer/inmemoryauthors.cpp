#include "inmemoryauthors.h"
#include <QSharedPointer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "author.h"
#include "authorfacade.h"


InMemoryAuthors::InMemoryAuthors(QObject *parent)
    : IAuthors{parent}
{
    loadData();
}

InMemoryAuthors::~InMemoryAuthors()
{}

QPair<bool, QString> InMemoryAuthors::challenge(const QString &email)
{
    QDateTime seed = QDateTime::currentDateTimeUtc();
    QPair<bool, QString> result;
    if (_idxAuthorsByEmail.contains(email)) {
        result.first = true;
        result.second = _authors[_idxAuthorsByEmail[email]]->challenge(seed);
        ChallengeCreation cc;
        cc.challenge = result.second;
        cc.moment = seed;
        _challenges[email] = cc;
    } else {
        result.first = false;
        result.second = "";
    }

    return result;
}

QPair<bool, QString> InMemoryAuthors::validateChallenge(const QString &email, const QString &challengeResponse)
{
    QPair<bool, QByteArray> result;

    if (_challenges.contains(email) && _idxAuthorsByEmail.contains(email)) {
        auto creationChallenge = _challenges[email];
        auto author = _authors[_idxAuthorsByEmail[email]];
        result.first = author->validateChallenge(creationChallenge.moment, challengeResponse);
        if (result.first) {
            _challenges.remove(email);
            TokenCreation tc;
            tc.moment = QDateTime::currentDateTimeUtc();
            tc.email = email;
            result.second = author->createToken(tc.moment).toUtf8();
            _tokens[result.second] = tc;
            return result;
        }
    }

    result.first = false;
    return result;
}

const bool InMemoryAuthors::isValidToken(const QString &token)
{
    return _tokens.contains(token);
}

bool InMemoryAuthors::addAuthor(QSharedPointer<AuthorFacade> authorFacade)
{
    auto email = authorFacade->email();
    if (_idxAuthorsByEmail.contains(email)) {
        return false;
    }
    QSharedPointer<Author> author = QSharedPointer<Author>::create();
    author->setEmail(authorFacade->email());
    author->setPassword(authorFacade->password());
    author->setName(authorFacade->name());
    _authors[author->hash()] = author;
    _idxAuthorsByEmail[author->email()] = author->hash();
    saveData();
    return true;
}

QVector<QSharedPointer<Author> > InMemoryAuthors::authors()
{
    return _authors.values();
}

QSharedPointer<Author> InMemoryAuthors::findByToken(const QByteArray &token)
{
    if (_tokens.contains(token)) {
        auto email = _tokens[token].email;
        if (_idxAuthorsByEmail.contains(email)) {
            auto hash = _idxAuthorsByEmail[email];
            if (_authors.contains(hash)) {
                return _authors[hash];
            }
        }
    }
    return nullptr;
}

const bool InMemoryAuthors::hasAuthors() const
{
    return _authors.count() > 0;
}

void InMemoryAuthors::loadData()
{
    QFile file("./authors.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jsonArray = jsonDoc.array();
        for (auto jsonVal: jsonArray) {
            auto author = QSharedPointer<Author>::create(jsonVal.toObject());
            _authors[author->hash()] = author;
            _idxAuthorsByEmail[author->email()] = author->hash();
        }
    }
}

void InMemoryAuthors::saveData()
{
    QJsonArray jsonArray;
    for (const auto &kv: _authors) {
        jsonArray.append(kv->toJson());
    }
    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArray);
    QFile file("./authors.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
    }
}
