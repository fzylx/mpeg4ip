/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2003.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Bill May 		wmay@cisco.com
 */
#include "mp4live.h"
#include "video_encoder.h"
#include "mp4live_config.h"

static uint16_t h261SizeWidthValues[] = {
  176, 352
};
static uint16_t h261SizeHeightValues[] = {
  144, 288
};
static char* h261SizeNames[] = {
	"176 x 144 QCIF",
	"352 x 288 CIF",
};

#if defined(HAVE_FFMPEG) || defined(HAVE_XVID_H) || defined(HAVE_XVID10)
static u_int16_t mpeg4SizeWidthValues[] = {
	128, 176, 320, 352, 352,
	640, 704, 720, 704, 720, 768,
};
static u_int16_t mpeg4SizeHeightValues[] = {
	96, 144, 240, 288, 480,
	480, 480, 480, 576, 576, 576,
};
static  char* mpeg4SizeNames[] = {
	"128 x 96 SQCIF", 
	"176 x 144 QCIF",
	"320 x 240 SIF",
	"352 x 288 CIF",
	"352 x 480 Half D1",
	"640 x 480 4SIF",
	"704 x 480 D1",
	"720 x 480 NTSC CCIR601",
	"704 x 576 4CIF",
	"720 x 576 PAL CCIR601",
	"768 x 576 PAL SQ Pixel"
}; 
#define MPEG4_SIZES (sizeof(mpeg4SizeWidthValues) / sizeof(*mpeg4SizeWidthValues))
#endif
#if defined(HAVE_FFMPEG)
static uint16_t h263SizeWidthValues[] = {
  128, 176, 352, 704, 1408,
};

static uint16_t h263SizeHeightValues[] = {
  96, 144, 288, 576, 1152,
};
static char *h263SizeNames[] = {
  "128 x 93 SQCIF", 
  "176 x 144 QCIF", 
  "352 x 288 CIF", 
  "704 x 576 CIF4", 
  "1408 x 1152 CIF16",
};
#define H263_SIZES (NUM_ELEMENTS_IN_ARRAY(h263SizeWidthValues))
#endif


const video_encoder_table_t video_encoder_table[] = {
#if defined(HAVE_XVID_H) || defined(HAVE_XVID10)
  { 
#ifdef HAVE_XVID_H
    "Mpeg4 - xvid release",
#else
    "Mpeg4 - xvid 1.0 release",
#endif
    VIDEO_ENCODING_MPEG4,
    VIDEO_ENCODER_XVID,
    MPEG4_SIZES - 3, // can't use last 2 sizes
    MPEG4_SIZES,
    MPEG4_SIZES,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeNames,
    mpeg4SizeNames,
    mpeg4SizeNames
  },
#endif
#ifdef HAVE_FFMPEG
  { 
    "Mpeg4 - ffmpeg",
    VIDEO_ENCODING_MPEG4,
    VIDEO_ENCODER_FFMPEG,
    MPEG4_SIZES - 3, // can't use last 2 sizes
    MPEG4_SIZES,
    MPEG4_SIZES,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeNames,
    mpeg4SizeNames,
    mpeg4SizeNames
  },
  {
    "Mpeg2 - ffmpeg", 
    VIDEO_ENCODING_MPEG2,
    VIDEO_ENCODER_FFMPEG,
    MPEG4_SIZES - 3,
    MPEG4_SIZES,
    MPEG4_SIZES,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeWidthValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeHeightValues,
    mpeg4SizeNames,
    mpeg4SizeNames,
    mpeg4SizeNames
  },
  {
    "H263 - ffmpeg", 
    VIDEO_ENCODING_H263,
    VIDEO_ENCODER_FFMPEG,
    H263_SIZES - 1,
    H263_SIZES - 1,
    H263_SIZES - 1,
    h263SizeWidthValues,
    h263SizeWidthValues,
    h263SizeWidthValues,
    h263SizeHeightValues,
    h263SizeHeightValues,
    h263SizeHeightValues,
    h263SizeNames,
    h263SizeNames,
    h263SizeNames
  },
#endif
  {
    "H261",
    VIDEO_ENCODING_H261,
    VIDEO_ENCODER_H261,
    2, 
    2, 
    2, 
    h261SizeWidthValues,
    h261SizeWidthValues,
    h261SizeWidthValues,
    h261SizeHeightValues,
    h261SizeHeightValues,
    h261SizeHeightValues,
    h261SizeNames, 
    h261SizeNames, 
    h261SizeNames, 
  },
};

const uint32_t video_encoder_table_size = 
   sizeof(video_encoder_table) / sizeof(*video_encoder_table);
