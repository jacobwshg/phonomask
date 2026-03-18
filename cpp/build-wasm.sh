#!/bin/bash

OUTF="phmask.js"

rm -rf ./$OUTF

emcc -O2 --bind -std=c++20 -s MODULARIZE=1 -s EXPORT_ES6=1 \
	-s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1\
	-o ./$OUTF\
	./em_if.cpp ./utils.cpp\
	./seg_fm_maps.cpp ./feat_idx_maps.cpp ./feature_profile.cpp\
	./rule.cpp ./word.cpp\
	-s USE_ICU=1\
	#-licuuc -licudata\

