#ifndef MODE_CREATION_DIALOG_H_INCLUDED__
#define  MODE_CREATION_DIALOG_H_INCLUDED__

#include <qdialog.h>
#include <QtGui/QStandardItemModel>
#include "ui_modecreationdialog.h"
#include <string>
#include <vector>
#include <QFileDialog>
#include <QMessageBox>
#include "urleditdialog.h"

using std::vector;
using std::string;

class ModeCreationDialog : public QDialog
{
    Q_OBJECT
public:
    void setup();
    ModeCreationDialog(QWidget *parent);
    ModeCreationDialog(string &name,
        string &description,
        vector<string> &procToControl,
        vector<string> &urlToControl,
        short &progress,
        QWidget *parent);
    ~ModeCreationDialog();

public slots:
    virtual void accept();
    void setProgressCheked(bool c);
    void deleteActiveItem();
    void addNewRestriction();
    void addUrlRestriction(string url);
    void addProgramRestriction();
    void openUrlEditDlg();
    void closeUrlEditDlg();

private:
    Ui::ModeCreationDialog ui;
    QStandardItemModel *urlList;
    QStandardItemModel *progList;
    string oldName;
    UrlEditDialog *urlEditDlg;



signals:
    void modeAccepted(string &name,
        string &description,
        vector<string> &procToControl,
        vector<string> &urlToControl,
        short &progress);
    void modeEditAccepted(string &oldname,
        string &name,
        string &description,
        vector<string> &procToControl,
        vector<string> &urlToControl,
        short &progress);
};

#endif