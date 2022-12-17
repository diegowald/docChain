#ifndef SIGNINGCALCULATION_H
#define SIGNINGCALCULATION_H

#include <QObject>

class SigningCalculation : public QObject
{
    Q_OBJECT
public:
    explicit SigningCalculation(QObject *parent = nullptr);
    virtual ~SigningCalculation();

    QByteArray calculate(const QByteArray &author, const QByteArray &previousChain, const QByteArray &payload);
signals:

};

#endif // SIGNINGCALCULATION_H
