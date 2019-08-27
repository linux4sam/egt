#!/bin/sh

convert $1 \( +clone -background black -shadow 80x3+2+2 \) +swap -background transparent -layers merge +repage $1
