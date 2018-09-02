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

static int ctts_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_ctts_box *self = (struct mp4_ctts_box *) box;
	int ret;
	uint32_t count, i;
	struct mp4_ctts_entry *entry;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	count = mp4_bs_read_u32(bs);
	verbose_read("entry count: %d \n", count);

	for (i = 0; i < count; i++) {
		entry = (struct mp4_ctts_entry *) malloc(sizeof(struct mp4_ctts_entry));
		if (!entry) {
			verbose_err("malloc failed \n");
			return -1;
		}

		entry->sample_count = mp4_bs_read_u32(bs);
		entry->sample_offset = mp4_bs_read_u32(bs);
		verbose_read("entry[%d]: count: %u, offset: %u \n",
					 i, entry->sample_count, entry->sample_offset);
		mp4_list_add(self->entry_list, entry);
	}

	return 0;
}

static int ctts_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int ctts_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int ctts_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int ctts_box_destroy(struct mp4_box *box)
{
	struct mp4_ctts_box *self = (struct mp4_ctts_box *) box;
	struct mp4_ctts_entry *entry;

	if (!self)
		return -1;

	while (mp4_list_count(self->entry_list)) {
		entry = (struct mp4_ctts_entry *)
			mp4_list_get(self->entry_list, 0);

		assert(entry != NULL);
		mp4_list_rem(self->entry_list, 0);
		free(entry);
	}

	mp4_list_destroy(self->entry_list);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

struct mp4_box *ctts_box_create()
{
	struct mp4_ctts_box *self;

	self = (struct mp4_ctts_box *) malloc(sizeof(struct mp4_ctts_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_ctts_box));

	verbose_memleak("create %p...\n", self);

	self->entry_list = mp4_list_create();
	if (!self->entry_list) {
		verbose_err("list create failed \n");
		return NULL;
	}

	self->destroy = ctts_box_destroy;
	self->read = ctts_box_read;
	self->write = ctts_box_write;
	self->calc_size = ctts_box_calc_size;
	self->dump = ctts_box_dump;

	return (struct mp4_box *) self;
}
