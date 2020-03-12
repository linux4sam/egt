#!/bin/bash

all_outputs="mpeg1 mpeg2 mpeg4 vp8 vp9 uncompressed raw h264"

function usage()
{
    echo "$0 -i INPUT [-x WIDTH] [-y HEIGHT] [-f FORMAT] [-r FPS] [-s] [TYPE...]"
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

fps=24
single=0

while getopts ':hx:y:f:i:r:s' opt;
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
	r)
	    fps="$OPTARG"
	    ;;
	s)
	    single=1
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

audio_opts="-acodec pcm_s16le -ac 1 -ar 48000"
size_opts="-vf scale=(iw*sar)*max(${width}/(iw*sar)\,${height}/ih):\
ih*max(${width}/(iw*sar)\,${height}/ih),crop=${width}:${height} \
-s ${width}x${height}"
video_opts="-pix_fmt ${format} -r ${fps}"
mpeg_opts="-pre_dia_size 5 -dia_size 5 -qcomp 0.7 -qblur 0 -preme 2 \
-me_method dia -sc_threshold 0 -sc_factor 4 -bidir_refine 4 -profile:v 4 \
-mbd rd -mbcmp satd -precmp satd -cmp satd -subcmp satd -skipcmp satd -bf 2"

base="${in%.*}"

for t in $outputs
do
    case "$t" in
	mpeg1)
	    cmd=(ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		 -vcodec mpeg1video -qscale:v 2 -b_strategy 2 -brd_scale 2 \
		 ${mpeg_opts} -threads 4)

	    if [ $single -eq 0 ]; then
		"${cmd[@]}" -pass 1 -f avi /dev/null
		"${cmd[@]}" -pass 2 "${base}_mpeg1.avi"
	    else
		"${cmd[@]}" "${base}_mpeg1.avi"
	    fi
	    ;;
	mpeg2)
	    cmd=(ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		 -vcodec mpeg2video -qscale:v 2 -b_strategy 2 -brd_scale 2 \
		 ${mpeg_opts} -threads 4)

	    if [ $single -eq 0 ]; then
		"${cmd[@]}" -pass 1 -f avi /dev/null
		"${cmd[@]}" -pass 2 "${base}_mpeg2.avi"
	    else
		"${cmd[@]}" "${base}_mpeg2.avi"
	    fi
	    ;;
	mpeg4)
	    cmd=(ffmpeg -y -i "${in}" ${size_opts} ${audio_opts} ${video_opts} \
		 -vcodec mpeg4 -qscale:v 2 ${mpeg_opts} -threads 4)

	    if [ $single -eq 0 ]; then
		"${cmd[@]}" -pass 1 -f avi /dev/null
		"${cmd[@]}" -pass 2 "${base}_mpeg4.avi"
	    else
		"${cmd[@]}" "${base}_mpeg4.avi"
	    fi
	    ;;
	vp8)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} ${video_opts} -vcodec libvpx -crf 5 -qmin 0 \
		   -qmax 50 -q:v 3 "${base}_vp8.avi"
	    ;;
	vp9)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} ${video_opts} -vcodec libvpx-vp9 -crf 5 -b:v 0 \
		   -qmin 0 -qmax 50 -q:v 3 "${base}_vp9.avi"
	    ;;
	h264)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   ${audio_opts} -strict experimental ${video_opts} -vcodec h264 \
		   -profile:v baseline -q:v 0 "${base}_h264.avi"
	    ;;
	uncompressed)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   -c:v rawvideo ${video_opts} -q:v 2 ${audio_opts} \
		   "${base}_uncompressed.avi"
	    ;;
	raw)
	    ffmpeg -y -i "${in}" ${size_opts} \
		   -c:v rawvideo ${video_opts} -q:v 2 "${base}.yuv"
	    ;;
	*)
	    echo "type $type not supported."
	    exit 1
	    ;;
    esac
done
