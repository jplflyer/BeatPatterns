#include <iostream>

#include "CLI.h"
#include "Preferences.h"
#include <showpage/OptionHandler.h>

using std::cout;
using std::endl;

/**
 * Entrypoint.
 */
int main(int argc, char **argv) {
    SongEditor::Preferences::setupForCLI();
    SongEditor::CLI cli;
    cli.parseArgs(argc, argv);
    cli.run();
}

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
        { "new",        no_argument, [=](const char *) { createNew = true; }, "", "Start a brand new song map." },
        { "generate",   no_argument, [=](const char *) { createNew = true; }, "", "Start a brand new song map." },
        { "difficulty", required_argument, [=](const char *arg) { difficulty = toLevelDifficulty(arg); }, "Hard", "Specify the difficulty." },
        { "song",       required_argument, [=](const char *arg) { songFilePath = arg; }, "song.egg", "Name or path to the song.egg file." },
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
    cout << progName << "[options] map_file_or_directory\n"
         << "\n"
         << "map_file_or_directory can be the info.dat file or the directory containing the file.\n"
         << "Options:\n"
         << " --new            Start a new song map.\n"
         << " --song song.egg  Path to the song.egg file.\n"
         << "\n"
         << "The map_file_or_directory can be the info.dat file or the containing directory.\n"
         << "For --new, if song.egg has a path, it is copied to the output directory.\n"
         << "For --generate, --difficulty is required.\n"
         ;
}

/**
 * Run whatever we were told to run.
 */
void
CLI::run() {
}

} // namespace SongEditor
