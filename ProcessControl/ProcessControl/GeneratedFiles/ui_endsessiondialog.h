/********************************************************************************
** Form generated from reading UI file 'endsessiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENDSESSIONDIALOG_H
#define UI_ENDSESSIONDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EndSessionDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *editName;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QVBoxLayout *verticalLayout;
    QProgressBar *progressBar;
    QSlider *sliderProgress;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EndSessionDialog)
    {
        if (EndSessionDialog->objectName().isEmpty())
            EndSessionDialog->setObjectName(QStringLiteral("EndSessionDialog"));
        EndSessionDialog->resize(437, 193);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(EndSessionDialog->sizePolicy().hasHeightForWidth());
        EndSessionDialog->setSizePolicy(sizePolicy);
        EndSessionDialog->setMinimumSize(QSize(437, 193));
        EndSessionDialog->setMaximumSize(QSize(437, 193));
        EndSessionDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout = new QGridLayout(EndSessionDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(EndSessionDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        editName = new QLineEdit(EndSessionDialog);
        editName->setObjectName(QStringLiteral("editName"));
        editName->setReadOnly(true);

        horizontalLayout->addWidget(editName);


        verticalLayout_2->addLayout(horizontalLayout);

        groupBox = new QGroupBox(EndSessionDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        progressBar = new QProgressBar(groupBox);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        sliderProgress = new QSlider(groupBox);
        sliderProgress->setObjectName(QStringLiteral("sliderProgress"));
        sliderProgress->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(sliderProgress);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(EndSessionDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);

        verticalLayout_2->addWidget(buttonBox);

        verticalLayout_2->setStretch(1, 1);
        verticalLayout_2->setStretch(2, 1);

        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        retranslateUi(EndSessionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), EndSessionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EndSessionDialog, SLOT(reject()));
        QObject::connect(progressBar, SIGNAL(valueChanged(int)), sliderProgress, SLOT(setValue(int)));
        QObject::connect(sliderProgress, SIGNAL(valueChanged(int)), progressBar, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(EndSessionDialog);
    } // setupUi

    void retranslateUi(QDialog *EndSessionDialog)
    {
        EndSessionDialog->setWindowTitle(QApplication::translate("EndSessionDialog", "End of the session", 0));
        label->setText(QApplication::translate("EndSessionDialog", "End of the session in mode:", 0));
        editName->setText(QString());
        groupBox->setTitle(QApplication::translate("EndSessionDialog", "Progress", 0));
        label_2->setText(QApplication::translate("EndSessionDialog", "Set new progress:", 0));
    } // retranslateUi

};

namespace Ui {
    class EndSessionDialog: public Ui_EndSessionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENDSESSIONDIALOG_H
