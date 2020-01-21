/* -*- Mode: c; c-basic-offset: 4; tab-width:4; indent-tabs-mode:t -*-
 * vim: set noet ts=4 sw=4: */
/*
 * libmp4 - a library for reading and writing mp4 files
 * Copyright (C) 2007-2008 Limin Wang(lance.lmwang@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* public libmp4 library interface */
#ifndef __LIB_MP4_INCLUDED__
#define __LIB_MP4_INCLUDED__
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* the mp4 file handle */
typedef struct mp4_handle mp4_handle_t;

/* mp4 movie open mode */
enum {
	MP4_OPEN_READ,
	MP4_OPEN_READ_DUMP,
	MP4_OPEN_WRITE
};


/* direction for sample search (including SyncSamples search)
   Function using search allways specify the desired time in dts

   (Sample N-1)    DesiredTime     (Sample N)

   FORWARD: will give the next sample given the desired time (eg, N)
   BACKWARD: will give the previous sample given the desired time (eg, N-1)
   SYNCFORWARD: will search from the desired point in time for a sync sample if any
   If no sync info, behaves as FORWARD
   SYNCBACKWARD: will search till the desired point in time for a sync sample if any
   If no sync info, behaves as BACKWARD
   SYNCSHADOW: use the sync shadow information to retrieve the sample.
   If no SyncShadow info, behave as SYNCBACKWARD
*/
enum
{
    MP4_SEARCH_FORWARD          =   1,
    MP4_SEARCH_BACKWARD         =   2,
    MP4_SEARCH_SYNC_FORWARD     =   3,
    MP4_SEARCH_SYNC_BACKWARD    =   4,
    MP4_SEARCH_SYNC_SHADOW      =   5
};


/* MP4 verbosity levels - e.g. mp4_set_verbosity() */
#define MP4_DETAILS_NONE            0x00000000
#define MP4_DETAILS_ALL             0xFFFFFFFF
#define MP4_DETAILS_ERROR           0x00000001
#define MP4_DETAILS_WARNING         0x00000002
#define MP4_DETAILS_READ            0x00000004
#define MP4_DETAILS_WRITE           0x00000008
#define MP4_DETAILS_FIND            0x00000010
#define MP4_DETAILS_TABLE           0x00000020
#define MP4_DETAILS_SAMPLE          0x00000040
#define MP4_DETAILS_MEMLEAK         0x00000080


#define MP4_DETAILS_READ_ALL        \
    (MP4_DETAILS_READ | MP4_DETAILS_TABLE | MP4_DETAILS_SAMPLE)
#define MP4_DETAILS_WRITE_ALL       \
    (MP4_DETAILS_WRITE | MP4_DETAILS_TABLE | MP4_DETAILS_SAMPLE)

/* Big Endian */
#define MP4_FOURCC( a, b, c, d ) (((a)<<24)|((b)<<16)|((c)<<8)|(d))


/*returns 1 if target file is an mp4 file, 0 otherwise*/
uint8_t mp4_probe(const char *file_name );

/* open a mp4 file and get the handle */
mp4_handle_t* mp4_open( const char* file_name, int mode );

/* close the mp4 handle */
void mp4_close( mp4_handle_t *handle );

void mp4_set_verbosity( uint32_t verbosity );
uint32_t mp4_get_verbosity( void );

/*return the timescale of the movie, 0 if error*/
uint32_t mp4_get_timescale(mp4_handle_t *handle);

/*return the duration of the movie, 0 if error*/
uint64_t mp4_get_duration(mp4_handle_t *handle);

/*return the creation info of the movie*/
int mp4_get_creation_time(mp4_handle_t *handle, uint64_t *creation_time,
						  uint64_t *modification_time);

/*return the number of tracks in the movie*/
uint32_t mp4_get_nr_of_tracks( mp4_handle_t *handle );


/**
 * return the track id of track index n, or 0 if error
 * the valid index range is [0, nr_track-1]
*/
uint32_t mp4_get_track_id(mp4_handle_t *handle, uint32_t index);


/*return the track number of the track of specified ID, or 0 if error*/
uint32_t mp4_get_track_idx_by_id(mp4_handle_t *handle, uint32_t track_id);

/*gets the enable flag of a track 0: NO, 1: YES, 2: error*/
uint8_t mp4_is_track_enabled(mp4_handle_t *handle, uint32_t track_id);

/* determines if the track is encrypted 0: NO, 1: YES, 2: error*/
uint8_t mp4_is_track_encrypted(mp4_handle_t *handle, uint32_t track_id);

/*get the track duration return 0 if bad param*/
uint64_t mp4_get_track_duration(mp4_handle_t *handle, uint32_t track_id);

/*return the media type FOUR CHAR code type of the media(soun, vide, hint)*/
uint32_t mp4_get_media_type(mp4_handle_t *handle, uint32_t track_id);

/*return the media sub type FOUR CHAR code type of the media(mp4a, mp4v, avc1...)*/
uint32_t mp4_get_media_sub_type(mp4_handle_t *handle, uint32_t track_id);

/* get esds decconfigDescr info, or -1 if esd is NULL */
int mp4_get_esds_dec_config_info(mp4_handle_t *handle, uint32_t track_id,
									  uint8_t *object_type_ind, uint8_t *stream_type,
									  uint8_t *up_stream, uint32_t *buf_size_db,
									  uint32_t *max_bitrate, uint32_t *avg_bitrate );

/* get esds decConfigDescr decSpecificInfo */
int mp4_get_esds_dec_specific_info(mp4_handle_t *handle, uint32_t track_id,
								   uint8_t**config, uint32_t* config_size );
	
/*get the timescale of media*/
uint32_t mp4_get_media_timescale(mp4_handle_t *handle, uint32_t track_id);

/*get duration of the media(without edit) */
uint64_t mp4_get_media_duration(mp4_handle_t *handle, uint32_t track_id);


/*get width, height of the given visual sample desc
  return -1 if not a visual track */
int mp4_get_visual_info(mp4_handle_t *handle, uint32_t track_id,
						uint32_t *width, uint32_t *height);

/*get samplerate, channels and bps, version of the given audio track
  return -1 if not a audio track*/
int mp4_get_audio_info(mp4_handle_t *handle, uint32_t track_id,
					   uint32_t *sample_rate, uint32_t *channels,
					   uint8_t *bits_per_sample, uint8_t *version );


/*return the numbers of samples */
uint32_t mp4_get_nr_of_samples(mp4_handle_t *handle,uint32_t track_id);

/*return the max sample size of the track */
uint32_t mp4_get_max_sample_size(mp4_handle_t *handle,uint32_t track_id);

/*return the sample size of the track sample, or 0 if error */
uint32_t mp4_get_sample_size(mp4_handle_t *handle,uint32_t track_id, uint32_t sample_id);

/* return sample id by the time */
uint32_t mp4_get_sample_from_time(mp4_handle_t *handle, uint32_t track_id,
								  uint64_t when, uint8_t search_mode );

/* get sample time, it's dts */
uint64_t mp4_get_sample_dts(mp4_handle_t *handle, uint32_t track_id, uint32_t sample_id );

/* get sample duration in media time scale*/
uint64_t mp4_get_sample_duration(mp4_handle_t *handle, uint32_t track_id, uint32_t sample_id );

/* get avc decoder cfg info, memory is allocate and free by library, return <
   0 if error  */
int mp4_get_track_h264_seq_pic_headers(mp4_handle_t *handle, uint32_t track_id,
									   uint8_t *nr_of_seq_hdr,
									   char ***seq_hdr,
									   uint16_t **seq_hdr_size,
									   uint8_t *nr_of_pic_hdr,
									   char ***pic_hdr,
									   uint16_t **pic_hdr_size);

/* get avc decConfig info  */
int mp4_get_track_h264_dec_config_info(mp4_handle_t *handle, uint32_t track_id,
									   uint8_t *nalu_size, uint8_t *cfg_ver,
									   uint8_t *profile_ind, uint8_t *profile_comp,
									   uint8_t *level_ind );
	
/* read a sample by sample id, return < 0 if error */
int mp4_read_sample( mp4_handle_t *handle, uint32_t track_id, uint32_t sample_id,
					 char *buf, uint32_t* size,
					 uint64_t *DTS, uint64_t *CTS,
					 uint64_t *duration_time, uint8_t *is_sync );

/* read a sample by given a desired decoding time IN MEDIA TIME SCALE, return
   < 0 if error */
int mp4_read_sample_from_time( mp4_handle_t *handle, uint32_t track_id, uint64_t when,
							   uint8_t search_mode,
							   char *buf, uint32_t* size,
							   uint64_t *DTS, uint64_t *CTS,
							   uint64_t *duration_time );


/* convert FOURCC to string */
int mp4_fourcc_to_str(uint32_t type, char *str, int str_len);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_MP4_INCLUDED__ */
