#ifndef SRC_LIB_FILEUTILITIES_H_
#define SRC_LIB_FILEUTILITIES_H_

#include <string>

namespace FileUtilities {
	std::string readFile(const std::string &filename);
    bool exists(const std::string &filename);
    bool isDirectory(const std::string &filename);

    void makeDirectoryPath(const std::string &path);
}

#endif /* SRC_LIB_FILEUTILITIES_H_ */
