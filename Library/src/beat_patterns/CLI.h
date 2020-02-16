#ifndef CLI_H
#define CLI_H

#include <string>
#include "Common.h"
#include "Song.h"

namespace BeatPatterns {

/**
 * This defines our CLI.
 */
class CLI
{
private:
    std::string		songFilePath;		// Path to where this is stored
    std::string		songFilename;		// Where to get the song file from.
    std::string		songName;			// Name of the song.
    std::string		artist;				// Artist.
    std::string		levelAuthorName;	// Person who create the level.
    std::string		coverImageFilename;	// Cover image jpg.
    int				bpm = 0;

    LevelDifficulty	difficulty = LevelDifficulty::All;

    // These are the various commands we can perform.
    bool			init = false;
    bool			createNew = false;
    bool			update = false;
    bool			generate = false;

    Song			song;

    void doInit();
    void doCreate();
    void doUpdate();
    void doGenerate();
    void doGenerateFor(LevelDifficulty thisDifficulty);

    std::string copyIfNecessary(const std::string & from);

public:
    CLI();
    void parseArgs(int, char **);
    void usage(const char * progName);
    void run();
};

} // namespace BeatPatterns

#endif // CLI_H
