#include <iostream>

#include <boost/filesystem.hpp>
#include <showpage/OptionHandler.h>
#include <showpage/FileUtilities.h>

#include "CLI.h"
#include "Preferences.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

namespace SongEditor {

/**
 * Constructor.
 */
CLI::CLI() {
}

/**
 * Parse command line arguments.
 */
void
CLI::parseArgs(int argc, char **argv) {
    const char * progName = argv[0];
    OptionHandler::Argument args[] = {
        // Global or semi-global
        { "song",       required_argument, [=](const char *arg) { songFilePath = arg; }},

        { "init",       no_argument, [=](const char *) { init = true; } },

        // Specific to --new
        { "new",        no_argument, [=](const char *) { createNew = true; } },
        { "ogg",        required_argument, [=](const char *arg) { song.info.songFilename = arg; } },
        { "name",		required_argument, [=](const char *arg) { song.info.songName = arg; } },
        { "artist",		required_argument, [=](const char *arg) { song.info.songAuthorName = arg; } },
        { "bpm",		required_argument, [=](const char *arg) { song.info.beatsPerMinute = atoi(arg); } },
        { "cover-image",required_argument, [=](const char *arg) { song.info.coverImageFilename = arg; } },

        // Specific to --generate
        { "generate",   no_argument, [=](const char *) { createNew = true; }},
        { "difficulty", required_argument, [=](const char *arg) { difficulty = toLevelDifficulty(arg); }},
        {nullptr}
    };

    OptionHandler::ArgumentVector vec;
    vec.addAll(args);

    OptionHandler::handleOptions(argc, argv, vec, [=]() { usage(progName); exit(0); } );
}

/**
 * How to use us.
 */
void
CLI::usage(const char *progName) {
    cout << progName << "\n"
         << "\n"
         << "Options:\n"
         << "\n"
         << " --init               Initialize your ~/.SongEditorConfig file. You can then hand-edit it.\n"
         << "\n"
         << " --song directory     Path to the song.egg file.\n"
         << "\n"
         << " --new                Start a new song map.\n"
         << " --ogg song.ogg       Location of the .ogg song file to copy into the --song output directory."
         << " --name name of song  The name of the song.\n"
         << " --artist Artist Name The name of the song's artist.\n"
         << " --bpm bpm            Beats Per Minute.\n"
         << "\n"
         << " --generate           Generate one or more maps.\n"
         << " --difficulty hard    Easy, Normal, Hard, Expert, Expert+, or All.\n"
         << "\n"
         << "The song directory can be the info.dat file or the containing directory.\n"
         ;
}

/**
 * Run whatever we were told to run.
 */
void
CLI::run() {
    if (init) {
        doInit();
        return;
    }

    if (createNew) {
        doCreate();
        return;
    }

    if (FileUtilities::exists(songFilePath)) {
        song.open(songFilePath);
    }
    else {
        string tryPath = Preferences::getLibraryPath() + songFilePath;
        if ( FileUtilities::exists(tryPath)) {
            song.open(tryPath);
        }
        else {
            cerr << "Cannot find your song in either " << songFilePath << " or " << tryPath << endl;
            exit(1);
        }
    }

    if (generate) {
        doGenerate();
    }
}

/**
 * Initializing the preferences is trivial.
 */
void
CLI::doInit() {
    Preferences::save();
    cout << "Your ~/.SongEditorConfig has been created and can now be edited.\n";
}

/**
 * Create a new song.
 */
void
CLI::doCreate() {
    if (songFilePath.length() == 0) {
        cout << "Enter name of directory (within the library) for the new song: ";
        std::cin >> songFilePath;
    }
    if (songFilePath.length() == 0) {
        cerr << "No directory name for output. Quitting.\n";
        exit(1);
    }

    if (songFilePath.at(0) == '/') {
        cerr << "The directory should be a simple directory name.\n";
        exit(1);
    }

    // Create the Library directory
    string dir = Preferences::getLibraryPath() + "/" + songFilePath;
    cout << "Create directory: " << dir << endl;
    FileUtilities::makeDirectoryPath(dir);

    song.setLoadedFrom(dir);

    // Copy the song.ogg into place.
    boost::filesystem::path songInputPath(song.info.songFilename);
    song.info.songFilename = songInputPath.filename().string();

    boost::filesystem::path songOutputPath ( dir + "/" + song.info.songFilename );

    cout << "Song File Path: " << songFilePath << endl;
    cout << "Copy " << songInputPath.string() << " to " << songOutputPath.string() << endl;
    boost::filesystem::copy_file(songInputPath, songOutputPath.string());

    song.info.levelAuthorName = Preferences::getLevelAuthorName();

    // Copy the cover image into place.
    if (song.info.coverImageFilename.length() > 0) {
        boost::filesystem::path coverInputPath(song.info.coverImageFilename);
        boost::filesystem::path coverOutputPath ( dir + "/" + coverInputPath.filename().string());
        cout << "Copy " << coverInputPath.string() << " to " << coverOutputPath.string() << endl;
        boost::filesystem::copy_file(coverInputPath, coverOutputPath);
        song.info.coverImageFilename = coverInputPath.filename().string();
    }

    //
    cout << "song.save()\n";
    song.save();
}

void
CLI::doGenerate() {
}

} // namespace SongEditor
