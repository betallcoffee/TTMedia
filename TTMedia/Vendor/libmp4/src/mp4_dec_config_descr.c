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
#include "mp4_descriptor.h"
#include "mp4_debug.h"

/* add the child desc to parent */
static int dec_config_descr_add_child_descr(struct mp4_descriptor *p,
											struct mp4_descriptor *c)
{
	struct mp4_dec_config_descriptor *par =
		(struct mp4_dec_config_descriptor *) p;

	if (!p || !c) {
		verbose_err("Input is NULL \n");
		return -1;
	}

	switch (c->tag) {
	case DecSpecificInfoTag:
		if (par->dec_specific_info) {
			verbose_err("dec_specific_info isn't NULL \n");
			return -1;
		}
		par->dec_specific_info = c;
		break;
	case profileLevelIndicationIndexDescrTag:
		mp4_list_add(par->profile_level_indication_index_descr, c);
		break;
	default:
		verbose_read("unsupport descr\n");
		c->destroy(c);
		return -1;
	}

	return 0;
}

static int mp4_dec_config_descr_read(struct mp4_descriptor *descr,
									 mp4_bits_t * bs)
{
	struct mp4_dec_config_descriptor *self =
		(struct mp4_dec_config_descriptor *) descr;
	int hdr_size = 0;
	int ret;

	assert(self != NULL);

	self->object_type_indication = mp4_bs_read_int(bs, 8);
	self->stream_type = mp4_bs_read_int(bs, 6);
	self->up_stream = mp4_bs_read_int(bs, 1);
	self->reserved = mp4_bs_read_int(bs, 1);
	self->buffer_size_db = mp4_bs_read_int(bs, 24);
	self->maxbitrate = mp4_bs_read_u32(bs);
	self->avgbitrate = mp4_bs_read_u32(bs);
	hdr_size += 13;

	verbose_read("object_type_indication: %d, stream_type: %d, "
				 "up_stream: %d, buffer_size_db: %d, "
				 "maxbitrate: %d, avgbitrate: %d \n",
				 self->object_type_indication,
				 self->stream_type, self->up_stream,
				 self->buffer_size_db, self->maxbitrate, self->avgbitrate);

	self->size -= hdr_size;

	ret = mp4_read_child_descr_list(descr, bs,
									dec_config_descr_add_child_descr);
	return ret;
}

static int mp4_dec_config_descr_write(struct mp4_descriptor *descr,
									  mp4_bits_t * bs)
{
	return 0;
}

static int mp4_dec_config_descr_calc_size(struct mp4_descriptor *descr)
{
	return 0;
}

static int mp4_dec_config_descr_dump(struct mp4_descriptor *descr, FILE * fd)
{
	return 0;
}

static int mp4_dec_config_descr_destroy(struct mp4_descriptor *descr)
{
	struct mp4_dec_config_descriptor *self =
		(struct mp4_dec_config_descriptor *) descr;

	if (!self)
		return -1;

	if (self->dec_specific_info)
		self->dec_specific_info->destroy(self->dec_specific_info);

	mp4_descr_list_destroy(self->profile_level_indication_index_descr);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_descriptor *mp4_dec_config_descr_create()
{
	struct mp4_dec_config_descriptor *descr;

	descr = (struct mp4_dec_config_descriptor *)
		malloc(sizeof(struct mp4_dec_config_descriptor));
	if (!descr)
		return NULL;
	memset(descr, 0, sizeof(struct mp4_dec_config_descriptor));

	descr->profile_level_indication_index_descr = mp4_list_create();
	if (!descr->profile_level_indication_index_descr) {
		verbose_read("list create failed \n");
		return NULL;
	}

	verbose_memleak("create %p...\n", descr);

	descr->read = mp4_dec_config_descr_read;
	descr->write = mp4_dec_config_descr_write;
	descr->calc_size = mp4_dec_config_descr_calc_size;
	descr->destroy = mp4_dec_config_descr_destroy;
	descr->dump = mp4_dec_config_descr_dump;

	return (struct mp4_descriptor *) descr;
}
