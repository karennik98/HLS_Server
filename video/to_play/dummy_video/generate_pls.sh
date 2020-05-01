#!/bin/sh

ffmpeg -y \
 -i testsrc.mp4 \
 -c:v mpeg2video -qscale:v 10 \
 -c:a mp2 -b:a 192k \
 -vf scale=720x496 \
 -f segment \
 -segment_time 1 \
 -segment_list "playlist1.m3u8" \
 -segment_list_type m3u8 \
 "seg_test_src_%d.ts"
