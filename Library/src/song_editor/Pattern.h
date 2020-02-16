#ifndef PATTERN_H
#define PATTERN_H

#include <iostream>
#include <vector>
#include <map>

#include <showpage/JSON_Serializable.h>
#include <showpage/PointerVector.h>

#include "Common.h"

//
// Definitions of the various slice patterns we understand. Patterns are laoded
// from a resource file in our install location. The user can also add custom
// patterns.
//
namespace SongEditor {

// Forward reference
class Pattern;


/**
 * We use a weighting system to manage how frequently a pattern is used.
 * It can vary by difficulty. Easy maps use a lot of single stroke patterns. Harder
 * maps use more complex patterns and will have only a few single strokes.
 *
 * For the values, use:
 *
 * 		0:		never use at this difficulty
 * 		500:	an average value
 * 		1000:	twice as often
 *
 * You can fine-tune all you want. If you set somethign to a very large number, it's
 * going to get used constantly. If you set something to a 5, you might see it every
 * 100 songs you build.
 */
class UseWeights: public JSON_Serializable {
public:
    int easy = 500;
    int normal = 500;
    int hard = 500;
    int expert = 500;
    int expertPlus = 500;

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

/**
 * This defines one note.
 *
 * There are 9 slash directions:
 *
 *  4 0 5
 *  2 8 3
 *  6 1 7
 *
 * Advanced mapping tools allow variable numbers. I'm not supporting those yet.
 */
class Note: public JSON_Serializable {
public:
    CubeType		cubeType;
    CutDirection	cutDirection;
    int				relativeX;		// This is relative from the pattern's initial start
    int				relativeY;		// This is relative from the pattern's initial start

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

/**
 * This defines several notes all at the same point in time.
 */
class NoteSet: public JSON_Serializable_Vector<Note> {
};

/**
 * This defines a sequence of notes that define an extended pattern.
 */
class NoteSequence: public JSON_Serializable_Vector<NoteSet> {
public:
    // Must override because the standard one doesn't realize we're a vector of vectors.
    void toJSON(nlohmann::json & json) const;
};

/**
 * A location.
 */
class Location: public JSON_Serializable {
public:
    /**
     * For now, we'll stick to the standard -- no extensions. Range is 0 (leftmost)
     * to 3 (rightmost).
     */
    int lineIndex;

    /**
     * Height. 0 (bottom) to 2 (top);
     */
    int lineLayer;

    /**
     * When this Location is used for a starting location, is this one of the
     * preferred (versus okay, but use sparingly) locations?
     */
    bool preferred;

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

std::ostream& operator<<(std::ostream &, const Location &);

/**
 * A set of locations, probably used for starting locations.
 */
class Location_Vec: public JSON_Serializable_Vector<Location> {
};

/**
 * This represents transformation information. A pattern can be a transformation
 * from another pattern.
 */
class Transformation: public JSON_Serializable {
public:
    /** What is the name of the pattern we transform from? */
    std::string		patternName;

    /** A pointer to it, resolved when we're done loading everything. */
    Pattern *		pattern = nullptr;

    /** Is this pattern based on swapping red and blue cubes? */
    bool			swapColors = false;

    /** Do we swap everything left-for-right. This means locations and cut directions. */
    bool			mirrorLeftRight = false;

    /** This swaps arrow directions from upwards to downwards but does not adjust locations. */
    bool			swapUpDown = false;

    /** When we set this, the pattern keeps the cubes of the original but then
     * ALSO does a color & mirror, so it's a merge of the original pattern
     * plus the transformation. Thie can lead to complicated, busy patterns. */
    bool			duplicateCubes = false;

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;
};

/**
 * The full pattern.
 */
class Pattern: public JSON_Serializable
{
private:
    Pattern *			transformedPattern = nullptr;

public:
    std::string			name;
    PatternDifficulty	difficulty;
    UseWeights			useWeights;

    Location_Vec		startingLocations;
    NoteSequence		noteSequence;

    Transformation		transformation;

public:
    Pattern();
    virtual ~Pattern();

    void fromJSON(const nlohmann::json & json);
    void toJSON(nlohmann::json & json) const;

    int getWeight(LevelDifficulty forDifficulty) const;
    bool isTransformation() const;
    Pattern * getTransformation();

    void getStartingLocation(int &lineLayer, int &lineIndex);

    bool compatibleWithSaberLocations(SaberLocation &redLocation, SaberLocation &blueLocation);
};

class Pattern_Vec: public JSON_Serializable_PointerVector<Pattern> {
public:
    Pattern_Vec() {}
    Pattern_Vec(bool v): JSON_Serializable_PointerVector<Pattern>(v) { }
    virtual ~Pattern_Vec();

    void load(const std::string &fileOrDirName);
    void mapInto(std::map<std::string, Pattern *> & map);

    Pattern * selectPattern(LevelDifficulty forDifficulty);
};


}	// namespace SongEditor

#endif // PATTERN_H
