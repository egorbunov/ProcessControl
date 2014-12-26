#ifndef URL_EDIT_DIALOG_H_INCLUDED__
#define URL_EDIT_DIALOG_H_INCLUDED__

#include <QtWidgets/QDialog>
#include "ui_urleditdialog.h"
#include <string>

using std::string;

class UrlEditDialog : public QDialog
{
    Q_OBJECT
public:
    UrlEditDialog(QWidget *parent);
    ~UrlEditDialog();

public slots:
    virtual void accept();

signals:
    void urlAccepted(string s);

private:
    Ui::UrlEditDialog ui;
    bool isUrl(string s);
};

#endif
