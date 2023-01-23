#include "signingmanager.h"

#include <QMutexLocker>

#include "signingcalculation.h"
#include "iblockchain.h"
#include "inmemoryblockchain.h"
#include "iauthors.h"
#include "inmemoryauthors.h"
#include "authorfacade.h"
#include "author.h"

SigningManager::SigningManager(QObject *parent)
    : QObject{parent}
{
    _blockchain = new InMemoryBlockChain(this);
    _authors = new InMemoryAuthors(this);
}


SigningManager::~SigningManager()
{}


QByteArray SigningManager::createSignature(const QByteArray &token, const QByteArray &payload)
{
    auto authorPtr = _authors->findByToken(token);
    SigningCalculation calculator;
    QMutexLocker locker(&_createMutex);
    auto lastId = _blockchain->getLastId();
    auto lastLink = _blockchain->findById(lastId);
    auto signature = calculate(lastId, authorPtr->hash().toUtf8(), payload);

    QSharedPointer<Link> link = QSharedPointer<Link>::create(authorPtr->hash().toUtf8(), signature);
    _blockchain->add(link);

    return signature;
}

QByteArray SigningManager::calculate(uint64_t lastId, const QByteArray &author, const QByteArray &payload)
{
    SigningCalculation calculator;

    auto lastLink = _blockchain->findById(lastId);
    QByteArray lastSignature;
    if (lastLink != nullptr) {
        lastSignature = lastLink->signature();
    }

    auto signature = calculator.calculate(author, lastSignature, payload);
    return signature;
}

SigningManager::SignatureValidation SigningManager::isValidSignature(const QByteArray &payload, const QByteArray &signature) {
    bool result = false;

    auto link = _blockchain->findBySignature(signature);
    if (link != nullptr) {
        auto calculatedSignature = calculate(link->id() - 1, link->author(), payload);
        if (calculatedSignature != signature) {
            return SignatureValidation::InvalidSignature;
        }
        return SignatureValidation::ValidSignature;
    }

    return SignatureValidation::NonExistentSignature;
}

const QPair<bool, QString> SigningManager::challenge(const QString &email)
{
    return _authors->challenge(email);
}

const QPair<bool, QString> SigningManager::validateChallenge(const QString &email, const QString &challengeResult)
{
    return _authors->validateChallenge(email, challengeResult);
}

const bool SigningManager::isValidToken(const QString &token)
{
    return _authors->isValidToken(token);
}

const bool SigningManager::hasAuthors() const
{
    return _authors->hasAuthors();
}

void SigningManager::addUser(const QString &email, const QString &password)
{
    QSharedPointer<AuthorFacade> author = QSharedPointer<AuthorFacade>::create();
    author->setEmail(email);
    author->setPassword(password);
    _authors->addAuthor(author);
}

