#include <iostream>

#include <boost/filesystem.hpp>
#include <showpage/OptionHandler.h>
#include <showpage/FileUtilities.h>

#include "CLI.h"
#include "Preferences.h"
#include "Generator.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

namespace BeatPatterns {

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
    levelAuthorName = Preferences::getLevelAuthorName();

    const char * progName = argv[0];
    OptionHandler::Argument args[] = {
        // Global or semi-global
        { "song",       required_argument, [=](const char *arg) { songFilePath = arg; }},

        { "init",       no_argument, [=](const char *) { init = true; } },

        // Specific to --new or --update
        { "new",        no_argument, [=](const char *) { createNew = true; } },
        { "update",     no_argument, [=](const char *) { update = true; } },
        { "ogg",        required_argument, [=](const char *arg) { songFilename = arg; } },
        { "name",		required_argument, [=](const char *arg) { songName = arg; } },
        { "artist",		required_argument, [=](const char *arg) { artist = arg; } },
        { "level-by",	required_argument, [=](const char *arg) { levelAuthorName = arg; } },
        { "bpm",		required_argument, [=](const char *arg) { bpm = atoi(arg); } },
        { "cover-image",required_argument, [=](const char *arg) { coverImageFilename = arg; } },

        // Specific to --generate
        { "generate",   no_argument, [=](const char *) { generate = true; }},
        { "difficulty", required_argument, [=](const char *arg) { difficulty = toLevelDifficulty(arg); }},
        {nullptr}
    };

    OptionHandler::ArgumentVector vec;
    vec.addAll(args);

    OptionHandler::handleOptions(argc, argv, vec, [=]() { usage(progName); exit(0); } );

    cout << "Level author name: " << levelAuthorName << endl;
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
         << " --init               Initialize your ~/.BeatPatternsConfig file. You can then hand-edit it.\n"
         << "\n"
         << " --song directory     Path to the song.egg file.\n"
         << "\n"
         << " --new                Start a new song map.\n"
         << " --update             Update an existing map, applying changes from the below list.\n"
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
    }

    if (FileUtilities::exists(songFilePath)) {
        if (song.open(songFilePath) != 0) {
            cout << "Song failed to open. Exiting.\n";
            exit(1);
        }
    }
    else {
        string tryPath = Preferences::getLibraryPath() + "/" + songFilePath;
        if ( FileUtilities::exists(tryPath)) {
            if (song.open(tryPath) != 0) {
                cout << "Song failed to open. Exiting.\n";
                exit(1);
            }
        }
        else {
            cerr << "Cannot find your song in either " << songFilePath << " or " << tryPath << endl;
            exit(1);
        }
    }

    if (update) {
        doUpdate();
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
    cout << "Your ~/.BeatPatternsConfig has been created and can now be edited.\n";
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

    cout << "Doing update...\n";

    if (songName.length() > 0) {
        song.info.songName = songName;
    }
    if (levelAuthorName.length() > 0) {
        song.info.levelAuthorName = levelAuthorName;
    }
    if (artist.length() > 0) {
        song.info.songAuthorName = artist;
    }
    if (bpm > 0) {
        song.info.beatsPerMinute = bpm;
    }

    //----------------------------------------------------------------------
    // If this is a new, or if they're putting in a new song file, they'll
    // have specified a path, and we'll do a copy.
    //----------------------------------------------------------------------
    newName = copyIfNecessary(songFilename);
    if (newName.length() > 0) {
        song.info.songFilename = newName;
    }

    //----------------------------------------------------------------------
    // Do we need to copy the cover art?
    //----------------------------------------------------------------------
    newName = copyIfNecessary(coverImageFilename);
    if (newName.length() > 0) {
        song.info.coverImageFilename = newName;
    }

    //----------------------------------------------------------------------
    // Perform a save.
    //----------------------------------------------------------------------
    song.save();
    cout << "Update done\n";
}

void
CLI::doGenerate() {
    cout << "Doing generate.\n";
    if (difficulty == LevelDifficulty::All) {
        doGenerateFor(LevelDifficulty::Easy);
        doGenerateFor(LevelDifficulty::Normal);
        doGenerateFor(LevelDifficulty::Hard);
        doGenerateFor(LevelDifficulty::Expert);
        doGenerateFor(LevelDifficulty::ExpertPlus);
    }
    else {
        doGenerateFor(difficulty);
    }

    song.save();
}

void CLI::doGenerateFor(LevelDifficulty thisDifficulty) {
    cout << "Generate for difficulty: " << thisDifficulty << endl;

    SongDifficulty * songDifficulty = song.getDifficulty(thisDifficulty);
    SongBeatmapData * beatmapData = song.getBeatmap(songDifficulty->beatmapFilename);

    if (songDifficulty == nullptr) {
        cout << "Null songDifficulty.\n";
        exit(1);
    }
    if (beatmapData == nullptr) {
        cout << "Null beatmapData.\n";
        exit(1);
    }

    cout << "Create the generator.\n";
    Generator generator(song, *songDifficulty, *beatmapData);
    cout << "Run the generator.\n";
    generator.generateEntireSong();

    cout << "Generate done for difficulty: " << thisDifficulty << endl;
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
        cout << "Copy from " << inputPath << " to " << outputPath << endl;
        boost::filesystem::copy_file(inputPath.string(), outputPath.string(), boost::filesystem::copy_option::overwrite_if_exists);
    }

    return retVal;
}


} // namespace BeatPatterns
