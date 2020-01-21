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
#include "mp4_file_data_map.h"
#include "mp4.h"
#include "mp4_debug.h"

/* add the child box to parent */
static int mdia_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_mdia_box *par = (struct mp4_mdia_box *) p;

	switch (c->type) {
	case MP4_BOX_TYPE_mdhd:
		if (par->mdhd)
			return -1;
		par->mdhd = c;
		break;

	case MP4_BOX_TYPE_hdlr:
		if (par->hdlr)
			return -1;
		par->hdlr = c;
		break;

	case MP4_BOX_TYPE_minf:
		if (par->minf)
			return -1;
		par->minf = c;
		break;

	default:
		/* unknown box, destroy it */
		c->destroy(c);
		break;
	}

	return 0;
}

static int mdia_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	return mp4_read_child_box_list(box, bs, mdia_add_child_box);
}

static int mdia_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int mdia_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int mdia_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int mdia_box_destroy(struct mp4_box *box)
{
	struct mp4_mdia_box *self = (struct mp4_mdia_box *) box;

	if (!self)
		return -1;

	if (self->mdhd)
		self->mdhd->destroy(self->mdhd);

	if (self->hdlr)
		self->hdlr->destroy(self->hdlr);

	if (self->minf)
		self->minf->destroy(self->minf);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	self = NULL;

	return 0;
}

static mp4_fdm_t *mdia_get_media_fdm(struct mp4_box *box, uint32_t sample_id)
{
	struct mp4_mdia_box *self = (struct mp4_mdia_box *) box;
	struct mp4_trak_box *trak;
	struct mp4_moov_box *moov;
	struct mp4_handle *mov;

	if (!self)
		return NULL;

	/* todo */

	trak = (struct mp4_trak_box *) self->trak;
	if (!trak) {
		verbose_err("trak is NULL \n");
		return NULL;
	}

	moov = (struct mp4_moov_box *) trak->moov;
	if (!moov) {
		verbose_err("moov is NULL \n");
		return NULL;
	}

	mov = (struct mp4_handle *) moov->mov;

	return mov->data_map;
}

static int mdia_read_sample(struct mp4_box *box, uint32_t sample_id,
							char *buf, uint32_t * size,
							uint64_t * DTS, uint64_t * CTS,
							uint64_t * duration_time, uint8_t * is_sync)
{
	struct mp4_mdia_box *self = (struct mp4_mdia_box *) box;
	uint32_t sample_size;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t CTS_offset;
	mp4_fdm_t *fdm;
	uint32_t bytes_read;
	uint64_t offset;

	if (!buf || !size || !DTS || !CTS || !duration_time || !is_sync || !self) {
		verbose_err("Input ptr is NULL \n");
		return -1;
	}

	minf = (struct mp4_minf_box *) self->minf;
	if (!minf)
		return -1;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return -1;

	/* get sample size */
	sample_size = stbl->get_sample_size((struct mp4_box *) stbl, sample_id);
	if (*size < sample_size) {
		verbose_err("buffer is too small \n");
		return -1;
	}
	*size = sample_size;

	/* get duration and DTS */
	stbl->get_sample_times((struct mp4_box *) stbl, sample_id, duration_time,
						   DTS);

	/* get CTS */
	stbl->get_sample_cts_offset((struct mp4_box *) stbl, sample_id,
								&CTS_offset);
	*CTS = *DTS + CTS_offset;

	/* get is_sync */
	stbl->get_sample_sync_flag((struct mp4_box *) stbl, sample_id, is_sync);

	/*overwrite sync sample with sample dep if any */

	/* shadow sync? */

	/* get sample file offset */
	stbl->get_sample_file_offset((struct mp4_box *) stbl, sample_id, &offset);

	/* get mp4_fdm_t object */
	fdm = mdia_get_media_fdm(box, sample_id);
	if (!fdm) {
		verbose_err("fdm is NULL \n");
		return -1;
	}

	/* read the data */
	bytes_read = mp4_fdm_get_data(fdm, buf, sample_size, offset);
	if (bytes_read < sample_size) {
		verbose_err("incomplete file \n");
		return -1;
	}

	return 0;
}

int mdia_get_sync_sample(struct mp4_box *box, uint32_t sample_id,
						 uint32_t * sync_id, uint8_t search_mode)
{
	struct mp4_mdia_box *self = (struct mp4_mdia_box *) box;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	struct mp4_stss_box *stss;
	struct mp4_stsz_box *stsz;
	uint32_t prev = 0xFFFFFFFF;
	uint32_t next = 0xFFFFFFFF;
	int i;

	minf = (struct mp4_minf_box *) self->minf;
	if (!minf)
		return -1;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return -1;

	stss = (struct mp4_stss_box *) stbl->stss;
	if (!stss)
		return -1;

	stsz = (struct mp4_stsz_box *) stbl->stsz;
	if (!stsz)
		return -1;

	/* set to current sample if we don't find a sync sample */
	*sync_id = sample_id;

	if ((search_mode == MP4_SEARCH_SYNC_FORWARD) &&
		(sample_id >= stsz->sample_count - 1)) {
		*sync_id = stsz->sample_count - 1;
		return 0;
	}

	if ((search_mode == MP4_SEARCH_SYNC_BACKWARD) && sample_id == 0xFFFFFFFF) {
		*sync_id = 0;
		return 0;
	}

	for (i = 0; i < stss->entry_count; i++) {
		verbose_read_table("sample_number[%d]: %d, sample_id: %d \n",
			   i, stss->sample_number[i], sample_id);
		if (stss->sample_number[i] == sample_id + 1) {
			*sync_id = stss->sample_number[i] - 1;
			return 0;
		} else if (stss->sample_number[i] > sample_id + 1) {
			next = stss->sample_number[i] - 1;
			break;
		} else {
			prev = stss->sample_number[i] - 1;
		}
	}

	verbose_read_table("prev: %d, next: %d\n", prev, next);
	if (search_mode == MP4_SEARCH_SYNC_FORWARD) {
		if (next != 0xFFFFFFFF)
			*sync_id = next;
	} else {
		if (prev != 0xFFFFFFFF)
			*sync_id = prev;
	}

	verbose_read_table("sync_id is %d \n", *sync_id);

	return 0;
}

/* get the sample_id from desired time */
uint32_t mdia_get_sample_from_time(struct mp4_box * box, uint64_t when,
								   uint8_t search_mode)
{
	struct mp4_mdia_box *self = (struct mp4_mdia_box *) box;
	struct mp4_minf_box *minf;
	struct mp4_stbl_box *stbl;
	uint32_t sample_id, prev_sample_id;
	int use_shadow;
	int sync_search;
	int ret;
	uint32_t sync_id;

	minf = (struct mp4_minf_box *) self->minf;
	if (!minf)
		return -1;

	stbl = (struct mp4_stbl_box *) minf->stbl;
	if (!stbl)
		return -1;

	/* get sample id of the desired time */
	stbl->find_sample_from_time((struct mp4_box *) stbl, when,
								&sample_id, &prev_sample_id);

	/*if no shadow table, search sync */
	use_shadow = 0;
	if (!stbl->stsh && (search_mode == MP4_SEARCH_SYNC_SHADOW))
		search_mode = MP4_SEARCH_SYNC_BACKWARD;

	/*if no sync sample table, disable sync search, as all samples are sync */
	if (!stbl->stss) {
		if (search_mode == MP4_SEARCH_SYNC_FORWARD)
			search_mode = MP4_SEARCH_FORWARD;
		if (search_mode == MP4_SEARCH_SYNC_BACKWARD)
			search_mode = MP4_SEARCH_BACKWARD;
	}

	sync_search = 0;
	
	verbose_read_table("search_mode: %d\n", search_mode);
	verbose_read_table("sample_id: 0x%x, prev_sample_id: 0x%x\n",
					   sample_id, prev_sample_id);
	switch (search_mode) {
	case MP4_SEARCH_SYNC_FORWARD:
		sync_search = 1;
	case MP4_SEARCH_FORWARD:
		if (sample_id == 0xFFFFFFFF && prev_sample_id == 0xFFFFFFFF) {
			prev_sample_id = ((struct mp4_stsz_box *) (stbl->stsz))->sample_count-1;
		}else if( sample_id == 0xFFFFFFFF ) {
			if (prev_sample_id !=
				((struct mp4_stsz_box *) (stbl->stsz))->sample_count - 1) {
				sample_id = prev_sample_id + 1;
			} else {
				sample_id = prev_sample_id;
			}
		}
		break;

	case MP4_SEARCH_SYNC_BACKWARD:
		sync_search = 1;
	case MP4_SEARCH_SYNC_SHADOW:
	case MP4_SEARCH_BACKWARD:
	default:
		verbose_read_table("sample_id: 0x%x\n", sample_id);
		if (sample_id == 0xFFFFFFFF && prev_sample_id == 0xFFFFFFFF) {
			sample_id =
				((struct mp4_stsz_box *) (stbl->stsz))->sample_count - 1;
		} else if (sample_id == 0xFFFFFFFF) {
			sample_id = prev_sample_id;
		}
		break;
	}

	if (sync_search) {
		verbose_read_table("sync: %d \n", sample_id);
		ret = mdia_get_sync_sample(box, sample_id, &sync_id, search_mode);
		if (ret) {
			verbose_err("get sync sampled failed \n");
			return ret;
		}

		if (sync_id != 0xFFFFFFFF)
			sample_id = sync_id;
	}

	/* todo, shadow sync support */
	verbose_read_table("end sample_id: %d \n", sample_id);

	return sample_id;
}

int mdia_read_sample_from_time(struct mp4_box *box, uint64_t when,
							   uint8_t search_mode, char *buf,
							   uint32_t * size, uint64_t * DTS,
							   uint64_t * CTS, uint64_t * duration_time)
{
	uint32_t sample_id;

	sample_id = mdia_get_sample_from_time(box, when, search_mode);

	return mdia_read_sample(box, sample_id, buf, size, DTS,
							CTS, duration_time, NULL);
}

struct mp4_box *mdia_box_create()
{
	struct mp4_mdia_box *self;

	self = (struct mp4_mdia_box *) malloc(sizeof(struct mp4_mdia_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_mdia_box));

	verbose_memleak("create %p...\n", self);

	self->destroy = mdia_box_destroy;
	self->read = mdia_box_read;
	self->write = mdia_box_write;
	self->calc_size = mdia_box_calc_size;
	self->dump = mdia_box_dump;

	self->read_sample = mdia_read_sample;
	self->read_sample_from_time = mdia_read_sample_from_time;
	self->get_sample_from_time = mdia_get_sample_from_time;

	return (struct mp4_box *) self;
}
