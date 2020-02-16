#ifndef SONGINFOFORM_H
#define SONGINFOFORM_H

#include <QWidget>

#include <beat_patterns/Song.h>

namespace Ui {
class SongInfoForm;
}

class SongInfoForm : public QWidget
{
    Q_OBJECT

private:
    Ui::SongInfoForm *ui;

    BeatPatterns::Song * currentSong = nullptr;

public:
    explicit SongInfoForm(QWidget *parent = nullptr);
    ~SongInfoForm();

    void setSong(BeatPatterns::Song *value);
};

#endif // SONGINFOFORM_H
