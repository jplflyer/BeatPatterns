#ifndef NOTESVIEWPANEL_H
#define NOTESVIEWPANEL_H

#include <string>

#include <QFrame>
#include <QGridLayout>
#include <QToolButton>
#include <QLabel>
#include <QLineEdit>
#include <QIcon>

#include <song_editor/Song.h>

class IconHolder {
public:
    static QIcon *	up;
    static QIcon *	down;
    static QIcon *	left;
    static QIcon *	right;
    static QIcon *	upLeft;
    static QIcon *	upRight;
    static QIcon *	downLeft;
    static QIcon *	downRight;
    static QIcon *	dot;

    static void load(const std::string &prefix);

    QIcon * iconForDirection(int direction);
};

/**
 * This is a 2d view of a single point in time. It consists of a label along the top that
 * explains what time point this is and then a 4x3 grid of what is alive during that time.
 */
class NotesViewPanel : public QFrame
{
    Q_OBJECT

private:
    static QIcon *	blankIcon;

    static IconHolder blueIcons;
    static IconHolder redIcons;

    std::string	titleStr;

    QGridLayout * gridLayout;
    QLabel *label;
    QLineEdit *timeToPreviousTF;
    QLineEdit *timeToNextTF;

    // Left to right, bottom to top
    QToolButton * buttons[3][4];

    SongEditor::SongBeatmapData * beatmapData = nullptr;
    int myIndex;

    void showNote(SongEditor::SongBeatmapData::Note &note);

public:
    NotesViewPanel(const std::string &title, QWidget *parent = nullptr);

    void setLocation(int index, SongEditor::SongBeatmapData * dataP);

    static QIcon * makeIcons();

signals:

};

#endif // NOTESVIEWPANEL_H
