# Introduction
This is not yet ready for prime time! Or any time, really, but it CAN auto-generate a Beat Saber song map with a single click. Note that you can't currently build anything because I have to move some things into the library that doesn't yet exist.

If you play Beat Saber, bookmark this repo and come back in a couple of weeks.

This is Joe's Beat Saber Song Map Editor. It's written in C++. There's a CLI that should run on any UNIX-like system. The GUI is written in Qt. I've compiled for MacOS. Someone enterprising should be able to get this to work under Windows.

# Finding Binaries
If you just need the binaries, email me at jpl@showpage.org. I can only give you Mac binaries, but eventually maybe someone will build for Windows.

My goal is to provide a common location to download precompiled binaries.

# Contributing
Anyone can contribute to this project. You don't need to be a programmer. Here are some areas where I could use some real assistance. The first set is how non-programmers can help.

* More patterns. See PATTERNS.md for more information.
* Lighting patterns. I haven't touched this yet. Email me if you want to help with lighting design.
* Suggestions on how to improve flow from pattern to pattern.
* I haven't found documentation on 360-degree maps. Help?
* Any general suggestions on improvement, but understand that I have to triage.
* Any comments on how other programs handle shift when either BPM is imperfect or the song has sped up or slowed down?

Now, if you're a programmer, even if you don't know a thing about Qt.

* Windows port. Start with the library.
* Windows port of the GUI. Requires Qt.
* If you can do a better editor that looks like all the editors, that would rock.
* Work with me to make the generator better. I can envision lots of features about patterns.
* I want a built-in tapper of some sort to help determine BPM.


# Current Status
This is an initial checkin. The GUI is still crude, and the viewer is beyond weak. The generator is kind of cool.

Right now, the generator doesn't understand past context, so it can do several down-cuts in a row rather than knowing it should pick something else. It will get there.

And my code is a bit on the naive side when it comes to mapping extensions. I might very well destroy data you would like to have.

I've shifted to working through the CLI. Creating new mapped levels from the GUI may still be broken.

# The Generator
The goal is to be able to automatically map an entire song with the push of a button. The program works off the concept of patterns. For instance, a down-up cut is a pattern. The generator doesn't think about individual notes as much as it does patterns.

# Structure
The Library directory holds the none-GUI code.

* A library for manipulating the song files
* The CLI
* Test cases

The BeatPatterns directory holds the GUI. This is a Qt application.

# To Build
If you want to build or contribute as a programmer, you'll need:

* Qt Open Source Version
* Xcode command line utilities (Mac)
* cppunit for building and running the unit tests
* gmake
* These libraries: -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -lboost_filesystem

Clone the git repo. Build the library first. I don't have any autoconfigure tools yet, but maybe someone will produce them. Just run make. It works as is on Mac OS. No comment on any other environment.

If you want to put the CLI tools into /usr/local/bin and /usr/local/etc, then sudo make install. The CLI itself is not required to build the GUI, but you have to build the library.

To build the GUI, I do it from Qt Creator. Run Qt Creator and open the BeatPatterns project. Then build like you would normally. I'm sorry -- I'm not going to provide a tutorial on Qt.

# Program Structure and Qt Sometimes Sucks
In the library directory, there are some important includes with their corresponding .cpp files.

* Common.h includes a few simple structures that are used between a song and a pattern.
* Preferences.h provides a preferences system.
* Song.h provides everything for reading/writing the files that BeatSaber expects.
* Pattern.h provides everything for managing the patterns.

Note: Qt on Mac has a problem when running apps from the Qt Creator. There's an easy workaround. You have to do this once.

For inside Qt Creator, in the very left side, go to Projects (Under Welcome, Edit, etc). That gives you a secondary (wider) navigation area. Under Build and Run, under Desktop Qt (etc), click on Run.

This brings you to run settings. In the middle of the screen near the top is a grey box with some stuff. One of those checkboxes near the bottom of that grey box is "Add build library search path to DYLD_LIBRARY_PATH...".

Click that sucker OFF.

## The Generator

* Doesn't currently understand flow between patterns
* And doesn't currently use sub-beats
* I need a LOT more patterns. See the section on creating more patterns.
* It seems to be putting red cubes in column 3 far more than the random number generator suggests it should.
* It doesn't yet know a thing about lighting.

If you edit the patterns, PLEASE make sure they are still valid JSON. Use some sort of checker.

## The GUI

* I'd like the GUI to be able to edit Patterns rather than editing them manually.
* The editor section is really really lame. Did I mention how lame it is? It's not really functional, and I don't know how to do a 3D view yet.
* I don't work with custom cube colors.

# Using the GUI
Run the GUI. So far, it requires an existing map, but of course, New Map is one of the goals. So Open an existing map. It can be really sparse, but you need at least one level to work on.

Go to the Song Map in the left toolbar. The buttons along the top let you switch between the levels you have configured. Click one of the View ones (The create ones crash right now.)

You'll get some stats filled in. The bottom area is bogus, but the middle area is kind of cool.

Hit Regenerate. this will destroy your level map and fill it in. So don't do this if you actually like your map, but it's great to give you a playable map!

Then save it.

To install it on your machine, you'll need to know how to use BMBF. And you'll need to zip it yourself.

# About the Patterns files
The code expects to find patterns inside the BeatPatterns.app directory:

    BeatPatterns.app/Contents/Resources/Patterns

The build puts them there. However, it doesn't necessarily update them reliably. If you edit the file in the main project area, you might have to clean-all and rebuild to get it there.

OR there's a Copy.sh script in the BeatPatterns directory you can run.

Note that I also have a bunch of image files that get copied the same way, but there rather static, so it should just be fine. Copy.sh doesn't touch them.

# Immediate Next Steps

* CLI
  * Build the CLI. It will make the next immediate steps more efficient. Estimated 2 hours.
  * Fix creation of new difficulty layer. Estimated 30 minutes.
  * Generator: better flow between patterns. Estimated 2 hours.
  * Generator: some initial default lighting, even if it's weak. Estimated 30 minutes.
  * Save: should generate a new zip file. Estimated 30 minutes.
  * Implement New. Estimated 1 hour.

* GUI
  * Step through the map: Estimated 2 hours.
  * Autoplay through the song: Estimated 30 minutes.
  * Map Set Selection. I think the beatmap sets are for the different forms -- single saber, 90-degrees, 360-degrees, and whatever else. Estimated: 30 minutes
  * About page. Estimated 30 minutes.

* Preferences
  * Different min/max fields based on difficulty. Estimated 30 minutes.

* Packaging
  * Version. Estimated 15 minutes.
  * Store builds somewhere for easy download. Estimated 30 minutes.
