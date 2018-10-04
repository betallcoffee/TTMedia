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
#include <stdio.h>
#include "mp4_boxes.h"
#include "mp4_debug.h"

/* add the child box to parent */
static int moov_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_moov_box *par = (struct mp4_moov_box *) p;

	switch (c->type) {
	case MP4_BOX_TYPE_iods:
		if (par->iods)
			return -1;
		par->iods = c;
		/* must */
		if (!((struct mp4_iods_box *) c)->descriptor) {
			par->iods = NULL;
			c->destroy(c);
		}
		break;

	case MP4_BOX_TYPE_mvhd:
		if (par->mvhd)
			return -1;
		par->mvhd = c;
		break;

	case MP4_BOX_TYPE_udta:
		if (par->udta)
			return -1;
		par->udta = c;
		break;

	case MP4_BOX_TYPE_meta:
		if (par->meta)
			return -1;
		par->meta = c;
		break;

	case MP4_BOX_TYPE_trak:
		/*set our pointer to parent */
		((struct mp4_trak_box *) c)->moov = p;
		/* add the child to track list */
		mp4_list_add(par->trak_boxes, c);
		break;

	default:
		/* add the unknown box to other box list */
		mp4_list_add(par->other_boxes, c);
		break;
	}

	return 0;
}

static int moov_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	return mp4_read_child_box_list(box, bs, moov_add_child_box);
}

static int moov_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int moov_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int moov_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int moov_box_destroy(struct mp4_box *box)
{
	struct mp4_moov_box *self = (struct mp4_moov_box *) box;

	if (!self)
		return -1;

	if (self->iods)
		self->iods->destroy(self->iods);

	if (self->mvhd)
		self->mvhd->destroy(self->mvhd);

	if (self->udta)
		self->udta->destroy(self->udta);

	if (self->meta)
		self->meta->destroy(self->meta);

	mp4_box_list_destroy(self->trak_boxes);
	mp4_box_list_destroy(self->other_boxes);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

static uint32_t moov_get_nr_of_tracks(struct mp4_moov_box *moov)
{
	return mp4_list_count(moov->trak_boxes);
}

static uint32_t moov_get_track_id(struct mp4_moov_box *moov, uint32_t index)
{
	struct mp4_trak_box *trak;
	struct mp4_tkhd_box *tkhd;

	if (!moov || (index >= mp4_list_count(moov->trak_boxes)))
		return 0;

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	tkhd = (struct mp4_tkhd_box *) trak->tkhd;

	return tkhd->track_ID;
}

static uint32_t moov_get_track_idx(struct mp4_moov_box *moov, uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_tkhd_box *tkhd;
	uint32_t i, count;

	if (!moov)
		return 0;

	count = mp4_list_count(moov->trak_boxes);
	for (i = 0; i < count; i++) {
		trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, i);
		if (!trak)
			return 0;
		tkhd = (struct mp4_tkhd_box *) trak->tkhd;
		if (tkhd->track_ID == track_id)
			return i;
	}

	return 0;
}

static struct mp4_stbl_box *moov_get_track_of_idx(struct mp4_moov_box *moov, uint32_t index)
{
    struct mp4_trak_box *trak;
    struct mp4_mdia_box *mdia;
    struct mp4_minf_box *minf;
    struct mp4_stbl_box *stbl;
    
    if (!moov)
        return NULL;
    
    trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
    if (!trak)
        return NULL;
    
    mdia = (struct mp4_mdia_box *) trak->mdia;
    if (!mdia)
        return NULL;
    
    minf = (struct mp4_minf_box *) mdia->minf;
    if (!minf)
        return NULL;
    
    stbl = (struct mp4_stbl_box *) minf->stbl;
    if (!stbl)
        return NULL;
    
    return stbl;
}

static uint32_t moov_is_track_enable(struct mp4_moov_box *moov,
									 uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_tkhd_box *tkhd;
	uint32_t index;

	if (!moov)
		return 0;
	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	tkhd = (struct mp4_tkhd_box *) trak->tkhd;

	return (tkhd->flags & 0x000001) ? 1 : 0;
}

static uint64_t moov_get_track_duration(struct mp4_moov_box *moov,
										uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_tkhd_box *tkhd;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	tkhd = (struct mp4_tkhd_box *) trak->tkhd;

	return tkhd->duration;
}

static uint32_t moov_get_media_timescale(struct mp4_moov_box *moov,
										 uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_mdhd_box *mdhd;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	mdhd = (struct mp4_mdhd_box *) mdia->mdhd;
	if (!mdhd)
		return 0;

	return mdhd->timescale;
}

static uint64_t moov_get_media_duration(struct mp4_moov_box *moov,
										uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_mdhd_box *mdhd;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	mdhd = (struct mp4_mdhd_box *) mdia->mdhd;
	if (!mdhd)
		return 0;

	return mdhd->duration;
}

static uint32_t moov_get_media_type(struct mp4_moov_box *moov,
									uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_hdlr_box *hdlr;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	hdlr = (struct mp4_hdlr_box *) mdia->hdlr;
	if (!hdlr)
		return 0;

	return hdlr->handler_type;
}

static uint32_t moov_get_media_sub_type(struct mp4_moov_box *moov,
										uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	struct mp4_stsd_box *stsd;
	struct mp4_box *entry;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return 0;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return 0;

	return entry->type;
}

static uint32_t moov_get_nr_of_samples(struct mp4_moov_box *moov,
									   uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	struct mp4_stsz_box *stsz;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	stsz = (struct mp4_stsz_box *) stbl->stsz;
	if (!stsz)
		return 0;

	return stsz->sample_count;
}

static uint32_t moov_get_max_sample_size(struct mp4_moov_box *moov,
										 uint32_t track_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	return stbl->get_max_sample_size((struct mp4_box *) stbl);
}

static uint32_t moov_get_sample_size(struct mp4_moov_box *moov,
									 uint32_t track_id, uint32_t sample_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	return stbl->get_sample_size((struct mp4_box *) stbl, sample_id);
}

static uint64_t moov_get_sample_dts(struct mp4_moov_box *moov,
									uint32_t track_id, uint32_t sample_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;
	uint64_t dts = 0;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	stbl->get_sample_times((struct mp4_box *) stbl, sample_id, NULL, &dts);

	return dts;
}

static uint64_t moov_get_sample_duration(struct mp4_moov_box *moov,
										 uint32_t track_id, uint32_t sample_id)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;
	uint64_t duration = 0;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	stbl->get_sample_times((struct mp4_box *) stbl, sample_id, &duration, NULL);

	return duration;
}

static int moov_get_visual_info(struct mp4_moov_box *moov,
								uint32_t track_id, uint32_t * width,
								uint32_t * height)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	return stbl->get_visual_info((struct mp4_box *) stbl, width, height);
}

static int moov_get_audio_info(struct mp4_moov_box *moov,
							   uint32_t track_id, uint32_t * sample_rate,
							   uint32_t * channels, uint8_t * bits_per_sample,
							   uint8_t * version)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return 0;

	index = moov_get_track_idx(moov, track_id);

	trak = (struct mp4_trak_box *) mp4_list_get(moov->trak_boxes, index);
	if (!trak)
		return 0;

	mdia = (struct mp4_mdia_box *) trak->mdia;
	if (!mdia)
		return 0;

	minf = (struct mp4_minf_box *) mdia->minf;
	if (!minf)
		return 0;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return 0;

	return stbl->get_audio_info((struct mp4_box *) stbl, sample_rate,
								channels, bits_per_sample, version);
}

int moov_get_h264_seq_pic_hdrs(struct mp4_moov_box *moov, uint32_t track_id,
							   uint8_t * nr_of_seq_hdr,
							   char ***seq_hdr,
							   uint16_t ** seq_hdr_size,
							   uint8_t * nr_of_pic_hdr,
							   char ***pic_hdr, uint16_t ** pic_hdr_size)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return -1;

	index = moov_get_track_idx(moov, track_id);

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

	return stbl->get_h264_seq_pic_hdrs((struct mp4_box *) stbl,
									   nr_of_seq_hdr, seq_hdr,
									   seq_hdr_size, nr_of_pic_hdr,
									   pic_hdr, pic_hdr_size);
}

int moov_get_h264_dec_config_info(struct mp4_moov_box *moov, uint32_t track_id,
								  uint8_t * nalu_size,
								  uint8_t * cfg_ver,
								  uint8_t * profile_ind, uint8_t * profile_comp,
								  uint8_t * level_ind)
{
	struct mp4_trak_box *trak;
	struct mp4_mdia_box *mdia;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t index;

	if (!moov)
		return -1;

	index = moov_get_track_idx(moov, track_id);

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

	return stbl->get_h264_dec_config_info((struct mp4_box *) stbl, nalu_size,
										  cfg_ver, profile_ind, profile_comp,
										  level_ind);
}

struct mp4_box *moov_box_create()
{
	struct mp4_moov_box *self;

	self = (struct mp4_moov_box *) malloc(sizeof(struct mp4_moov_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_moov_box));

	verbose_memleak("create %p...\n", self);

	self->trak_boxes = mp4_list_create();
	if (!self->trak_boxes) {
		free(self);
		verbose_err("create trak boxes list failed \n");
		return NULL;
	}

	self->other_boxes = mp4_list_create();
	if (!self->other_boxes) {
		free(self->trak_boxes);
		free(self);
		verbose_err("create other boxes list failed \n");
		return NULL;
	}

	self->destroy = moov_box_destroy;
	self->read = moov_box_read;
	self->write = moov_box_write;
	self->calc_size = moov_box_calc_size;
	self->dump = moov_box_dump;
	self->get_nr_of_tracks = moov_get_nr_of_tracks;
	self->get_track_id = moov_get_track_id;
	self->get_track_idx = moov_get_track_idx;
    self->get_track_of_idx = moov_get_track_of_idx;
	self->is_track_enable = moov_is_track_enable;
	self->get_track_duration = moov_get_track_duration;
	self->get_media_timescale = moov_get_media_timescale;
	self->get_media_duration = moov_get_media_duration;
	self->get_media_type = moov_get_media_type;
	self->get_media_sub_type = moov_get_media_sub_type;
	self->get_nr_of_samples = moov_get_nr_of_samples;
	self->get_max_sample_size = moov_get_max_sample_size;
	self->get_sample_size = moov_get_sample_size;
	self->get_sample_dts = moov_get_sample_dts;
	self->get_sample_duration = moov_get_sample_duration;
	self->get_visual_info = moov_get_visual_info;
	self->get_audio_info = moov_get_audio_info;
	self->get_h264_seq_pic_hdrs = moov_get_h264_seq_pic_hdrs;
	self->get_h264_dec_config_info = moov_get_h264_dec_config_info;

	return (struct mp4_box *) self;
}
