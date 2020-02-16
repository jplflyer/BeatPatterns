#!/bin/bash

for dirname in ../build-BeatPatterns-Desktop*; do
	cp -R Patterns $dirname/BeatPatterns.app/Contents/Resources
done
