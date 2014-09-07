#pragma once

#include "nx/node.hpp"
#include <QStringList>

namespace nodeUtil {

QString nodeTypeAsString(nl::node node);
QString nodeValueAsString(nl::node node);
void const * getBitmapData(nl::node n);
QStringList findNodes(nl::node root, QString name, QString path = QString());
}
