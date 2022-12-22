#include "signingcalculation.h"

#include <QCryptographicHash>


SigningCalculation::SigningCalculation(QObject *parent)
    : QObject{parent}
{

}

SigningCalculation::~SigningCalculation()
{}


QByteArray SigningCalculation::calculate(const QByteArray &author, const QByteArray &previousChain, const QByteArray &payload)
{
    QByteArray result;
    QByteArray request = payload;
    request += previousChain;
    request += author;

    result = QCryptographicHash::hash(request, QCryptographicHash::Algorithm::Md5);

    return result;
}
