#ifndef SIGNINGMANAGER_H
#define SIGNINGMANAGER_H

#include <QObject>
#include <QMutex>

class IBlockChain;

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

    SignatureValidation isValidSignature(const QByteArray &payload, const QByteArray &signature);

private:
    QByteArray calculate(uint64_t lastId, const QByteArray &author, const QByteArray &payload);

signals:

private:
    IBlockChain *_blockchain;
    QMutex _createMutex;
};

#endif // SIGNINGMANAGER_H
