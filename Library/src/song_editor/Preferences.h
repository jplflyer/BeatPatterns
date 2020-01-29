#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <string>
#include <map>

#include <showpage/JSON_Serializable.h>
#include <showpage/StringVector.h>

#include "Pattern.h"

namespace SongEditor {

class Preferences: public JSON_Serializable
{
protected:
    static Preferences * s_singleton;

    static std::string		appLocation;

    /** Our home directory as best we can tell. */
    std::string		homeDir;

    /** Location of our config file, saved only if the user makes changes. */
    std::string		configFileName;

    /**
     * Where do we put all our works in progress?
     * Default: ~/Music/BeatSaber
     */
    std::string		libraryPath;

    /**
     * Default editor's name for new maps.
     */
    std::string		levelAuthorName = "Your Name Here";

    /**
     * The history of things we've loaded in the past.
     */
    StringVector history;

    /**
     * The configured patterns.
     */
    Pattern_Vec patterns;
    std::map<std::string, Pattern *> patternsMap;

    Preferences();
    void load();
    void loadPatterns(const std::string &fromDir);
    void _save() const;
    void _pushHistory(const std::string &dirName);

public:
    static Preferences * getSingleton();

    static void setAppLocation(const std::string &value) { appLocation = value; }
    static void setupForCLI();
    static std::string & getAppLocation() { return getSingleton()->appLocation; }
    static std::string & getLibraryPath() { return getSingleton()->libraryPath; }
    static std::string & getHomeDirectory() { return getSingleton()->homeDir; }
    static std::string & getConfigFileName() { return getSingleton()->configFileName; }
    static Pattern_Vec & getPatterns() { return getSingleton()->patterns; }
    static std::string & getLevelAuthorName() { return getSingleton()->levelAuthorName; }

    static void setLibraryPath(const std::string &value) { getSingleton()->libraryPath = value; }
    static void addHistory(const std::string &dirName) { getSingleton()->_pushHistory(dirName); }

    static void save();

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

};

#endif // PREFERENCES_H
