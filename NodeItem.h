#ifndef NODEITEM_H
#define NODEITEM_H

#include <QTreeWidgetItem>
#include "NoLifeNx/NX.hpp"

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const NL::Node& n) :
        node(n)
    {
        setText(0, QString::fromStdString(node.Name()));
    }

    NL::Node node;
};

#endif // NODEITEM_H
