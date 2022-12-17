#include "link.h"


Link::Link(const QByteArray &author, const QByteArray &signature, QObject *parent)
    : QObject{parent}, _id{0}, _author{author}, _signature{signature}
{
}

Link::~Link()
{}

uint64_t Link::id() const
{
    return _id;
}

void Link::setId(const uint64_t &id) {
    _id = id;
}

QByteArray Link::author() const
{
    return _author;
}

QByteArray Link::signature() const
{
    return _signature;
}
