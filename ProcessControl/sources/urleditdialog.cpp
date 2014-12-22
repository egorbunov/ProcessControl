#include "urleditdialog.h"


UrlEditDialog::UrlEditDialog(QWidget *parent)
{
    ui.setupUi(this);
}

void UrlEditDialog::accept()
{
    string url = ui.editUrl->text().toStdString();

    emit urlAccepted(url);
}

UrlEditDialog::~UrlEditDialog()
{
}

bool UrlEditDialog::isUrl(string s)
{
    return true;
}