#include "EndSessionDialog.h"

EndSessionDialog::EndSessionDialog(string& name, int progress, QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    ui.editName->setText(QString(name.c_str()));
    sProgress = progress;
    modeName = name;
    if (progress < 0) {
        ui.groupBox->hide();
        //ui.sliderProgress->hide();
        //ui.progressBar->hide();
    }
    else
    {
        ui.sliderProgress->setValue(progress);
    }
}

void EndSessionDialog::accept()
{
    if (sProgress == -1)
        emit progressChanged(modeName, -1);
    else
        emit progressChanged(modeName, ui.progressBar->value());

    QDialog::accept();
}

EndSessionDialog::~EndSessionDialog()
{
}
