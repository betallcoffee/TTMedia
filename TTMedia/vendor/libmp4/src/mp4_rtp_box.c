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

static int rtp_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_rtp_box *self = (struct mp4_rtp_box *) box;
	struct mp4_box *c;
	int ret;

	if (self->size < 16)
		return -1;

	self->size -= 16;
	mp4_bs_read_data(bs, self->reserved, 6);
	self->data_reference_index = mp4_bs_read_u16(bs);
	self->hinttrackversion = mp4_bs_read_u16(bs);
	self->highestcompatibleversion = mp4_bs_read_u16(bs);
	self->maxpacketsize = mp4_bs_read_u32(bs);

	while (self->size) {
		ret = mp4_parse_box(&c, bs);
		if (ret)
			return ret;
		ret = mp4_list_add(self->boxes, c);
		if (ret)
			return ret;
		self->size -= c->size;
	}

	return 0;
}

static int rtp_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int rtp_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int rtp_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int rtp_box_destroy(struct mp4_box *box)
{
	struct mp4_rtp_box *self = (struct mp4_rtp_box *) box;

	if (!self)
		return -1;

	mp4_box_list_destroy(self->boxes);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_box *rtp_box_create()
{
	struct mp4_rtp_box *self;

	self = (struct mp4_rtp_box *) malloc(sizeof(struct mp4_rtp_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_rtp_box));

	verbose_memleak("create %p...\n", self);
	self->boxes = mp4_list_create();
	if (!self->boxes) {
		verbose_err("create boxes list failed \n");
		free(self);
		return NULL;
	}

	self->hinttrackversion = 1;
	self->highestcompatibleversion = 1;

	self->destroy = rtp_box_destroy;
	self->read = rtp_box_read;
	self->write = rtp_box_write;
	self->calc_size = rtp_box_calc_size;
	self->dump = rtp_box_dump;

	return (struct mp4_box *) self;
}
