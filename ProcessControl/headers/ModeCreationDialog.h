#ifndef MODE_CREATION_DIALOG_H_INCLUDED__
#define MODE_CREATION_DIALOG_H_INCLUDED__

#include <qdialog.h>
#include <QtGui/QStandardItemModel>
#include <string>
#include <vector>
#include <QFileDialog>
#include <QMessageBox>

#include "urleditdialog.h"
#include "ui_modecreationdialog.h"

#include "mode.h"


using std::vector;
using std::string;

class ModeCreationDialog : public QDialog
{
    Q_OBJECT
public:
    void setup();
    ModeCreationDialog(QWidget *parent);
    ModeCreationDialog(Mode &mode, QWidget *parent);
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
    void modeAccepted(Mode &mode);
    void modeEditAccepted(string &oldname, Mode &mode);
};

#endif