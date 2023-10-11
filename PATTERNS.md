# Beat Patterns
This program works on the concept of a pattern. For instance, one down-cut is a pattern. Down-up is another pattern. Side-by-side red/blue cuts is a pattern. You get the idea.

Patterns that automatically ship with the program are in the BeatSaber/Patterns directory. Currently there's Basic.json, but we can add as many as we want.

The files are in JSON, but total non-programmers can help provide new patterns. First, you can try to figure out the JSON. It isn't too hard. I'll talk about it below. Or see the section on "I Can't Do JSON". So please, if you want to contribute patterns, don't be afraid of the techie stuff.

# The JSON
Okay, now that I said that, here's some techie stuff. Each JSON file should contain an array of patterns:

    Foo.json:
    [
      {
        ....
      },
      {
        ....
      }
    ]

That is, a JSON array.

Here is the first pattern from Basic.json:

    {
      "name": "Down",
      "difficulty": "Easy",
      "useWeights": { "easy": 500, "normal": 500, "hard": 400, "expert": 200, "expertPlus": 100},
      "startingLocations": [
        { "lineIndex": 0, "lineLayer": 0},
        { "lineIndex": 1, "lineLayer": 0},
        { "lineIndex": 2, "lineLayer": 0, "preferred": true},
        { "lineIndex": 3, "lineLayer": 0, "preferred": true},

        { "lineIndex": 0, "lineLayer": 1},
        { "lineIndex": 3, "lineLayer": 1, "preferred": true},

        { "lineIndex": 0, "lineLayer": 2},
        { "lineIndex": 1, "lineLayer": 2},
        { "lineIndex": 2, "lineLayer": 2},
        { "lineIndex": 3, "lineLayer": 2}
      ],
      "noteSequence": [
        [
          { "cubeType": "Blue", "cutDirection": "Down"}
        ]
      ]
    }

This is a simple down slice. It's set up for a blue, but we automatically create a red version that is mirrored. Here are the parts:

    * Name: This is just the name of the pattern. Must be unique.
    * Difficulty: Not really using this yet, but values are Easy, Medium, and Hard. This does not correspond at all to level difficulty.
    * Use Weights: I'll talk more below, but this basically says "how often should we use this pattern".
    * Starting Locations: Where should the first cube be? In this example, we list everywhere except directly in front of our eyes, but some locations are preferred more than others. When we use this pattern, we'll automatically select one of the locations.
    * Note Sequence: The sequence of cuts that determine this pattern. This is an array of arrays. Each of the inner arrays will have all the notes that should be cut at one time. In this case, we have a single blue note. If you had a red and blue note together, there would be two entries.

## Use Weights
You can weight a pattern for the difficulty level. A pattern that should never appear in easy should have the easy difficulty set to zero -- and we won't use it. If you want a pattern to appear, but not too often, give it a smaller value. if you want it to appear a lot, give it a bigger value. Generally speaking, stick to values between 0 and 1000, with 500 being average. If you're putting this pattern into your own library of patterns and want it pretty much all the time, feel free to give it a really big value, but if you send them to me, I'll tone it way, way down.

Clearly, very easy patterns, like a single slice, will be weighted low for the higher levels, but not be "never". Similarly, very difficult patterns will be weighted low for the easier levels.

## Starting Locations
lineLayer goes from 0 (bottom row) to 2 (top row).

lineIndex goes from 0 (left) to 3 (right).

Avoid putting notes directly in front of the player's eyes, as doing so will block notes behind it. Thus, my patterns rarely to never have notes in layer: 1, indexes 1 and 2.

## Notes and Note Sequences
An individual note has these fields:

    * cubeType: Blue or Red
    * cutDirection: Up, Down, Center, Left, Right, UpLeft, UpRight, DownLeft, DownRight
    * relativeX: the X-offset from the initial note.
    * relativeY: the Y-offset from the initial note.

CutDirection Center means a dot-note.

RelativeX and RelativeY are how notes are placed in the pattern. Because the pattern can start anywhere you provide, you can't provide absolute locations for the rest of the pattern. Instead, you specify an offset.

For instance, a red-blue side-by-side combination would look like:

    [
      [
         { "cubeType": "Blue", "cutDirection": "Down" },
         { "cubeType": "Red", "cutDirection": "Down", relativeX: -1, relativeY: 0 }
      ]
    ]

And an up-down pattern across two beats would be:

    [
      [
         { "cubeType": "Blue", "cutDirection": "Down" }
      ],
      [
         { "cubeType": "Blue", "cutDirection": "Up", relativeX: 0, relativeY: 0 }
      ]
    ]

In the first example, the Red cube has a relativeX of -1, meaning it's one position to the left of the initial location. The Blue cube has no relativeX or Y, so they get the default of 0.

In the second location, I've specified the relativeX and Y of zero, so it's down and then up through the same location.

# I Can't Do JSON
Okay, so you have your favorite patterns, but all this really scares you. If you email me at jpl@showpage.org with this information, I can convert to JSON:

Name: <What do you want it called>
Relative Difficulty: Easy, Medium, Hard
Weights: <5 values or skip this and I'll assign some myself>
Starting Locations: <see below>
Sequence: <see below>

## Starting Locations
I need a list of possible places for the first note. It should be rows and columns, where 0 is the bottom row, 2 is the top row, 0 is the leftmost column, and 3 is the rightmost column. For instance:

    Starting Locations: (0,0), (0, 3)

That would mean put it in one of the two lowest corners

    Starting Locations: (0,1), (1,2)

Bottom row, either centerish spots.

## Sequence
To produce the sequence, it's good if you start with a sheet of paper and draw a little grid, 3 rows high, 4 columns wide. Pick one of your starting locations. That's where the first note should go. After that, for every other cut, decide if it's left or right, up or down from that first cut. Then put one beat per line, so your email might look like this:

Name: My Great Pattern
Relative Difficulty: Medium
Weights 0, 100, 300, 500, 700
Starting Locations: (0,2)
Sequence: Blue Down
Blue Up 0 0, Red Down 0, -1
Blue Right 0 1, Red UpLeft 0, -2

This sequence means:

    1. Put the first note a blue down cut at the starting location.
    2. For the next beat (the next line), one blue up cut right where the first one was, and a red down cut just to the left of it
    3. For the third and final beat, a blue right cut to the right of the starting location, and a red up-left cut two positions to the left.

 |     |  0  |  1  |  2  |  3  |
 | --- | --- | --- | --- | --- |
 |  2  |     |     |     |     |
 |  1  |     |     |     |     |
 |  0  |     |     |  Bv |     |

 |     |  0  |  1  |  2  |  3  |
 | --- | --- | --- | --- | --- |
 |  2  |     |     |     |     |
 |  1  |     |     |     |     |
 |  0  |     | Rv  |  B^ |     |

 |     |  0  |  1  |  2  |  3  |
 | --- | --- | --- | --- | --- |
 |  2  |     |     |     |     |
 |  1  |     |     |     |     |
 |  0  | \R  |     |     |  B> |

 This is probably a poor pattern. But you get the idea. If you're only sending me a small number of patterns, you can be informal, but if you're sending me a bunch, then I need to be able to parse it with a little program, so the data should be something I can predict.
