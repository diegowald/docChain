#include "signingmanager.h"

#include <QMutexLocker>

#include "signingcalculation.h"
#include "iblockchain.h"
#include "inmemoryblockchain.h"

SigningManager::SigningManager(QObject *parent)
    : QObject{parent}
{
    _blockchain = new InMemoryBlockChain(this);
}


SigningManager::~SigningManager()
{}


QByteArray SigningManager::createSignature(const QByteArray &author, const QByteArray &payload)
{
    SigningCalculation calculator;

    QMutexLocker locker(&_createMutex);
    auto lastId = _blockchain->getLastId();
    auto lastLink = _blockchain->findById(lastId);
    auto signature = calculate(lastId, author, payload);

    QSharedPointer<Link> link = QSharedPointer<Link>::create(author, signature);
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
