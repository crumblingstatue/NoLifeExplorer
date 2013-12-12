#ifndef NODEUTIL_HPP
#define NODEUTIL_HPP

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "NoLifeNx/node.hpp"
#pragma GCC diagnostic pop
#include <QString>

QString nodeTypeAsString(nl::node node);
QString nodeValueAsString(nl::node node);

#endif // NODEUTIL_HPP
