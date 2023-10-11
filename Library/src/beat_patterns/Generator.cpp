#include <iostream>
#include <math.h>

#include "Generator.h"

using std::cout;
using std::endl;

namespace BeatPatterns {

/**
 * Return a random number between these two values.
 */
static double randomValue(double a, double b) {
    double random = rand() /  RAND_MAX;
    double diff = b - a;
    double r = random * diff;

    return a + r;
}

/**
 * Constructor.
 */
Generator::Generator(Song &_song, SongDifficulty &_difficulty, SongBeatmapData &_data)
    : song(_song), difficulty(_difficulty), beatmapData(_data)
{
    Preferences::DifficultyDefaults & difficultyDefaults = Preferences::getDifficultyDefaults(difficulty.difficulty);

    minimumInitialWhitespace = difficultyDefaults.minimumInitialWhitespace;
    minimumDelayBetweenPatterns = difficultyDefaults.minimumDelayBetweenPatterns;
    maximumDelayBetweenPatterns = difficultyDefaults.maximumDelayBetweenPatterns;

    song.fixBeatDuration();
}

/**
 * This version performs a generation for the entire song, throwing out anything we'd done before.
 */
void Generator::generateEntireSong() {
    beatmapData.hasChanged = true;
    beatmapData.notes.eraseAll();

    // We need to calculate the beat number for the first note. We begin with the minimum
    // white space, then we round up to the nearest whole beat.
    timeOfFirstNote = minimumInitialWhitespace;
    beatNumber = std::ceil(timeOfFirstNote / song.beatDurationSeconds);
    currentTime = beatNumber * song.beatDurationSeconds;
    remainingDuration = song.duration - currentTime;
}

/**
 * This version is used to generate only over a range.
 */
void Generator::generateRange(int beatStart, int beatEnd) {
}

/**
 * This method gets called after we've figured out what we're doing. Call this to actually iterate over generation.
 */
void
Generator::generateUntilDone() {
    blueSaberLocation.reset();
    redSaberLocation.reset();

    while (remainingDuration > 0.5) {
        pickAndApplyPattern(beatmapData, -1, beatNumber, remainingDuration);

        SongBeatmapData::Note & mostRecentNote = *beatmapData.notes.back();

        // Start time of the next pattern.
        // This doesn't support patternSnapTo yet.
        currentTime = (mostRecentNote.time * song.beatDurationSeconds) + randomValue(minimumDelayBetweenPatterns, maximumDelayBetweenPatterns);
        beatNumber = std::ceil(currentTime / song.beatDurationSeconds);

        currentTime = beatNumber * song.beatDurationSeconds;
        remainingDuration = song.duration - currentTime;
    }
}

/**
 * This picks an appropriate pattern and applies it. Various constraints.
 *
 * 1. The starting time + duration of the pattern can not exceed maxDuration.
 * 		Note for inserting patterns in the middle, maxDuration should also take into consideration
 * 		white space before the next pattern that's already in the song data.
 *
 * 2. The new pattern can not begin in a bad place based on where we think the two
 * 		sabers currently are. However, the longer it's been since the last pattern
 * 		ended, the more wriggle room we allow. This is modifed by difficulty level.
 *
 */
int
Generator::pickAndApplyPattern(SongBeatmapData &output, int atIndex, double beatNumber, double maxDuration) {
    Pattern_Vec		patterns(false);
    Pattern *		pattern = nullptr;

    possiblePatterns(patterns, maxDuration);
    pattern = patterns.selectPattern(difficulty.difficulty);

    // This shouldn't happen, but if it does...
    if (pattern == nullptr) {
        if (patterns.size() > 0) {
            pattern = patterns.at(0);
        }
        else {
            pattern = Preferences::getPatterns().at(0);
        }
    }

    // Now we begin to apply it.
    if (pattern->isTransformation()) {
        pattern = pattern->getTransformation();
    }

    int lineLayer = 0;
    int lineIndex = 2;
    double stepBy = pattern->stepByFor(difficulty.difficulty, song.info.beatsPerMinute);

    if (stepBy <= 0.0) {
        stepBy = 1.0;
    }

    pattern->getStartingLocation(lineLayer, lineIndex);

    auto position = output.notes.begin();
    position += atIndex;

    for (NoteSet & noteSet: pattern->noteSequence) {
        for (Note & note: noteSet) {
            SongBeatmapData::Note * newNote = new SongBeatmapData::Note();

            newNote->time = beatNumber;
            newNote->type = cubeTypeToInt(note.cubeType);
            newNote->cutDirection = cutDirectionToInt(note.cutDirection);
            newNote->lineIndex = lineIndex + note.relativeX;
            newNote->lineLayer = lineLayer + note.relativeY;

            if (note.cubeType == CubeType::Blue) {
                this->blueSaberLocation.apply(*newNote, beatNumber);
            }
            else if (note.cubeType == CubeType::Red) {
                this->redSaberLocation.apply(*newNote, beatNumber);
            }

            if (atIndex == -1) {
                beatmapData.notes.push_back(newNote);
            }
            else {
                beatmapData.notes.insert(position++, newNote);
                ++atIndex;
            }
        }

        // Set to the next location.
        beatNumber += stepBy;
    }

    // Make sure our next pattern starts on the next beat and isn't insanely close.
    // This is a little arbitrary, and I'll have to see how it works. I may want to
    // add another value to the pattern that helps calculate this.
    double nextBeatNumber = ceil(beatNumber);
    if (nextBeatNumber - beatNumber < (stepBy * 2.0)) {
        nextBeatNumber += 1.0;
    }
    beatNumber = nextBeatNumber;

    return atIndex - 1;
}

/**
 * Get the patterns we might use for the current level difficulty.
 * For now, I'm ignoring max duration.
 *
 * I exclude patterns that do not flow well from the current location and inertia.
 * Rules:
 *
 * 	1. If a particular saber is moving in a particular direction, don't pick a
 *		pattern with a similar initial direction below Hard level.
 *
 *  2. Don't pick a pattern that starts in the current location.
 */
void
Generator::possiblePatterns(Pattern_Vec &vec, double ) {
    for (Pattern *pattern: Preferences::getPatterns()) {
        int weight = pattern->getWeight(difficulty.difficulty);
        if (weight > 0) {
            vec.push_back(pattern);
        }
    }
}


} // namespace BeatPatterns
