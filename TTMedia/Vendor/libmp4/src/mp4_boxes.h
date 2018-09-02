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
#ifndef __MP4_BOXES_INCLUDED__
#define __MP4_BOXES_INCLUDED__
#include "lib_mp4.h"
#include "mp4_util.h"
#include "mp4_bits.h"
#include "mp4_descriptor.h"
#include "mp4_list.h"

enum {
	/* 14496-12 */
	MP4_BOX_TYPE_co64 = MP4_FOURCC('c', 'o', '6', '4'),
	MP4_BOX_TYPE_cprt = MP4_FOURCC('c', 'p', 'r', 't'),
	MP4_BOX_TYPE_ctts = MP4_FOURCC('c', 't', 't', 's'),

	MP4_BOX_TYPE_dinf = MP4_FOURCC('d', 'i', 'n', 'f'),
	MP4_BOX_TYPE_dref = MP4_FOURCC('d', 'r', 'e', 'f'),

	MP4_BOX_TYPE_edts = MP4_FOURCC('e', 'd', 't', 's'),
	MP4_BOX_TYPE_elst = MP4_FOURCC('e', 'l', 's', 't'),

	MP4_BOX_TYPE_free = MP4_FOURCC('f', 'r', 'e', 'e'),
	MP4_BOX_TYPE_ftyp = MP4_FOURCC('f', 't', 'y', 'p'),

	MP4_BOX_TYPE_hdlr = MP4_FOURCC('h', 'd', 'l', 'r'),
	MP4_BOX_TYPE_hmhd = MP4_FOURCC('h', 'm', 'h', 'd'),

	MP4_BOX_TYPE_mdat = MP4_FOURCC('m', 'd', 'a', 't'),
	MP4_BOX_TYPE_mdhd = MP4_FOURCC('m', 'd', 'h', 'd'),
	MP4_BOX_TYPE_mdia = MP4_FOURCC('m', 'd', 'i', 'a'),
	MP4_BOX_TYPE_mehd = MP4_FOURCC('m', 'e', 'h', 'd'),
	MP4_BOX_TYPE_meta = MP4_FOURCC('m', 'e', 't', 'a'),
	MP4_BOX_TYPE_minf = MP4_FOURCC('m', 'i', 'n', 'f'),
	MP4_BOX_TYPE_moov = MP4_FOURCC('m', 'o', 'o', 'v'),
	MP4_BOX_TYPE_mvex = MP4_FOURCC('m', 'v', 'e', 'x'),
	MP4_BOX_TYPE_mvhd = MP4_FOURCC('m', 'v', 'h', 'd'),

	MP4_BOX_TYPE_nmhd = MP4_FOURCC('n', 'm', 'h', 'd'),

	MP4_BOX_TYPE_padb = MP4_FOURCC('p', 'a', 'd', 'b'),

	MP4_BOX_TYPE_rtp = MP4_FOURCC('r', 't', 'p', ' '),

	MP4_BOX_TYPE_sbpg = MP4_FOURCC('s', 'b', 'p', 'g'),
	MP4_BOX_TYPE_sdtp = MP4_FOURCC('s', 'd', 't', 'p'),
	MP4_BOX_TYPE_sgpd = MP4_FOURCC('s', 'g', 'p', 'd'),
	MP4_BOX_TYPE_smhd = MP4_FOURCC('s', 'm', 'h', 'd'),
	MP4_BOX_TYPE_skip = MP4_FOURCC('s', 'k', 'i', 'p'),
	MP4_BOX_TYPE_stbl = MP4_FOURCC('s', 't', 'b', 'l'),
	MP4_BOX_TYPE_stco = MP4_FOURCC('s', 't', 'c', 'o'),
	MP4_BOX_TYPE_stdp = MP4_FOURCC('s', 't', 'd', 'p'),
	MP4_BOX_TYPE_stsc = MP4_FOURCC('s', 't', 's', 'c'),
	MP4_BOX_TYPE_stsd = MP4_FOURCC('s', 't', 's', 'd'),
	MP4_BOX_TYPE_stsh = MP4_FOURCC('s', 't', 's', 'h'),
	MP4_BOX_TYPE_stss = MP4_FOURCC('s', 't', 's', 's'),
	MP4_BOX_TYPE_stsz = MP4_FOURCC('s', 't', 's', 'z'),
	MP4_BOX_TYPE_stz2 = MP4_FOURCC('s', 't', 'z', '2'),
	MP4_BOX_TYPE_stts = MP4_FOURCC('s', 't', 't', 's'),
	MP4_BOX_TYPE_snro = MP4_FOURCC('s', 'n', 'r', 'o'),
	MP4_BOX_TYPE_subs = MP4_FOURCC('s', 'u', 'b', 's'),

	MP4_BOX_TYPE_tims = MP4_FOURCC('t', 'i', 'm', 's'),
	MP4_BOX_TYPE_tkhd = MP4_FOURCC('t', 'k', 'h', 'd'),
	MP4_BOX_TYPE_trex = MP4_FOURCC('t', 'r', 'e', 'x'),
	MP4_BOX_TYPE_tref = MP4_FOURCC('t', 'r', 'e', 'f'),
	MP4_BOX_TYPE_trak = MP4_FOURCC('t', 'r', 'a', 'k'),
	MP4_BOX_TYPE_tsro = MP4_FOURCC('t', 's', 'r', 'o'),

	MP4_BOX_TYPE_udta = MP4_FOURCC('u', 'd', 't', 'a'),
	MP4_BOX_TYPE_url = MP4_FOURCC('u', 'r', 'l', ' '),
	MP4_BOX_TYPE_urn = MP4_FOURCC('u', 'r', 'n', ' '),
	MP4_BOX_TYPE_uuid = MP4_FOURCC('u', 'u', 'i', 'd'),

	MP4_BOX_TYPE_vmhd = MP4_FOURCC('v', 'm', 'h', 'd'),

	/* 14496-14 */
	MP4_BOX_TYPE_esds = MP4_FOURCC('e', 's', 'd', 's'),
	MP4_BOX_TYPE_iods = MP4_FOURCC('i', 'o', 'd', 's'),

	MP4_BOX_TYPE_mp4a = MP4_FOURCC('m', 'p', '4', 'a'),
	MP4_BOX_TYPE_mp4s = MP4_FOURCC('m', 'p', '4', 's'),
	MP4_BOX_TYPE_mp4v = MP4_FOURCC('m', 'p', '4', 'v'),

	/* 14496-15 */
	MP4_BOX_TYPE_avc1 = MP4_FOURCC('a', 'v', 'c', '1'),
	MP4_BOX_TYPE_avcC = MP4_FOURCC('a', 'v', 'c', 'C'),
	MP4_BOX_TYPE_avcp = MP4_FOURCC('a', 'v', 'c', 'p'),
	MP4_BOX_TYPE_btrt = MP4_FOURCC('b', 't', 'r', 't'),
	MP4_BOX_TYPE_m4ds = MP4_FOURCC('m', '4', 'd', 's'),

	/* todo */
	MP4_BOX_TYPE_todo = MP4_FOURCC('t', 'o', 'd', 'o')
};

#define MP4_BASE_BOX  \
	uint32_t type; \
	uint64_t  size; \
	uint8_t   uuid[16]; \
	int (*read)(struct mp4_box *self, mp4_bits_t *bs); \
	int (*write)(struct mp4_box *self, mp4_bits_t *bs); \
	int (*calc_size)(struct mp4_box *self ); \
	int (*dump)(struct mp4_box *self, FILE *fd );	 \
	int (*destroy)(struct mp4_box *self )

#define MP4_FULL_BOX  \
	MP4_BASE_BOX; \
	uint8_t version; \
	uint32_t flags

struct mp4_box {
	MP4_BASE_BOX;
};

struct mp4_full_box {
	MP4_FULL_BOX;
};

struct mp4_box *mp4_box_create(uint32_t type);

int mp4_full_box_read(struct mp4_box *box, mp4_bits_t * bs);

/* basic box parse function */
int mp4_parse_root_box(struct mp4_box **box, mp4_bits_t * bs);
int mp4_parse_box(struct mp4_box **box, mp4_bits_t * bs);

void mp4_box_list_destroy(mp4_list_t * box);
int mp4_read_child_box_list(struct mp4_box *par, mp4_bits_t * bs,
							int (*add_child) (struct mp4_box * par,
											  struct mp4_box * ch));

/* 14496-12 4.3.2, file type and compatibility */
struct mp4_ftyp_box {
	MP4_BASE_BOX;
	uint32_t major_brand;
	uint32_t minor_version;
	uint32_t compatible_brands_count;
	uint32_t *compatible_brands;
};

/* 14496-12 8.2, media data container */
struct mp4_mdat_box {
	MP4_BASE_BOX;
	uint64_t data_size;
	char *data;
};

/* 14496-12 8.24, free space */
struct mp4_free_box {
	MP4_BASE_BOX;
	uint32_t data_size;
	char *data;
};

/* 14496-12 8.24, free space */
struct mp4_skip_box {
	MP4_BASE_BOX;
	uint32_t data_size;
	char *data;
};

/* 14496-12 8.27, user-data */
struct mp4_udta_box {
	MP4_BASE_BOX;
};

/* 14496-12 8.43, progressive download information */
struct mp4_pdin_box {
	MP4_FULL_BOX;
	uint32_t count;
	uint32_t rate;
	uint32_t initial_delay;
};

/* 14496-12 8.3, movie header, overall declarations */
struct mp4_mvhd_box {
	MP4_FULL_BOX;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;
	uint32_t rate;
	uint16_t volume;
	uint16_t reserved1;
	uint32_t reserved2[2];
	uint32_t matrix[9];
	uint32_t pre_defined[6];
	uint32_t next_track_id;
};

/* 14496-12 8.5, track header, overall information about the track */
struct mp4_tkhd_box {
	MP4_FULL_BOX;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t track_ID;
	uint32_t reserved1;
	uint64_t duration;

	uint32_t reserved2[2];
	int16_t layer;
	int16_t alternate_group;

	int16_t volume;
	uint16_t reserved3;
	int32_t matrix[9];
	int32_t width;
	int32_t height;
};

/* 14496-12 8.6, track reference container */
struct mp4_tref_box {
	MP4_BASE_BOX;
	uint32_t track_ids_count;
	uint32_t *track_ids;
};

struct mp4_elst_entry {
	uint64_t segment_duration;
	int64_t media_time;
	int16_t media_rate_integer;
	int16_t media_rate_fraction;
};

/* 14496-12 8.26, an edit list */
struct mp4_elst_box {
	MP4_FULL_BOX;

	mp4_list_t *entry_list;
};

/* 14496-12 8.25, edit list container */
struct mp4_edts_box {
	MP4_FULL_BOX;
	struct mp4_box *elst;
};

/* 14496-12 8.8, media header, overall information in a track */
struct mp4_mdhd_box {
	MP4_FULL_BOX;
	uint64_t creation_time;
	uint64_t modification_time;
	uint32_t timescale;
	uint64_t duration;

	/* one bit for pad */
	uint8_t pad;
	/* unsigned int(5)[3] language */
	unsigned char language[3];
	uint16_t pre_defined;
};

/* 14496-12 8.9, handler, declares the media(handle) type */
struct mp4_hdlr_box {
	MP4_FULL_BOX;
	uint32_t pre_defined;
	uint32_t handler_type;
	uint32_t reserved[3];
	char *name;					/* in UTF-8 */
};

/* 14496-12 8.11.2, video media header, overall information(video track only) */
struct mp4_vmhd_box {
	MP4_FULL_BOX;
	uint16_t graphicsmode;
	uint16_t opcolor[3];
};

/* 14496-12 8.11.3, sound media header, overall information(sound track only) */
struct mp4_smhd_box {
	MP4_FULL_BOX;
	uint16_t balance;
	uint16_t reserved;
};

/* 14496-12 8.11.4, hint media header, overall information(hint tracks only) */
struct mp4_hmhd_box {
	MP4_FULL_BOX;
	uint16_t maxPDUsize;
	uint16_t avgPDUsize;
	uint32_t maxbitrate;
	uint32_t avgbitrate;
	uint32_t reserved;
};

/* 14496-12, 8.11.5, null media header, overall information(some tracks only) */
struct mp4_nmhd_box {
	MP4_FULL_BOX;
};

/* 14496-12, 8.13, data reference box, declares source(s) of media data in track */
struct mp4_dref_box {
	MP4_FULL_BOX;
	mp4_list_t *data_entry_box;
};

struct mp4_url_box {
	MP4_FULL_BOX;
	char *location;
};

struct mp4_urn_box {
	MP4_FULL_BOX;
	char *name;
	char *location;
};

struct mp4_data_entry_box {
	MP4_FULL_BOX;
	char *location;
};

struct mp4_data_entry_url_box {
	MP4_FULL_BOX;
	char *location;
};

struct mp4_data_entry_urn_box {
	MP4_FULL_BOX;
	char *location;
	char *name;
};

/* 14496-12, 8.12, data information box, container */
struct mp4_dinf_box {
	MP4_BASE_BOX;
	struct mp4_box *dref;
};

/* 14496-12, 8.16, sample describes(codec types, initialization etc. */
struct mp4_stsd_box {
	MP4_FULL_BOX;
	mp4_list_t *sample_entry_boxes;

	int (*get_esds_dec_cfg_info) (struct mp4_box * box,
								  uint8_t * object_type_ind,
								  uint8_t * stream_type,
								  uint8_t * up_stream,
								  uint32_t * buf_size_db,
								  uint32_t * max_bitrate,
								  uint32_t * avg_bitrate);
	int (*get_esds_dec_specfic_info) (struct mp4_box * box,
									  uint8_t ** config,
									  uint32_t * config_size);
};

#define MP4_SAMPLE_ENTRY_FIELDS					\
    MP4_BASE_BOX;								\
    char reserved[6];							\
    uint16_t data_reference_index;

struct mp4_sample_entry {
	MP4_SAMPLE_ENTRY_FIELDS;
};

/* MpegSampleEntry, all other Mpeg stream types */
struct mp4_mp4s_box {
	MP4_SAMPLE_ENTRY_FIELDS;
	struct mp4_box *esd;		/* mp4_esd_box */
};

#define VISUAL_SAMPLE_ENTRY_FIELDS				\
    MP4_SAMPLE_ENTRY_FIELDS;						\
	uint16_t pre_defined;						\
	uint16_t  reserved1;						\
    uint32_t  pre_defined1[3];					\
    uint16_t  width;							\
	uint16_t  height;							\
    uint32_t  horiz_res;						\
	uint32_t  vert_res;							\
    uint32_t  reserved2;						\
    uint16_t  frames_count;						\
    char      compressor_name[33];				\
    uint16_t  bit_depth;						\
    int16_t   pre_defined2

struct visual_sample_entry_box {
	VISUAL_SAMPLE_ENTRY_FIELDS;
};

int visual_sample_entry_read(struct mp4_box *a, mp4_bits_t * bs);

struct mp4_mp4v_box {
	VISUAL_SAMPLE_ENTRY_FIELDS;
	struct mp4_box *esd;		/* mp4_esd_box */
};

struct mp4_avc1_box {
	VISUAL_SAMPLE_ENTRY_FIELDS;
	struct mp4_box *avcC;
	struct mp4_box *btrt;
	/*ext descriptors */
	struct mp4_box *m4ds;
};

#define AUDIO_SAMPLE_ENTRY_FIELDS				\
    MP4_SAMPLE_ENTRY_FIELDS;					\
	uint16_t version;							\
	uint16_t revision;							\
	uint32_t reserved1;							\
	uint16_t channelcount;						\
	uint16_t samplesize;						\
	uint16_t pre_defined;						\
	uint16_t reserved2;							\
	uint16_t sampleratehi;						\
    uint16_t sampleratelo

struct audio_sample_entry_box {
	AUDIO_SAMPLE_ENTRY_FIELDS;
};

struct mp4_mp4a_box {
	AUDIO_SAMPLE_ENTRY_FIELDS;
	struct mp4_box *esd;
};

int audio_sample_entry_read(struct mp4_box *a, mp4_bits_t * bs);

/* 14496-12 10.2, rtp hint sample entry */
struct mp4_rtp_box {
	MP4_SAMPLE_ENTRY_FIELDS;
	uint16_t hinttrackversion;
	uint16_t highestcompatibleversion;
	uint32_t maxpacketsize;

	mp4_list_t *boxes;
};

struct mp4_tims_box {
	MP4_BASE_BOX;
	uint32_t timescale;
};

struct mp4_tsro_box {
	MP4_BASE_BOX;
	uint32_t offset;
};

struct mp4_snro_box {
	MP4_BASE_BOX;
	uint32_t offset;
};

struct mp4_stts_entry {
	uint32_t sample_count;
	uint32_t sample_delta;

};

/* 14496-12, 8.15.2, (decoding) time-to-sample */
struct mp4_stts_box {
	MP4_FULL_BOX;
	mp4_list_t *entry_list;
};

struct mp4_ctts_entry {
	uint32_t sample_count;
	uint32_t sample_offset;

};

/* 14496-12, 8.15.3, (composition) time to sample */
struct mp4_ctts_box {
	MP4_FULL_BOX;
	mp4_list_t *entry_list;
};

struct mp4_stsc_entry {
	uint32_t first_chunk;
	uint32_t samples_per_chunk;
	uint32_t sample_description_index;

};

/* 14496-12, 8.18, sample-to-chunk, partial data-offset information */
struct mp4_stsc_box {
	MP4_FULL_BOX;

	mp4_list_t *entry_list;

	/* self use */
	uint32_t found_entry_id;
	uint32_t found_entry_first_sample_id;

	int (*get_sample_info) (struct mp4_box * box, uint32_t sample_id,
							uint32_t * chunk_id, uint32_t * first_sample_id);
};

/* 14496-12, 8.17.2, sample size(framing) */
struct mp4_stsz_box {
	MP4_FULL_BOX;
	uint32_t sample_size;
	uint32_t sample_count;
	/* if sample_size is 0 */
	uint32_t *entry_size;

	int (*get_sample_offset_in_chunk) (struct mp4_box * box, uint32_t sample_id,
									   uint32_t first_sample,
									   uint32_t * sample_offset);

};

/* 14496-12, 8.17.3, compact sample size(framing) */
struct mp4_stz2_box {
	MP4_FULL_BOX;
	uint8_t reserved[3];
	uint8_t field_size;
	uint32_t sample_count;
	uint32_t *entry_size;		/* array: unsigned int(field_size) entry_size */
};

/* 14496-12, 8.19, chunk offset, partial data-offset information */
struct mp4_stco_box {
	MP4_FULL_BOX;
	uint32_t entry_count;
	uint32_t *chunk_offset;

	 uint32_t(*get_chunk_offset) (struct mp4_box * box, uint32_t chunk_nr);
};

/* 14496-12, 8.19, 64-bit chunk offset */
struct mp4_co64_box {
	MP4_FULL_BOX;
	uint32_t entry_count;
	uint64_t *chunk_offset;

	 uint64_t(*get_chunk_offset) (struct mp4_box * box, uint32_t chunk_nr);
};

/* 14496-12, 8.20, sync sample table(random access points) */
struct mp4_stss_box {
	MP4_FULL_BOX;
	uint32_t entry_count;
	uint32_t *sample_number;
};

/* 14496-12, 8.21, shadow sync sample table */
struct mp4_stsh_box {
	MP4_FULL_BOX;
	uint32_t entry_count;
	uint32_t *shadow_sample_number;
	uint32_t *sync_sample_number;
};

/* 14496-12, 8.23, sample padding bits */
struct mp4_padb_box {
	MP4_FULL_BOX;
	uint32_t sample_count;

	uint8_t *reserved1;			/* 1bit  */
	uint8_t *pad2;				/* 3bits */
	uint8_t *reserved2;			/* 1bit  */
	uint8_t *pad1;				/* 3bits */
};

/* 14496-12, 8.22, sample degradation priority */
struct mp4_stdp_box {
	MP4_FULL_BOX;
	uint16_t *priority;			/* table size is sample_count */
};

/* 14496-12, 8.40.2, independent and disposable sample */
struct mp4_sdtp_box {
	MP4_FULL_BOX;
	uint8_t *reserved;			/* 2bit  */
	uint8_t *sample_depends_on;	/* 2bits */
	uint8_t *sample_is_depended_on;	/* 2bit  */
	uint8_t *sample_has_redundancy;	/* 2bits */
};

/* 14496-12, 8.40.3.2, sample-to-group */
struct mp4_sbgp_box {
	MP4_FULL_BOX;				/* don't support yet */
};

/* 14496-12, 8.40.3.3, sample group description */
struct mp4_sgpd_box {
	MP4_FULL_BOX;				/* don't support yet */
};

/* 14496-12, 8.42, sub-sample information */
struct mp4_subs_box {
	MP4_FULL_BOX;				/* don't support yet */
};

/* 14496-12, 8.14, sample table box, container for the time/space map */
struct mp4_stbl_box {
	MP4_BASE_BOX;
	struct mp4_box *stsd;
	struct mp4_box *stts;
	struct mp4_box *ctts;
	struct mp4_box *stsc;
	struct mp4_box *stsz;
	struct mp4_box *co64;
	struct mp4_box *stss;
	struct mp4_box *stsh;
	struct mp4_box *padb;
	struct mp4_box *stdp;
	struct mp4_box *sdtp;

	 uint32_t(*get_sample_size) (struct mp4_box * stbl, uint32_t sample_id);
	 uint32_t(*get_max_sample_size) (struct mp4_box * stbl);
	int (*get_sample_times) (struct mp4_box * stbl, uint32_t sample_id,
							 uint64_t * duration, uint64_t * dts);
	int (*get_sample_cts_offset) (struct mp4_box * stbl, uint32_t sample_id,
								  uint32_t * cts);
	int (*get_sample_sync_flag) (struct mp4_box * stbl, uint32_t sample_id,
								 uint8_t * is_sync);
	int (*get_sample_file_offset) (struct mp4_box * stbl, uint32_t sample_id,
								   uint64_t * offset);
	int (*get_visual_info) (struct mp4_box * box, uint32_t * width,
							uint32_t * height);
	int (*get_audio_info) (struct mp4_box * box, uint32_t * sample_rate,
						   uint32_t * channels, uint8_t * bits_per_sample,
						   uint8_t * version);
	int (*get_h264_seq_pic_hdrs) (struct mp4_box * box,
								  uint8_t * nr_of_seq_hdr, char ***seq_hdr,
								  uint16_t ** seq_hdr_size,
								  uint8_t * nr_of_pic_hdr, char ***pic_hdr,
								  uint16_t ** pic_hdr_size);
	int (*get_h264_dec_config_info) (struct mp4_box * box, uint8_t * nalu_size,
									 uint8_t * cfg_ver,
									 uint8_t * profile_ind,
									 uint8_t * profile_comp,
									 uint8_t * level_ind);
	int (*find_sample_from_time) (struct mp4_box * box, uint64_t when,
								  uint32_t * sample_id,
								  uint32_t * prev_sample_id);

	/* todo */
};

/* 14496-12 8.10, media information container */
struct mp4_minf_box {
	MP4_BASE_BOX;
	struct mp4_box *header;
	struct mp4_box *dinf;
	struct mp4_box *stbl;
};

/* 14496-12 8.7, container for the media information in a track */
struct mp4_mdia_box {
	MP4_BASE_BOX;
	struct mp4_box *mdhd;
	struct mp4_box *hdlr;
	struct mp4_box *minf;

	struct mp4_box *trak;		/* parent */

	int (*read_sample) (struct mp4_box * box, uint32_t sample_id,
						char *buf, uint32_t * size,
						uint64_t * DTS, uint64_t * CTS,
						uint64_t * duration_time, uint8_t * is_sync);
	int (*read_sample_from_time) (struct mp4_box * box, uint64_t when,
								  uint8_t search_mode, char *buf,
								  uint32_t * size,
								  uint64_t * DTS, uint64_t * CTS,
								  uint64_t * duration_time);
	 uint32_t(*get_sample_from_time) (struct mp4_box * box, uint64_t when,
									  uint8_t search_mode);
};

/* 14496-12 8.44.1, metadata */
struct mp4_meta_box {
	MP4_FULL_BOX;
	/* todo */
};

/* 14496-12 8.1, container for all the metadata */
struct mp4_moov_box {
	MP4_BASE_BOX;
	struct mp4_box *mvhd;
	struct mp4_box *iods;
	struct mp4_box *udta;
	struct mp4_box *meta;

	mp4_list_t *trak_boxes;
	mp4_list_t *other_boxes;
	mp4_handle_t *mov;

	 uint32_t(*get_nr_of_tracks) (struct mp4_moov_box * moov);
	 uint32_t(*get_track_id) (struct mp4_moov_box * moov, uint32_t index);
	 uint32_t(*get_track_idx) (struct mp4_moov_box * moov, uint32_t track_id);
	 uint32_t(*is_track_enable) (struct mp4_moov_box * moov, uint32_t track_id);
	 uint64_t(*get_track_duration) (struct mp4_moov_box * moov,
									uint32_t track_id);
	 uint32_t(*get_media_timescale) (struct mp4_moov_box * moov,
									 uint32_t track_id);
	 uint64_t(*get_media_duration) (struct mp4_moov_box * moov,
									uint32_t track_id);
	 uint32_t(*get_media_type) (struct mp4_moov_box * moov, uint32_t track_id);
	 uint32_t(*get_media_sub_type) (struct mp4_moov_box * moov,
									uint32_t track_id);
	 uint32_t(*get_nr_of_samples) (struct mp4_moov_box * moov,
								   uint32_t track_id);
	 uint32_t(*get_max_sample_size) (struct mp4_moov_box * moov,
									 uint32_t track_id);
	 uint32_t(*get_sample_size) (struct mp4_moov_box * moov,
								 uint32_t track_id, uint32_t sample_id);
	 uint64_t(*get_sample_dts) (struct mp4_moov_box * moov,
								uint32_t track_id, uint32_t sample_id);
	 uint64_t(*get_sample_duration) (struct mp4_moov_box * moov,
									 uint32_t track_id, uint32_t sample_id);
	int (*get_visual_info) (struct mp4_moov_box * moov, uint32_t track_id,
							uint32_t * width, uint32_t * height);
	int (*get_audio_info) (struct mp4_moov_box * moov,
						   uint32_t track_id, uint32_t * sample_rate,
						   uint32_t * channels, uint8_t * bits_per_sample,
						   uint8_t * version);
	int (*get_h264_seq_pic_hdrs) (struct mp4_moov_box * moov, uint32_t track_id,
								  uint8_t * nr_of_seq_hdr, char ***seq_hdr,
								  uint16_t ** seq_hdr_size,
								  uint8_t * nr_of_pic_hdr, char ***pic_hdr,
								  uint16_t ** pic_hdr_size);
	int (*get_h264_dec_config_info) (struct mp4_moov_box * moov,
									 uint32_t track_id, uint8_t * nalu_size,
									 uint8_t * cfg_ver, uint8_t * profile_ind,
									 uint8_t * profile_comp,
									 uint8_t * level_ind);
};

/* 14496-12 8.4, container for an individual track or stream */
struct mp4_trak_box {
	MP4_BASE_BOX;
	struct mp4_box *tkhd;
	struct mp4_box *tref;
	struct mp4_box *edts;
	struct mp4_box *mdia;

	struct mp4_box *udta;
	struct mp4_box *meta;

	struct mp4_box *moov;		/* parent */

	mp4_list_t *other_boxes;
};

/* unknown box */
struct mp4_unknown_box {
	MP4_BASE_BOX;
	uint64_t data_size;
	char *data;
};

/* 14496-14 5.1, object descriptor box */
struct mp4_iods_box {
	MP4_FULL_BOX;
	struct mp4_descriptor *descriptor;
};

/* 14496-15 5.6.1, ES descriptor box */
struct mp4_esds_box {
	MP4_FULL_BOX;
	struct mp4_descriptor *es;
};

/* used by SPS and PPS */
struct avc_parameter_sets {
	uint16_t size;
	char *data;
};

/* 14496-15 5.2.4.1.1, AVC decoder configuration record */
struct avc_dec_cfg_record {
	uint8_t configuration_version;
	uint8_t avc_profile_indication;
	uint8_t profile_compatibility;
	uint8_t avc_level_indication;
	uint8_t nal_unit_size;

	mp4_list_t *sequence_parameter_sets_nalu;
	mp4_list_t *picture_parameter_sets_nalu;

	char **pp_seq_hdr;
	char **pp_pic_hdr;
	uint16_t *p_seq_hdr_size;
	uint16_t *p_pic_hdr_size;

};

/* 14496-15 5.3.4.1.2, AVC Configuration Box  */
struct mp4_avcC_box {
	MP4_BASE_BOX;
	struct avc_dec_cfg_record *avc_config;
};

/* 14496-15 5.3.4.1.2, MPEG4 Bitrate Box */
struct mp4_btrt_box {
	MP4_BASE_BOX;
	uint32_t buffer_size_DB;
	uint32_t max_bitrate;
	uint32_t avg_bitrate;
};

/* 14496-15 5.3.4.1.2, MPEG4 Extension Descriptors Box */
struct mp4_m4ds_box {
	MP4_BASE_BOX;
	mp4_list_t *descriptors;
};

struct mp4_box *free_box_create();
struct mp4_box *ftyp_box_create();

struct mp4_box *udta_box_create();

struct mp4_box *mdat_box_create();
struct mp4_box *moov_box_create();

struct mp4_box *mvhd_box_create();
struct mp4_box *trak_box_create();

struct mp4_box *tkhd_box_create();
struct mp4_box *tref_box_create();
struct mp4_box *edts_box_create();
struct mp4_box *elst_box_create();
struct mp4_box *mdia_box_create();

struct mp4_box *mdhd_box_create();
struct mp4_box *hdlr_box_create();
struct mp4_box *minf_box_create();

struct mp4_box *vmhd_box_create();
struct mp4_box *smhd_box_create();
struct mp4_box *hmhd_box_create();
struct mp4_box *nmhd_box_create();
struct mp4_box *dinf_box_create();
struct mp4_box *dref_box_create();
struct mp4_box *url_box_create();
struct mp4_box *urn_box_create();
struct mp4_box *stbl_box_create();

struct mp4_box *stsd_box_create();
struct mp4_box *stts_box_create();
struct mp4_box *ctts_box_create();
struct mp4_box *stsc_box_create();
struct mp4_box *stsz_box_create();
struct mp4_box *stz2_box_create();
struct mp4_box *stco_box_create();
struct mp4_box *co64_box_create();
struct mp4_box *stss_box_create();
struct mp4_box *stsh_box_create();
struct mp4_box *padb_box_create();
struct mp4_box *mp4v_box_create();
struct mp4_box *mp4a_box_create();
struct mp4_box *mp4s_box_create();
struct mp4_box *esds_box_create();
struct mp4_box *rtp_box_create();
struct mp4_box *tims_box_create();
struct mp4_box *tsro_box_create();
struct mp4_box *snro_box_create();
struct mp4_box *avc1_box_create();
struct mp4_box *avcC_box_create();
struct mp4_box *btrt_box_create();
struct mp4_box *m4ds_box_create();

struct mp4_box *unknown_box_create();

#endif /*__MP4_BOXES_INCLUDED__ */
