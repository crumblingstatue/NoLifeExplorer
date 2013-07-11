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
        setText(1, typeStr());
        setText(2, valueStr());
    }

    QString typeStr()
    {
        switch (node.T())
        {
        case NL::Node::Type::Audio:
            return "Audio";
        case NL::Node::Type::Bitmap:
            return "Bitmap";
        case NL::Node::Type::Float:
            return "Float";
        case NL::Node::Type::Int:
            return "Int";
        case NL::Node::Type::String:
            return "String";
        case NL::Node::Type::Vector:
            return "Vector";
        case NL::Node::Type::None:
            return QString();
        }

        return QString("?????");
    }

    QString valueStr()
    {
        switch (node.T())
        {
        case NL::Node::Type::Float:
        case NL::Node::Type::Int:
        case NL::Node::Type::String:
            return QString::fromStdString(node.GetString());
        case NL::Node::Type::Vector:
            return QString::number(node.GetVector().first) + ", " + QString::number(node.GetVector().second);
        default:
            return QString();
        }

        return QString();
    }

    NL::Node node;
    bool childrenAdded = false;
};

#endif // NODEITEM_H
