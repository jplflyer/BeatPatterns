#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>

//
// This has a bunch of common classes.
//

namespace BeatPatterns {

const int NoteType_Red = 0;
const int NoteType_Blue = 1;

const int NoteDirection_Up = 0;
const int NoteDirection_Down = 1;
const int NoteDirection_Left = 2;
const int NoteDirection_Right = 3;
const int NoteDirection_UpLeft = 4;
const int NoteDirection_UpRight = 5;
const int NoteDirection_DownLeft = 6;
const int NoteDirection_DownRight = 7;
const int NoteDirection_None = 8;

/** This matches difficulty levels inside Beast Saber. */
enum class LevelDifficulty { Easy, Normal, Hard, Expert, ExpertPlus, All};
std::string levelDifficultyToString(LevelDifficulty);
LevelDifficulty toLevelDifficulty(const std::string &);
std::ostream & operator<<(std::ostream & os, const LevelDifficulty& levelDifficulty);

/** This is simply used for how difficult a particular pattern is. */
enum class PatternDifficulty { Easy, Medium, Hard};
std::string patternDifficultyToString(PatternDifficulty);
PatternDifficulty toPatternDifficulty(const std::string &);
std::ostream & operator<<(std::ostream & os, const PatternDifficulty& patternDifficulty);

/** The types of things we can place on the page in a specific cell. */
enum class CubeType {
    Red,		// The left hand saber color
    Blue,		// The right hand saber color
    Bomb		// Don't slash me!
};
std::string cubeTypeToString(CubeType);
int cubeTypeToInt(CubeType);
CubeType toCubeType(const std::string &);
CubeType swapCubeType(CubeType);
std::ostream & operator<<(std::ostream & os, const CubeType & value);

/** Cut directions */
enum class CutDirection {
    Up, Down, Left, Right, UpLeft, UpRight, DownLeft, DownRight, Center
};
std::string cutDirectionToString(CutDirection);
int cutDirectionToInt(CutDirection);
CutDirection toCutDirection(const std::string &);
CutDirection toCutDirection(int);

CutDirection mirrorCutDirection(CutDirection, bool mirrorLeftRight = true, bool mirrorUpDown = false);
std::ostream & operator<<(std::ostream & os, const CutDirection & value);

} // namespace BeatPatterns;

#endif // COMMON_H
