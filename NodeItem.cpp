#include "NodeItem.hpp"
#include "nodeUtil.hpp"

namespace {

}

NodeItem::NodeItem(const nl::node& n) :
    node(n)
{
    setText(0, QString::fromStdString(node.name()));
    setText(1, nodeTypeAsString(node));
    setText(2, nodeValueAsString(node));
}
