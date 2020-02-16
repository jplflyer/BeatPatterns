#include <string>
#include <boost/filesystem.hpp>


#include <QApplication>
#include <QCoreApplication>

#include <song_editor/Preferences.h>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialize our preferences system.
    std::string appDir = QCoreApplication::applicationDirPath().toStdString();

    boost::filesystem::path appDirPath(appDir);
    boost::filesystem::path contentsPath = appDirPath.parent_path();
    boost::filesystem::path applicationPath = contentsPath.parent_path();

    SongEditor::Preferences::setAppLocation( applicationPath.string() );
    SongEditor::Preferences::getSingleton();

    MainWindow w;
    w.show();
    return a.exec();
}
