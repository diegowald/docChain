#ifndef LINK_H
#define LINK_H

#include <QObject>
#include <QDateTime>

class Link : public QObject
{
    Q_OBJECT
public:
    explicit Link(const QByteArray &author, const QByteArray &signature, QObject *parent = nullptr);
    virtual ~Link();
    uint64_t id() const;
    void setId(const uint64_t &id);
    QDateTime creation() const;
    void setCreation(const QDateTime &moment);
    QByteArray author() const;
    QByteArray signature() const;
signals:

private:
    int _id;
    QByteArray _author;
    QByteArray _signature;
    QDateTime _creation;
};

#endif // LINK_H
