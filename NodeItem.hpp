#pragma once

#include <QTreeWidgetItem>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "nx/node.hpp"
#pragma GCC diagnostic pop

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const nl::node& n);
    const nl::node node;
    bool childrenAdded = false;
};
