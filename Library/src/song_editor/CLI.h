#ifndef CLI_H
#define CLI_H

#include <string>
#include "Common.h"
#include "Song.h"

namespace SongEditor {

/**
 * This defines our CLI.
 */
class CLI
{
private:
    std::string		songFilePath;		// Path to where this is stored
    std::string		oggFilePath;		// Used in new: copy song file from here.

    LevelDifficulty	difficulty = LevelDifficulty::All;

    // These are the various commands we can perform.
    bool			init = false;
    bool			createNew = false;
    bool			generate = false;

    Song			song;

    void doInit();
    void doCreate();
    void doGenerate();

public:
    CLI();
    void parseArgs(int, char **);
    void usage(const char * progName);
    void run();
};

} // namespace SongEditor

#endif // CLI_H
