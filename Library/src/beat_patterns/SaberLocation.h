#ifndef SABERLOCATION_H
#define SABERLOCATION_H

#include "Song.h"

/** We track the left and right saber locations after each pattern we apply,
 * and we can use that determine a reasonable next pattern. We wouldn't want
 * a low upslash right after a high upslash. */

namespace BeatPatterns {

class SaberLocation {
public:
    SaberLocation();

    double lastSliceBeat = 0.0;
    int row = 1;
    int col = 2;
    CutDirection lastCutDirection = CutDirection::Center;

    void apply(const SongBeatmapData::Note &note, double beatNumber);
    void reset() { row = 1; col = 2; lastSliceBeat=0.0; lastCutDirection = CutDirection::Center; }
};

} // namespace BeatPatterns

#endif // SABERLOCATION_H
