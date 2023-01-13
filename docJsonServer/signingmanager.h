#ifndef SIGNINGMANAGER_H
#define SIGNINGMANAGER_H

#include <QObject>
#include <QMutex>

class IBlockChain;
class IAuthors;

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


    const bool hasAuthors() const;
    const QPair<bool, QString> challenge(const QString &email);
    const QPair<bool, QString> validateChallenge(const QString &email, const QString &challengeResult);
    const bool isValidToken(const QString &token);
    void addUser(const QString &email, const QString &password);

private:
    QByteArray calculate(uint64_t lastId, const QByteArray &author, const QByteArray &payload);

signals:

private:
    IBlockChain *_blockchain;
    QMutex _createMutex;
    IAuthors *_authors;
};

#endif // SIGNINGMANAGER_H
