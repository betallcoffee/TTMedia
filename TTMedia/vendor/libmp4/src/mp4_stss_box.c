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

static int stss_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_stss_box *self = (struct mp4_stss_box *) box;
	int ret;
	uint32_t i;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	self->entry_count = mp4_bs_read_u32(bs);
	if (self->entry_count > 0) {
		self->sample_number =
			(uint32_t *) malloc(self->entry_count * sizeof(uint32_t));
		if (!self->sample_number) {
			verbose_err("malloc failed \n");
			return -1;
		}

		for (i = 0; i < self->entry_count; i++) {
			self->sample_number[i] = mp4_bs_read_u32(bs);
			verbose_read("sample_number[%u]: %u\n", i, self->sample_number[i]);
		}
	}

	return 0;
}

static int stss_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int stss_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int stss_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int stss_box_destroy(struct mp4_box *box)
{
	struct mp4_stss_box *self = (struct mp4_stss_box *) box;

	if (!self)
		return -1;

	if (self->sample_number)
		free(self->sample_number);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_box *stss_box_create()
{
	struct mp4_stss_box *self;

	self = (struct mp4_stss_box *) malloc(sizeof(struct mp4_stss_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_stss_box));

	verbose_memleak("create %p...\n", self);

	self->destroy = stss_box_destroy;
	self->read = stss_box_read;
	self->write = stss_box_write;
	self->calc_size = stss_box_calc_size;
	self->dump = stss_box_dump;

	return (struct mp4_box *) self;
}
