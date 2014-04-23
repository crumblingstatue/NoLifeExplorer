#pragma once

#include "nx/node.hpp"
#include <QStringList>

QString nodeTypeAsString(nl::node node);
QString nodeValueAsString(nl::node node);
const void *getBitmapData(nl::node n);
QStringList findNodes(nl::node root, QString name, QString path = QString());
