#ifndef END_SESSION_DIALOG_H_INCLUDED__
#define END_SESSION_DIALOG_H_INCLUDED__

#include <QtWidgets/QDialog>
#include "ui_endsessiondialog.h"
#include <string>

using std::string;

class EndSessionDialog : public QDialog
{
    Q_OBJECT
public:
    EndSessionDialog(string& name, int progress, QWidget *parent);
    ~EndSessionDialog();
public slots:
    virtual void accept();
private:
    Ui::EndSessionDialog ui;
    string modeName;
    int sProgress;
signals:
    void progressChanged(string&, int);
};

#endif