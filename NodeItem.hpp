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
            return "audio";
        case nl::node::type::bitmap:
            return "bitmap";
        case nl::node::type::real:
            return "real";
        case nl::node::type::integer:
            return "integer";
        case nl::node::type::string:
            return "string";
        case nl::node::type::vector:
            return "vector";
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
        case nl::node::type::vector: {
            const auto vec = node.get_vector();
            return QString("{%1, %2}").arg(vec.first).arg(vec.second);
        }
        default:
            return QString();
        }

        return QString();
    }

    nl::node node;
    bool childrenAdded = false;
};

#endif // NODEITEM_HPP
