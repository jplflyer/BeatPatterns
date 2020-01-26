#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "Song.h"
#include "SetupForm.h"
#include "SongInfoForm.h"
#include "NotesTableForm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    SongEditor::Song currentSong;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Menu Bar
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionClose_triggered();

    // Tool Bar.
    void on_actionSetup_triggered();
    void on_actionSong_Info_triggered();
    void on_actionSong_Map_triggered();

private:
    Ui::MainWindow *ui;

    QWidget * centralForm = nullptr;
    SetupForm * setupForm = nullptr;
    SongInfoForm * songInfoForm = nullptr;
    NotesTableForm * notesTableForm = nullptr;

    void switchForm(QWidget *widget);

};
#endif // MAINWINDOW_H
