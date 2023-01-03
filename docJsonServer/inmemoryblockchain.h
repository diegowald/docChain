#ifndef INMEMORYBLOCKCHAIN_H
#define INMEMORYBLOCKCHAIN_H

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "iblockchain.h"

class InMemoryBlockChain : public QObject, public IBlockChain
{
    Q_OBJECT
public:
    explicit InMemoryBlockChain(QObject *parent = nullptr);
    virtual ~InMemoryBlockChain();

    uint64_t getLastId() const override;
    void add(QSharedPointer<Link> link) override;
    QSharedPointer<Link> findById(uint64_t id) override;
    QSharedPointer<Link> findBySignature(const QByteArray &signature) override;
signals:

private:
    void loadFromDisk();
    void saveToDisk();

private:
    QHash<uint64_t, QSharedPointer<Link>> _block;
    QHash<QByteArray, uint64_t> _idxBySignature;
    uint64_t _lastId;
};

#endif // INMEMORYBLOCKCHAIN_H
