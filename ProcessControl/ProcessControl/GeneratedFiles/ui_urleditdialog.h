/********************************************************************************
** Form generated from reading UI file 'urleditdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_URLEDITDIALOG_H
#define UI_URLEDITDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_UrlEditDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *editUrl;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UrlEditDialog)
    {
        if (UrlEditDialog->objectName().isEmpty())
            UrlEditDialog->setObjectName(QStringLiteral("UrlEditDialog"));
        UrlEditDialog->resize(394, 69);
        UrlEditDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout = new QGridLayout(UrlEditDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(UrlEditDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        editUrl = new QLineEdit(UrlEditDialog);
        editUrl->setObjectName(QStringLiteral("editUrl"));

        horizontalLayout->addWidget(editUrl);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(UrlEditDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(UrlEditDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UrlEditDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UrlEditDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UrlEditDialog);
    } // setupUi

    void retranslateUi(QDialog *UrlEditDialog)
    {
        UrlEditDialog->setWindowTitle(QApplication::translate("UrlEditDialog", "URL input", 0));
        label->setText(QApplication::translate("UrlEditDialog", "www.", 0));
    } // retranslateUi

};

namespace Ui {
    class UrlEditDialog: public Ui_UrlEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_URLEDITDIALOG_H
