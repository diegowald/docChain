#ifndef IBLOCKCHAIN_H
#define IBLOCKCHAIN_H
#include "link.h"

struct IBlockChain {
    virtual uint64_t getLastId() const = 0;
    virtual void add(QSharedPointer<Link> link) = 0;
    virtual QSharedPointer<Link> findById(uint64_t id) = 0;
    virtual QSharedPointer<Link> findBySignature(const QByteArray &signature) = 0;
};

#endif // IBLOCKCHAIN_H
