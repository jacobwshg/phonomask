#!/bin/bash

rm -rf ./*.out

clang++ -Wall -Wextra -std=c++20 -o phmask.out \
	./utils.cpp \
	./seg_fm_maps.cpp ./feat_idx_maps.cpp ./feature_profile.cpp \
	./rule.cpp ./word.cpp \
	./test.cpp -licuuc -licudata 

