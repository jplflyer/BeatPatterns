#include <iostream>
#include <math.h>

#include "Generator.h"

using std::cout;
using std::endl;

namespace SongEditor {

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
}

/**
 * This version performs a generation for the entire song, throwing out anything we'd done before.
 */
void Generator::generateEntireSong() {
    beatmapData.hasChanged = true;
    beatmapData.notes.eraseAll();

    // We need to calculate the beat number for the first note. We begin with the minimum
    // white space, then we round up to the nearest whole beat.
    double timeOfFirstNote = minimumInitialWhitespace;
    double beatNumber = std::ceil(timeOfFirstNote / song.beatDurationSeconds);
    double currentTime = beatNumber * song.beatDurationSeconds;
    double remainingDuration = song.duration - currentTime;
    int nextIndex = 0;

    lastNoteBeat = 0.0;
    blueSaberLocation.resetBlue();
    redSaberLocation.resetRed();

    while (remainingDuration > 0.5) {
        int index = pickAndApplyPattern(beatmapData, nextIndex, beatNumber, remainingDuration);

        SongBeatmapData::Note & mostRecentNote = *beatmapData.notes.at(index);

        // Start time of the next pattern.
        // This doesn't support patternSnapTo yet.
        currentTime = (mostRecentNote.time * song.beatDurationSeconds) + randomValue(minimumDelayBetweenPatterns, maximumDelayBetweenPatterns);
        beatNumber = std::ceil(currentTime / song.beatDurationSeconds);

        currentTime = beatNumber * song.beatDurationSeconds;
        remainingDuration = song.duration - currentTime;
        nextIndex = index + 1;
    }
}

/**
 * This version is used to generate only over a range.
 */
void Generator::generateRange(int , int ) {

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
 * Note that this method updates lastNoteBeat, blueSaberLocation, and redSaberLocation.
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

            beatmapData.notes.insert(position++, newNote);
            ++atIndex;
        }

        // Set to the next location.
        // TODO: This needs to be way smarter.
        beatNumber += 1;
    }

    return atIndex - 1;
}

/**
 * Get the patterns we might use for the current level difficulty.
 * For now, I'm ignoring max duration.
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


} // namespace SongEditor
