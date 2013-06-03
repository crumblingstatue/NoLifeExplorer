#ifndef SOUNDITEM_H
#define SOUNDITEM_H

#include <QStandardItem>

struct SoundItem : public QStandardItem {
    SoundItem(const QString& text) :
        QStandardItem(text) {}
    u_int32_t length;
    const void* data;
};

#endif // SOUNDITEM_H
