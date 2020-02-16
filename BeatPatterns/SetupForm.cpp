#include <iostream>
#include <QFileDialog>

#include <beat_patterns/Generator.h>

#include "SetupForm.h"
#include "ui_SetupForm.h"

using namespace std;
using namespace BeatPatterns;

SetupForm::SetupForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupForm)
{
    ui->setupUi(this);

    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, Qt::lightGray);
    setAutoFillBackground(true);
    setPalette(pal);

    on_resetPB_clicked();
}

/**
 * Destructor.
 */
SetupForm::~SetupForm() {
    delete ui;
}

/**
 * Save the config.
 */
void SetupForm::on_savePB_clicked() {
    QString dirNameQ = ui->libraryPathTF->text();
    Preferences::setLibraryPath(dirNameQ.toStdString());
    Preferences::save();
}

/**
 * Reset the config.
 */
void SetupForm::on_resetPB_clicked() {
    const string & libraryPath = Preferences::getLibraryPath();
    cout << "Library path: " << libraryPath << endl;
    ui->libraryPathTF->setText(QString::fromStdString(libraryPath));
}

/**
 * They hit the button to select a different path to our song library.
 */
void SetupForm::on_libraryPathOpenPB_clicked()
{
    QString origDirName = ui->libraryPathTF->text();
    QString dirNameQ = QFileDialog::getExistingDirectory(this, "Select Song Library",  origDirName);
    if (!dirNameQ.isNull() && !dirNameQ.isEmpty()) {
        ui->libraryPathTF->setText(dirNameQ);
    }
}
