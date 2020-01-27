#ifndef CLI_H
#define CLI_H

#include <string>
#include "Common.h"

namespace SongEditor {

/**
 * This defines our CLI.
 */
class CLI
{
private:
    std::string		songFilePath;
    LevelDifficulty	difficulty = LevelDifficulty::Easy;
    bool			createNew = false;
    bool			generate = false;

public:
    CLI();
    void parseArgs(int, char **);
    void usage(const char * progName);
    void run();
};

} // namespace SongEditor

#endif // CLI_H
