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

    if (update) {
        doUpdate();
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
    FileUtilities::makeDirectoryPath(dir);

    song.setLoadedFrom(dir);


    //
    doUpdate();
}

/**
 * Perform an update.
 */
void
CLI::doUpdate() {
    string newName;

    //----------------------------------------------------------------------
    // If this is a new, or if they're putting in a new song file, they'll
    // have specified a path, and we'll do a copy.
    //----------------------------------------------------------------------
    newName = copyIfNecessary(song.info.songFilename);
    if (newName.length() > 0) {
        song.info.songFilename = newName;
    }

    //----------------------------------------------------------------------
    // Do we need to copy the cover art?
    //----------------------------------------------------------------------
    newName = copyIfNecessary(song.info.coverImageFilename);
    if (newName.length() > 0) {
        song.info.coverImageFilename = newName;
    }

    //----------------------------------------------------------------------
    // Perform a save.
    //----------------------------------------------------------------------
    song.save();
}

void
CLI::doGenerate() {
}

/**
 * Copy a file only if it seems necessary. If we do, then we return the bare name.
 * So we can tell a copy happened if the return value has length > 0.
 */
std::string
CLI::copyIfNecessary(const std::string & fromName) {
    string retVal;
    boost::filesystem::path inputPath(fromName);
    if (boost::filesystem::exists(inputPath)) {
        retVal = inputPath.filename().string();

        boost::filesystem::path outputPath ( song.getLoadedFrom() + "/" + retVal );
        boost::filesystem::copy_file(inputPath, outputPath);
    }

    return retVal;
}


} // namespace SongEditor
