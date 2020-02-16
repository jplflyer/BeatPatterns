#include <cstdlib>
#include <iostream>
#include <boost/filesystem.hpp>

#include <showpage/StringMethods.h>

#include "Pattern.h"

using std::cout;
using std::endl;
using std::string;

using JSON = nlohmann::json;

namespace BeatPatterns {

/**
 * Constructor.
 */
Pattern::Pattern() {
}

Pattern::~Pattern() {
    if (transformedPattern != nullptr) {
        delete transformedPattern;
        transformedPattern = nullptr;
    }
}

/**
 * Build a pattern from this json.
 */
void Pattern::fromJSON(const JSON & json) {
    name = stringValue(json, "name");

    difficulty = toPatternDifficulty(stringValue(json, "difficulty"));

    JSON useWeightsJSON = jsonValue(json, "useWeights");
    JSON startingLocationsJSON = jsonValue(json, "startingLocations");
    JSON noteSequenceJSON = jsonValue(json, "noteSequence");

    useWeights.fromJSON(useWeightsJSON);
    startingLocations.fromJSON(startingLocationsJSON);
    noteSequence.fromJSON(noteSequenceJSON);

    auto finder = json.find("transformation");
    if (finder != json.end()) {
        JSON transformationJSON = *finder;
        transformation.fromJSON(transformationJSON);
    }
}

/**
 * Output this pattern to this json.
 */
void Pattern::toJSON(JSON & json) const {
    JSON useWeightsJSON = JSON::object();
    JSON startingLocationsJSON = JSON::array();
    JSON noteSequenceJSON = JSON::array();

    useWeights.toJSON(useWeightsJSON);
    startingLocations.toJSON(startingLocationsJSON);
    noteSequence.toJSON(noteSequenceJSON);

    json["name"] = name;
    json["difficulty"] = patternDifficultyToString(difficulty);
    json["useWeights"] = useWeightsJSON;

    if (transformation.patternName.length() > 0) {
        JSON transformationJSON = JSON::object();
        transformation.toJSON(transformationJSON);
        json["transformation"] = transformationJSON;
    }
    else {
        json["startingLocations"] = startingLocationsJSON;
        json["noteSequence"] = noteSequenceJSON;
    }
}

/**
 * Get our weight based on this difficulty.
 */
int
Pattern::getWeight(LevelDifficulty forDifficulty) const {
    switch (forDifficulty) {
        case LevelDifficulty::Easy: return useWeights.easy;
        case LevelDifficulty::Normal: return useWeights.normal;
        case LevelDifficulty::Hard: return useWeights.hard;
        case LevelDifficulty::Expert: return useWeights.expert;
        case LevelDifficulty::ExpertPlus: return useWeights.expertPlus;
        case LevelDifficulty::All: return useWeights.easy;
    }
    return useWeights.easy;
}

/**
 * Is this a transformation?
 */
bool
Pattern::isTransformation() const {
    return transformation.pattern != nullptr;
}

/**
 * We're a transformation.
 */
Pattern *
Pattern::getTransformation() {
    // If we're not a transformation, then we return ourself.
    if (transformation.patternName.length() == 0) {
        return this;
    }

    if (transformation.pattern == nullptr) {
        cout << "Our transformation pattern is missing." << endl;
        return this;
    }

    if (transformedPattern == nullptr) {
        cout << "Produce transformation from " << transformation.patternName
             << " to produce " << name
             << (transformation.mirrorLeftRight ? " Mirror Left/Right" : "")
             << endl;

        transformedPattern = new Pattern();

        Pattern * fromPattern = transformation.pattern->getTransformation();

        // Handle the starting locations
        for (Location &loc: fromPattern->startingLocations) {
            Location newLocation;

            newLocation.preferred = loc.preferred;
            newLocation.lineLayer = loc.lineLayer;
            newLocation.lineIndex = transformation.mirrorLeftRight ? 3 - loc.lineIndex : loc.lineIndex;

            cout << "... Transform from " << loc << " to " << newLocation << endl;

            startingLocations.push_back(newLocation);
        }

        // Handle the noteSequence
        for (NoteSet &noteSet: fromPattern->noteSequence) {
            NoteSet newNoteSet;

            for (Note &note: noteSet) {
                Note newNote;

                newNote.cubeType = transformation.swapColors ? swapCubeType(note.cubeType) : note.cubeType;
                newNote.cutDirection = mirrorCutDirection(note.cutDirection, transformation.mirrorLeftRight, transformation.swapUpDown);
                newNote.relativeX = transformation.mirrorLeftRight ? -note.relativeX : note.relativeX;
                newNote.relativeY = note.relativeY;

                newNoteSet.push_back(newNote);
            }

            // Do after building as it's a copy.
            transformedPattern->noteSequence.push_back(newNoteSet);
        }
    }
    return transformedPattern;
}

/**
 * Pick one of our starting locations.
 */
void Pattern::getStartingLocation(int &lineLayer, int &lineIndex) {
    double sum = 0;

    for (Location & loc: startingLocations) {
        sum += loc.preferred ? 10 : 3;
    }

    double multiplier = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    double select = sum * multiplier;

    sum = 0;
    for (Location & loc: startingLocations) {
        sum += loc.preferred ? 10 : 3;
        if (sum >= select) {
            lineLayer = loc.lineLayer;
            lineIndex = loc.lineIndex;
            return;
        }
    }
}

//----------------------------------------------------------------------
// The vec
//----------------------------------------------------------------------

/**
 * Destructor exists just to handle vtable stuff.
 */
Pattern_Vec::~Pattern_Vec() {}

/**
  * We can load our patterns from a single file or a directory containing
  * multiple files.
  *
  * Files can have a single pattern or an array of patterns. We simply load the
  * JSON and then see if it's an object or an array.
  */
void
Pattern_Vec::load(const std::string &fileOrDirName) {
    boost::filesystem::path path(fileOrDirName);

    // Reading files.
    if (boost::filesystem::is_regular(path)) {
        string contents = readFileContents(fileOrDirName);
        JSON json = JSON::parse(contents);

        // An entire array of Patterns.
        if (json.is_array()) {
            fromJSON(json);
        }

        // A single Pattern.
        else if (json.is_object()) {
            Pattern * pattern = new Pattern();
            pattern->fromJSON(json);
            push_back(pattern);
        }
    }

    // If it's a directory, read the contents (and recurse).
    else if (boost::filesystem::is_directory(path)) {
        boost::filesystem::directory_iterator end_iter;
        for ( boost::filesystem::directory_iterator dir_itr( path ); dir_itr != end_iter; ++dir_itr ) {
            boost::filesystem::path childPath = dir_itr->path();

            // Load any files that don't begin with a dot.
            if (childPath.filename().string().at(0) != '.') {
                load(childPath.string());
            }
        }
    }
}

/**
 * This is where we map from name to the pattern and then resolve any links.
 */
void
Pattern_Vec::mapInto(std::map<string, Pattern *> &map) {
    // Put them all in the map
    for (Pattern *pattern: *this) {
        map.insert(std::pair<string, Pattern *>(pattern->name, pattern) );
    }

    // Then find any that need pointers set.
    for (Pattern *pattern: *this) {
        if (pattern->transformation.patternName.length() > 0) {
            auto pos = map.find(pattern->transformation.patternName);
            if (pos != map.end()) {
                pattern->transformation.pattern = pos->second;
            }
        }
    }
}

/**
 * This method randomly selects one of the patterns.
 */
Pattern *
Pattern_Vec::selectPattern(LevelDifficulty forDifficulty) {
    double sum = 0.0;

    for (auto iter = this->cbegin(); iter != this->cend(); iter++) {
        Pattern * const pattern = *iter;
        sum += pattern->getWeight(forDifficulty);
    }

    double multiplier = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    double select = sum * multiplier;

    Pattern * retVal = nullptr;
    sum = 0.0;
    for (auto iter = this->cbegin(); iter != this->cend(); iter++) {
        retVal = *iter;
        int thisWeight = retVal->getWeight(forDifficulty);

        sum += thisWeight;
        if (sum > select) {
            break;
        }
    }

    return retVal;
}

//======================================================================
// Transformation
//======================================================================
void Transformation::fromJSON(const JSON & json) {
    pattern = nullptr;

    patternName = stringValue(json, "patternName");
    swapColors = boolValue(json, "swapColors");
    mirrorLeftRight = boolValue(json, "mirrorLeftRight");
    swapUpDown = boolValue(json, "swapUpDown");
    duplicateCubes = boolValue(json, "duplicateCubes");
}

void Transformation::toJSON(JSON & json) const {
    json["patternName"] = patternName;

    // Only need the true values.
    if (swapColors) json["swapColors"] = swapColors;
    if (mirrorLeftRight) json["mirrorLeftRight"] = mirrorLeftRight;
    if (duplicateCubes) json["duplicateCubes"] = duplicateCubes;
    if (swapUpDown) json["swapUpDown"] = swapUpDown;
}

//======================================================================
// Allowed Level Difficultues.
//======================================================================

/**
 * We're stored as an array of strings.
 */
void UseWeights::fromJSON(const JSON & json) {
    easy = intValue(json, "easy");
    normal = intValue(json, "normal");
    hard = intValue(json, "hard");
    expert = intValue(json, "expert");
    expertPlus = intValue(json, "expertPlus");
}

/**
 * We're stored as an array of strings.
 */
void UseWeights::toJSON(JSON & json) const {
    if (easy > 0) json["easy"] = easy;
    if (normal > 0) json["normal"] = normal;
    if (hard > 0) json["hard"] = hard;
    if (expert > 0) json["expert"] = expert;
    if (expertPlus > 0) json["expertPlus"] = expertPlus;
}

//======================================================================
// Note.
//======================================================================

/**
 * Build this Note from this JSON.
 */
void Note::fromJSON(const JSON & json) {
    cubeType = toCubeType(stringValue(json, "cubeType"));
    cutDirection = toCutDirection(stringValue(json, "cutDirection"));
    relativeX = intValue(json, "relativeX");
    relativeY = intValue(json, "relativeY");
}

/**
 * Output this Note to this JSON.
 */
void Note::toJSON(JSON & json) const {
    json["cubeType"] = cubeTypeToString(cubeType);
    json["cutDirection"] = cutDirectionToString(cutDirection);
    json["relativeX"] = relativeX;
    json["relativeY"] = relativeY;
}

void
NoteSequence::toJSON(JSON & json) const {
    for (const NoteSet &noteSet: *this) {
        JSON array = JSON::array();
        noteSet.toJSON(array);
        json.push_back(array);
    }
}

//======================================================================
// Location.
//======================================================================
void Location::fromJSON(const JSON & json) {
    lineIndex = intValue(json, "lineIndex");
    lineLayer = intValue(json, "lineLayer");
    preferred = boolValue(json, "preferred");
}

void Location::toJSON(JSON & json) const {
    json["lineIndex"] = lineIndex;
    json["lineLayer"] = lineLayer;

    // Don't need to write it for false.
    if (preferred) json["preferred"] = preferred;
}

std::ostream & operator<<(std::ostream &os, const Location &loc) {
    os << "(" << loc.lineLayer << ", " << loc.lineIndex << (loc.preferred ? ", preferred)" : ")");
    return os;
}


} // namespace BeatPatterns
