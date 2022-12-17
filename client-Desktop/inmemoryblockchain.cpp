#include "inmemoryblockchain.h"

InMemoryBlockChain::InMemoryBlockChain(QObject *parent)
    : QObject{parent}
{
    _lastId = 0;
}

InMemoryBlockChain::~InMemoryBlockChain()
{}

uint64_t InMemoryBlockChain::getLastId() const {
    return _lastId;
}

void InMemoryBlockChain::add(QSharedPointer<Link> link)
{
    ++_lastId;
    link->setId(_lastId);
    _block[_lastId] = link;
    _idxBySignature[link->signature()] = _lastId;
}

QSharedPointer<Link> InMemoryBlockChain::findById(uint64_t id)
{
    if (_block.contains(id)) {
        return _block[id];
    }
    return nullptr;
}

QSharedPointer<Link> InMemoryBlockChain::findBySignature(const QByteArray &signature)
{
    if (_idxBySignature.contains(signature)) {
        return findById(_idxBySignature[signature]);
    }
    return nullptr;
}
