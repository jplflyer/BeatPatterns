#include "Common.h"

namespace SongEditor {

//======================================================================
// Various enum conversion methods.
//======================================================================

std::string levelDifficultyToString(LevelDifficulty ld) {
    switch(ld) {
        case LevelDifficulty::Easy: return "Easy";
        case LevelDifficulty::Normal: return "Normal";
        case LevelDifficulty::Hard: return "Hard";
        case LevelDifficulty::Expert: return "Expert";
        case LevelDifficulty::ExpertPlus: return "ExpertPlus";
        case LevelDifficulty::All: return "All";
    }
    // Won't get here.
    return "Easy";
}

LevelDifficulty toLevelDifficulty(const std::string &str) {
    if (str == "Easy") return LevelDifficulty::Easy;
    if (str == "Normal") return LevelDifficulty::Normal;
    if (str == "Hard") return LevelDifficulty::Hard;
    if (str == "Expert") return LevelDifficulty::Expert;
    if (str == "ExpertPlus") return LevelDifficulty::ExpertPlus;
    if (str == "All") return LevelDifficulty::All;

    // Better not get here.
    return LevelDifficulty::Easy;
}

std::ostream &
operator<<(std::ostream & os, const LevelDifficulty& levelDifficulty) {
    os << levelDifficultyToString(levelDifficulty);
    return os;
}


std::string patternDifficultyToString(PatternDifficulty pd) {
    switch(pd) {
        case PatternDifficulty::Easy: return "Easy";
        case PatternDifficulty::Medium: return "Medium";
        case PatternDifficulty::Hard: return "Hard";
    }

    // Won't get here.
    return "Easy";
}

PatternDifficulty toPatternDifficulty(const std::string &str) {
    if (str == "Easy") return PatternDifficulty::Easy;
    if (str == "Medium") return PatternDifficulty::Medium;
    if (str == "Hard") return PatternDifficulty::Hard;

    // Better not get here.
    return PatternDifficulty::Easy;
}

std::ostream &
operator<<(std::ostream & os, const PatternDifficulty & value) {
    os << patternDifficultyToString(value);
    return os;
}

std::string cubeTypeToString(CubeType ct) {
    switch (ct) {
        case CubeType::Red: return "Red";
        case CubeType::Blue: return "Blue";
        case CubeType::Bomb: return "Bomb";
    }
    // Won't get here.
    return "Blue";
}

int cubeTypeToInt(CubeType cubeType) {
    switch (cubeType) {
        case CubeType::Red: return 0;
        case CubeType::Blue: return 1;
        case CubeType::Bomb: return 3;		// I don't know if this is right.
    }

    // Won't get here.
    return 0;
}


CubeType toCubeType(const std::string &str) {
    if (str == "Red") return CubeType::Red;
    if (str == "Blue") return CubeType::Blue;
    if (str == "Bomb") return CubeType::Bomb;

    // Better not get here.
    return CubeType::Red;
}

CubeType swapCubeType(CubeType cubeType) {
    switch (cubeType) {
        case CubeType::Red: return CubeType::Blue;
        case CubeType::Blue: return CubeType::Red;
        case CubeType::Bomb: return CubeType::Bomb;
    }
    // Shouldn't happen.
    return cubeType;
}

std::ostream &
operator<<(std::ostream & os, const CubeType & value) {
    os << cubeTypeToString(value);
    return os;
}

//======================================================================
// Cut Directions. In our Patterns, we store strings. But in the
// .dat files for a song, they're numbers. See NoteDirection_*
// constants in Common.h.
//======================================================================

std::string cutDirectionToString(CutDirection cutDirection) {
    switch(cutDirection) {
        case CutDirection::Up:			return "Up";
        case CutDirection::Down:		return "Down";
        case CutDirection::Center:		return "Center";

        case CutDirection::Left:		return "Left";
        case CutDirection::Right:		return "Right";
        case CutDirection::UpLeft:		return "UpLeft";
        case CutDirection::UpRight:		return "UpRight";
        case CutDirection::DownLeft:	return "DownLeft";
        case CutDirection::DownRight:	return "DownRight";
    }

    // Shouldn't happen.
    return "Down";
}

int cutDirectionToInt(CutDirection cutDirection) {
    switch(cutDirection) {
        case CutDirection::Up:			return NoteDirection_Up;
        case CutDirection::Down:		return NoteDirection_Down;
        case CutDirection::Center:		return NoteDirection_None;

        case CutDirection::Left:		return NoteDirection_Left;
        case CutDirection::Right:		return NoteDirection_Right;
        case CutDirection::UpLeft:		return NoteDirection_UpLeft;
        case CutDirection::UpRight:		return NoteDirection_UpRight;
        case CutDirection::DownLeft:	return NoteDirection_DownLeft;
        case CutDirection::DownRight:	return NoteDirection_DownRight;
    }

    // Shouldn't happen.
    return NoteDirection_Down;
}

CutDirection toCutDirection(const std::string &str) {
    if (str == "Up") return CutDirection::Up;
    if (str == "Down") return CutDirection::Down;
    if (str == "Center") return CutDirection::Center;
    if (str == "Left") return CutDirection::Left;
    if (str == "Right") return CutDirection::Right;
    if (str == "UpLeft") return CutDirection::UpLeft;
    if (str == "UpRight") return CutDirection::UpRight;
    if (str == "DownLeft") return CutDirection::DownLeft;
    if (str == "DownRight") return CutDirection::DownRight;

    return CutDirection::Down;
}

CutDirection mirrorUpDown(CutDirection cutDirection) {
    switch(cutDirection) {
        // These don't mirror up/down.
        case CutDirection::Left:
        case CutDirection::Right:
        case CutDirection::Center: return cutDirection;

        // These all mirror.
        case CutDirection::Up:			return CutDirection::Down;
        case CutDirection::Down:		return CutDirection::Up;
        case CutDirection::UpLeft:		return CutDirection::DownLeft;
        case CutDirection::UpRight:		return CutDirection::DownRight;
        case CutDirection::DownLeft:	return CutDirection::UpLeft;
        case CutDirection::DownRight:	return CutDirection::UpRight;
    }

    // Shouldn't happen.
    return cutDirection;
}

CutDirection mirrorLeftRight(CutDirection cutDirection) {
    switch(cutDirection) {
        // These don't mirror.
        case CutDirection::Up:
        case CutDirection::Down:
        case CutDirection::Center: return cutDirection;

        // These all mirror.
        case CutDirection::Left:		return CutDirection::Right;
        case CutDirection::Right:		return CutDirection::Left;
        case CutDirection::UpLeft:		return CutDirection::UpRight;
        case CutDirection::UpRight:		return CutDirection::UpLeft;
        case CutDirection::DownLeft:	return CutDirection::DownRight;
        case CutDirection::DownRight:	return CutDirection::DownLeft;
    }

    // Shouldn't happen.
    return cutDirection;
}

/**
 * This function does a left/right mirror.
 */
CutDirection mirrorCutDirection(CutDirection cutDirection, bool doLeftRight, bool doUpDown) {
    if (doLeftRight) {
        cutDirection = mirrorLeftRight(cutDirection);
    }
    if (doUpDown) {
        cutDirection = mirrorUpDown(cutDirection);
    }
    return cutDirection;
}

std::ostream &
operator<<(std::ostream & os, const CutDirection & value) {
    os << cutDirectionToString(value);
    return os;
}

} // namespace SongEditor
