#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include <iostream>

#include <SFML/Audio.hpp>

#include <QCoreApplication>
#include <QLayout>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <song_editor/Preferences.h>

using namespace std;
using namespace SongEditor;

/**
 * Constructor.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralForm = ui->helloLabel;
}

/**
 * Destructor.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switchForm(QWidget *widget) {
    // Only switch if this is a change.
    if (centralForm != widget) {
        // Hide the old one.
        if (centralForm != nullptr) {
            centralForm->hide();
            centralForm = nullptr;
        }

        // Assign the new one.
        if (widget != nullptr) {
            centralForm = widget;
            centralForm->show();
            ui->centralwidget->layout()->addWidget(centralForm);
        }
    }
}

/**
 * Create a new song.
 */
void MainWindow::on_actionNew_triggered()
{
    cout << "New..." << endl;
}

/**
 * Open an existing song.
 */
void MainWindow::on_actionOpen_triggered()
{
    //cout << "App Dir Path: " << QCoreApplication::applicationDirPath().toStdString() << endl;

    const string &libraryPath = Preferences::getLibraryPath();
    QString dirNameQ = QFileDialog::getExistingDirectory(this, "Open Song Directory",  QString::fromStdString(libraryPath) );
    if (dirNameQ.isNull() || dirNameQ.isEmpty()) {
        return;
    }

    string dirName = dirNameQ.toStdString();
    if (dirName == libraryPath) {
        return;
    }

    Preferences::addHistory(dirName);
    Preferences::save();

    int rv = currentSong.open(dirName);
    if (rv == 0) {
        Song::setCurrentSong(&currentSong);

//        currentSong.startPlaying();

        // Switch to the song info page.
        on_actionSong_Info_triggered();
        songInfoForm->setSong(&currentSong);
    }
}

/**
 * Save the current song.
 */
void MainWindow::on_actionSave_triggered()
{
    Song * song = Song::getCurrentSong();
    if (song != nullptr) {
        cout << "Save..." << endl;
        song->save();
    }
}

/**
 * Close out the current song without saving.
 */
void MainWindow::on_actionClose_triggered()
{
    currentSong.close();
}

void MainWindow::on_actionSetup_triggered()
{
    if (setupForm == nullptr) {
        setupForm = new SetupForm(ui->centralwidget);
    }
    switchForm(setupForm);
}

void MainWindow::on_actionSong_Info_triggered()
{
    if (songInfoForm == nullptr) {
        songInfoForm = new SongInfoForm(ui->centralwidget);
    }
    switchForm(songInfoForm);
}

void MainWindow::on_actionSong_Map_triggered()
{
    if (notesTableForm == nullptr) {
        notesTableForm = new NotesTableForm(ui->centralwidget);
    }
    switchForm(notesTableForm);
}
