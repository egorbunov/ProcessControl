#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    createTrayIcon();

    QIcon icon;
    icon.addFile(QStringLiteral(":/MainWindow/Resources/mainTray.ico"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
    modeCrDlg = NULL;

    ModeListItemDelegate *listDelegate = new ModeListItemDelegate();
    ui.modeListView->setItemDelegate(listDelegate);

    modeModel = new QStandardItemModel();
    ui.modeListView->setModel(modeModel);


    fillModeModel();

    connect(ui.actionNewMode, SIGNAL(triggered()), this, SLOT(openModeCreationDialog()));
    connect(ui.btnChoose, SIGNAL(clicked()), this, SLOT(collapseToTray()));
    connect(ui.btnEdit, SIGNAL(clicked()), this, SLOT(openModeCreationDialogEdit()));
    connect(ui.btnDelete, SIGNAL(clicked()), this, SLOT(deleteSelectedMode()));
    connect(ui.checkTillDown, SIGNAL(clicked(bool)), this, SLOT(setTillShutdownChecked(bool)));

    sessionTimer = new QTimer(this);
    controlTimer = new QTimer(this);
    refreshTimer = new QTimer(this);
    startTime = new QTime();
}

int MainWindow::fillModeModel()
{
    modeModel->clear();
    vector<string> names;
    vector<string> descriptions;
    vector<short> progress;

    processController.getModsNames(names);
    processController.getModsDscriprions(descriptions);
    processController.getModsProgress(progress);

    _ASSERT(names.size() == descriptions.size());
    _ASSERT(names.size() == progress.size());

    QStandardItem *item;
    char sprogress[20];
    for (int i = 0; i < names.size(); ++i)
    {
        item = new QStandardItem();
        item->setData(names[i].c_str(), ModeListItemDelegate::modeNameRole);
        item->setData(descriptions[i].c_str(), ModeListItemDelegate::descriptionRole);
        if (progress[i] < 0) {
            item->setData("no progress", ModeListItemDelegate::progressRole);
        }
        else
        {
            itoa(progress[i], sprogress, 10);
            strcat(sprogress, " / 100 %");
            item->setData(sprogress, ModeListItemDelegate::progressRole);
        }
        item->setEditable(false);
        modeModel->appendRow(item);
    }
    
    return 0;
}

MainWindow::~MainWindow()
{
    delete trayIcon;
    delete sessionTimer;
    delete controlTimer;
    delete modeCrDlg;
    delete modeModel;
    delete endSessionDlg;
    delete trayIconMenu;
    delete startTime;
}

void MainWindow::openModeCreationDialog()
{
    if (modeCrDlg != NULL) {
        modeCrDlg->reject();
    }
    modeCrDlg = new ModeCreationDialog(this);
    connect(modeCrDlg,
        SIGNAL(modeAccepted(string&, string&, vector<string>&, vector<string>&, short&)),
        this,
        SLOT(addNewMode(string&, string&, vector<string>&, vector<string>&, short&)));
    connect(modeCrDlg, SIGNAL(rejected()), this, SLOT(closeModeCreationDialog()));
    modeCrDlg->exec();
}

void MainWindow::openModeCreationDialogEdit()
{
    QModelIndexList list = ui.modeListView->selectionModel()->selectedIndexes();

    if (list.length() == 0) {
        QMessageBox::about(this, tr("Error"),
            tr("No mode chosen!"));
        return;
    }

    QModelIndex index = list.at(0);
    
    string name;
    string description;
    vector<string> procToControl;
    vector<string> urlToControl;
    short progress;

    name = index.data(ModeListItemDelegate::modeNameRole).toString().toStdString();
    if (processController.getModeInfo(name, description, procToControl, urlToControl, progress))
        return;

    if (modeCrDlg != NULL) {
        modeCrDlg->reject();
    }
    modeCrDlg = new ModeCreationDialog(name, description, procToControl, urlToControl, progress, this);
    connect(modeCrDlg, 
        SIGNAL(modeEditAccepted(string&, string&, string&, vector<string>&, vector<string>&, short&)), 
        this, 
        SLOT(editMode(string&, string&, string&, vector<string>&, vector<string>&, short&)));
    connect(modeCrDlg, SIGNAL(rejected()), this, SLOT(closeModeCreationDialog()));
    modeCrDlg->exec();
}

void MainWindow::setTillShutdownChecked(bool c)
{
    if (c)
        ui.timeEdit->setEnabled(false);
    else
        ui.timeEdit->setEnabled(true);
}
void MainWindow::collapseToTray()
{
    // getting name of the active mode to start session
    QModelIndexList list = ui.modeListView->selectionModel()->selectedIndexes();

    if (list.length() == 0) {
        QMessageBox::about(this, tr("Error"),
            tr("No mode chosen!"));
        return;
    }
    QModelIndex index = list.at(0);
    string name = index.data(ModeListItemDelegate::modeNameRole).toString().toStdString();

    // setting current session mode
    this->processController.setSessionMode(name);

    // starting timer for control stabs - every CONTOL_STAB ms it will run function
    // for stopping restricted programs and injecting dll with restriction into browsers
    connect(controlTimer, SIGNAL(timeout()), this, SLOT(controlStab()));
    controlTimer->start(CONTROL_STAB);

    // getting time of session stand
    int sessionStand;

    if (ui.checkTillDown->isChecked()) {
        sessionStand = INT_MAX;
        // tray icon refresh tooltip
        trayIcon->setToolTip("Process Control\nMode: " + QString(name.c_str()) + "\nTime left: till shutdown");
    }
    else
    {
        sessionStand = ui.timeEdit->time().msec() +
            ui.timeEdit->time().second() * MS_IN_S +
            ui.timeEdit->time().minute() * SEC_IN_MIN * MS_IN_S +
            ui.timeEdit->time().hour() * MIN_IN_HOUR * SEC_IN_MIN * MS_IN_S;
        //timer for tooltip refreshing
        connect(refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTrayIconTooltip()));
        refreshTimer->start(REFRESH_TRAY_STAB);
    }



    QMessageBox::about(this, tr("Process control"),
        tr("The program will keep running in the "
        "system tray. Reboot to prematurely terminate program."));
    this->hide();

    // starting end session timer
    connect(sessionTimer, SIGNAL(timeout()), this, SLOT(endSession()));
    sessionTimer->setSingleShot(true);
    sessionTimer->start(sessionStand);
    startTime->restart();

    trayIcon->setVisible(true);
    trayIcon->show();
}

void MainWindow::closeModeCreationDialog()
{
    delete modeCrDlg;
    modeCrDlg = NULL;
}

void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon;
    icon.addFile(QStringLiteral(":/MainWindow/Resources/mainTray.ico"), QSize(), QIcon::Normal, QIcon::Off);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Process Control\nMode: work\nTime left: 01:30");
    trayIcon->setVisible(false);
}

int MainWindow::addNewMode(string &name, string &description, vector<string> &procToControl, vector<string> &urlToControl, short &progress)
{
    int f = processController.addNewMode(name, description, procToControl, urlToControl, progress);
    switch (f)
    {
    case BAD_NAME_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Bad mode name!"));
        break;
    case EMPTY_LISTS_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Link or program lists must be filled!"));
        break;
    case MODE_ALREADY_EXISTS:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Mode with that name is already exist!"));
        break;
    case TOO_BIG_DESCRIPTION_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Too long description!"));
        break;
    case 0:
        //closing dialog
        emit modeCrDlg->rejected();
        this->fillModeModel();
    }

    return 0;
}

int MainWindow::editMode(string &oldName, string &newName, string &newDescr, vector<string> &newProcs, vector<string> &newUrls, short &newProgress)
{
    int f = processController.editMode(oldName, newName, newDescr, newProcs, newUrls, newProgress);
    switch (f)
    {
    case BAD_NAME_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Bad mode name!"));
        break;
    case EMPTY_LISTS_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Link or program lists must be filled!"));
        break;
    case MODE_ALREADY_EXISTS:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Mode with that name is already exist!"));
        break;
    case TOO_BIG_DESCRIPTION_ERROR:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("Too long description!"));
        break;
    case NO_MODE_TO_EDIT:
        QMessageBox::about(modeCrDlg, tr("Error"),
            tr("No mode to edit!"));
        break;
    case 0:
        //closing dialog
        emit modeCrDlg->rejected();
        this->fillModeModel();
    }
    return 0;
}

int MainWindow::deleteSelectedMode()
{
    QModelIndexList list = ui.modeListView->selectionModel()->selectedIndexes();

    if (list.length() == 0) {
        QMessageBox::about(this, tr("Error"),
            tr("No mode chosen!"));
        return 0;
    }

    QModelIndex index = list.at(0);
    string name;

    name = index.data(ModeListItemDelegate::modeNameRole).toString().toStdString();

    processController.deleteMode(name);
    this->fillModeModel();
    return 0;
}

int MainWindow::controlStab()
{
    this->processController.control();
    return 0;
}

int MainWindow::endSession()
{
    this->show();
    trayIcon->setVisible(false);
    trayIcon->hide();

    sessionTimer->stop();
    controlTimer->stop();
    refreshTimer->stop();
    disconnect(controlTimer, SIGNAL(timeout()), this, SLOT(controlStab()));
    disconnect(refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTrayIconTooltip()));
    disconnect(sessionTimer, SIGNAL(timeout()), this, SLOT(endSession()));

    string name = this->processController.getActiveModeName();
    int progress = this->processController.getModeProgress(name);
    this->processController.setSessionMode(name);
    endSessionDlg = new EndSessionDialog(name, progress, this);
    connect(endSessionDlg, SIGNAL(progressChanged(string&, int)), this, SLOT(editProgress(string&, int)));
    endSessionDlg->exec();
    delete endSessionDlg;
    endSessionDlg = NULL;
    this->fillModeModel();

    return 0;
}

void MainWindow::refreshTrayIconTooltip()
{
    int remsec = (sessionTimer->interval() - startTime->elapsed()) / MS_IN_S;
    int remh = (remsec / (SEC_IN_MIN * MIN_IN_HOUR));
    int remmin = remsec / SEC_IN_MIN - remh * MIN_IN_HOUR; 
    remsec = remsec % SEC_IN_MIN;
    QString timeLeft = QString::number(remh) + ":" + QString::number(remmin) + ":" + QString::number(remsec);
    QString mode = QString(processController.getActiveModeName().c_str());
    trayIcon->setToolTip("Process Control\nMode: " + mode + "\nTime left: " + timeLeft);
}

int MainWindow::editProgress(string& name, int newProgress)
{
    return this->processController.editProgress(name, newProgress);
}