#ifndef NODEITEM_H
#define NODEITEM_H

#include <QStandardItem>
#include "NoLifeNx/NX.hpp"

struct NodeItem : public QStandardItem
{
    NodeItem(const NL::Node& n) :
        node(n)
    {
        setText(QString::fromStdString(node.Name()));
    }

    NL::Node node;
};

#endif // NODEITEM_H
