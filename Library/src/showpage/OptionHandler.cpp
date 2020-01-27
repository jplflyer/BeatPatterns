#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#include "OptionHandler.h"

using namespace std;

/**
 * Parse the command line. Returns true if clean, false if bad args or help was provided.
 */
bool
OptionHandler::handleOptions(int argc, const char * const *argv, Argument *arguments, HelpFunction helpFunction) {
    ArgumentVector vec;
    vec.addAll(arguments);
    return handleOptions(argc, argv, vec, helpFunction);
}

/**
 * Parse the command line. Returns true if clean, false if bad args or help was provided.
 */
bool
OptionHandler::handleOptions(int argc, const char * const *argv, ArgumentVector & vec, HelpFunction helpFunction) {
    bool retVal = true;

    if (!helpFunction && vec.anyHaveHelpText() && (vec.searchForLongName("help") == nullptr) ) {
        char helpShortCode = (vec.searchForShortCode('h') == nullptr) ? '?' : 0;
        vec.push_back( new Argument ("help", no_argument, helpShortCode, [&](const char *) { vec.autoHelp(); retVal = false; }, "", "Provide this help" ) );
    }

    unsigned int count = static_cast<unsigned int>(vec.size());
    string shortArgs;
    option * options = vec.getOptions(shortArgs);
    int optionIndex = -1;
    int opt;
    const char *shortArgsPtr = shortArgs.c_str();

    while ( (opt = getopt_long (argc, const_cast<char **>(argv), shortArgsPtr, options, &optionIndex)) != -1) {
        // This happens with short options.
        if (optionIndex < 0) {
            for (unsigned int index = 0; index < count; ++index) {
                if (vec.at(index)->val == opt) {
                    optionIndex = static_cast<int>(index);
                    break;
                }
            }
        }

        if (optionIndex < 0) {
            if (helpFunction != nullptr) {
                helpFunction();
            }
            retVal = false;
            break;
        }

        vec.at(static_cast<unsigned int>(optionIndex))->callbackFunction(optarg);
        optionIndex = -1;
    }

    return retVal;
}

//======================================================================
// Argument.
//======================================================================

/**
 * Constructor.
 */
OptionHandler::Argument::Argument() {
}

/**
 * Complicated constructor.
 */
OptionHandler::Argument::Argument(
    const char *_name,
    int _has_arg, char _val,
    CallbackFunction cb,
    const string & argText,
    const string & help )
: has_arg(_has_arg), val(_val), callbackFunction(cb), argumentText(argText), helpText(help)
{
    if (_name != nullptr) {
        name = _name;
    }
}

/**
 * Copy constructor.
 */
OptionHandler::Argument::Argument(const Argument &arg)
:	name ( arg.name ),
    has_arg ( arg.has_arg ),
    val ( arg.val ),
    callbackFunction ( arg.callbackFunction ),
    argumentText ( arg.argumentText ),
    helpText ( arg.helpText )
{
}

string
OptionHandler::Argument::argumentForHelp() const {
    string retVal;

    if (name.length() > 0) {
        retVal = string("--") + name;
        if (isprint(val)) {
            retVal += string(" (-") + val + string(")");
        }
    }
    else if (isprint(val)) {
        retVal = string("-") + val;
    }

    if (argumentText.length() > 0) {
        retVal += string(" ") + argumentText;
    }

    return retVal;
}

/**
 * For auto-generated help, we're going to write something like:
 *
 *    --foo (-f) fooValue
 *
 * Return the string length.
 */
int
OptionHandler::Argument::argumentLength() const {
    return static_cast<int>(argumentForHelp().length());
}

string
OptionHandler::Argument::paddedArgument(int length) const {
    string str = argumentForHelp();

    if (str.length() < length) {
        str += string(length - str.length(), ' ');
    }

    return str;
}

//======================================================================
// Argument Vector.
//======================================================================

/**
 * Vector constructor.
 */
OptionHandler::ArgumentVector::ArgumentVector()
    : options(nullptr), optionsCount(0)
{
}

OptionHandler::ArgumentVector::~ArgumentVector() {
    if (options != nullptr) {
        delete[] options;
        options = nullptr;
        optionsCount = 0;
    }
}

/**
 * Append these.
 */
void
OptionHandler::ArgumentVector::addAll(Argument *arguments) {
    unsigned long count = 0;
    while(arguments[count].name.length() > 0 || arguments[count].val != 0) {
        ++count;
    }

    for (unsigned long index = 0; index < count; ++index) {
        Argument &arg = arguments[index];
        Argument * newArg = new Argument(arg);
        push_back(newArg);
    }
}

/**
 * Append.
 */
void
OptionHandler::ArgumentVector::addAll(ArgumentVector &vec) {
    for (Argument *arg: vec) {
        Argument * newArg = new Argument(*arg);
        push_back(newArg);
    }
}

/**
 * We use getopt_long to actually do the parsing. He assumes an array of struct option. We produce one.
 * Note that if you fiddle with our contents without changing our size, we sort of screw up, so if you call
 * this method more than once, and you make changes between, it might be a problem.
 */
struct option *
OptionHandler::ArgumentVector::getOptions(std::string &shortArgsReturned) {
    if (options == nullptr || optionsCount != size()) {
        optionsCount = size() + 1;
        options = new option[optionsCount];
        shortArgs = "";

        int index = 0;
        for (Argument *arg: *this) {
            option &opt = options[index++];

            opt.name = arg->name.c_str();
            opt.has_arg = arg->has_arg;
            opt.flag = nullptr;
            opt.val = arg->val;

            if (arg->val != 0) {
                shortArgs += arg->val;
                if (opt.has_arg != no_argument) {
                    shortArgs += ":";
                }
            }
        }
        {
            // Do the last one.
            option &opt = options[index];
            opt.name = nullptr;
            opt.has_arg = 0;
            opt.flag = nullptr;
            opt.val = 0;
        }

    }

    shortArgsReturned = shortArgs;
    return options;
}

OptionHandler::Argument *
OptionHandler::ArgumentVector::searchForLongName(const std::string &value) {
    Argument * retVal = nullptr;
    for (auto it = begin(); it != end(); ++it) {
        Argument *arg = *it;
        if (arg->name == value) {
            retVal = arg;
            break;
        }
    }
    return retVal;
}


OptionHandler::Argument *
OptionHandler::ArgumentVector::searchForShortCode(char code) {
    Argument * retVal = nullptr;
    for (auto it = begin(); it != end(); ++it) {
        Argument *arg = *it;
        if (arg->val == code) {
            retVal = arg;
            break;
        }
    }
    return retVal;
}

bool
OptionHandler::ArgumentVector::anyHaveHelpText() const {
    bool retVal = false;
    for (auto it = begin(); it != end(); ++it) {
        Argument *arg = *it;
        if (arg->argumentText.length() > 0 || arg->helpText.length() > 0) {
            retVal = arg;
            break;
        }
    }
    return retVal;
}

void
OptionHandler::ArgumentVector::autoHelp() const {
    int longest = 0;
    for (auto it = cbegin(); it != cend(); ++it) {
        Argument * arg = *it;
        longest = max(longest, arg->argumentLength());
    }

    for (auto it = cbegin(); it != cend(); ++it) {
        Argument * arg = *it;
        cout << arg->paddedArgument(longest) << " -- " << arg->helpText << endl;
    }
}
