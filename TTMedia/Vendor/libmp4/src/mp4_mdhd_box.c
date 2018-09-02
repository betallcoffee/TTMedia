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

static int mdhd_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_mdhd_box *self = (struct mp4_mdhd_box *) box;
	int ret;
	int i;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	if (self->version == 1) {
		self->creation_time = mp4_bs_read_u64(bs);
		self->modification_time = mp4_bs_read_u64(bs);
		self->timescale = mp4_bs_read_u32(bs);
		self->duration = mp4_bs_read_u64(bs);
	} else {					/* version==0 */
		self->creation_time = mp4_bs_read_u32(bs);
		self->modification_time = mp4_bs_read_u32(bs);
		self->timescale = mp4_bs_read_u32(bs);
		self->duration = mp4_bs_read_u32(bs);
	}

	verbose_read("creation_time: %llu, modification_time: %llu, "
				 "timescale: %u, duration: %llu\n",
				 self->creation_time, self->modification_time,
				 self->timescale, self->duration);

	self->pad = mp4_bs_read_int(bs, 1);
	for (i = 0; i < 3; i++)
		self->language[i] = mp4_bs_read_int(bs, 5);
	self->pre_defined = mp4_bs_read_int(bs, 16);

	return 0;
}

static int mdhd_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int mdhd_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int mdhd_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int mdhd_box_destroy(struct mp4_box *box)
{
	struct mp4_mdhd_box *self = (struct mp4_mdhd_box *) box;

	if (!self)
		return -1;

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_box *mdhd_box_create()
{
	struct mp4_mdhd_box *self;

	self = (struct mp4_mdhd_box *) malloc(sizeof(struct mp4_mdhd_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_mdhd_box));

	verbose_memleak("create %p...\n", self);
	self->destroy = mdhd_box_destroy;
	self->read = mdhd_box_read;
	self->write = mdhd_box_write;
	self->calc_size = mdhd_box_calc_size;
	self->dump = mdhd_box_dump;

	return (struct mp4_box *) self;
}
