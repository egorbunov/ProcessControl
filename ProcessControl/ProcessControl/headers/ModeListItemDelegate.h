#ifndef MODE_LIST_ITEM_DELEGATE_H_INCLUDED__
#define MODE_LIST_ITEM_DELEGATE_H_INCLUDED__

#include <QtWidgets/QStyledItemDelegate>
#include <QApplication>
#include <QtGui/QPainter>

class ModeListItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum datarole {
        modeNameRole = Qt::UserRole + 100,
        progressRole,
        iconeRole,
        descriptionRole
    };

    ModeListItemDelegate();
    ~ModeListItemDelegate();
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif

