#ifndef NODEITEM_HPP
#define NODEITEM_HPP

#include <QTreeWidgetItem>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "NoLifeNx/nx.hpp"
#include "NoLifeNx/node.hpp"
#pragma GCC diagnostic pop

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const nl::node& n);
    QString typeStr();
    QString valueStr();

    nl::node node;
    bool childrenAdded = false;
};

#endif // NODEITEM_HPP
