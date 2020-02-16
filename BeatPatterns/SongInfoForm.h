#ifndef SONGINFOFORM_H
#define SONGINFOFORM_H

#include <QWidget>

#include <song_editor/Song.h>

namespace Ui {
class SongInfoForm;
}

class SongInfoForm : public QWidget
{
    Q_OBJECT

private:
    Ui::SongInfoForm *ui;

    SongEditor::Song * currentSong = nullptr;

public:
    explicit SongInfoForm(QWidget *parent = nullptr);
    ~SongInfoForm();

    void setSong(SongEditor::Song *value);
};

#endif // SONGINFOFORM_H
