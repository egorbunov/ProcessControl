#include "ModeListItemDelegate.h"


ModeListItemDelegate::ModeListItemDelegate()
{
}

ModeListItemDelegate::~ModeListItemDelegate()
{
}

QSize ModeListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QIcon icon;
    icon.addFile(QStringLiteral(":/MainWindow/Resources/decor1.ico"), QSize(), QIcon::Normal, QIcon::Off);
    QSize iconSize = icon.actualSize(option.decorationSize);
    QFont font = QApplication::font();
    QFontMetrics fm(font);
    return QSize(iconSize.width(), iconSize.height() + fm.height() + 15);
}

void ModeListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    painter->save();

    QFont modeNameFont = QApplication::font();
    modeNameFont.setBold(true);
    QFont progressFont = QApplication::font();
    progressFont.setWeight(progressFont.weight() - 2);

    QFontMetrics fm(modeNameFont);

    QIcon icon;
    icon.addFile(QStringLiteral(":/MainWindow/Resources/decor1.ico"), QSize(), QIcon::Normal, QIcon::Off);
    
    QString progressText = qvariant_cast<QString>(index.data(progressRole));
    

    QSize iconSize = icon.actualSize(option.decorationSize);

    QRect modeNameRect = option.rect;
    QRect descriptRect = option.rect;
    QRect iconRect = option.rect;
    QRect progressRect = option.rect;

    progressRect.setLeft(progressRect.right() - 60);
    progressRect.setTop(progressRect.bottom() - progressRect.height()/2 - 7);
    

    iconRect.setRight(iconSize.width() * 2.5);
    iconRect.setTop(iconRect.top());
    modeNameRect.setLeft(iconRect.right());
    descriptRect.setLeft(iconRect.right());
    modeNameRect.setTop(modeNameRect.top() + 5);
    modeNameRect.setBottom(modeNameRect.top() + fm.height());
    descriptRect.setTop(modeNameRect.bottom() - 5);

    painter->drawPixmap(QPoint(iconRect.left() + iconSize.width() / 2 + 2, 
        iconRect.top() + iconSize.height() + 3), 
        icon.pixmap(iconSize.width(), iconSize.height()));

    painter->setFont(modeNameFont);

    int len = option.rect.right() - option.rect.left();
    QString headerText = qvariant_cast<QString>(index.data(modeNameRole));
    QString nameCutText = headerText.left((len - 120) / fm.width('c'));
    if (nameCutText.length() != headerText.length())
        nameCutText = nameCutText + "...";
    painter->drawText(modeNameRect, nameCutText);

    painter->setFont(progressFont);
    QString descriptionText = qvariant_cast<QString>(index.data(descriptionRole));
    QString descrCutText = descriptionText.left((len - 60) / fm.width('c'));
    if (descrCutText.length() != descriptionText.length())
        descrCutText = descrCutText + "...";
    painter->drawText(descriptRect.left(), descriptRect.top() + 17, descrCutText);

    QPen pen;  // creates a default pen
    pen.setBrush(QColor(0, 155, 0));
    painter->setPen(pen);
    
    painter->drawText(progressRect, progressText);
    

    painter->restore();
}


