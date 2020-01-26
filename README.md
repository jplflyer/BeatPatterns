# Introduction
This is not yet ready for prime time! Or any time, really, but it CAN auto-generate a Beat Saber song map with a single click.

This is Joe's Beat Saber Song Map Editor. It's written in C++. There's a CLI that should run on any UNIX-like system. The GUI is written in Qt. I've compiled for MacOS. Someone enterprising should be able to get this to work under Windows.

# Finding Binaries
If you just need the binaries, email me at jpl@showpage.org. I can only give you Mac binaries, but eventually maybe someone will build for Windows.

My goal is to provide a common location to download precompiled binaries.

# Current Status
This is an initial checkin. The GUI is still crude, and the viewer is beyond weak. The generator is kind of cool.

Right now, the generator doesn't understand past context, so it can do several down-cuts in a row rather than knowing it should pick something else. It will get there.

I cannot create a new difficulty level map yet. For instance, my sample song only has a Normal. It crashes if I try to create an Easy.

And my code is a bit on the naive side when it comes to mapping extensions. I might very well destroy data you would like to have.

# The Generator
The goal is to be able to automatically map an entire song with the push of a button. The program works off the concept of patterns. For instance, a down-up cut is a pattern. The generator doesn't think about individual notes as much as it does patterns.

# Structure
The Library directory holds the none-GUI code.

* A library for manipulating the song files
* The CLI
* Test cases

The SongEditor directory holds the GUI. This is a Qt application.

# To Build
If you want to build or contribute, you'll need:

* Qt Open Source Version
* Xcode command line utilities (Mac)
* cppunit for building and running the unit tests
* gmake
* These libraries: -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -lboost_filesystem

Clone the git repo. Build the library first. I don't have any autoconfigure tools yet, but maybe someone will produce them. Just run make.

If you want to put the CLI tools into /usr/local/bin and /usr/local/etc, then sudo make install. The CLI itself is not required to build the GUI, but you have to build the library.

To build the GUI, I do it from Qt Creator. Run Qt Creator and open the SongEditor project. Then build like you would normally. I'm sorry -- I'm not going to provide a tutorial on Qt.

# Contributing
In the library directory, there are some important includes with their corresponding .cpp files.

* Common.h includes a few simple structures that are used between a song and a pattern.
* Preferences.h provides a preferences system.
* Song.h provides everything for reading/writing the files that BeatSaber expects.
* Pattern.h provides everything for managing the patterns.

# Code Goals
There's a lot to do.

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

# Using the CLI
Okay, I haven't written it yet.

# Using the GUI
Run the GUI. So far, it requires an existing map, but of course, New Map is one of the goals. So Open an existing map. It can be really sparse, but you need at least one level to work on.

Go to the Song Map in the left toolbar. The buttons along the top let you switch between the levels you have configured. Click one of the View ones (The create ones crash right now.)

You'll get some stats filled in. The bottom area is bogus, but the middle area is kind of cool.

Hit Regenerate. this will destroy your level map and fill it in. So don't do this if you actually like your map, but it's great to give you a playable map!

Then save it.

To install it on your machine, you'll need to know how to use BMBF. And you'll need to zip it yourself.
