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
    Mode mode;
    mode.setName(ui.editModeName->text().toStdString());
    mode.setDescription(ui.editDescription->text().toStdString());
    mode.setProgress(ui.progressCheckBox->isChecked() ? ui.progressBar->value() : -1);

    vector<string> progToControl;
    vector<string> urlsToControl;

    for (int i = 0; i < progList->rowCount(); ++i)
        mode.addProcess(progList->item(i)->data(Qt::DisplayRole).toString().toStdString());

    for (int i = 0; i < urlList->rowCount(); ++i)
        mode.addUrl(urlList->item(i)->data(Qt::DisplayRole).toString().toStdString());

    if (oldName == "")
        emit modeAccepted(mode);
    else
        emit modeEditAccepted(oldName, mode);
}

ModeCreationDialog::ModeCreationDialog(Mode &mode, QWidget *parent) : QDialog(parent)
{
    oldName = mode.getName();
    ui.setupUi(this);
    this->setup();

    ui.editModeName->setText(QString(mode.getName().c_str()));
    ui.editDescription->setText(QString(mode.getDescription().c_str()));

    QStandardItem *item;
    const vector<ProcessDesc> &procToControl = mode.getProcesses();
    for (int i = 0; i < procToControl.size(); ++i)
    {
        item = new QStandardItem();
        item->setData(procToControl[i].getPath().c_str(), Qt::DisplayRole);
        item->setEditable(false);
        progList->appendRow(item);
    }

    const vector<string> &urlToControl = mode.getUrls();
    for (int i = 0; i < urlToControl.size(); ++i)
    {
        item = new QStandardItem();
        item->setData(urlToControl[i].c_str(), Qt::DisplayRole);
        item->setEditable(false);
        urlList->appendRow(item);
    }

    if (mode.getProgress() != -1)
    {
        this->setProgressCheked(true);
        ui.progressBarSlider->setValue(mode.getProgress());
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