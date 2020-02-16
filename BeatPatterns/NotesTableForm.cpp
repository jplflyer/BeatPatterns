#include <iostream>
#include <string>

#include <beat_patterns/Song.h>
#include <beat_patterns/Generator.h>

#include "NotesTableForm.h"
#include "ui_NotesTableForm.h"


using namespace BeatPatterns;
using std::string;

/**
 * Constructor.
 */
NotesTableForm::NotesTableForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotesTableForm)
{
    ui->setupUi(this);

    previousNotesPanel = new NotesViewPanel("Previous", ui->innerTableWidget);
    currentNotesPanel = new NotesViewPanel("Current", ui->innerTableWidget);
    upcomingNotesPanel = new NotesViewPanel("Upcoming", ui->innerTableWidget);

    ui->innerTableWidget->layout()->addWidget(previousNotesPanel);
    ui->innerTableWidget->layout()->addWidget(currentNotesPanel);
    ui->innerTableWidget->layout()->addWidget(upcomingNotesPanel);

    previousNotesPanel->show();
    currentNotesPanel->show();
    upcomingNotesPanel->show();

    if (Song::getCurrentSong() != nullptr) {
        songLoaded();
    }
}

/**
 * Destructor.
 */
NotesTableForm::~NotesTableForm() {
    delete ui;
}

/**
 * A song has been loaded.
 */
void
NotesTableForm::songLoaded() {
    easy = normal = hard = expert = expertPlus = nullptr;
    currentDifficulty = nullptr;
    data = nullptr;

    currentSong = Song::getCurrentSong();
    if (currentSong == nullptr) {
        return;
    }

    easy = currentSong->getDifficulty(LevelDifficulty::Easy);
    normal = currentSong->getDifficulty(LevelDifficulty::Normal);
    hard = currentSong->getDifficulty(LevelDifficulty::Hard);
    expert = currentSong->getDifficulty(LevelDifficulty::Expert);
    expertPlus = currentSong->getDifficulty(LevelDifficulty::ExpertPlus);

    std::string view("View");
    std::string create("Create");

    ui->viewEasyPB->setText( QString::fromStdString( (easy != nullptr ? view : create) + " Easy" ));
    ui->viewNormalPB->setText( QString::fromStdString( (normal != nullptr ? view : create) + " Normal" ));
    ui->viewHardPB->setText( QString::fromStdString( (hard != nullptr ? view : create) + " Hard" ));
    ui->viewExpertPB->setText( QString::fromStdString( (expert != nullptr ? view : create) + " Expert" ));
    ui->viewExpertPlusPB->setText( QString::fromStdString( (expertPlus != nullptr ? view : create) + " ExpertPlus" ));
}

/**
 * View the map for this difficulty, which might involve creating it first.
 */
void
NotesTableForm::view(BeatPatterns::LevelDifficulty difficulty) {
    switch(difficulty) {
        case BeatPatterns::LevelDifficulty::Easy: currentDifficulty = easy; break;
        case BeatPatterns::LevelDifficulty::Normal: currentDifficulty = normal; break;
        case BeatPatterns::LevelDifficulty::Hard: currentDifficulty = hard; break;
        case BeatPatterns::LevelDifficulty::Expert: currentDifficulty = expert; break;
        case BeatPatterns::LevelDifficulty::ExpertPlus: currentDifficulty = expertPlus; break;
    }

    if (currentDifficulty == nullptr) {
        currentDifficulty = currentSong->createDifficulty(difficulty);
        songLoaded();	// Rather than reproducing a bunch of code.
    }

    data = currentSong->getBeatmap(currentDifficulty->beatmapFilename);

    previousNotesPanel->setLocation(-1, data);
    currentNotesPanel->setLocation(0, data);
    upcomingNotesPanel->setLocation(1, data);

    updateDetails();
}

void
NotesTableForm::updateDetails() {
    long noteCount = data->notes.size();
    ui->totalNotesTF->setText(QString::fromStdString(std::to_string(noteCount)));

    double duration = currentSong->duration;
    double notesPerSecond = 0.0;
    if (duration > 1.0) {
        notesPerSecond = noteCount / duration;
    }
    char buf[16];
    sprintf(buf, "%.2f", notesPerSecond);
    ui->notesPerSecondTF->setText(QString::fromStdString(string(buf)));

    sprintf(buf, "%.2f seconds", data->largestGap(duration, currentSong->info.beatsPerMinute));
    ui->largestGapTF->setText(QString::fromStdString(string(buf)));

    sprintf(buf, "%d", data->numberLargeGaps(duration, currentSong->info.beatsPerMinute));
    ui->numLargsGapsTF->setText(QString::fromStdString(string(buf)));

    ui->redCutsTF->setText( QString::fromStdString( std::to_string(data->getCutsCount(CubeType::Red)) ));
    ui->blueCutsTF->setText( QString::fromStdString( std::to_string(data->getCutsCount(CubeType::Blue)) ));
    ui->upDownCutsTF->setText( QString::fromStdString( std::to_string(data->getUpDownCuts()) ));
    ui->leftRightCutsTF->setText( QString::fromStdString( std::to_string(data->getLeftRightCuts()) ));
    ui->diagonalCutsTF->setText( QString::fromStdString( std::to_string(data->getDiagonalCuts()) ));
}

//======================================================================
// Actions.
//======================================================================

void NotesTableForm::on_upLeftBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_UpLeft;
}

void NotesTableForm::on_upBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_Up;
}

void NotesTableForm::on_upRightBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_UpRight;
}

void NotesTableForm::on_leftBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_Left;
}

void NotesTableForm::on_centerBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_None;
}

void NotesTableForm::on_rightBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_Right;
}

void NotesTableForm::on_downLeftBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_DownLeft;
}

void NotesTableForm::on_downBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_Down;
}

void NotesTableForm::on_downRightBtn_clicked() {
    Song::currentCutDirection = BeatPatterns::NoteDirection_DownRight;
}

void NotesTableForm::on_redBtn_clicked() {
    Song::currentNoteType = BeatPatterns::NoteType_Red;
}

void NotesTableForm::on_blueBtn_clicked() {
    Song::currentNoteType = BeatPatterns::NoteType_Blue;
}

void NotesTableForm::on_viewEasyPB_clicked() {
    view(BeatPatterns::LevelDifficulty::Easy);
}

void NotesTableForm::on_viewNormalPB_clicked() {
    view(BeatPatterns::LevelDifficulty::Normal);
}

void NotesTableForm::on_viewHardPB_clicked() {
    view(BeatPatterns::LevelDifficulty::Hard);
}

void NotesTableForm::on_viewExpertPB_clicked() {
    view(BeatPatterns::LevelDifficulty::Expert);
}

void NotesTableForm::on_viewExpertPlusPB_clicked() {
    view(BeatPatterns::LevelDifficulty::ExpertPlus);
}

void NotesTableForm::on_regeneratePB_clicked()
{
    Generator generator( *currentSong, *currentDifficulty, *data );
    generator.generateEntireSong();

    updateDetails();

    previousNotesPanel->setLocation(-1, data);
    currentNotesPanel->setLocation(0, data);
    upcomingNotesPanel->setLocation(1, data);
}
