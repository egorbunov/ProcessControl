/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionAbout;
    QAction *actionDocumentation;
    QAction *actionNewMode;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *listGroupBox;
    QGridLayout *gridLayout_4;
    QListView *modeListView;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QTimeEdit *timeEdit;
    QCheckBox *checkTillDown;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnChoose;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QMenuBar *menuBar;
    QMenu *menuHelp;
    QStatusBar *statusBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(400, 450);
        MainWindowClass->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        actionAbout = new QAction(MainWindowClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainWindow/Resources/about.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon);
        actionDocumentation = new QAction(MainWindowClass);
        actionDocumentation->setObjectName(QStringLiteral("actionDocumentation"));
        actionNewMode = new QAction(MainWindowClass);
        actionNewMode->setObjectName(QStringLiteral("actionNewMode"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/MainWindow/Resources/createMode.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionNewMode->setIcon(icon1);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setStyleSheet(QStringLiteral(""));
        centralWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(16);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        listGroupBox = new QGroupBox(centralWidget);
        listGroupBox->setObjectName(QStringLiteral("listGroupBox"));
        listGroupBox->setEnabled(true);
        gridLayout_4 = new QGridLayout(listGroupBox);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        modeListView = new QListView(listGroupBox);
        modeListView->setObjectName(QStringLiteral("modeListView"));

        gridLayout_4->addWidget(modeListView, 0, 0, 1, 1);


        verticalLayout->addWidget(listGroupBox);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        timeEdit = new QTimeEdit(groupBox);
        timeEdit->setObjectName(QStringLiteral("timeEdit"));

        gridLayout_2->addWidget(timeEdit, 0, 1, 1, 1);

        checkTillDown = new QCheckBox(groupBox);
        checkTillDown->setObjectName(QStringLiteral("checkTillDown"));

        gridLayout_2->addWidget(checkTillDown, 0, 0, 1, 1);

        gridLayout_2->setColumnStretch(1, 1);

        verticalLayout->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        btnChoose = new QPushButton(centralWidget);
        btnChoose->setObjectName(QStringLiteral("btnChoose"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnChoose->sizePolicy().hasHeightForWidth());
        btnChoose->setSizePolicy(sizePolicy1);
        btnChoose->setLayoutDirection(Qt::LeftToRight);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/MainWindow/Resources/btnChoose.ico"), QSize(), QIcon::Normal, QIcon::Off);
        btnChoose->setIcon(icon2);

        horizontalLayout->addWidget(btnChoose);

        btnEdit = new QPushButton(centralWidget);
        btnEdit->setObjectName(QStringLiteral("btnEdit"));
        sizePolicy1.setHeightForWidth(btnEdit->sizePolicy().hasHeightForWidth());
        btnEdit->setSizePolicy(sizePolicy1);
        btnEdit->setBaseSize(QSize(100, 111));
        btnEdit->setAutoFillBackground(false);
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/MainWindow/Resources/btnEdit.ico"), QSize(), QIcon::Normal, QIcon::Off);
        btnEdit->setIcon(icon3);

        horizontalLayout->addWidget(btnEdit);

        btnDelete = new QPushButton(centralWidget);
        btnDelete->setObjectName(QStringLiteral("btnDelete"));
        sizePolicy1.setHeightForWidth(btnDelete->sizePolicy().hasHeightForWidth());
        btnDelete->setSizePolicy(sizePolicy1);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/MainWindow/Resources/delete.ico"), QSize(), QIcon::Normal, QIcon::Off);
        btnDelete->setIcon(icon4);

        horizontalLayout->addWidget(btnDelete);


        verticalLayout->addLayout(horizontalLayout);

        verticalLayout->setStretch(0, 1);
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 21));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindowClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuHelp->menuAction());
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionDocumentation);
        mainToolBar->addAction(actionNewMode);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Process Control", 0));
        actionAbout->setText(QApplication::translate("MainWindowClass", "About", 0));
        actionDocumentation->setText(QApplication::translate("MainWindowClass", "Documentation", 0));
        actionNewMode->setText(QApplication::translate("MainWindowClass", "NewMode", 0));
#ifndef QT_NO_TOOLTIP
        actionNewMode->setToolTip(QApplication::translate("MainWindowClass", "Create new mode", 0));
#endif // QT_NO_TOOLTIP
        listGroupBox->setTitle(QApplication::translate("MainWindowClass", "Mode List", 0));
        groupBox->setTitle(QApplication::translate("MainWindowClass", "Time Period", 0));
        checkTillDown->setText(QApplication::translate("MainWindowClass", "Till shutdown", 0));
        btnChoose->setText(QApplication::translate("MainWindowClass", "Choose", 0));
        btnEdit->setText(QApplication::translate("MainWindowClass", "Edit / Info ", 0));
        btnDelete->setText(QApplication::translate("MainWindowClass", "Delete", 0));
        menuHelp->setTitle(QApplication::translate("MainWindowClass", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
