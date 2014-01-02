#pragma once

#include <QTreeWidgetItem>
#include "nx/node.hpp"

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const nl::node& n);
    const nl::node node;
    bool childrenAdded = false;
};
