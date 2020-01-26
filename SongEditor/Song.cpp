#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/filesystem.hpp>
#include <vitac/StringMethods.h>

#include "Song.h"

using namespace std;

using JSON = nlohmann::json;

namespace SongEditor {

Song * Song::currentSong = nullptr;

int Song::currentCutDirection = NoteDirection_Up;
int Song::currentNoteType = NoteType_Red;

/**
 * Constructor.
 */
Song::Song()
{
}

/**
 * We should be passed either a directory or a file inside a directory.
 * Try to open and load everything. Return 0 on success, -1 on any failure.
 */
int
Song::open(const std::string fromLocation) {
    close();

    boost::filesystem::path path(fromLocation);

    if (! boost::filesystem::is_directory(path)) {
        path = path.parent_path();
    }

    if (! boost::filesystem::exists(path)) {
        cerr << fromLocation << ": Does not exist." << endl;
        return -1;
    }

    // Exists and should be a directory.
    bool didIt = false;
    dirName = path.string();
    boost::filesystem::directory_iterator end_iter;
    loadedFrom = path.string();

    for ( boost::filesystem::directory_iterator dir_itr( path ); dir_itr != end_iter; ++dir_itr ) {
        boost::filesystem::path childPath = dir_itr->path();

        if (childPath.filename().string() == "info.dat") {
            cout << " Must load info from: " << childPath.string() << endl;
            info.load(childPath.string());
            didIt = true;
            break;
        }
    }

    if (!didIt) {
        return -1;
    }

    // Now load all the existing maps files.
    for (SongDifficultySet *set: info.getDifficultySets()) {
        for (SongDifficulty *songDifficulty: set->difficulties) {
            SongBeatmapData * beatmapData = beatmapDataMap.get(songDifficulty->beatmapFilename);
            if (beatmapData == nullptr) {
                string fullPath = path.string() + "/" + songDifficulty->beatmapFilename;

                beatmapData = new SongBeatmapData();
                beatmapData->load(fullPath);

                beatmapDataMap[songDifficulty->beatmapFilename] = beatmapData;
            }
        }
    }

    if (info.songFilename.length() > 0) {
        music.openFromFile(dirName + "/" + info.songFilename);
        duration = static_cast<double>(music.getDuration().asSeconds());
    }

    beatDurationSeconds = 60.0 / info.beatsPerMinute;

    isOpen = true;

    return 0;
}

/**
 * Perform a save. We're going to write out:
 *
 * 		loadedFrom/info.dat		<-- from SongInfo
 * 		loadedFrom/<Normal>.dat	<-- any modified songbeat data.
 */
void
Song::save() {
    string infoDatFilename = loadedFrom + "/info.dat";
    info.save(infoDatFilename);

    for (SongDifficultySet * diffSet: info.getDifficultySets()) {
        for (SongDifficulty * difficulty: diffSet->difficulties) {
            string filename = difficulty->beatmapFilename;

            auto pos = beatmapDataMap.find(filename);
            if (pos != beatmapDataMap.end()) {
                SongBeatmapData * beatmapData = pos->second;

                beatmapData->save(loadedFrom + "/" + filename);
            }

        }
    }
}

/**
 * Stop.
 */
void
Song::close() {
    if (isOpen) {
        music.stop();
        info.clear();
        beatmapDataMap.eraseAll();

        duration = 0.0;
        beatDurationSeconds = 1.0;

        isOpen = false;
    }
}

void
Song::startPlaying() {
    if (isOpen) {
        music.play();
    }
}

/**
 * Return the data for a map at this difficulty.
 */
SongDifficulty *
Song::getDifficulty(LevelDifficulty difficulty) {
    if (info.difficultySets.size() > 0) {
        // This is a set of songs for a particularly beatmap characteristic, which I don't really understand.
        SongDifficultySet & firstSet = *info.difficultySets.at(0);

        for (SongDifficulty * sd: firstSet.difficulties) {
            if (sd->difficulty == difficulty) {
                return sd;
            }
        }
    }
    return nullptr;
}

SongDifficulty *
Song::createDifficulty(LevelDifficulty difficulty) {
    // Make sure we have at least one difficulty set.
    if (info.difficultySets.size() == 0) {
        SongDifficultySet * set = new SongDifficultySet();
        info.difficultySets.push_back(set);
    }

    // Don't over-create.
    SongDifficulty * sd = getDifficulty(difficulty);
    if (sd != nullptr) {
        return sd;
    }

    sd = new SongDifficulty();
    sd->difficulty = difficulty;
    sd->beatmapFilename = levelDifficultyToString(difficulty) + ".dat";

    info.difficultySets.at(0)->difficulties.push_back(sd);

    return sd;
}

/**
 * Get the beats from this file.
 */
SongBeatmapData *
Song::getBeatmap(const std::string & filename) {
    SongBeatmapData * data = nullptr;
    auto pos = beatmapDataMap.find(filename);

    if (pos != beatmapDataMap.end()) {
        data = pos->second;
    }

    else {
        data = new SongBeatmapData();
        beatmapDataMap[filename] = data;
    }

    return data;
}



//======================================================================
// SongInfo
//======================================================================

/**
 * Load our contents from a file. It's in JSON.
 */
void SongInfo::load(const std::string &fileName) {
    string contents = readFileContents(fileName);
    nlohmann::json json = nlohmann::json::parse(contents);

    fromJSON(json);
}

void
SongInfo::save(const std::string &filename) {
    JSON json;

    toJSON(json);
    ofstream output(filename);
    output << json.dump(2);
    output.close();
}

void SongInfo::clear() {
    difficultySets.eraseAll();
}

/**
 * Parse from this json.
 */
void SongInfo::fromJSON(const nlohmann::json & json) {
    version = stringValue(json, "_version");
    songName = stringValue(json, "_songName");
    songSubName = stringValue(json, "_songSubName");
    songAuthorName = stringValue(json, "_songAuthorName");
    levelAuthorName = stringValue(json, "_levelAuthorName");
    beatsPerMinute = intValue(json, "_beatsPerMinute");
    songTimeOffset = intValue(json, "_songTimeOffset");
    shuffle = intValue(json, "_shuffle");
    shufflePeriod = doubleValue(json, "_shufflePeriod");
    previewStartTime = intValue(json, "_previewStartTime");
    previewDuration = intValue(json, "_previewDuration");
    songFilename = stringValue(json, "_songFilename");
    coverImageFilename = stringValue(json, "_coverImageFilename");
    environmentName = stringValue(json, "_environmentName");

    // Custom data
    nlohmann::json customData = jsonValue(json, "_customData");
    nlohmann::json editorSettings = jsonValue(customData, "_editorSettings");
    nlohmann::json modSettings = jsonValue(editorSettings, "modSettings");
    nlohmann::json customColors = jsonValue(modSettings, "customColors");
    nlohmann::json mappingExtensions = jsonValue(modSettings, "mappingExtensions");

    editor = stringValue(customData, "_editor");

    // customData.editorSettings.modSettings.customColors  section
    customColorsEnabled = boolValue(customColors, "isEnabled");
    colorLeft = stringValue(customColors, "colorLeft");
    colorLeftOverdrive = intValue(customColors, "colorLeftOverdrive");
    colorRight = stringValue(customColors, "colorRight");
    colorRightOverdrive = intValue(customColors, "colorRightOverdrive");
    envColorLeft = stringValue(customColors, "envColorLeft");
    envColorLeftOverdrive = intValue(customColors, "envColorLeftOverdrive");
    envColorRight = stringValue(customColors, "envColorRight");
    envColorRightOverdrive = intValue(customColors, "envColorRightOverdrive");
    obstacleColor = stringValue(customColors, "obstacleColor");
    obstacleColorOverdrive = intValue(customColors, "obstacleColorOverdrive");

    // customData.editorSettings.modSettings.mappingExtensions:
    mappingExtensionsEnabled = boolValue(mappingExtensions, "isEnabled");
    numRows = intValue(mappingExtensions, "numRows");
    numCols = intValue(mappingExtensions, "numCols");
    colWidth = intValue(mappingExtensions, "colWidth");
    rowHeight = intValue(mappingExtensions, "rowHeight");

    // Beatmap sets.
    static nlohmann::json difficultySetsJson = jsonValue(json, "_difficultyBeatmapSets");
    difficultySets.fromJSON(difficultySetsJson);
}

void SongInfo::toJSON(nlohmann::json & json) const {
    json["_version"] = version;

    json["_songName"] = songName;
    json["_songSubName"] = songSubName;
    json["_songAuthorName"] = songAuthorName;
    json["_levelAuthorName"] = levelAuthorName;
    json["_beatsPerMinute"] = beatsPerMinute;
    json["_songTimeOffset"] = songTimeOffset;
    json["_shuffle"] = shuffle;
    json["_shufflePeriod"] = shufflePeriod;
    json["_previewStartTime"] = previewStartTime;
    json["_previewDuration"] = previewDuration;
    json["_songFilename"] = songFilename;
    json["_coverImageFilename"] = coverImageFilename;
    json["_environmentName"] = environmentName;

    nlohmann::json customData = nlohmann::json::object();
    nlohmann::json editorSettings = nlohmann::json::object();
    nlohmann::json modSettings = nlohmann::json::object();
    nlohmann::json customColors = nlohmann::json::object();
    nlohmann::json mappingExtensions = nlohmann::json::object();
    nlohmann::json difficultySetsJson = nlohmann::json::array();

    json["_customData"] = customData;
    customData["_editor"] = editor;
    customData["_editorSettings"] = editorSettings;
    editorSettings["modSettings"] = modSettings;
    modSettings["customColors"] = customColors;
    modSettings["mappingExtensions"] = mappingExtensions;

    customColors["isEnabled"] = customColorsEnabled;
    customColors["colorLeft"] = colorLeft;
    customColors["colorLeftOverdrive"] = colorLeftOverdrive;
    customColors["colorRight"] = colorRight;
    customColors["colorRightOverdrive"] = colorRightOverdrive;
    customColors["envColorLeft"] = envColorLeft;
    customColors["envColorLeftOverdrive"] = envColorLeftOverdrive;
    customColors["envColorRight"] = envColorRight;
    customColors["envColorRightOverdrive"] = envColorRightOverdrive;
    customColors["obstacleColor"] = obstacleColor;
    customColors["obstacleColorOverdrive"] = obstacleColorOverdrive;

    difficultySets.toJSON(difficultySetsJson);
    json["_difficultyBeatmapSets"] = difficultySetsJson;
}



//======================================================================
// SongDifficulty and its vector
//======================================================================
void SongDifficulty::fromJSON(const nlohmann::json & json) {
    difficulty = toLevelDifficulty(stringValue(json, "_difficulty"));
    difficultyRank = intValue(json, "_difficultyRank");
    beatmapFilename = stringValue(json, "_beatmapFilename");
    noteJumpMovementSpeed = intValue(json, "_noteJumpMovementSpeed");
    noteJumpStartBeatOffset = intValue(json, "_noteJumpStartBeatOffset");
}

void SongDifficulty::toJSON(nlohmann::json & json) const {
    json["_difficulty"] = levelDifficultyToString(difficulty);
    json["_difficultyRank"] = difficultyRank;
    json["_beatmapFilename"] = beatmapFilename;
    json["_noteJumpMovementSpeed"] = noteJumpMovementSpeed;
    json["_noteJumpStartBeatOffset"] = noteJumpStartBeatOffset;
}

/**
 * Read ourself from this JSON. It's expected to be an array.
 */
void SongDifficulty_Vec::fromJSON(const nlohmann::json & array) {
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        nlohmann::json obj = *iter;

        SongDifficulty * thisDiff = new SongDifficulty();
        thisDiff->fromJSON(obj);
        push_back(thisDiff);
    }
}

void SongDifficulty_Vec::toJSON(nlohmann::json & json) const {
    for (SongDifficulty *obj: *this) {
        nlohmann::json childJson = nlohmann::json::object();
        obj->toJSON(childJson);
        json.push_back(childJson);
    }
}

//======================================================================
// SongDifficultySet and its vector
//======================================================================


/**
 * Read ourself from this JSON.
 */
void SongDifficultySet::fromJSON(const nlohmann::json & json) {
    beatmapCharacteristicName = stringValue(json, "_beatmapCharacteristicName");
    nlohmann::json difficultBeatmapsJSON = jsonValue(json, "_difficultyBeatmaps");
    difficulties.fromJSON(difficultBeatmapsJSON);
}

/**
 * Output to JSON.
 */
void SongDifficultySet::toJSON(nlohmann::json & json) const {
    json["_beatmapCharacteristicName"] = beatmapCharacteristicName;
    nlohmann::json difficultBeatmapsJSON = nlohmann::json::array();
    difficulties.toJSON(difficultBeatmapsJSON);
    json["_difficultyBeatmaps"] = difficultBeatmapsJSON;
}

/**
 * Read ourself from this JSON.
 */
void SongDifficultySet_Vec::fromJSON(const nlohmann::json & array) {
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        nlohmann::json obj = *iter;

        SongDifficultySet * diffSet = new SongDifficultySet();
        diffSet->fromJSON(obj);
        push_back(diffSet);
    }
}

/**
 * Output to JSON.
 */
void SongDifficultySet_Vec::toJSON(nlohmann::json & json) const {
    for (SongDifficultySet *set: *this) {
        nlohmann::json childJson = nlohmann::json::object();
        set->toJSON(childJson);
        json.push_back(childJson);
    }
}

//======================================================================
// SongData
//======================================================================

/**
 * Load from this file.
 */
void SongBeatmapData::load(const std::string &fileName) {
    string contents = readFileContents(fileName);
    nlohmann::json json = nlohmann::json::parse(contents);

    fromJSON(json);
}

void
SongBeatmapData::save(const std::string &filename) {
    JSON json;
    toJSON(json);
    ofstream output(filename);
    output << json.dump();		// Do NOT do pretty-print, as we expect a single line, and it might matter.
    output.close();
}

/**
 * Read the entire map from this JSON.
 */
void SongBeatmapData::fromJSON(const nlohmann::json & json) {
    version = stringValue(json, "_version");

    nlohmann::json eventsJson = jsonValue(json, "_events");
    nlohmann::json notesJson = jsonValue(json, "_notes");

    events.fromJSON(eventsJson);
    notes.fromJSON(notesJson);
}

/**
 * Output the entire map to JSON.
 */
void SongBeatmapData::toJSON(nlohmann::json & json) const {
    json["_version"] = version;

    nlohmann::json eventsJson = nlohmann::json::array();
    nlohmann::json notesJson = nlohmann::json::array();

    events.toJSON(eventsJson);
    notes.toJSON(notesJson);

    json["_events"] = eventsJson;
    json["_notes"] = notesJson;
}

/**
 * Run through the data and see what the largest gap is.
 */
double SongBeatmapData::largestGap(double songLength, int bpm) const {
    double retVal = 0.0;
    double timeOfLastNote = 0.0;
    double secondsPerBeat = static_cast<double>(bpm) / 60.0;
    double delta;

    for (Note * note: notes) {
        double thisTime = secondsPerBeat * note->time;

        delta = thisTime - timeOfLastNote;
        if (delta > retVal) {
            retVal = delta;
        }
        timeOfLastNote = thisTime;
    }

    delta = songLength - timeOfLastNote;
    if (delta > retVal) {
        retVal = delta;
    }

    return retVal;
}

/**
 * How many large gaps are there?
 */
int SongBeatmapData::numberLargeGaps(double songLength, int bpm) const {
    int rv = 0;
    double secondsPerBeat = static_cast<double>(bpm) / 60.0;
    double timeOfLastNote = 0.0;

    for (Note * note: notes) {
        double thisTime = secondsPerBeat * note->time;

        if (thisTime - timeOfLastNote > 5.0) {
            ++rv;
        }

        timeOfLastNote = thisTime;
    }

    if (songLength - timeOfLastNote > 5.0) {
        ++rv;
    }

    return rv;
}

int SongBeatmapData::getCutsCount(CubeType cubeType) const {
    int rv = 0;
    int cubeTypeInt = cubeTypeToInt(cubeType);

    for (Note *note: notes) {
        if (note->type == cubeTypeInt) {
            ++rv;
        }
    }
    return rv;
}

int SongBeatmapData::getUpDownCuts() const {
    int rv = 0;
    for (Note *note: notes) {
        if (note->cutDirection == NoteDirection_Up || note->cutDirection == NoteDirection_Down) {
            ++rv;
        }
    }
    return rv;
}

int SongBeatmapData::getLeftRightCuts() const {
    int rv = 0;
    for (Note *note: notes) {
        if (note->cutDirection == NoteDirection_Left || note->cutDirection == NoteDirection_Right) {
            ++rv;
        }
    }
    return rv;
}

int SongBeatmapData::getDiagonalCuts() const {
    int rv = 0;
    for (Note *note: notes) {
        if (note->cutDirection == NoteDirection_UpLeft || note->cutDirection == NoteDirection_UpRight
                || note->cutDirection == NoteDirection_DownLeft || note->cutDirection == NoteDirection_DownRight) {
            ++rv;
        }
    }
    return rv;
}

/**
 * Return the index to the next note after this time.
 */
int SongBeatmapData::indexAfter(double time) const {
    int index = notes.binarySearchForIndex( [=](Note *note) {
        double diff = (note->time - time) * 1000;
        return static_cast<int>(diff);
    }, true, 0, 0);

    return index;
}

SongBeatmapData::Note * SongBeatmapData::getNote(int index) {
    if (index >= 0 && index < notes.size()) {
        return notes.at(index);
    }
    return nullptr;
}

SongBeatmapData::Note * SongBeatmapData::getPreviousNote(int index) {
    if (index <= 0) {
        return nullptr;
    }
    Note & baseNote = *notes.at(index);

    for (int searchIndex = index - 1; searchIndex >= 0; --searchIndex) {
        Note & thisNote = *notes.at(searchIndex);
        double delta = baseNote.time - thisNote.time;

        // We only want to return it if it's at least 1/10th of a beat earlier.
        if (delta > 0.1) {
            return &thisNote;
        }
    }

    return nullptr;
}

SongBeatmapData::Note * SongBeatmapData::getNextNote(int index) {
    if (index < 0) {
        index = -1;
    }

    // Have to only compare against size if index is non-negative due to
    // automatic typecasting when comparing against an unsigned long.
    if (index >= 0 && index >= notes.size()) {
        return nullptr;
    }

    double compareToTime = index >= 0 ? notes.at(index)->time : 0.0;
    for (int searchIndex = index + 1; searchIndex < notes.size(); ++searchIndex) {
        Note & thisNote = *notes.at(searchIndex);
        double delta = thisNote.time - compareToTime;

        // We only want to return it if it's at least 1/10th of a beat later.
        if (delta > 0.1) {
            return &thisNote;
        }
    }

    return nullptr;
}

//----------------------------------------------------------------------
// Beatmap sub-objects.
//----------------------------------------------------------------------

/**
 * Read the Event from this JSON.
 */
void SongBeatmapData::Event::fromJSON(const nlohmann::json & json) {
    time = doubleValue(json, "_time");
    type = intValue(json, "_type");
    value = intValue(json, "_value");
}

/**
 * Output the Event to JSON.
 */
void SongBeatmapData::Event::toJSON(nlohmann::json &json) const {
    json["_time"] = time;
    json["_type"] = type;
    json["_value"] = value;
}

/**
 * Read the Event Vector from this JSON.
 */
void SongBeatmapData::Event_Vec::fromJSON(const nlohmann::json &array) {
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        nlohmann::json obj = *iter;

        SongBeatmapData::Event * event = new SongBeatmapData::Event();
        event->fromJSON(obj);
        push_back(event);
    }
}

/**
 * Output the Event Vector to JSON.
 */
void SongBeatmapData::Event_Vec::toJSON(nlohmann::json & json) const {
    for (SongBeatmapData::Event *event: *this) {
        nlohmann::json childJson = nlohmann::json::object();
        event->toJSON(childJson);
        json.push_back(childJson);
    }
}

/**
 * Read the Note from this JSON.
 */
void SongBeatmapData::Note::fromJSON(const nlohmann::json & json) {
    time = doubleValue(json, "_time");
    lineIndex = intValue(json, "_lineIndex");
    lineLayer = intValue(json, "_lineLayer");
    type = intValue(json, "_type");
    cutDirection = intValue(json, "_cutDirection");
}

/**
 * Output the Note to JSON.
 */
void SongBeatmapData::Note::toJSON(nlohmann::json & json) const {
    json["_time"] = time;
    json["_lineIndex"] = lineIndex;
    json["_lineLayer"] = lineLayer;
    json["_type"] = type;
    json["_cutDirection"] = cutDirection;
}

/**
 * Read the Note Vector from this JSON.
 */
void SongBeatmapData::Note_Vec::fromJSON(const nlohmann::json &array) {
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        nlohmann::json obj = *iter;

        SongBeatmapData::Note * note = new SongBeatmapData::Note();
        note->fromJSON(obj);
        push_back(note);
    }
}

/**
 * Output the Event Vector to JSON.
 */
void SongBeatmapData::Note_Vec::toJSON(nlohmann::json & json) const {
    for (SongBeatmapData::Note *note: *this) {
        nlohmann::json childJson = nlohmann::json::object();
        note->toJSON(childJson);
        json.push_back(childJson);
    }
}



}
