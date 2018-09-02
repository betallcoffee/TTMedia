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

static int stsz_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_stsz_box *self = (struct mp4_stsz_box *) box;
	int ret;
	uint32_t i;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	self->sample_size = mp4_bs_read_u32(bs);
	self->sample_count = mp4_bs_read_u32(bs);
	verbose_read("sample_size: %d, sample count: %d \n",
				 self->sample_size, self->sample_count);

	if (self->sample_size == 0) {
		self->entry_size =
			(uint32_t *) malloc(self->sample_count * sizeof(uint32_t));
		if (!self->entry_size) {
			verbose_err("malloc failed \n");
			return -1;
		}

		for (i = 0; i < self->sample_count; i++) {
			self->entry_size[i] = mp4_bs_read_u32(bs);
			verbose_read_table("sample_size[%d]: %d\n", i, self->entry_size[i]);
		}
	}

	return 0;
}

static int stsz_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int stsz_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int stsz_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int stsz_box_destroy(struct mp4_box *box)
{
	struct mp4_stsz_box *self = (struct mp4_stsz_box *) box;

	if (!self)
		return -1;

	if (self->entry_size)
		free(self->entry_size);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

static int stsz_get_sample_offset_in_chunk(struct mp4_box *box,
										   uint32_t sample_id,
										   uint32_t first_sample,
										   uint32_t * sample_offset)
{
	struct mp4_stsz_box *self = (struct mp4_stsz_box *) box;

	if (!self)
		return -1;

	if (sample_id >= self->sample_count)
		return -1;

	if (first_sample > sample_id || !sample_offset)
		return -1;

	if (self->sample_size) {
		*sample_offset = (sample_id - first_sample) * self->sample_size;
	} else {
		uint32_t i;
		uint32_t offset = 0;

		for (i = first_sample; i < sample_id; i++)
			offset += self->entry_size[i];
		*sample_offset = offset;
	}

	return 0;
}

struct mp4_box *stsz_box_create()
{
	struct mp4_stsz_box *self;

	self = (struct mp4_stsz_box *) malloc(sizeof(struct mp4_stsz_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_stsz_box));

	verbose_memleak("create %p...\n", self);
	self->destroy = stsz_box_destroy;
	self->read = stsz_box_read;
	self->write = stsz_box_write;
	self->calc_size = stsz_box_calc_size;
	self->dump = stsz_box_dump;

	self->get_sample_offset_in_chunk = stsz_get_sample_offset_in_chunk;

	return (struct mp4_box *) self;
}
