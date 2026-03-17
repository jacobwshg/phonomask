#!/bin/bash

OUTF="phmask"

rm -rf ./$OUTF

clang++ -Wall -Wextra -std=c++20 -o ./$OUTF \
	./utils.cpp \
	./seg_fm_maps.cpp ./feat_idx_maps.cpp ./feature_profile.cpp \
	./rule.cpp ./word.cpp \
	./test_main.cpp -licuuc -licudata 

