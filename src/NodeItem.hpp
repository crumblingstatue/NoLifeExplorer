#pragma once

#include <QTreeWidgetItem>
#include "nx/node.hpp"

struct NodeItem : public QTreeWidgetItem {
    NodeItem(nl::node const & n);
    nl::node const node;
    bool childrenAdded = false;
};
