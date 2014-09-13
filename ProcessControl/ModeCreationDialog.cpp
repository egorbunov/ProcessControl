#include "ModeCreationDialog.h"

void ModeCreationDialog::setup()
{
    progList = new QStandardItemModel();
    urlList = new QStandardItemModel();
    ui.sitesListView->setModel(urlList);
    ui.programsListView->setModel(progList);

    connect(ui.btnDeleteRestriction, SIGNAL(clicked()), this, SLOT(deleteActiveItem()));
    connect(ui.btnAddRestriction, SIGNAL(clicked()), this, SLOT(addNewRestriction()));
    connect(ui.progressCheckBox, SIGNAL(clicked(bool)), this, SLOT(setProgressCheked(bool)));
    setProgressCheked(false);

    urlEditDlg = NULL;
}

void ModeCreationDialog::openUrlEditDlg()
{
    if (urlEditDlg != NULL) {
        urlEditDlg->reject();
    }

    urlEditDlg = new UrlEditDialog(this);
    connect(urlEditDlg, SIGNAL(urlAccepted(string)),
        this, SLOT(addUrlRestriction(string)));
    connect(urlEditDlg, SIGNAL(rejected()), this, SLOT(closeUrlEditDlg()));
    urlEditDlg->exec();
}

void ModeCreationDialog::closeUrlEditDlg()
{
    delete urlEditDlg;
    urlEditDlg = NULL;
}

void ModeCreationDialog::deleteActiveItem()
{
    QWidget *curWidget = ui.tabWidget->currentWidget();

    QListView *curList;
    if (ui.restrSites == curWidget)
        curList = ui.sitesListView;

    if (ui.restrPrograms == curWidget)
        curList = ui.programsListView;

    QModelIndexList list = curList->selectionModel()->selectedIndexes();

    if (list.length() == 0)
        return;

    QModelIndex index = list.at(0);
    
    if (curList == ui.programsListView)
        progList->removeRow(index.row());
    if (curList == ui.sitesListView)
        urlList->removeRow(index.row());
        
}

ModeCreationDialog::ModeCreationDialog(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    this->setup();
    oldName = "";
}

void ModeCreationDialog::accept()
{
    string name = ui.editModeName->text().toStdString();
    string descr = ui.editDescription->text().toStdString();
    short progress = -1;
    if (ui.progressCheckBox->isChecked())
        progress = ui.progressBar->value();

    vector<string> progToControl;
    vector<string> urlsToControl;

    QStandardItem *item;
    int row = progList->rowCount();
    for (int i = 0; i < row; ++i)
    {
        item = progList->item(i);
        progToControl.push_back(item->data(Qt::DisplayRole).toString().toStdString());
    }
    row = urlList->rowCount();
    for (int i = 0; i < row; ++i)
    {
        item = urlList->item(i);
        urlsToControl.push_back(item->data(Qt::DisplayRole).toString().toStdString());
    }

    if (oldName == "")
        emit modeAccepted(name, descr, progToControl, urlsToControl, progress);
    else
        emit modeEditAccepted(oldName, name, descr, progToControl, urlsToControl, progress);
}

ModeCreationDialog::ModeCreationDialog(string &name,
    string &description,
    vector<string> &procToControl,
    vector<string> &urlToControl,
    short &progress,
    QWidget *parent) : QDialog(parent)
{
    oldName = name;
    ui.setupUi(this);
    this->setup();

    ui.editModeName->setText(QString(name.c_str()));
    ui.editDescription->setText(QString(description.c_str()));


    QStandardItem *item;
    for (int i = 0; i < procToControl.size(); ++i)
    {
        item = new QStandardItem();
        item->setData(procToControl[i].c_str(), Qt::DisplayRole);
        item->setEditable(false);
        progList->appendRow(item);
    }
    for (int i = 0; i < urlToControl.size(); ++i)
    {
        item = new QStandardItem();
        item->setData(urlToControl[i].c_str(), Qt::DisplayRole);
        item->setEditable(false);
        urlList->appendRow(item);
    }
    if (progress != -1)
    {
        this->setProgressCheked(true);
        ui.progressBarSlider->setValue(progress);
        ui.progressCheckBox->setChecked(true);
    }
}

ModeCreationDialog::~ModeCreationDialog()
{
    delete urlList;
    delete progList;
}

void ModeCreationDialog::setProgressCheked(bool c)
{
    if (c == false)
    {
        //ui.progressBarSlider->setValue(0);
        ui.progressBar->setEnabled(false);
        ui.progressBarSlider->setEnabled(false);
        ui.progressBarSlider->setVisible(false);
        ui.progressBar->setVisible(false);
    }
    else
    {
        //ui.progressBarSlider->setValue(0);
        ui.progressBar->setEnabled(true);
        ui.progressBarSlider->setEnabled(true);
        ui.progressBarSlider->setVisible(true);
        ui.progressBar->setVisible(true);
    }

}
void ModeCreationDialog::addNewRestriction()
{
    QWidget *curWidget = ui.tabWidget->currentWidget();

    if (ui.restrSites == curWidget)
    {
        openUrlEditDlg();
    }

    if (ui.restrPrograms == curWidget)
    {
        addProgramRestriction();
    }
}
void ModeCreationDialog::addProgramRestriction()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.exe)"));
    QString newProgram = "\"" + fileName + "\"";

    QList<QStandardItem* > lst = urlList->findItems(newProgram);
    if (lst.size() != 0)
    {
        QMessageBox::about(urlEditDlg, tr("Error"),
            tr("Program is already in list!"));
        return;
    }

    QStandardItem* item = new QStandardItem();
    item->setData(newProgram, Qt::DisplayRole);
    item->setEditable(false);
    progList->appendRow(item);
}

void ModeCreationDialog::addUrlRestriction(string url)
{
    QList<QStandardItem* > lst = urlList->findItems(QString(url.c_str()));
    if (lst.size() != 0)
    {
        QMessageBox::about(urlEditDlg, tr("Error"),
            tr("Link is already in list!"));
        return;
    }
        
    QStandardItem* item = new QStandardItem();
    item->setData(QString(url.c_str()), Qt::DisplayRole);
    item->setEditable(false);
    urlList->appendRow(item);

    emit urlEditDlg->rejected();
}