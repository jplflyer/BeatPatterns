#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include <boost/filesystem.hpp>

#include <showpage/StringMethods.h>

#include "Preferences.h"

using namespace std;
using JSON = nlohmann::json;

namespace BeatPatterns {

Preferences * Preferences::s_singleton = nullptr;
std::string Preferences::appLocation;

static mutex myMytex;

static char const * lookForDirectory(char const * array[]);

/**
 * Constructor.
 */
Preferences::Preferences()
{
}

/**
 * Setup to run with the CLI.
 */
void Preferences::setupForCLI() {
    unique_lock<mutex> myLock;

    if (s_singleton != nullptr) {
        return;
    }

    s_singleton = new Preferences();
    s_singleton->load();

    // Can we find the Patterns?
    char const * possibleLocations[] = {
        "/usr/local/etc/BeatPatterns/Patterns",
        "/Applications/BeatPatterns.app/Contents/Resources/Patterns",
        "Patterns",
        "../BeatPatterns/Patterns",
        nullptr
    };
    const char * location = lookForDirectory(possibleLocations);

    if (location != nullptr) {
        s_singleton->loadPatterns(location);
    }
    else {
        std::cerr << "Cannot find the Patterns directory. Did you install it?\n";
        exit(0);
    }
}

/**
 * Get our singleton.
 */
Preferences *
Preferences::getSingleton() {
    unique_lock<mutex> myLock;

    if (s_singleton == nullptr) {
        s_singleton = new Preferences();
        s_singleton->load();

        string patternsDirName = appLocation + "/Contents/Resources/Patterns";
        s_singleton->loadPatterns(patternsDirName);
    }

    return s_singleton;
}

/**
 * Load preferences.
 */
void
Preferences::load() {
    // Get the home directory.
    char * home = getenv("HOME");

    if (home == nullptr) {
        struct passwd * pw = getpwuid(getuid());
        if (pw != nullptr) {
            home = pw->pw_dir;
        }
    }

    homeDir = home;
    configFileName = homeDir + "/.BeatPatternsConfig";
    libraryPath = homeDir + "/Music/BeatSaber";

    if ( access( configFileName.c_str(), F_OK ) != -1 ) {
        string contents = readFileContents(configFileName);
        nlohmann::json json = nlohmann::json::parse(contents);

        fromJSON(json);
    }
}

/**
 * Try to load the patterns from this location.
 */
void
Preferences::loadPatterns(const std::string &fromDir) {
    patterns.load(fromDir);
    patterns.mapInto(patternsMap);
}

/**
 * Static version -- save our preferences.
 */
void
Preferences::save() {
    getSingleton()->_save();
}

/**
 * Save our preferences.
 */
void
Preferences::_save() const {
    nlohmann::json json;
    toJSON(json);
    ofstream output(configFileName);
    output << json.dump(2) << endl;
    output.close();
}

/**
 * Push this onto the history.
 */
void
Preferences::_pushHistory(const std::string &dirName) {
    history.remove(dirName);
    history.push_back(new string(dirName));
}

void
Preferences::fromJSON(const nlohmann::json &json) {
    libraryPath = stringValue(json, "libraryPath");
    levelAuthorName = stringValue(json, "levelAuthorName");

    history.eraseAll();

    nlohmann::json historyJson = jsonValue(json, "history");
    if (!historyJson.is_null()) {
        for (auto iter = historyJson.begin(); iter != historyJson.end(); ++iter) {
            string str = *iter;
            history.push_back(new string(str));
        }
    }

    nlohmann::json difficultyDefaultsJSON = jsonValue(json, "difficultyDefaults");
    if (!difficultyDefaultsJSON.is_null()) {
        difficultyDefaults.fromJSON(difficultyDefaultsJSON);
    }
}

void
Preferences::toJSON(nlohmann::json &json) const {
    json["libraryPath"] = libraryPath;
    json["levelAuthorName"] = levelAuthorName;

    nlohmann::json historyJson = nlohmann::json::array();
    for (string *str: history) {
        historyJson.push_back(*str);
    }

    nlohmann::json difficultyDefaultsJSON = nlohmann::json::array();
    difficultyDefaults.toJSON(difficultyDefaultsJSON);

    json["history"] = historyJson;
    json["difficultyDefaults"] = difficultyDefaultsJSON;
}

/**
 * Get the preferences for this difficulty level.
 */
Preferences::DifficultyDefaults &
Preferences::getDifficultyDefaults(LevelDifficulty difficulty) {
    return getSingleton()->getOrBuildDifficultyDefaults(difficulty);
}

/**
 * Get the preferences for this difficulty level, building it if necessary.
 */
Preferences::DifficultyDefaults &
Preferences::getOrBuildDifficultyDefaults(LevelDifficulty difficulty) {
    DifficultyDefaults * dd = difficultyDefaults.find(difficulty);
    if (dd == nullptr) {
        dd = new DifficultyDefaults();
        dd->difficulty = difficulty;
        difficultyDefaults.push_back(dd);
    }
    return *dd;
}

//======================================================================
// Our defaults for each difficulty level.
//======================================================================

void Preferences::DifficultyDefaults::fromJSON(const JSON & json) {
    difficulty = toLevelDifficulty(stringValue(json, "difficulty"));
    minimumInitialWhitespace = doubleValue(json, "minimumInitialWhitespace");
    minimumDelayBetweenPatterns = doubleValue(json, "minimumDelayBetweenPatterns");
    maximumDelayBetweenPatterns = doubleValue(json, "maximumDelayBetweenPatterns");
}

void Preferences::DifficultyDefaults::toJSON(JSON & json) const {
    json["difficulty"] = levelDifficultyToString(difficulty);
    json["minimumInitialWhitespace"] = minimumInitialWhitespace;
    json["minimumDelayBetweenPatterns"] = minimumDelayBetweenPatterns;
    json["maximumDelayBetweenPatterns"] = maximumDelayBetweenPatterns;
}

/**
 * Search. If not found, return the values for All.
 */
Preferences::DifficultyDefaults *
Preferences::DifficultyDefaults_Vec::find(LevelDifficulty difficulty) {
    for (DifficultyDefaults *dd: *this) {
        if (dd->difficulty == difficulty) {
            return dd;
        }
    }

    if (difficulty != LevelDifficulty::All) {
        return find(LevelDifficulty::All);
    }
    return nullptr;
}


/**
 * Helper method
 */
char const *
lookForDirectory(char const * array[]) {
    struct stat statBuf;
    for (int index = 0; array[index] != nullptr; ++index) {
        int rv = stat(array[index], &statBuf);
        if (rv == 0 && S_ISDIR(statBuf.st_mode)) {
            return array[index];
        }
    }
    return nullptr;
}

}
