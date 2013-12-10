#ifndef NODEITEM_H
#define NODEITEM_H

#include <QTreeWidgetItem>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "NoLifeNx/nx.hpp"
#include "NoLifeNx/node.hpp"
#pragma GCC diagnostic pop

struct NodeItem : public QTreeWidgetItem
{
    NodeItem(const nl::node& n) :
        node(n)
    {
        setText(0, QString::fromStdString(node.name()));
        setText(1, typeStr());
        setText(2, valueStr());
    }

    QString typeStr()
    {
        switch (node.data_type())
        {
        case nl::node::type::audio:
            return "Audio";
        case nl::node::type::bitmap:
            return "Bitmap";
        case nl::node::type::real:
            return "Float";
        case nl::node::type::integer:
            return "Int";
        case nl::node::type::string:
            return "String";
        case nl::node::type::vector:
            return "Vector";
        case nl::node::type::none:
            return QString();
        }

        return QString("?????");
    }

    QString valueStr()
    {
        switch (node.data_type())
        {
        case nl::node::type::real:
        case nl::node::type::integer:
        case nl::node::type::string:
            return QString::fromStdString(node.get_string());
        case nl::node::type::vector:
            return QString::number(node.get_vector().first) + ", " + QString::number(node.get_vector().second);
        default:
            return QString();
        }

        return QString();
    }

    nl::node node;
    bool childrenAdded = false;
};

#endif // NODEITEM_H
