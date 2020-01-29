#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>

#include "FileUtilities.h"

using namespace std;

namespace FileUtilities {

/**
 * Read file contents.
 */
string
readFile(const string &filename)
{
	std::ifstream ifs(filename);
	return string( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
}

bool
exists(const string &filename) {
    struct stat statBuf;
    int rv = stat(filename.c_str(), &statBuf);
    return (rv == 0);
}

bool
isDirectory(const string &filename) {
    struct stat statBuf;
    int rv = stat(filename.c_str(), &statBuf);
    return (rv == 0) && S_ISDIR(statBuf.st_mode);
}

void
makeDirectoryPath(const std::string &dirName) {
    if (exists(dirName.c_str())) {
        if (!isDirectory(dirName.c_str())) {
            cerr << dirName << " exists but is not a directory.\n";
        }
        return;
    }

    boost::filesystem::path path(dirName);
    makeDirectoryPath(path.parent_path().string());

    mkdir(dirName.c_str(), 0733);
}

} // namespace FileUtilities
