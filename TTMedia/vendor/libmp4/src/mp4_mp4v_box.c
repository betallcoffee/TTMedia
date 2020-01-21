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
static int mp4v_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_mp4v_box *par = (struct mp4_mp4v_box *) p;

	switch (c->type) {
	case MP4_BOX_TYPE_esds:
		if (par->esd) {
			verbose_err("esd is NULL \n");
			return -1;
		}
		par->esd = c;
		break;
	default:
		verbose_err("unknown box: %d \n", c->type);
		c->destroy(c);
		return -1;
	}

	return 0;
}

static int mp4v_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	int ret;

	ret = visual_sample_entry_read(box, bs);
	if (ret) {
		verbose_err("visual sample entry read failed \n");
		return ret;
	}

	return mp4_read_child_box_list(box, bs, mp4v_add_child_box);
}

static int mp4v_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int mp4v_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int mp4v_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int mp4v_box_destroy(struct mp4_box *box)
{
	struct mp4_mp4v_box *self = (struct mp4_mp4v_box *) box;

	if (!self)
		return -1;

	if (self->esd)
		self->esd->destroy(self->esd);

	verbose_memleak("destroy %p...\n", self);
	free(self);
	return 0;
}

struct mp4_box *mp4v_box_create()
{
	struct mp4_mp4v_box *self;

	self = (struct mp4_mp4v_box *) malloc(sizeof(struct mp4_mp4v_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_mp4v_box));

	verbose_memleak("create %p...\n", self);
	self->destroy = mp4v_box_destroy;
	self->read = mp4v_box_read;
	self->write = mp4v_box_write;
	self->calc_size = mp4v_box_calc_size;
	self->dump = mp4v_box_dump;

	return (struct mp4_box *) self;
}
