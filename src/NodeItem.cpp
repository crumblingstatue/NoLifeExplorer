#include "NodeItem.hpp"
#include "nodeUtil.hpp"

NodeItem::NodeItem(const nl::node &n) : node(n) {
    setText(0, QString::fromStdString(node.name()));
    setText(1, nodeUtil::nodeTypeAsString(node));
    setText(2, nodeUtil::nodeValueAsString(node));
}
