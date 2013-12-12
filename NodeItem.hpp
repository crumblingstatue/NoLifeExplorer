#ifndef NODEITEM_HPP
#define NODEITEM_HPP

#include <QTreeWidgetItem>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "NoLifeNx/node.hpp"
#pragma GCC diagnostic pop

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const nl::node& n);
    const nl::node node;
    bool childrenAdded = false;
};

#endif // NODEITEM_HPP
