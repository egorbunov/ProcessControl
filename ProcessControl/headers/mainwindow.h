#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSystemTrayIcon>
#include <QtGui/QStandardItemModel>
#include <QTimer>
#include <climits>

#include <QDebug>

#include "ui_mainwindow.h"
#include "ModeCreationDialog.h"
#include "modelistitemdelegate.h"
#include "endsessiondialog.h"
#include "ProcController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    //model part
private:
    ProcController processController; //main process controller, it sends all data to ui.
    //view part
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openModeCreationDialog();
    void openModeCreationDialogEdit();
    void closeModeCreationDialog();
    void collapseToTray();

    int addNewMode(Mode &mode);

    int editMode(string &oldName, Mode &mode);

    int editProgress(string& name, int newProgress);

    int deleteSelectedMode();
    int controlStab();
    int endSession();
    void refreshTrayIconTooltip();

    void setTillShutdownChecked(bool c);

private:
    Ui::MainWindowClass ui;

    int fillModeModel();

    void createTrayIcon();
    void setTrayIconText();

    /// <summary>
    /// Mode creation dialog pointer.
    /// </summary>
    ModeCreationDialog *modeCrDlg;
    EndSessionDialog *endSessionDlg;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu; //it's useless for now, TODO: do something with that
    QStandardItemModel *modeModel; //model with modes, list

    QTimer *sessionTimer;
    QTimer *controlTimer;
    QTimer *refreshTimer;
    QTime *startTime;
};

#endif // MAINWINDOW_H
