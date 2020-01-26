#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include <boost/filesystem.hpp>

#include <QCoreApplication>

#include <vitac/StringMethods.h>

#include "Preferences.h"

using namespace std;
using JSON = nlohmann::json;

namespace SongEditor {

Preferences * Preferences::s_singleton = nullptr;
static mutex myMytex;

/**
 * Constructor.
 */
Preferences::Preferences()
{
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
    configFileName = homeDir + "/.SongEditorConfig";
    libraryPath = homeDir + "/Music/BeatSaber";

    if ( access( configFileName.c_str(), F_OK ) != -1 ) {
        string contents = readFileContents(configFileName);
        nlohmann::json json = nlohmann::json::parse(contents);

        fromJSON(json);
    }

    // Then we load the standard patterns.
    // This will correspond to the SongEditor.app/Contents/MacOS directory.
    string appDir = QCoreApplication::applicationDirPath().toStdString();

    boost::filesystem::path appDirPath(appDir);
    boost::filesystem::path contentsPath = appDirPath.parent_path();
    boost::filesystem::path applicationPath = contentsPath.parent_path();

    appLocation = applicationPath.string();

    string patternsDirName = contentsPath.string() + "/Resources/Patterns";
    patterns.load(patternsDirName);
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

    history.eraseAll();

    nlohmann::json historyJson = jsonValue(json, "history");
    if (!historyJson.is_null()) {
        for (auto iter = historyJson.begin(); iter != historyJson.end(); ++iter) {
            string str = *iter;
            history.push_back(new string(str));
        }
    }
}

void
Preferences::toJSON(nlohmann::json &json) const {
    json["libraryPath"] = libraryPath;

    nlohmann::json historyJson = nlohmann::json::array();
    for (string *str: history) {
        historyJson.push_back(*str);
    }

    json["history"] = historyJson;
}


}
