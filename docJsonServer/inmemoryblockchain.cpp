#include "inmemoryblockchain.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>


InMemoryBlockChain::InMemoryBlockChain(QObject *parent)
    : QObject{parent}
{
    _lastId = 0;
    loadFromDisk();
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
    link->setCreation(QDateTime::currentDateTimeUtc());
    _block[_lastId] = link;
    _idxBySignature[link->signature()] = _lastId;
    saveToDisk();
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

void InMemoryBlockChain::loadFromDisk()
{
    _lastId = 0;
    QFile file("./chain.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jsonArray = jsonDoc.array();
        for (auto jsonVal: jsonArray) {
            auto link = QSharedPointer<Link>::create(jsonVal.toObject());
            int id = link->id();
            _block[id] = link;
            _idxBySignature[link->signature()] = id;
            if (_lastId < id) {
                _lastId = id;
            }
        }
    }
}

void InMemoryBlockChain::saveToDisk()
{
    QJsonArray jsonArray;
    for (auto kv: _block) {
        jsonArray.append(kv->toJson());
    }
    QJsonDocument jsonDoc;
    jsonDoc.setArray(jsonArray);
    QFile file("./chain.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson());
    }
}
