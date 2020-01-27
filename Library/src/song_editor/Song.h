#ifndef SONG_H
#define SONG_H

#include <string>
#include <SFML/Audio.hpp>

#include <showpage/JSON_Serializable.h>
#include <showpage/PointerVector.h>
#include <showpage/PointerMap.h>

#include "Common.h"

namespace SongEditor {

/**
 * This is what appears in Normal.dat
 */
class SongBeatmapData: public JSON_Serializable {
// Class definitions for the lighting events and notes.
public:
    class Event: public JSON_Serializable {
    public:
        double time;
        int type;
        int value;

        void fromJSON(const nlohmann::json & json);
        void toJSON(nlohmann::json & json) const;
    };

    class Event_Vec: public PointerVector<Event>, public JSON_Serializable {
    public:
        void fromJSON(const nlohmann::json & json);
        void toJSON(nlohmann::json & json) const;
    };

    class Note: public JSON_Serializable {
    public:
        double time;
        int lineIndex;
        int lineLayer;
        int type;
        int cutDirection;

        void fromJSON(const nlohmann::json & json);
        void toJSON(nlohmann::json & json) const;
    };

    class Note_Vec: public PointerVector<Note>, public JSON_Serializable {
    public:
        void fromJSON(const nlohmann::json & json);
        void toJSON(nlohmann::json & json) const;
    };

public:
    std::string version;
    Event_Vec events;
    Note_Vec notes;
    bool hasChanged = false;

public:
    void load(const std::string &fileName);
    void save(const std::string &fileName);

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;

    double largestGap(double songLength, int bpm) const;
    int numberLargeGaps(double songLength, int bpm) const;
    int indexAfter(double time) const;

    int getCutsCount(CubeType) const;
    int getUpDownCuts() const;
    int getLeftRightCuts() const;
    int getDiagonalCuts() const;

    Note * getNote(int index);
    Note * getPreviousNote(int index);
    Note * getNextNote(int index);
};


/**
 * This represents one section from the info.dat file. For a particular difficultyBeatmapSet,
 * we have a difficultyBeatmap, which just has a tiny bit of data and then references the Foo.dat file.
 */
class SongDifficulty: public JSON_Serializable {
public:
    LevelDifficulty difficulty;
    int difficultyRank = 3;
    std::string beatmapFilename;
    int noteJumpMovementSpeed = 10;
    int noteJumpStartBeatOffset= 0;

    // "_customData": {
    //     "_editorOffset": 0,
    //     "_requirements": []
    // }

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

class SongDifficulty_Vec: public PointerVector<SongDifficulty>, public JSON_Serializable {
public:
    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

class SongDifficultySet: public JSON_Serializable {
public:
    std::string beatmapCharacteristicName = "Standard";
    SongDifficulty_Vec difficulties;

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

class SongDifficultySet_Vec: public PointerVector<SongDifficultySet>, public JSON_Serializable {
public:
    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

/**
 * This is the info that might appear in info.dat
 */
class SongInfo: public JSON_Serializable {
public:
    std::string	version = "2.0.0";
    std::string songName;
    std::string songSubName;

    std::string songAuthorName;
    std::string levelAuthorName;
    int beatsPerMinute = 63;
    int songTimeOffset = 0;
    int shuffle = 0;
    double shufflePeriod = 0.5;
    int previewStartTime = 12;
    int previewDuration = 10;
    std::string songFilename;
    std::string coverImageFilename;
    std::string environmentName;

    // This is the customData section.
    std::string editor;

    // customData.editorSettings.modSettings.customColors  section
    bool customColorsEnabled = false;
    std::string colorLeft;
    int colorLeftOverdrive = 0;
    std::string colorRight;
    int colorRightOverdrive = 0;
    std::string envColorLeft;
    int envColorLeftOverdrive = 0;
    std::string envColorRight;
    int envColorRightOverdrive = 0;
    std::string obstacleColor;
    int obstacleColorOverdrive = 0;

    // customData.editorSettings.modSettings.mappingExtensions:
    bool mappingExtensionsEnabled = false;
    int numRows = 3;
    int numCols = 4;
    int colWidth = 1;
    int rowHeight = 1;

    // _difficultyBeatmapSets array
    SongDifficultySet_Vec difficultySets;

public:
    void load(const std::string &fileName);
    void save(const std::string &fileName);
    void clear();

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;

    SongDifficultySet_Vec & getDifficultySets() { return difficultySets; }
};


/**
 * This is all the information about a song.
 */
class Song
{
private:
    static Song *		currentSong;

    std::string			loadedFrom;

public:
    SongInfo	info;
    PointerMap<std::string, SongBeatmapData> beatmapDataMap;

    /** This is the SFML object that holds our music. We need this for data and for playback. */
    sf::Music music;
    bool isOpen = false;

    std::string		dirName;

    double duration;
    double beatDurationSeconds;

    // Information on what we're currently doing.
    static int currentCutDirection;	// Up
    static int currentNoteType;		// Red

public:
    Song();

    int open(const std::string fromLocation);
    void save();
    void close();
    void startPlaying();

    static void setCurrentSong(Song *song) { currentSong = song; }
    static Song * getCurrentSong() { return currentSong; }

    SongDifficulty *	getDifficulty(LevelDifficulty difficulty);
    SongDifficulty *	createDifficulty(LevelDifficulty difficulty);
    SongBeatmapData *	getBeatmap(const std::string & filename);
};

}

#endif // SONG_H
