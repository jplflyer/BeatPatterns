#ifndef SRC_LIB_STRINGMETHODS_H_
#define SRC_LIB_STRINGMETHODS_H_

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <iostream>
#include <chrono>

#include <boost/preprocessor.hpp>

//
// These are a bunch of string methods that SHOULD be part of the standard, but aren't. The first set
// change the argument. The _copy methods return a copy of the object.
//
// I got these from a thread on stack exchange. The only change I made was I have all of them return
// the string, so you can do trim(foo).whatever instead of requiring a second line of code.
//

//======================================================================
// These defines help with enum definitions. To create an enum class
// Color with Red, White, and Blue:
//
//		DEFINE_ENUMERATION(Color, (Red)(White)(Blue))
//======================================================================
#define X_DEFINE_ENUMERATION(r, datatype, elem) case datatype::elem : return BOOST_PP_STRINGIZE(elem);

// The data portion of the FOR_EACH should be (variable type)(value)
#define X_DEFINE_ENUMERATION2(r, dataseq, elem) \
	if (BOOST_PP_SEQ_ELEM(1, dataseq) == BOOST_PP_STRINGIZE(elem) ) return BOOST_PP_SEQ_ELEM(0, dataseq)::elem;

#define DEFINE_ENUMERATION_MASTER(modifier, name, toFunctionName, enumerators)    \
    enum class name {                                                         \
		Undefined,                                                            \
        BOOST_PP_SEQ_ENUM(enumerators)                                        \
    };                                                                        \
                                                                              \
    modifier const char* toString(const name & v)                               \
    {                                                                         \
        switch (v)                                                            \
        {                                                                     \
            BOOST_PP_SEQ_FOR_EACH(                                            \
                X_DEFINE_ENUMERATION,                                         \
                name,                                                         \
                enumerators                                                   \
            )                                                                 \
            default: return "[Unknown " BOOST_PP_STRINGIZE(name) "]";         \
        }                                                                     \
    }                                                                         \
                                                                              \
	modifier const name toFunctionName(const std::string & value)               \
	{                                                                         \
		BOOST_PP_SEQ_FOR_EACH(                                                \
			X_DEFINE_ENUMERATION2,                                            \
			(name)(value),                                                    \
			enumerators                                                       \
		)                                                                     \
		return name::Undefined;                                               \
	}

#define DEFINE_ENUMERATION(name, toFunctionName, enumerators)                 \
	DEFINE_ENUMERATION_MASTER(inline, name, toFunctionName, enumerators)

#define DEFINE_ENUMERATION_INSIDE_CLASS(name, toFunctionName, enumerators)                 \
	DEFINE_ENUMERATION_MASTER(static, name, toFunctionName, enumerators)

//======================================================================
//
//======================================================================
bool stringStartsWith(const char *str, const char *prefix);

// trim from start (in place)
std::string & ltrim(std::string &s);

// trim from end (in place)
std::string & rtrim(std::string &s);

// trim from both ends (in place)
std::string & trim(std::string &s);

// trim from start (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string ltrim_copy(std::string s);

// trim from end (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string rtrim_copy(std::string s);

// trim from both ends (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string trim_copy(std::string s);

std::string replaceAll(const std::string &input, const std::string &searchFor, const std::string &replaceWith);

std::string digitsOnly(const std::string &str);

std::ostream & log(std::ostream &stream);

std::string systemTimePointToString(const std::chrono::system_clock::time_point &tp, const std::string &delim, const std::string &suffix = std::string(""));

std::string systemTimePointToStringTwelveHour(const std::chrono::system_clock::time_point &tp, const std::string &delim);

std::string nowAsString();
std::string todayAsString();
std::string nowAsMDY();

long timePointToMillis(const std::chrono::time_point<std::chrono::system_clock> &tp);

std::string getEnvWithDefault(const std::string &key, const std::string &defaultValue);
std::chrono::time_point<std::chrono::system_clock> millisToSystemTimePoint(const long millis);


std::string controlStringFromBytes(const char *data, const int length);

std::string intToZeroPaddedString(int value, int digits);

std::string readFileContents(const std::string& path);

#endif /* SRC_LIB_STRINGMETHODS_H_ */
