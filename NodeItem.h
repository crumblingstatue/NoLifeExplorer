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
        case NL::Node::audio:
            return "Audio";
        case NL::Node::bitmap:
            return "Bitmap";
        case NL::Node::dreal:
            return "dreal";
        case NL::Node::ireal:
            return "ireal";
        case NL::Node::string:
            return "String";
        case NL::Node::vector:
            return "Vector";
        case NL::Node::none:
            return QString();
        }

        return QString("?????");
    }

    QString valueStr()
    {
        switch (node.T())
        {
        case NL::Node::dreal:
        case NL::Node::ireal:
        case NL::Node::string:
            return QString::fromStdString(node.GetString());
        case NL::Node::vector:
            return QString::number(node.GetVector().first) + ", " + QString::number(node.GetVector().second);
        default:
            return QString();
        }

        return QString();
    }

    NL::Node node;
};

#endif // NODEITEM_H
