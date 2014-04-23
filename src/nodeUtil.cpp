#include "nodeUtil.hpp"
#include "nx/bitmap.hpp"

QString nodeTypeAsString(nl::node node) {
    switch (node.data_type()) {
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

QString nodeValueAsString(nl::node node) {
    switch (node.data_type()) {
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

const void *getBitmapData(nl::node n) {
    if (n["source"]) {
        std::string src = n["source"];
        n = n.root().resolve(src.substr(src.find_first_of('/') + 1));
    }

    return n.get_bitmap().data();
}

QStringList findNodes(nl::node root, QString name, QString path) {
    QStringList results;
    for (auto n : root) {
        if (QString::fromStdString(n.name()).contains(name)) {
            results.append(path + QString::fromStdString(root.name()) + "/" +
                           QString::fromStdString(n.name()));
        }
        results += findNodes(n, name,
                             path + "/" + QString::fromStdString(root.name()));
    }
    return results;
}
