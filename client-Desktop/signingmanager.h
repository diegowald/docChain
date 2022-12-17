#ifndef SIGNINGMANAGER_H
#define SIGNINGMANAGER_H

#include <QObject>
#include <QMutex>

#include "inmemoryblockchain.h"

class SigningManager : public QObject
{
    Q_OBJECT
public:
    enum class SignatureValidation {
        ValidSignature,
        NonExistentSignature,
        InvalidSignature
    };
    explicit SigningManager(QObject *parent = nullptr);

    virtual ~SigningManager();

    QByteArray createSignature(const QByteArray &author, const QByteArray &payload);
    QByteArray calculate(uint64_t lastId, const QByteArray &author, const QByteArray &payload);

    SignatureValidation isValidSignature(const QByteArray &payload, const QByteArray &signature);

signals:

private:
    InMemoryBlockChain _blockchain;
    QMutex _createMutex;
};

#endif // SIGNINGMANAGER_H
