#ifndef SETUPFORM_H
#define SETUPFORM_H

#include <QWidget>

namespace Ui {
class SetupForm;
}

class SetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit SetupForm(QWidget *parent = nullptr);
    ~SetupForm();

private slots:

    void on_savePB_clicked();

    void on_resetPB_clicked();

    void on_libraryPathOpenPB_clicked();

private:
    Ui::SetupForm *ui;
};

#endif // SETUPFORM_H
