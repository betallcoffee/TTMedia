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

static int stsc_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_stsc_box *self = (struct mp4_stsc_box *) box;
	int ret;
	uint32_t count, i;
	struct mp4_stsc_entry *entry;

	ret = mp4_full_box_read(box, bs);
	if (ret) {
		verbose_err("full box read failed \n");
		return ret;
	}

	count = mp4_bs_read_u32(bs);
	verbose_read("entry count: %d \n", count);

	for (i = 0; i < count; i++) {
		entry = (struct mp4_stsc_entry *) malloc(count);
		if (!entry) {
			verbose_err("malloc failed \n");
			return -1;
		}

		entry->first_chunk = mp4_bs_read_u32(bs);
		entry->samples_per_chunk = mp4_bs_read_u32(bs);
		entry->sample_description_index = mp4_bs_read_u32(bs);
		verbose_read_table("entry[%d]: first_chunk: %d, sample_per_chunk: %d, "
						   "sample_description_index: %d \n",
						   i, entry->first_chunk, entry->samples_per_chunk,
						   entry->sample_description_index);
		mp4_list_add(self->entry_list, entry);
	}

	return 0;
}

static int stsc_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int stsc_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int stsc_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int stsc_box_destroy(struct mp4_box *box)
{
	struct mp4_stsc_box *self = (struct mp4_stsc_box *) box;
	struct mp4_stsc_entry *entry;

	if (!self)
		return -1;

	while (mp4_list_count(self->entry_list)) {
		entry = (struct mp4_stsc_entry *)
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

static int stsc_get_sample_info(struct mp4_box *box, uint32_t sample_id,
								uint32_t * chunk_id,
								uint32_t * first_sample_id_in_chunk)
{
	struct mp4_stsc_box *self = (struct mp4_stsc_box *) box;
	struct mp4_stsc_entry *entry;
	int i;
	uint32_t first_chunk_in_run;
	uint32_t first_sample_this_chunk_run;
	uint32_t entry_count;

	if (!self || !chunk_id || !first_sample_id_in_chunk)
		return -1;

	entry_count = mp4_list_count(self->entry_list);

	first_sample_this_chunk_run = 0;

	*chunk_id = 0;

	for (i = 0; i < entry_count; i++) {
		entry = mp4_list_get(self->entry_list, i);
		first_chunk_in_run = entry->first_chunk - 1;	/* chunk start from 0 */

		if (i == entry_count - 1) {	/* the last run */
			uint32_t sample_offset_in_this_chunk;

			sample_offset_in_this_chunk =
				(sample_id - first_sample_this_chunk_run);
			*chunk_id =
				first_chunk_in_run +
				(sample_offset_in_this_chunk / entry->samples_per_chunk);
			*first_sample_id_in_chunk =
				((*chunk_id - first_chunk_in_run) * entry->samples_per_chunk) +
				first_sample_this_chunk_run;
			self->found_entry_id = i;
		} else {
			uint32_t next_chunk;
			uint32_t samples_this_chunk_run;
			struct mp4_stsc_entry *next_entry;

			next_entry = mp4_list_get(self->entry_list, i + 1);
			next_chunk = next_entry->first_chunk - 1;	/* chunk start from 0 */

			samples_this_chunk_run = (next_chunk - first_chunk_in_run) *
				entry->samples_per_chunk;
			if (sample_id <=
				(first_sample_this_chunk_run + samples_this_chunk_run)) {
				uint32_t sample_offset_in_this_chunk;

				sample_offset_in_this_chunk =
					(sample_id - first_sample_this_chunk_run);
				*chunk_id =
					first_chunk_in_run +
					(sample_offset_in_this_chunk / entry->samples_per_chunk);
				*first_sample_id_in_chunk =
					((*chunk_id -
					  first_chunk_in_run) * entry->samples_per_chunk) +
					first_sample_this_chunk_run;
				self->found_entry_id = i;

				break;
			} else {
				first_sample_this_chunk_run += samples_this_chunk_run;
			}

		}
	}

	return 0;
}

struct mp4_box *stsc_box_create()
{
	struct mp4_stsc_box *self;

	self = (struct mp4_stsc_box *) malloc(sizeof(struct mp4_stsc_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_stsc_box));

	self->entry_list = mp4_list_create();
	if (!self->entry_list) {
		verbose_err("list create failed \n");
		return NULL;
	}

	verbose_memleak("create %p...\n", self);

	self->destroy = stsc_box_destroy;
	self->read = stsc_box_read;
	self->write = stsc_box_write;
	self->calc_size = stsc_box_calc_size;
	self->dump = stsc_box_dump;
	self->get_sample_info = stsc_get_sample_info;

	return (struct mp4_box *) self;
}
