#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "nx/node.hpp"
#pragma GCC diagnostic pop
#include <QStringList>

QString nodeTypeAsString(nl::node node);
QString nodeValueAsString(nl::node node);
const void* getBitmapData(nl::node n);
QStringList findNodes(nl::node root, QString name, QString path = QString());
