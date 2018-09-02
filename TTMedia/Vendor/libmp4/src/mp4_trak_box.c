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
static int trak_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_trak_box *par = (struct mp4_trak_box *) p;

	switch (c->type) {
	case MP4_BOX_TYPE_tkhd:
		if (par->tkhd)
			return -1;
		par->tkhd = c;
		break;

	case MP4_BOX_TYPE_tref:
		if (par->tref)
			return -1;
		par->tref = c;
		break;

	case MP4_BOX_TYPE_edts:
		if (par->edts)
			return -1;
		par->edts = c;
		break;

	case MP4_BOX_TYPE_mdia:
		if (par->mdia)
			return -1;
		par->mdia = c;
		/*set our pointer to parent */
		((struct mp4_mdia_box *) c)->trak = p;
		break;

	case MP4_BOX_TYPE_udta:
		if (par->udta)
			return -1;
		par->udta = c;
		break;

	case MP4_BOX_TYPE_meta:
		if (par->meta)
			return -1;
		par->meta = c;
		break;

	default:
		/* add the unknown box to other box list */
		mp4_list_add(par->other_boxes, c);
		break;
	}

	return 0;
}

static int trak_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	return mp4_read_child_box_list(box, bs, trak_add_child_box);
}

static int trak_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int trak_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int trak_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int trak_box_destroy(struct mp4_box *box)
{
	struct mp4_trak_box *self = (struct mp4_trak_box *) box;

	if (!self)
		return -1;

	if (self->tkhd)
		self->tkhd->destroy(self->tkhd);

	if (self->tref)
		self->tref->destroy(self->tref);

	if (self->edts)
		self->edts->destroy(self->edts);

	if (self->mdia)
		self->mdia->destroy(self->mdia);

	if (self->udta)
		self->udta->destroy(self->udta);

	if (self->meta)
		self->meta->destroy(self->meta);

	mp4_box_list_destroy(self->other_boxes);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	self = NULL;

	return 0;
}

struct mp4_box *trak_box_create()
{
	struct mp4_trak_box *self;

	self = (struct mp4_trak_box *) malloc(sizeof(struct mp4_trak_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_trak_box));

	verbose_memleak("create %p...\n", self);

	self->other_boxes = mp4_list_create();
	if (!self->other_boxes) {
		verbose_err("list create failed \n");
		free(self);
		return NULL;
	}

	self->destroy = trak_box_destroy;
	self->read = trak_box_read;
	self->write = trak_box_write;
	self->calc_size = trak_box_calc_size;
	self->dump = trak_box_dump;

	return (struct mp4_box *) self;
}
