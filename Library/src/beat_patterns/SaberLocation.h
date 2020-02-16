#ifndef SABERLOCATION_H
#define SABERLOCATION_H

/** We track the left and right saber locations after each pattern we apply,
 * and we can use that determine a reasonable next pattern. We wouldn't want
 * a low upslash right after a high upslash. */

namespace BeatPatterns {

class SaberLocation {
public:
    SaberLocation();

    int row = 1;
    int col = 2;

    // What direction were the sabers moving after the last slice?
    int dirRow = 0;
    int dirCol = 0;

    // The longer since we did a slice, the less we care where we were, as the player has had
    // time to anticipate.
    double lastSliceBeat;

    // These methods reset where we think we are, sabers out in front, side-by-side.
    void reset() { row = 1; col = 2; dirRow = 0; dirCol = 0; }
};

} // namespace BeatPatterns

#endif // SABERLOCATION_H
