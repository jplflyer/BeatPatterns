#!/bin/bash

for dirname in ../build-SongEditor-Desktop*; do
	cp -R Patterns $dirname/SongEditor.app/Contents/Resources
done
