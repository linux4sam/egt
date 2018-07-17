#!/bin/sh

set -e

#ffmpeg -y -i $1 -c:a aac -strict experimental -vcodec h264 -b:v 500K -s 352x288 -r 24 $1.h264.mp4

#ffmpeg -y -i $1 -strict experimental -acodec pcm_s16le -ac 1 -vcodec h263 -b:v 500K -s 352x288 -q:v 15 -r 16 $1.h263.avi -f avi
#ffmpeg -y -i $1 -strict experimental -acodec pcm_s16le -ac 1 -vcodec h263 -s 352x288 -q:v 15 -r 16 $1.h263.avi -f avi
#ffmpeg -y -i $1 -vcodec h263 -b:v 500K -s 352x288 -q:v 5 -r 16 $1.h263

#ffmpeg -y -i $1 -c:v rawvideo -s 400x240 -r 16 -pix_fmt yuyv422 -q:v 0 $1.yuv

#ffmpeg -y -i $1 -vf "scale=(iw*sar)*max(480/(iw*sar)\,240/ih):ih*max(480/(iw*sar)\,240/ih), crop=480:240" -c:v rawvideo -s 400x240 -r 24 -pix_fmt yuyv422 -q:v 0 $1.yuv

width=320
height=192
#width=400
#height=240

format=yuyv422
#format=yuv420p

ffmpeg -y -i $1 -vf "scale=(iw*sar)*max(${width}/(iw*sar)\,${height}/ih):ih*max(${width}/(iw*sar)\,${height}/ih), crop=${width}:${height}" -c:v rawvideo -s ${width}x${height} -r 24 -pix_fmt ${format} -q:v 0 -crf 18 -acodec pcm_s16le -ac 2 $1.avi -f avi
