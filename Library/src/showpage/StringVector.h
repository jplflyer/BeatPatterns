#ifndef SRC_MANAGEMENT_STRINGVECTOR_H_
#define SRC_MANAGEMENT_STRINGVECTOR_H_

#include <string>
#include "PointerVector.h"

/**
 * This is a vector of string pointers that owns the contents (but we'll give them away if you ask nicely).
 *
 * On destruction or calls to erase(), any strings we still hold are deleted. This means if you have
 * lingering pointers to them, you lose.
 */
class StringVector: public PointerVector<std::string> {
public:
	StringVector();
	virtual ~StringVector();

    void remove(const std::string &value);
	void tokenize(const char *str, const char *splitter);
};

#endif /* SRC_MANAGEMENT_STRINGVECTOR_H_ */
