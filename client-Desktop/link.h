#ifndef LINK_H
#define LINK_H

#include <QObject>

class Link : public QObject
{
    Q_OBJECT
public:
    explicit Link(const QByteArray &author, const QByteArray &signature, QObject *parent = nullptr);
    virtual ~Link();
    uint64_t id() const;
    void setId(const uint64_t &id);
    QByteArray author() const;
    QByteArray signature() const;
signals:

private:
    int _id;
    QByteArray _author;
    QByteArray _signature;
};

#endif // LINK_H
