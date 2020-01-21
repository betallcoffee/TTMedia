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

static int mp4_default_descr_read(struct mp4_descriptor *descr, mp4_bits_t * bs)
{
	struct mp4_default_descriptor *self =
		(struct mp4_default_descriptor *) descr;
	uint32_t byte_to_read;

	if (self->size > 0xFFFFFFFF)
		return -1;

	byte_to_read = (uint32_t) self->size;
	if (byte_to_read) {
		self->data = (char *) malloc(byte_to_read);
		if (!self->data) {
			verbose_err("malloc failed \n");
			/* skip the data */
			return -1;
		}
		memset(self->data, 0, byte_to_read);

		verbose_read("read %d data \n", byte_to_read);
		self->data_size = byte_to_read;

		mp4_bs_read_data(bs, self->data, byte_to_read);
	}

	return 0;
}

static int mp4_default_descr_write(struct mp4_descriptor *descr,
								   mp4_bits_t * bs)
{
	return 0;
}

static int mp4_default_descr_calc_size(struct mp4_descriptor *descr)
{
	return 0;
}

static int mp4_default_descr_dump(struct mp4_descriptor *descr, FILE * fd)
{
	return 0;
}

static int mp4_default_descr_destroy(struct mp4_descriptor *descr)
{
	struct mp4_default_descriptor *self =
		(struct mp4_default_descriptor *) descr;

	if (!self)
		return -1;

	if (self->data)
		free(self->data);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

struct mp4_descriptor *mp4_default_descr_create()
{
	struct mp4_default_descriptor *descr;

	descr = (struct mp4_default_descriptor *)
		malloc(sizeof(struct mp4_default_descriptor));
	if (!descr)
		return NULL;

	memset(descr, 0, sizeof(struct mp4_default_descriptor));

	verbose_memleak("create %p...\n", descr);

	descr->read = mp4_default_descr_read;
	descr->write = mp4_default_descr_write;
	descr->calc_size = mp4_default_descr_calc_size;
	descr->destroy = mp4_default_descr_destroy;
	descr->dump = mp4_default_descr_dump;

	return (struct mp4_descriptor *) descr;
}
