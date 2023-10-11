#include "SaberLocation.h"

namespace BeatPatterns {

/**
 * Constructor.
 */
SaberLocation::SaberLocation() {
}

/**
 * Apply the effects of this note to our location.
 */
void
SaberLocation::apply(const SongBeatmapData::Note &note, double beatNumber) {
    row = note.lineLayer;
    col = note.lineIndex;
    lastCutDirection = toCutDirection(note.cutDirection);
    lastSliceBeat = beatNumber;
}

} // namespace BeatPatterns
