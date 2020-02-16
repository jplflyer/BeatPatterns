#ifndef NOTESTABLEFORM_H
#define NOTESTABLEFORM_H

#include <QWidget>
#include "NotesViewPanel.h"

#include <beat_patterns/Song.h>

namespace Ui {
class NotesTableForm;
}

class NotesTableForm : public QWidget
{
    Q_OBJECT

public:
    explicit NotesTableForm(QWidget *parent = nullptr);
    ~NotesTableForm();

    /** Fire this if we've loaded a new song. */
    void songLoaded();

private slots:
    void on_upLeftBtn_clicked();
    void on_upBtn_clicked();
    void on_upRightBtn_clicked();
    void on_leftBtn_clicked();
    void on_centerBtn_clicked();
    void on_rightBtn_clicked();
    void on_downLeftBtn_clicked();
    void on_downBtn_clicked();
    void on_downRightBtn_clicked();
    void on_redBtn_clicked();
    void on_blueBtn_clicked();

    void on_viewEasyPB_clicked();
    void on_viewNormalPB_clicked();
    void on_viewHardPB_clicked();
    void on_viewExpertPB_clicked();
    void on_viewExpertPlusPB_clicked();

    void on_regeneratePB_clicked();

private:
    Ui::NotesTableForm *ui;

    NotesViewPanel *	previousNotesPanel = nullptr;
    NotesViewPanel *	currentNotesPanel = nullptr;
    NotesViewPanel *	upcomingNotesPanel = nullptr;

    BeatPatterns::Song *	currentSong = nullptr;
    BeatPatterns::SongDifficulty * currentDifficulty = nullptr;
    BeatPatterns::SongBeatmapData *data = nullptr;

    BeatPatterns::SongDifficulty * easy = nullptr;
    BeatPatterns::SongDifficulty * normal = nullptr;
    BeatPatterns::SongDifficulty * hard = nullptr;
    BeatPatterns::SongDifficulty * expert = nullptr;
    BeatPatterns::SongDifficulty * expertPlus = nullptr;

    void view(BeatPatterns::LevelDifficulty difficulty);
    void updateDetails();

};

#endif // NOTESTABLEFORM_H
