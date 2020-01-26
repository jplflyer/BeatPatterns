#include <string>

#include <boost/filesystem.hpp>

#include <QPixmap>

#include <vitac/StringMethods.h>

#include "NotesViewPanel.h"
#include "Preferences.h"

#include "Song.h"

using std::string;
using std::cout;
using std::endl;
using namespace SongEditor;

QIcon * NotesViewPanel::blankIcon = nullptr;
QIcon * IconHolder::up = nullptr;
QIcon * IconHolder::down = nullptr;
QIcon * IconHolder::left = nullptr;
QIcon * IconHolder::right = nullptr;
QIcon * IconHolder::upLeft = nullptr;
QIcon * IconHolder::upRight = nullptr;
QIcon * IconHolder::downLeft = nullptr;
QIcon * IconHolder::downRight = nullptr;
QIcon * IconHolder::dot = nullptr;

IconHolder NotesViewPanel::redIcons;
IconHolder NotesViewPanel::blueIcons;

/**
 * Static method to build the icons we use -- the blocks, the blank space...
 */
QIcon *
NotesViewPanel::makeIcons() {
    if (blankIcon == nullptr) {
        boost::filesystem::path appLocation( Preferences::getAppLocation() );
        string bluePrefix = appLocation.string() + "/Contents/Resources/Images/blue";
        string redPrefix = appLocation.string() + "/Contents/Resources/Images/red";

        blankIcon = new QIcon(QString::fromStdString( appLocation.string() + "/Contents/Resources/Images/blankIcon.png" ));
        blueIcons.load(bluePrefix);
        redIcons.load(redPrefix);
    }
    return blankIcon;
}

/**
 * Constructor.
 */
NotesViewPanel::NotesViewPanel(const std::string &_title, QWidget *parent)
: QFrame(parent), titleStr(_title)
{
    this->setFrameStyle(QFrame::Panel | QFrame::Raised);
    this->setLineWidth(2);

    gridLayout = new QGridLayout(this);

    label = new QLabel(QString::fromStdString(titleStr), this);
    label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );

    timeToPreviousTF = new QLineEdit(this);
    timeToNextTF = new QLineEdit(this);

    gridLayout->addWidget(timeToPreviousTF, 0, 0, 1, 1);
    gridLayout->addWidget(label,			0, 1, 1, 2);
    gridLayout->addWidget(timeToNextTF,		0, 3, 1, 1);

    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 3; ++row) {
            QToolButton * btn = new QToolButton(this);
            string buttonText = string("Hi ") + std::to_string(row) + " " + std::to_string(col);
            btn->setText(QString::fromStdString(buttonText));
            gridLayout->addWidget(btn, 3 - row, col, 1, 1);
            buttons[row][col] = btn;
        }
    }

    this->setLayout(gridLayout);
    this->show();
}

/**
 * Where are we in the file.
 */
void
NotesViewPanel::setLocation(int index, SongEditor::SongBeatmapData * dataP) {
    beatmapData = dataP;
    myIndex = index;

    makeIcons();	// Just to make sure we have them.

    QSize size(48, 48);
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 3; ++row) {
            QToolButton *btn = buttons[row][col];
            btn->setIcon(*blankIcon);
            btn->setIconSize(size);
        }
    }

    SongBeatmapData::Note *	previousNote = beatmapData->getPreviousNote(myIndex);
    SongBeatmapData::Note *	myNote = beatmapData->getNote(myIndex);
    SongBeatmapData::Note *	nextNote = beatmapData->getNextNote(myIndex);

    double songDuration = Song::getCurrentSong()->duration;
    double beatDurationSeconds = Song::getCurrentSong()->beatDurationSeconds;
    double timeSincePrevious = 0.0;
    double timeToNext = songDuration;

    if (myNote != nullptr) {
        double delta = previousNote != nullptr ? myNote->time - previousNote ->time : myNote->time;
        timeSincePrevious = delta * beatDurationSeconds;

        delta = nextNote != nullptr ? nextNote->time - myNote->time : songDuration - myNote->time;
        timeToNext = delta * beatDurationSeconds;
    }
    else if (nextNote != nullptr) {
        // We're null, but we have a next time.
        timeToNext = nextNote->time * beatDurationSeconds;
    }

    char buf[16];
    sprintf(buf, "%.2f", timeSincePrevious);
    timeToPreviousTF->setText(QString(buf));

    sprintf(buf, "%.2f", timeToNext);
    timeToNextTF->setText(QString(buf));

    if (myNote != nullptr) {
        SongBeatmapData::Note * firstNote = beatmapData->notes.at(myIndex);
        showNote(*firstNote);
        for (int index = myIndex; index < beatmapData->notes.size(); ++index) {
            SongBeatmapData::Note * note = beatmapData->notes.at(index);
            if (note->time > firstNote->time) {
                break;
            }
        }
    }
}

/**
 * Show this note.
 */
void
NotesViewPanel::showNote(SongBeatmapData::Note &note) {
    IconHolder * icons = nullptr;

    cout << "Note type: " << note.type << "  Direction: " << note.cutDirection << endl;

    // Which icon set to use?
    switch (note.type) {
        case NoteType_Blue: icons = &blueIcons;  break;
        case NoteType_Red: icons = &redIcons; break;
    }

    if (icons == nullptr) {
        cout << "Null icons." << endl;
        return;
    }

    QIcon *icon = icons->iconForDirection(note.cutDirection);
    if (icon != nullptr) {
        cout << "Set buttons[" << note.lineLayer << "][" << note.lineIndex << "] to an icon." << endl;
        QSize size(48, 48);
        buttons[note.lineLayer][note.lineIndex]->setIcon(*icon);
        buttons[note.lineLayer][note.lineIndex]->setIconSize(size);
    }
    else {
        cout << "Null icon." << endl;
    }
}


void
IconHolder::load(const string &prefix) {
    if (up == nullptr) {
        string upFilename = prefix + "Up.png";

        QPixmap pix(QString::fromStdString(prefix + "Up.png") );
        cout << upFilename << " width: " << pix.width() << ". Height: " << pix.height() << endl;

        up = new QIcon(QString::fromStdString( prefix + "Up.png" ));
        down = new QIcon(QString::fromStdString( prefix + "Down.png" ));
        left = new QIcon(QString::fromStdString( prefix + "Left.png" ));
        right = new QIcon(QString::fromStdString( prefix + "Right.png" ));
        upLeft = new QIcon(QString::fromStdString( prefix + "UpLeft.png" ));
        upRight = new QIcon(QString::fromStdString( prefix + "UpRight.png" ));
        downLeft = new QIcon(QString::fromStdString( prefix + "DownLeft.png" ));
        downRight = new QIcon(QString::fromStdString( prefix + "DownRight.png" ));
        dot = new QIcon(QString::fromStdString( prefix + "Dot.png" ));
    }

}

QIcon *
IconHolder::iconForDirection(int direction) {
    switch (direction) {
        case NoteDirection_Up:			return up;
        case NoteDirection_Down:		return down;
        case NoteDirection_Left:		return left;
        case NoteDirection_Right:		return right;
        case NoteDirection_UpLeft:		return upLeft;
        case NoteDirection_UpRight:		return upRight;
        case NoteDirection_DownLeft:	return downLeft;
        case NoteDirection_DownRight:	return downRight;
        case NoteDirection_None:		return dot;
    }

    return nullptr;
}
