#ifndef SIGNINGHANDLER_H
#define SIGNINGHANDLER_H

#include <QObject>
#include "httphandler.h"

class SigningHandler : public HttpHandler
{
    Q_OBJECT
public:
    explicit SigningHandler(const QString &url, QObject *parent = nullptr);
    virtual ~SigningHandler();

    enum class SignatureValidation {
        ValidSignature,
        NonExistentSignature,
        InvalidSignature
    };

    void createSignature(const QByteArray &author, const QByteArray &payload);
    void validateSignature(const QByteArray& payload, const QByteArray &signature);

    void login(const QString &email, const QString &password);
    void addUser(const QString &email, const QString &password);
private:
    void calculateResponse(const QString &challenge);
    void login2(const QString &email, const QString &challengeResponse);

signals:
    void signatureCreated(const QByteArray signature);
    void validateSignatureResult(const SignatureValidation result);

private slots:
    void onReceivedData(QString label, QJsonDocument response, int repsonseCode);

private:
    QString _email;
    QString _password;
    QString _token;
};

#endif // SIGNINGHANDLER_H
