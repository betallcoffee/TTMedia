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
#include "lib_mp4.h"
#include "mp4_boxes.h"
#include "mp4_file.h"
#include "mp4_debug.h"

/*returns 1 if target file is an mp4 file, 0 otherwise*/
uint8_t mp4_probe(const char *file_name)
{
	unsigned char data[8];
	int count;
	FILE *fd;
	uint32_t type;

	/* open the file */
	fd = fopen(file_name, "rb");
	if (!fd)
		return 0;

	/* read the first 8 byte(size(4byte), type(4byte)) */
	count = fread(data, 1, 8, fd);

	/* close the file */
	fclose(fd);

	/* get the type */
	type = MP4_FOURCC(data[4], data[5], data[6], data[7]);

	switch (type) {
	case MP4_BOX_TYPE_ftyp:
	case MP4_BOX_TYPE_moov:
	case MP4_BOX_TYPE_mdat:
	case MP4_BOX_TYPE_free:
	case MP4_BOX_TYPE_skip:
	case MP4_BOX_TYPE_udta:
	case MP4_BOX_TYPE_meta:
		return 1;
	default:
		break;
	}

	return 0;
}

/* open a mp4 file */
struct mp4_handle *mp4_open(const char *file, int mode)
{
	struct mp4_handle *mov = NULL;

	switch (mode & 0xFF) {
	case MP4_OPEN_READ_DUMP:
	case MP4_OPEN_READ:
		mov = mp4_open_file(file, mode);
		break;

	case MP4_OPEN_WRITE:		/* TODO */
	default:
		break;
	}

	return (struct mp4_handle *) mov;
}

/* close the mp4 file */
void mp4_close(struct mp4_handle *mov)
{
	if (!mov)
		return;

	mp4_close_file(mov);
}

/*return the timescale of the movie, 0 if error*/
uint32_t mp4_get_timescale(mp4_handle_t * handle)
{
	struct mp4_moov_box *moov;
	struct mp4_mvhd_box *mvhd;

	if (!handle || !handle->moov)
		return 0;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov->mvhd)
		return 0;
	mvhd = (struct mp4_mvhd_box *) moov->mvhd;

	return mvhd->timescale;
}

/*return the duration of the movie, 0 if error*/
uint64_t mp4_get_duration(mp4_handle_t * handle)
{
	struct mp4_moov_box *moov;
	struct mp4_mvhd_box *mvhd;

	if (!handle || !handle->moov)
		return 0;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov->mvhd)
		return 0;
	mvhd = (struct mp4_mvhd_box *) moov->mvhd;

	return mvhd->duration;
}

/*return the creation info of the movie*/
int mp4_get_creation_time(mp4_handle_t * handle, uint64_t * creation_time,
						  uint64_t * modification_time)
{
	struct mp4_moov_box *moov;
	struct mp4_mvhd_box *mvhd;

	if (!handle || !handle->moov)
		return -1;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov->mvhd)
		return -1;
	mvhd = (struct mp4_mvhd_box *) moov->mvhd;

	*creation_time = mvhd->creation_time;
	*modification_time = mvhd->modification_time;

	return 0;
}

/*return the number of tracks in the movie*/
uint32_t mp4_get_nr_of_tracks(mp4_handle_t * handle)
{
	struct mp4_moov_box *moov;

	if (!handle || !handle->moov)
		return -1;

	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_nr_of_tracks(moov);
}

/*return the track id of track index n, or 0 if error*/
uint32_t mp4_get_track_id(mp4_handle_t * handle, uint32_t index)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_track_id(moov, index);
}

/*return the track number of the track of specified ID, or 0 if error*/
uint32_t mp4_get_track_idx_by_id(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_track_idx(moov, track_id);
}

/*gets the enable flag of a track 0: NO, 1: YES, 2: error*/
uint8_t mp4_is_track_enabled(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->is_track_enable(moov, track_id);
}

/* determines if the track is encrypted 0: NO, 1: YES, 2: error*/
uint8_t mp4_is_track_encrypted(mp4_handle_t * handle, uint32_t track_id)
{
	/* todo */
	return 0;
}

/*get the track duration return 0 if bad param*/
uint64_t mp4_get_track_duration(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_track_duration(moov, track_id);
}

/*return the media type FOUR CHAR code type of the media(soun, vide, hint)*/
uint32_t mp4_get_media_type(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_media_type(moov, track_id);
}

/*return the media sub type FOUR CHAR code type of the media(mp4a, mp4v, avc1...)*/
uint32_t mp4_get_media_sub_type(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_media_sub_type(moov, track_id);
}

/*get the timescale of media*/
uint32_t mp4_get_media_timescale(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_media_timescale(moov, track_id);
}

/*get the media duration return 0*/
uint64_t mp4_get_media_duration(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_media_duration(moov, track_id);
}

/*get width, height and framerate of the given visual sample desc
  return -1 if not a visual track */
int mp4_get_visual_info(mp4_handle_t * handle, uint32_t track_id,
						uint32_t * width, uint32_t * height)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_visual_info(moov, track_id, width, height);
}

/*get samplerate, channels and bps of the given audio track
  return -1 if not a audio track*/
int mp4_get_audio_info(mp4_handle_t * handle, uint32_t track_id,
					   uint32_t * sample_rate, uint32_t * channels,
					   uint8_t * bits_per_sample, uint8_t * version)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_audio_info(moov, track_id, sample_rate, channels,
								bits_per_sample, version);
}

/*return the numbers of samples */
uint32_t mp4_get_nr_of_samples(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_nr_of_samples(moov, track_id);
}

/*return the max sample size of the track */
uint32_t mp4_get_max_sample_size(mp4_handle_t * handle, uint32_t track_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_max_sample_size(moov, track_id);
}

/*return the sample size of the track sample, or 0 if error */
uint32_t mp4_get_sample_size(mp4_handle_t * handle, uint32_t track_id,
							 uint32_t sample_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_sample_size(moov, track_id, sample_id);
}

/* return sample id by the time */
uint32_t mp4_get_sample_from_time(mp4_handle_t * handle, uint32_t track_id,
								  uint64_t when, uint8_t search_mode)
{
	struct mp4_moov_box *moov;
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	uint32_t index;

	if (!handle)
		return 0;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov)
		return 0;

	index = moov->get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	return mdia->get_sample_from_time((struct mp4_box *) mdia, when,
									  search_mode);
}

/* get sample time, it's dts */
uint64_t mp4_get_sample_dts(mp4_handle_t * handle, uint32_t track_id,
							uint32_t sample_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_sample_dts(moov, track_id, sample_id);
}

/* get sample duration in media time scale*/
uint64_t mp4_get_sample_duration(mp4_handle_t * handle, uint32_t track_id,
								 uint32_t sample_id)
{
	struct mp4_moov_box *moov;

	if (!handle)
		return 0;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_sample_duration(moov, track_id, sample_id);
}

/* get avc decoder cfg info  */
int mp4_get_track_h264_seq_pic_headers(mp4_handle_t * handle, uint32_t track_id,
									   uint8_t * nr_of_seq_hdr,
									   char ***seq_hdr,
									   uint16_t ** seq_hdr_size,
									   uint8_t * nr_of_pic_hdr,
									   char ***pic_hdr,
									   uint16_t ** pic_hdr_size)
{
	struct mp4_moov_box *moov;

	/* initialize the hdr number to 0 */
	*nr_of_seq_hdr = 0;
	*nr_of_pic_hdr = 0;

	if (!handle)
		return -1;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_h264_seq_pic_hdrs(moov, track_id, nr_of_seq_hdr, seq_hdr,
									   seq_hdr_size, nr_of_pic_hdr,
									   pic_hdr, pic_hdr_size);
}

/* get avc decConfig info  */
int mp4_get_track_h264_dec_config_info(mp4_handle_t * handle, uint32_t track_id,
									   uint8_t * nalu_size, uint8_t * cfg_ver,
									   uint8_t * profile_ind,
									   uint8_t * profile_comp,
									   uint8_t * level_ind)
{
	struct mp4_moov_box *moov;

	/* default to 1 if any error */
	*nalu_size = 1;

	if (!handle)
		return -1;
	moov = (struct mp4_moov_box *) handle->moov;

	return moov->get_h264_dec_config_info(moov, track_id, nalu_size, cfg_ver,
										  profile_ind, profile_comp, level_ind);
}

/* get esds decconfigDescr info, or -1 if esd is NULL */
int mp4_get_esds_dec_config_info(mp4_handle_t * handle, uint32_t track_id,
								 uint8_t * object_type_ind,
								 uint8_t * stream_type, uint8_t * up_stream,
								 uint32_t * buf_size_db, uint32_t * max_bitrate,
								 uint32_t * avg_bitrate)
{
	struct mp4_moov_box *moov;
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	struct mp4_stsd_box *stsd;
	uint32_t index;

	if (!handle)
		return -1;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov)
		return -1;

	index = moov->get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return -1;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return -1;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return -1;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	return stsd->get_esds_dec_cfg_info((struct mp4_box *) stsd, object_type_ind,
									   stream_type, up_stream, buf_size_db,
									   max_bitrate, avg_bitrate);
}

/* get esds decConfigDescr decSpecificInfo */
int mp4_get_esds_dec_specific_info(mp4_handle_t * handle, uint32_t track_id,
								   uint8_t ** config, uint32_t * config_size)
{
	struct mp4_moov_box *moov;
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	struct mp4_stsd_box *stsd;
	uint32_t index;

	if (!handle)
		return -1;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov)
		return -1;

	index = moov->get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return -1;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return -1;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return -1;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	return stsd->get_esds_dec_specfic_info((struct mp4_box *) stsd,
										   config, config_size);
}

/* read a sample by sample id */
int mp4_read_sample(mp4_handle_t * handle, uint32_t track_id,
					uint32_t sample_id, char *buf, uint32_t * size,
					uint64_t * DTS, uint64_t * CTS, uint64_t * duration_time,
					uint8_t * is_sync)
{
	struct mp4_moov_box *moov;
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	uint32_t index;

	if (!handle)
		return -1;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov)
		return -1;

	index = moov->get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return -1;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return -1;

	return mdia->read_sample((struct mp4_box *) mdia, sample_id, buf, size, DTS,
							 CTS, duration_time, is_sync);
}

/* read a sample by given a desired decoding time IN MEDIA TIME SCALE */
int mp4_read_sample_from_time(mp4_handle_t * handle, uint32_t track_id,
							  uint64_t when, uint8_t search_mode,
							  char *buf, uint32_t * size, uint64_t * DTS,
							  uint64_t * CTS, uint64_t * duration_time)
{
	struct mp4_moov_box *moov;
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	uint32_t index;

	if (!handle)
		return 0;

	moov = (struct mp4_moov_box *) handle->moov;
	if (!moov)
		return 0;

	index = moov->get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	return mdia->read_sample_from_time((struct mp4_box *) mdia, when,
									   search_mode, buf, size, DTS,
									   CTS, duration_time);
}
