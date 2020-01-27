#ifndef GENERATOR_H
#define GENERATOR_H

#include "Song.h"
#include "Pattern.h"
#include "Preferences.h"

namespace SongEditor {

/**
 * This is our smart generator for implementing patterns in our songs. I'm going to dump a lot of info
 * here.
 *
 * MUSIC NOTATION
 *		If you already play a musical instrument and can read sheet music, you know everything I'm
 * 		about to say. Skip ahead to TO USE down below. This is for people who don't know the difference
 * 		between whole notes, half notes, triplets, and the like.
 *
 * Types of Notes
 *
 *		Musical notes are written on a staff -- 5 horizontal lines across the page, then 5 more below
 * 		that, etc. You've seen this, and you've seen notes. Everyone knows what a musical note looks like.
 * 		But let's talk about that.
 *
 * 		Where the note appears defines the pitch, be it middle C or high F or whatever it is.
 * 		The exact shape of the note defines the duration. And those durations have names.
 *
 * 		A whole note is a donut -- a circle with a hole in the middle.
 *
 * 		A half note is exactly like a whole note, but it has a flag staff pointing into the air.
 * 		A half note lasts exactly half as long as a whole note.
 *
 * 		A quarter note is like a half note, but the circle is filled in. A quarter note is half a
 * 		half note or 1/4th as long as a whole note. Or there are 4 quarter notes to make up a whole
 * 		note.
 *
 * 		An eighth note looks like a quarter note but there is a flag waving on the top of the staff.
 * 		Eight eighth notes make up a whole note. In addition, these can be written together (2 of them)
 * 		and their flags are connected -- the flag from one connected to the top of the staff of the next.
 *
 * 		There are also sixteenth and even thirty-secondth notes, but you don't see them much.
 *
 * 		One more concept. If you add a little dot after a note, it lasts 50% longer. So a dotted
 * 		quarter note lasts as long as 3 eighth notes instead of two.
 *
 * Time Signature
 *
 *		This section is why I wrote the section above.
 *
 * 		Music is written in measures. If you go look at some sheet music, you'll see that there are vertical
 * 		lines written across the staff periodically -- every 4 quarter notes in most music. Those lines mark
 * 		out measures. Think of a measure as a phrase of music.
 *
 * 		To confuse it, most modern music actually uses two measures per phrase.
 *
 * 		So... to understand what I'm saying, play some rock music. (I'm an old fart and can't comment
 * 		on rap and other styles from the last 20 years.) Pick some Rod Stewart or something. Now, count
 *		the beats in your head. You'll probably say 1-2-3-4, 1-2-3-4. If you're a dancer, you MIGHT
 * 		say 1-2-3-4, 5-6-7-8.
 *
 * 		And if you're a swing dancer, you might say 1-and-2, 3-and-4, 5, 6, 7-and-8. The "ands" refer
 * 		to eighth notes instead of quarter notes.
 *
 * 		If you're a Waltz dancer and play some Waltz, you'll count 1-2-3, 1-2-3.
 *
 * 		Why am I talking about this?
 *
 * 		Music has a time signature -- normally appearing as two numbers, one on top of the other. Rock
 * 		music would be 4-4 (displayed vertically). Waltz is 3-4. 4-4 time is referred to as Common time.
 * 		2-4 is called Cut time. What does this mean.
 *
 * 		The bottom number describes what portion of a whole note defines one beat. So in 4-4 or 3-4
 * 		time, each uses a quarter note to define one beat. So 4-4 time has 4 beats per measure. 3-4
 * 		time has 3 beats per measure.
 *
 * 		And this is why we count 1-2-3-4, 1-2-3-4. Or 1-2-3 for waltz music.
 *
 * Why Does This Matter?
 *
 * 		It might not.
 *
 * 		Beat Saber wants to know the Beats Per Minute. It doesn't give a crap about time signatures or
 * 		quarter notes or what not. It just wants to know how quickly to throw cubes at you. So... you
 * 		pick the BPM. You can find a tool that lets you tap out the beats as you play the song, and they'll
 * 		tell you the BPM. Songs range from a very slow BPM of about 60 to a very high of around 220. That's
 * 		insanely high.
 *
 * 		You can arrange cubes so they only arrive on the beat. Or you can also arrange cubes so they arrive in the
 * 		middle of a beat. This happens if you have a fast down-up combo. And streams can be a bunch in a row.
 *
 * 		But the generator works on the concept of beats, and I wanted you to have all this background so you
 * 		understand some of the fields later.
 *
 * TO USE
 *
 * 		First, open your song and create or view the difficulty map. For this discussion, let's say you're
 * 		mapping Your Favorite Song on Hard Difficulty. You've determined there are 102 beats per minute.
 * 		This information is entered into your song details. You gave it your .ogg or .egg file. You're
 *		now ready to map (or edit your map) using this pattern generator.
 *
 * 		At this point, instantiate a Generator. Pass it the song, the difficulty, and the
 * 		beatmap data.
 *
 * 		The Generator remembers that and gathers some defaults from the Preferences. See the specific
 * 		comments embedded below about those defaults.
 *
 * 		You can then override those defaults as you prefer, or just leave them.
 *
 * 		Then -- run one of the generate methods. See the comments for each of them.
 */
class Generator
{
private:
    /** We track the left and right saber locations after each pattern we apply,
     * and we can use that determine a reasonable next pattern. We wouldn't want
     * a low upslash right after a high upslash. */
    class SaberLocation {
    public:
        int row = 1;
        int col = 2;

        // These methods reset where we think we are, sabers out in front, side-by-side.
        void resetBlue() { row = 1; col = 2; }
        void resetRed() { row = 1; col = 1; }
    };

private:
    Song &	song;
    SongDifficulty & difficulty;
    SongBeatmapData & beatmapData;

    int		patternSnapTo = 1;
    double	minimumInitialWhitespace = 4.0;
    double	minimumDelayBetweenPatterns = 0.05;
    double	maximumDelayBetweenPatterns = 4.0;

    /** This is the time of the last cut. */
    double lastNoteBeat;

    /** This is where we think the blue saber is. */
    SaberLocation blueSaberLocation;

    /** This is where we think the red saber is. */
    SaberLocation redSaberLocation;

    /** Returns the index of the last note added. */
    int pickAndApplyPattern(SongBeatmapData &output, int atIndex, double atBeat, double maxDuration);

    void possiblePatterns(Pattern_Vec &vec, double maxPatternDuration);


public:
    /** You create a generator to work on a particular map. */
    Generator(Song &_song, SongDifficulty &_difficulty, SongBeatmapData &_data);

    // Methods for overriding preferences

    int getPatternSnapTo() const { return patternSnapTo; }
    double getMinimumInitialWhitespace() const { return minimumInitialWhitespace; }
    double getMinimumDelayBetweenPatterns() const { return minimumDelayBetweenPatterns; }
    double getMaximumDelayBetweenPatterns() const { return maximumDelayBetweenPatterns; }

    /**
     * New patterns will snap forward. patternSnapTo indicates the granularity.
     * Set it to 1, and the next pattern while start on a fresh beat, regardless of
     * where the last pattern left off. Setting it to 2 means it can start on a
     * full beat or a half beat. 3 means full, one third, or two thirds.
     *
     * Generally speaking, leave this as one.
     */
    Generator & setPatternSnapTo(int value) { patternSnapTo = value; return *this; }

    /**
     * This is only used when generating an entire song, or generating from the
     * beginning. This is the amount of delay before cubes should arrive. 4 seconds
     * seems good. The first pattern will always start on a beat after this delay,
     * regardless of patternSnapTo.
     */
    Generator & setMinimumInitialWhitespace(double value) { minimumInitialWhitespace = value; return *this; }

    /**
     * This is in seconds and so it's tied to BPM, which we get from the Song's info.
     * If BPM is 120, then each beat is a half second. Assume patternSnapTo of 1.
     * If the previous pattern landed in the middle of the last beat, then we would
     * start the next pattern on the next beat.
     *
     * But for normal, this might be too fast. So this is the minimum number of
     * seconds. For normal, 2 seconds is a good value. For expert+, a quarter of a
     * second might be enough.
     *
     * We populate this value from preferences for the difficulty. You can override.
     */
    Generator & setMinimumDelayBetweenPatterns(double value) {
        minimumDelayBetweenPatterns = value;
        return *this;
    }

    /**
     * This is the opposite of the minimum delay above. This is the longest period.
     * You probably don't want 10 second delays between auto-generated patterns.
     * When moving from pattern to pattern, we'll pick a random period between the
     * minimum and maximum then do a snap to the nearest value indicated by
     * patternSnapTo (snapping forward if we'll be too fast or snapping backwards
     * if we'll be too long).
     *
     * This value should be sufficiently more than the minimum to give us flexibility.
     * For Normal, 4 seconds is probably good. For expert, maybe 2 seconds.
     */
    Generator & setMaximumDelayBetweenPatterns(double value) {
        maximumDelayBetweenPatterns = value;
        return *this;
    }

    /**
     * Generate the entire song. This destroys the existing notes from the
     * SongBeatmapData and generates starting fresh.
     */
    void generateEntireSong();

    /**
     * Generate for a range of the song. We retain the two referenced
     * notes and throw away everything between them.
     */
    void generateRange(int startingIndex, int endingIndex);
};


} // namespace SongEditor

#endif // GENERATOR_H
