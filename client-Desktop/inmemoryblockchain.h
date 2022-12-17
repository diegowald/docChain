#ifndef INMEMORYBLOCKCHAIN_H
#define INMEMORYBLOCKCHAIN_H

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "link.h"

class InMemoryBlockChain : public QObject
{
    Q_OBJECT
public:
    explicit InMemoryBlockChain(QObject *parent = nullptr);
    virtual ~InMemoryBlockChain();

    uint64_t getLastId() const;
    void add(QSharedPointer<Link> link);
    QSharedPointer<Link> findById(uint64_t id);
    QSharedPointer<Link> findBySignature(const QByteArray &signature);
signals:

private:
    QHash<uint64_t, QSharedPointer<Link>> _block;
    QHash<QByteArray, uint64_t> _idxBySignature;
    uint64_t _lastId;
};

#endif // INMEMORYBLOCKCHAIN_H
