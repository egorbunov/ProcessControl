/********************************************************************************
** Form generated from reading UI file 'modecreationdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODECREATIONDIALOG_H
#define UI_MODECREATIONDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModeCreationDialog
{
public:
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *lblModeName;
    QLineEdit *editModeName;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QLineEdit *editDescription;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnAddRestriction;
    QPushButton *btnDeleteRestriction;
    QSpacerItem *horizontalSpacer_2;
    QTabWidget *tabWidget;
    QWidget *restrSites;
    QGridLayout *gridLayout_4;
    QListView *sitesListView;
    QWidget *restrPrograms;
    QGridLayout *gridLayout_5;
    QListView *programsListView;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *progressCheckBox;
    QVBoxLayout *verticalLayout;
    QProgressBar *progressBar;
    QSlider *progressBarSlider;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ModeCreationDialog)
    {
        if (ModeCreationDialog->objectName().isEmpty())
            ModeCreationDialog->setObjectName(QStringLiteral("ModeCreationDialog"));
        ModeCreationDialog->resize(400, 458);
        ModeCreationDialog->setStyleSheet(QStringLiteral(""));
        ModeCreationDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_3 = new QGridLayout(ModeCreationDialog);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        lblModeName = new QLabel(ModeCreationDialog);
        lblModeName->setObjectName(QStringLiteral("lblModeName"));

        horizontalLayout->addWidget(lblModeName);

        editModeName = new QLineEdit(ModeCreationDialog);
        editModeName->setObjectName(QStringLiteral("editModeName"));

        horizontalLayout->addWidget(editModeName);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(ModeCreationDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        editDescription = new QLineEdit(ModeCreationDialog);
        editDescription->setObjectName(QStringLiteral("editDescription"));

        horizontalLayout_4->addWidget(editDescription);


        verticalLayout_2->addLayout(horizontalLayout_4);


        gridLayout_3->addLayout(verticalLayout_2, 0, 0, 1, 1);

        groupBox = new QGroupBox(ModeCreationDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinAndMaxSize);
        horizontalLayout_3->setContentsMargins(0, 5, 0, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        btnAddRestriction = new QPushButton(groupBox);
        btnAddRestriction->setObjectName(QStringLiteral("btnAddRestriction"));
        btnAddRestriction->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnAddRestriction->sizePolicy().hasHeightForWidth());
        btnAddRestriction->setSizePolicy(sizePolicy);
        btnAddRestriction->setMinimumSize(QSize(62, 25));
        QIcon icon;
        icon.addFile(QStringLiteral(":/ModeCreationDialog/Resources/add.ico"), QSize(), QIcon::Normal, QIcon::Off);
        btnAddRestriction->setIcon(icon);

        horizontalLayout_3->addWidget(btnAddRestriction);

        btnDeleteRestriction = new QPushButton(groupBox);
        btnDeleteRestriction->setObjectName(QStringLiteral("btnDeleteRestriction"));
        sizePolicy.setHeightForWidth(btnDeleteRestriction->sizePolicy().hasHeightForWidth());
        btnDeleteRestriction->setSizePolicy(sizePolicy);
        btnDeleteRestriction->setMinimumSize(QSize(65, 25));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/ModeCreationDialog/Resources/delete.ico"), QSize(), QIcon::Normal, QIcon::Off);
        btnDeleteRestriction->setIcon(icon1);

        horizontalLayout_3->addWidget(btnDeleteRestriction);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 2);
        horizontalLayout_3->setStretch(2, 2);
        horizontalLayout_3->setStretch(3, 1);

        gridLayout->addLayout(horizontalLayout_3, 1, 0, 1, 1);

        tabWidget = new QTabWidget(groupBox);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        restrSites = new QWidget();
        restrSites->setObjectName(QStringLiteral("restrSites"));
        gridLayout_4 = new QGridLayout(restrSites);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        sitesListView = new QListView(restrSites);
        sitesListView->setObjectName(QStringLiteral("sitesListView"));

        gridLayout_4->addWidget(sitesListView, 0, 0, 1, 1);

        tabWidget->addTab(restrSites, QString());
        restrPrograms = new QWidget();
        restrPrograms->setObjectName(QStringLiteral("restrPrograms"));
        gridLayout_5 = new QGridLayout(restrPrograms);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        programsListView = new QListView(restrPrograms);
        programsListView->setObjectName(QStringLiteral("programsListView"));

        gridLayout_5->addWidget(programsListView, 0, 0, 1, 1);

        tabWidget->addTab(restrPrograms, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(ModeCreationDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        progressCheckBox = new QCheckBox(groupBox_2);
        progressCheckBox->setObjectName(QStringLiteral("progressCheckBox"));

        horizontalLayout_2->addWidget(progressCheckBox);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        progressBar = new QProgressBar(groupBox_2);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(progressBar->sizePolicy().hasHeightForWidth());
        progressBar->setSizePolicy(sizePolicy1);
        progressBar->setValue(0);
        progressBar->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(progressBar);

        progressBarSlider = new QSlider(groupBox_2);
        progressBarSlider->setObjectName(QStringLiteral("progressBarSlider"));
        progressBarSlider->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(progressBarSlider->sizePolicy().hasHeightForWidth());
        progressBarSlider->setSizePolicy(sizePolicy2);
        progressBarSlider->setMaximum(100);
        progressBarSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(progressBarSlider);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ModeCreationDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setContextMenuPolicy(Qt::DefaultContextMenu);
        buttonBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        buttonBox->setInputMethodHints(Qt::ImhNone);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        gridLayout_3->addWidget(buttonBox, 3, 0, 1, 1);


        retranslateUi(ModeCreationDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ModeCreationDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ModeCreationDialog, SLOT(reject()));
        QObject::connect(progressBarSlider, SIGNAL(valueChanged(int)), progressBar, SLOT(setValue(int)));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ModeCreationDialog);
    } // setupUi

    void retranslateUi(QDialog *ModeCreationDialog)
    {
        ModeCreationDialog->setWindowTitle(QApplication::translate("ModeCreationDialog", "Mode creation dialog", 0));
        lblModeName->setText(QApplication::translate("ModeCreationDialog", "Name:", 0));
        label->setText(QApplication::translate("ModeCreationDialog", "Description:", 0));
        groupBox->setTitle(QApplication::translate("ModeCreationDialog", "Restrictions", 0));
        btnAddRestriction->setText(QApplication::translate("ModeCreationDialog", "Add", 0));
        btnDeleteRestriction->setText(QApplication::translate("ModeCreationDialog", "Delete", 0));
        tabWidget->setTabText(tabWidget->indexOf(restrSites), QApplication::translate("ModeCreationDialog", "Sites", 0));
        tabWidget->setTabText(tabWidget->indexOf(restrPrograms), QApplication::translate("ModeCreationDialog", "Programms", 0));
        groupBox_2->setTitle(QApplication::translate("ModeCreationDialog", "Progress", 0));
        progressCheckBox->setText(QApplication::translate("ModeCreationDialog", "Add", 0));
    } // retranslateUi

};

namespace Ui {
    class ModeCreationDialog: public Ui_ModeCreationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODECREATIONDIALOG_H
