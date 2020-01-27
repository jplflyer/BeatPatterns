#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "StringMethods.h"
#include "date.h"

using namespace std;
using namespace std::chrono;

/**
 * Does this string start with this prefix?
 */
bool
stringStartsWith(const char *str, const char *prefix)
{
    while(*prefix) {
        if( *prefix++ != *str++ ) {
            return false;
        }
    }

    return true;
}

/**
 * For logging, this just lets us start with a timestamp.
 */
ostream &
log(ostream &stream) {
	system_clock::time_point now = system_clock::now();
	string nowStr = systemTimePointToString(now, " ");

	stream << nowStr << ": ";
	return stream;
}

/** This is used for some of the time conversion methods. */
class TimeParts {
public:
    int		year;           /**< The year. */
    int		month;          /**< The month. */
    int		day;            /**< The day. */
    int		hour;           /**< The hour 0-23. */
    int		hourTwelveHour; /**< The hour 1-12. */
    int		minutes;        /**< The minutes 0-59. */
    int		seconds;        /**< The seconds 0-59. */
    long	milliseconds;   /**< The milliseconds. */
    long	microseconds;   /**< The microseconds. */
    long	nanoseconds;    /**< The nanoseconds. */
    bool	isPM;           /**< The is 12 PM or later. */

    /** Constructor. */
	TimeParts(const system_clock::time_point &tp);

    /** Destructor. */
	virtual ~TimeParts() {}
};

TimeParts::TimeParts(const system_clock::time_point &tp)
: year(0), month(0), day(0),
  hour(0), hourTwelveHour(0), minutes(0), seconds(0),
  milliseconds(0), microseconds(0), nanoseconds(0), isPM(false)
{
	// Time_t is only 1-second resolution.
    time_t							ttime_t = system_clock::to_time_t(tp);

    // Which we then use to create a new time_point.
    system_clock::time_point		tp_sec = system_clock::from_time_t(ttime_t);

    // And then use the different to get milliseconds.
    chrono::milliseconds			ms = duration_cast<chrono::milliseconds>(tp - tp_sec);
    chrono::microseconds			micros = duration_cast<chrono::microseconds>(tp - tp_sec);
    chrono::nanoseconds				ns = duration_cast<chrono::nanoseconds>(tp - tp_sec);

    tm *							ttm = localtime(&ttime_t);

	year = 1900 + ttm->tm_year;
	month = ttm->tm_mon + 1;
	day = ttm->tm_mday;
	hour = ttm->tm_hour;
	minutes = ttm->tm_min;
	seconds = ttm->tm_sec;
	milliseconds = ms.count();
	microseconds = micros.count();
	nanoseconds = ns.count();

	hourTwelveHour = hour;
	if (hourTwelveHour == 0) {
		hourTwelveHour = 12;
	}
	else if (hourTwelveHour == 12) {
		isPM = true;
	}
	else if (hourTwelveHour > 12) {
		hourTwelveHour -= 12;
		isPM = true;
	}
}

string
nowAsString() {
	system_clock::time_point now = system_clock::now();
	string nowStr = systemTimePointToString(now, " ", "");
	return nowStr;
}

string
todayAsString() {
	system_clock::time_point	now = system_clock::now();
    time_t					 	ttime_t = system_clock::to_time_t(now);
    tm *					 	ttm = localtime(&ttime_t);

    char buffer[100];
    sprintf(buffer, "%04d-%02d-%02d", 1900 + ttm->tm_year, ttm->tm_mon + 1, ttm->tm_mday);

	return string(buffer);
}

string
nowAsMDY() {
	system_clock::time_point now = system_clock::now();
	TimeParts tp(now);

    char buffer[100];
    sprintf(buffer, "%02d-%02d-%04d", tp.month, tp.day, tp.year);

	return string(buffer);
}


/**
 * Convert from this time to a string.
 */
string
systemTimePointToString(const system_clock::time_point &tp, const string &delim, const string &suffix)
{
	TimeParts timeParts(tp);

    // Convert the tm + millis into a string.
    char buffer[100];
    sprintf(buffer, "%04d-%02d-%02d%s%02d:%02d:%02d.%06ld%s",
    		timeParts.year, timeParts.month, timeParts.day,
			delim.c_str(),
			timeParts.hour, timeParts.minutes, timeParts.seconds, timeParts.microseconds,
			suffix.c_str()
			);

    string result(buffer);

    return result;
}

/**
 * Convert from this time to a string. It dumps as m-d-y h-m-s AM
 */
string
systemTimePointToStringTwelveHour(const system_clock::time_point &tp, const string &delim)
{
	TimeParts timeParts(tp);

    // Convert the tm + millis into a string.
    char buffer[100];
    sprintf(buffer,
    		"%02d-%02d-%04d%s%02d:%02d:%02d %s",
			timeParts.month, timeParts.day, timeParts.year,
			delim.c_str(),
			timeParts.hourTwelveHour, timeParts.minutes, timeParts.seconds,
			timeParts.isPM ? "PM" : "AM"
			);

    string result(buffer);

    return result;
}

/**
 * Get the environement variable listed but return the original if it's not found.
 */
string getEnvWithDefault(const string &key, const string &defaultValue) {
	const char * value = getenv(key.c_str());
	if (value != NULL) {
		return string(value);
	}
	return defaultValue;
}

long
timePointToMillis(const time_point<system_clock> &tp)
{
	long millis = duration_cast<milliseconds>(tp.time_since_epoch()).count();
	return millis;
}

time_point<system_clock>
millisToSystemTimePoint(const long millis) {
	milliseconds durMillis(millis);
	time_point<system_clock> tp(durMillis);

	return tp;
}

//======================================================================
// Methods for trimming strings.
//======================================================================

// trim from start (in place)
std::string & ltrim(std::string &s) {
    // OMG. Let's break this down. The erase isn't bad. It's going to erase
    // from the beginning until the last character before the end. So it's the find_if and everything
    // inside of it that is tricky.
    //
    // The find-if is going iterate through the entire string, one character at a time, passing the
    // contents of his *iter -> that std::not1() predicate thing. If that ever returns true, then
    // the corresponding iterator is returned, forming the tail + 1 of what to trim.
    //
    // And the predicate is basically "is not a space".
    //
    // In other words, delete from the beginning up to but not including the first non-space.
    //
    // Whew!
    //
    // Oh. And isspace takes and returns an int instead of taking a char and returning a boolean.
    // Otherwise we'd see <char,bool> near the end.
    //
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    return s;
}

// trim from end (in place)
std::string & rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());

    return s;
}

// trim from both ends (in place)
std::string & trim(std::string &s) {
    ltrim(s);
    rtrim(s);

    return s;
}

// trim from start (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
// Note this is pass by value, not by reference, which is why it's a copy.
std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

/**
 * Returns a new string that replaces all instances of searchFor with replaceWith.
 */
std::string replaceAll(const std::string &input, const std::string &searchFor, const std::string &replaceWith) {
    string retVal = input;
    unsigned long searchLen = searchFor.length();

    for (string::size_type pos = retVal.find(searchFor, 0); pos != string::npos; pos = retVal.find(searchFor, pos)) {
        retVal = retVal.replace(pos, searchLen, replaceWith);
    }

    return retVal;
}

std::string digitsOnly(const std::string &str) {
    string retVal;

    for (char c: str) {
        if (isdigit(c)) {
            retVal += c;
        }
    }

    return retVal;
}

template< typename T >
std::string hexString( T i )
{
  std::stringstream stream;
  stream << "0x"
         << std::setfill ('0') << std::setw(sizeof(T)*2)
         << std::hex << i;
  return stream.str();
}

static const char * LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Convert this data of raw bytes into a string, using ^X for control characters, \xx for other
 * non-printable characters, or the character itself for everything else. This is being used to
 * dump control-A codes we might receive, for instance.
 */
std::string controlStringFromBytes(const char *data, const int length) {
    string retVal = "";

    if (data != nullptr) {
        for (int index = 0; index < length; ++index) {
            unsigned char c = static_cast<unsigned char>(data[index]);
            if (c == 00) {
                retVal += "\\00";
            }
            else if (c == '\r') {
                retVal += "\\r";
            }
            else if (c == '\n') {
                retVal += "\\n";
            }
            else if (c <= 26) {
                // This is deliberate.
                retVal += string("^") + LETTERS[c-1];
            }
            else if (c < ' ') {
                retVal += "\\" + hexString(c);
            }
            else if (c < 127) {
                retVal += c;
            }
            else {
                retVal += "\\" + hexString(c);
            }
        }
    }

    return retVal;
}

std::string intToZeroPaddedString(int value, int digits) {
    char buf[10];

    sprintf(buf, "%%0%dd", digits);
    string str = boost::str(boost::format(buf) % value);
    return str;
}

/**
 * Read file contents.
 */
std::string readFileContents(const std::string& path) {

    auto close_file = [](FILE* f){fclose(f);};

    auto holder = std::unique_ptr<FILE, decltype(close_file)>(fopen(path.c_str(), "rb"), close_file);
    if (!holder) {
      return "";
    }

    FILE* f = holder.get();

    // in C++17 following lines can be folded into std::filesystem::file_size invocation
    if (fseek(f, 0, SEEK_END) < 0) {
      return "";
    }

    const long size = ftell(f);
    if (size < 0) {
      return "";
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        return "";
    }

    std::string res;
    res.resize(size);

    // C++17 defines .data() which returns a non-const pointer
    fread(const_cast<char*>(res.data()), 1, size, f);

    return res;
}
