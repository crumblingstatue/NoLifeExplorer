#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "NoLifeNx/node.hpp"
#pragma GCC diagnostic pop
#include <QString>

QString nodeTypeAsString(nl::node node);
QString nodeValueAsString(nl::node node);
const void* getBitmapData(nl::node n);
