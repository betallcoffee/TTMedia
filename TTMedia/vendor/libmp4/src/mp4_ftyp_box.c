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
#include "mp4_boxes.h"
#include "mp4_bits.h"
#include "mp4_debug.h"

static int ftyp_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_ftyp_box *self = (struct mp4_ftyp_box *) box;
	int i;
	char brand_str[5];

	self->major_brand = mp4_bs_read_u32(bs);
	self->minor_version = mp4_bs_read_u32(bs);
	self->size -= 8;

	mp4_fourcc_to_str(self->major_brand, brand_str, 5);
	verbose_read("brand: %s \n", brand_str);

	self->compatible_brands_count = self->size / sizeof(uint32_t);
	self->compatible_brands =
		(uint32_t *) malloc(self->compatible_brands_count * sizeof(uint32_t));
	if (!self->compatible_brands) {
		verbose_err("malloc failed \n");
		return -1;
	}

	for (i = 0; i < self->compatible_brands_count; i++)
		self->compatible_brands[i] = mp4_bs_read_u32(bs);

	return 0;
}

static int ftyp_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int ftyp_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int ftyp_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int ftyp_box_destroy(struct mp4_box *box)
{
	struct mp4_ftyp_box *self = (struct mp4_ftyp_box *) box;

	if (!self)
		return -1;

	if (self->compatible_brands)
		free(self->compatible_brands);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_box *ftyp_box_create()
{
	struct mp4_ftyp_box *self;

	self = (struct mp4_ftyp_box *) malloc(sizeof(struct mp4_ftyp_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_ftyp_box));

	verbose_memleak("create %p...\n", self);

	self->destroy = ftyp_box_destroy;
	self->read = ftyp_box_read;
	self->write = ftyp_box_write;
	self->calc_size = ftyp_box_calc_size;
	self->dump = ftyp_box_dump;

	return (struct mp4_box *) self;
}
