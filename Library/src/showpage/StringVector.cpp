#include <string.h>
#include <vector>
#include <string>
#include "StringVector.h"

using namespace std;

/** Constructor. */
StringVector::StringVector() {
}

/*
 * We actually do proper destruction knowing that we hold pointers.
 */
StringVector::~StringVector() {
}

void
StringVector::remove(const string &value) {
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        string * strP = *iter;
        if (strP != nullptr && *strP == value) {
            this->erase(iter);
            break;
        }
    }
}

/**
 * Populate ourself by splitting this string at any of these delimiters.
 */
void
StringVector::tokenize(const char *str, const char *splitter) {
	char * dup = strdup(str);
	char * dupPtr = dup;
    char * context = nullptr;
	char * ptr;

    while ((ptr = strtok_r(dupPtr, splitter, &context)) != nullptr) {
        dupPtr = nullptr;	// For subsequent calls

		push_back(new string(ptr));
	}

	// Free our duplicate copy.
	free(dup);
}
