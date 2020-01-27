#include <stdio.h>
#include <iostream>
#include <fstream>

#include "FileUtilities.h"

using namespace std;

/**
 * Read file contents.
 */
string
FileUtilities::readFile(const string &filename)
{
	std::ifstream ifs(filename);
	return string( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
}
