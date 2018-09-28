#!/bin/bash

all_outputs="mpeg1 mpeg2 mpeg4 vp8 vp9 uncompressed raw h264"

function usage()
{
    echo "$0 -i INPUT [-x WIDTH] [-y HEIGHT] [-f FORMAT] [TYPE...]"
    echo "    TYPE: $all_outputs"
    echo "    FORMAT: An output format from the command 'ffmpeg -pix_fmts'"
}

outputs=""
in=""

width=320
height=192
#format=yuyv422p
format=yuv420p
#format=nv21

while getopts ':hx:y:f:i:' opt;
do
    case "$opt" in
	h)
	    usage
	    exit 0
	    ;;
	i)
	    in="$OPTARG"
	    ;;
	x)
	    width="$OPTARG"
	    ;;
	y)
	    height="$OPTARG"
	    ;;
	f)
	    format="$OPTARG"
	    ;;
	:)
	    echo "Invalid option: $OPTARG requires an argument" 1>&2
	    exit 1
	    ;;
	\?)
	    echo "Invalid Option: -$OPTARG" 1>&2
	    exit 1
	    ;;
    esac
done
shift "$(($OPTIND -1))"

if [ -z "$(echo $in)" ]; then
    echo no input
    usage
    exit 1
fi

if [ "$#" -le 0 ]; then
    outputs=$all_outputs
else
    outputs=$@
fi

set -e

audio_opts="-acodec pcm_s16le -ac 2 -ar 48000"
size_opts="-vf scale=(iw*sar)*max(${width}/(iw*sar)\,${height}/ih):\
ih*max(${width}/(iw*sar)\,${height}/ih),crop=${width}:${height} \
-s ${width}x${height}"
video_opts="-pix_fmt ${format} -r 24"
mpeg_opts="-pre_dia_size 5 -dia_size 5 -qcomp 0.7 -qblur 0 -preme 2 \
-me_method dia -sc_threshold 0 -sc_factor 4 -bidir_refine 4 -profile:v 4 \
-mbd rd -mbcmp satd -precmp satd -cmp satd -subcmp satd -skipcmp satd -bf 2"

for t in $outputs
do
    case "$t" in
	mpeg1)
	    ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		   -vcodec mpeg1video -q:v 1 -b_strategy 2 -brd_scale 2 \
		   ${mpeg_opts} -pass 1 -f avi /dev/null

	    ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		   -vcodec mpeg1video -q:v 1 ${mpeg_opts} -pass 2 \
		   "${in}".mpeg1.avi
	    ;;
	mpeg2)
#	    ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
#		   -vcodec mpeg2video -q:v 1 -b_strategy 2 -brd_scale 2 \
#		   ${mpeg_opts} -pass 1 -f avi /dev/null

#	    ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
#		   -vcodec mpeg2video -q:v 1 ${mpeg_opts} -pass 2 "${in}".mpeg2.avi

    	    ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		   -vcodec mpeg2video -q:v 1 ${mpeg_opts} "${in}".mpeg2.avi
	    ;;
	mpeg4)
	    #ffmpeg -y -i "${in}" ${size_opts} \
	#	   ${audio_opts} ${video_opts} -vcodec mpeg4 -q:v 2 -pass 1 \
	#	   -f avi /dev/null

	 #   ffmpeg -y -i "${in}" ${size_opts} \
	#	   ${audio_opts} ${video_opts} -vcodec mpeg4 -q:v 2 -pass 2 \
	#	   "${in}".mpeg4.avi


	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} ${video_opts} -vcodec mpeg4 -q:v 2 \
		   "${in}".mpeg4.avi
	    ;;
	vp8)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} ${video_opts} -vcodec libvpx -crf 5 -qmin 0 \
		   -qmax 50 -q:v 3 "${in}".vp8.avi
	    ;;
	vp9)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} ${video_opts} -vcodec libvpx-vp9 -crf 5 -b:v 0 \
		   -qmin 0 -qmax 50 -q:v 3 "${in}".vp9.avi
	    ;;
	h264)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} -strict experimental ${video_opts} -vcodec h264 \
		   -profile:v baseline -q:v 0 "${in}".h264.avi
	    ;;
	uncompressed)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   -c:v rawvideo ${video_opts} -q:v 2 ${audio_opts} \
		   "${in}".uncompressed.avi
	    ;;
	raw)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   -c:v rawvideo ${video_opts} -q:v 2 "${in}".yuv
	    ;;
	*)
	    echo "type $type not supported."
	    exit 1
	    ;;
    esac
done
