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
static int stsd_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_stsd_box *par = (struct mp4_stsd_box *) p;
	int ret;
	struct mp4_unknown_box *unknown;

	switch (c->type) {
	case MP4_BOX_TYPE_mp4a:
	case MP4_BOX_TYPE_mp4v:
	case MP4_BOX_TYPE_avc1:
	case MP4_BOX_TYPE_rtp:
	case MP4_BOX_TYPE_mp4s:
		ret = mp4_list_add(par->sample_entry_boxes, c);
		break;

	default:
		/* unknow box */
		verbose_read("unknown box \n");
		unknown = (struct mp4_unknown_box *) c;
		/*we need at least 8 bytes for unknown sample entries */
		if (unknown->data_size < 8) {
			c->destroy(c);
			return -1;
		}

		ret = mp4_list_add(par->sample_entry_boxes, c);
		break;
	}

	return ret;
}

static int stsd_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	int ret;
	uint32_t count, i;
	struct mp4_box *c;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	count = mp4_bs_read_u32(bs);

	for (i = 0; i < count; i++) {
		ret = mp4_parse_box(&c, bs);
		if (ret) {
			verbose_err("parse box failed \n");
			return ret;
		}

		ret = stsd_add_child_box(box, c);
		if (ret) {
			verbose_err("add child box failed \n");
			return ret;
		}
	}

	return 0;
}

static int stsd_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int stsd_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int stsd_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int stsd_box_destroy(struct mp4_box *box)
{
	struct mp4_stsd_box *self = (struct mp4_stsd_box *) box;

	if (!self)
		return -1;

	mp4_box_list_destroy(self->sample_entry_boxes);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

static int stsd_get_esds_dec_cfg_info(struct mp4_box *box,
									  uint8_t * object_type_ind,
									  uint8_t * stream_type,
									  uint8_t * up_stream,
									  uint32_t * buf_size_db,
									  uint32_t * max_bitrate,
									  uint32_t * avg_bitrate)
{
	struct mp4_stsd_box *stsd = (struct mp4_stsd_box *) box;
	struct mp4_esds_box *esds;
	struct mp4_box *entry;
	struct mp4_mp4a_box *mp4a;
	struct mp4_mp4v_box *mp4v;
	struct mp4_es_descriptor *es;
	struct mp4_dec_config_descriptor *dec_config;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	if (entry->type == MP4_BOX_TYPE_mp4a) {
		mp4a = (struct mp4_mp4a_box *) entry;
		esds = (struct mp4_esds_box *) mp4a->esd;
		if (!esds) {
			verbose_read_table("NO esds BOX\n");
			return -1;
		}
	} else if (entry->type == MP4_BOX_TYPE_mp4v) {
		mp4v = (struct mp4_mp4v_box *) entry;
		esds = (struct mp4_esds_box *) mp4v->esd;
		if (!esds) {
			verbose_read_table("NO esds BOX\n");
			return -1;
		}
	} else {
		verbose_read_table("to be support \n");
		return -1;
	}

	es = (struct mp4_es_descriptor *) esds->es;
	if (!es) {
		verbose_read_table("no es decsr \n");
		return -1;
	}

	dec_config = (struct mp4_dec_config_descriptor *) es->dec_config_descr;
	if (!dec_config) {
		verbose_read_table("dec config descr is NULL \n");
		return -1;
	}

	if (object_type_ind)
		*object_type_ind = dec_config->object_type_indication;

	if (stream_type)
		*stream_type = dec_config->stream_type;

	if (up_stream)
		*up_stream = dec_config->up_stream;

	if (buf_size_db)
		*buf_size_db = dec_config->buffer_size_db;

	if (max_bitrate)
		*max_bitrate = dec_config->maxbitrate;

	if (avg_bitrate)
		*avg_bitrate = dec_config->avgbitrate;

	return 0;
}

static int stsd_get_esds_dec_specfic_info(struct mp4_box *box,
										  uint8_t ** config,
										  uint32_t * config_size)
{
	struct mp4_stsd_box *stsd = (struct mp4_stsd_box *) box;
	struct mp4_esds_box *esds;
	struct mp4_box *entry;
	struct mp4_mp4a_box *mp4a;
	struct mp4_mp4v_box *mp4v;
	struct mp4_es_descriptor *es;
	struct mp4_dec_config_descriptor *dec_config;
	struct mp4_default_descriptor *dec_specific_info;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	if (entry->type == MP4_BOX_TYPE_mp4a) {
		mp4a = (struct mp4_mp4a_box *) entry;
		esds = (struct mp4_esds_box *) mp4a->esd;
		if (!esds) {
			verbose_read_table("NO esds BOX\n");
			return -1;
		}
	} else if (entry->type == MP4_BOX_TYPE_mp4v) {
		mp4v = (struct mp4_mp4v_box *) entry;
		esds = (struct mp4_esds_box *) mp4v->esd;
		if (!esds) {
			verbose_read_table("NO esds BOX\n");
			return -1;
		}
	} else {
		verbose_read_table("to be support \n");
		return -1;
	}

	es = (struct mp4_es_descriptor *) esds->es;
	if (!es) {
		verbose_read_table("no es decsr \n");
		return -1;
	}

	dec_config = (struct mp4_dec_config_descriptor *) es->dec_config_descr;
	if (!dec_config) {
		verbose_read_table("dec config descr is NULL \n");
		return -1;
	}

	dec_specific_info =
		(struct mp4_default_descriptor *) dec_config->dec_specific_info;

	*config = (uint8_t *) dec_specific_info->data;

	if (config_size)
		*config_size = dec_specific_info->data_size;

	return 0;
}

struct mp4_box *stsd_box_create()
{
	struct mp4_stsd_box *self;

	self = (struct mp4_stsd_box *) malloc(sizeof(struct mp4_stsd_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_stsd_box));

	self->sample_entry_boxes = mp4_list_create();
	if (!self->sample_entry_boxes) {
		verbose_err("sample entry boxes create failed \n");
		free(self);
		return NULL;
	}

	verbose_memleak("create %p...\n", self);

	self->destroy = stsd_box_destroy;
	self->read = stsd_box_read;
	self->write = stsd_box_write;
	self->calc_size = stsd_box_calc_size;
	self->dump = stsd_box_dump;

	self->get_esds_dec_cfg_info = stsd_get_esds_dec_cfg_info;
	self->get_esds_dec_specfic_info = stsd_get_esds_dec_specfic_info;

	return (struct mp4_box *) self;
}
