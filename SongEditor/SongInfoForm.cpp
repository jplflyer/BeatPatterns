#include <vitac/StringMethods.h>

#include "SongInfoForm.h"
#include "ui_SongInfoForm.h"

using namespace std;
using namespace SongEditor;

SongInfoForm::SongInfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SongInfoForm)
{
    ui->setupUi(this);
}

SongInfoForm::~SongInfoForm()
{
    delete ui;
}

/**
 * Remember the current song.
 */
void SongInfoForm::setSong(Song *value) {
    currentSong = value;
    ui->dirNameTF->setText(QString::fromStdString(currentSong->dirName));
    ui->songNameTF->setText(QString::fromStdString(currentSong->info.songName));
    ui->songSubNameTF->setText(QString::fromStdString(currentSong->info.songSubName));
    ui->authorNameTF->setText(QString::fromStdString(currentSong->info.songAuthorName));
    ui->mapperNameTF->setText(QString::fromStdString(currentSong->info.levelAuthorName));
    ui->songFileNameTF->setText(QString::fromStdString(currentSong->info.songFilename));
    ui->coverImageTF->setText(QString::fromStdString(currentSong->info.coverImageFilename));
    ui->bpmTF->setText(QString::fromStdString(to_string(currentSong->info.beatsPerMinute)));
    ui->offsetTimeTF->setText(QString::fromStdString(to_string(currentSong->info.songTimeOffset)));

    sf::Time duration = currentSong->music.getDuration();
    int seconds = static_cast<int>(duration.asSeconds());
    int millis = duration.asMilliseconds();

    int minutes = seconds / 60;
    seconds = seconds % 60;
    millis = millis % 1000;

    string secondsStr = intToZeroPaddedString(seconds, 2);
    string millisStr = intToZeroPaddedString(millis, 3);

    ui->durationTF->setText(QString::fromStdString( to_string(minutes) + ":" + secondsStr + "." + millisStr   ));
}
