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

/* add the child desc to parent */
static int es_descr_add_child_descr(struct mp4_descriptor *p,
									struct mp4_descriptor *c)
{
	struct mp4_es_descriptor *par = (struct mp4_es_descriptor *) p;

	if (!p || !c) {
		verbose_err("parent: %p, child: %p \n", p, c);
		return -1;
	}

	verbose_read("parent: %p, child: %p \n", p, c);
	switch (c->tag) {
	case DecoderConfigDescrTag:
		if (par->dec_config_descr) {
			verbose_read("dec_config_descr not NULL \n");
			return -1;
		}
		par->dec_config_descr = c;
		break;
	case SLConfigDescrTag:
		if (par->sl_config_descr) {
			verbose_err("sl_config_descr not NULL \n");
			return -1;
		}
		par->sl_config_descr = c;
		break;
	case LanguageDescrTag:
		if (par->lang_descr) {
			verbose_err("lang_descr not NULL \n");
			return -1;
		}
		par->lang_descr = c;
		break;
	default:
		verbose_read("unsupport descr\n");
		c->destroy(c);
		break;
	}

	return 0;
}

static int mp4_es_descr_read(struct mp4_descriptor *descr, mp4_bits_t * bs)
{
	struct mp4_es_descriptor *self = (struct mp4_es_descriptor *) descr;
	int hdr_size = 0;
	int ret;

	if (!self)
		return -1;

	self->es_id = mp4_bs_read_int(bs, 16);
	self->stream_dependence_flag = mp4_bs_read_int(bs, 1);
	self->url_flag = mp4_bs_read_int(bs, 1);
	self->ocr_stream_flag = mp4_bs_read_int(bs, 1);
	self->stream_priority = mp4_bs_read_int(bs, 5);
	hdr_size += 3;

	verbose_read("es_id: %d \n", self->es_id);

	if (self->stream_dependence_flag) {
		self->depends_on_es_id = mp4_bs_read_int(bs, 16);
		hdr_size += 2;
	}

	if (self->url_flag) {
		self->url_len = mp4_bs_read_int(bs, 8);
		hdr_size += 1;

		self->url_string = (char *) malloc(self->url_len + 1);
		if (self->url_string) {
			verbose_err("malloc failed \n");
			return -1;
		}
		mp4_bs_read_data(bs, self->url_string, self->url_len);
		/* for safety */
		self->url_string[self->url_len] = 0;

		hdr_size += self->url_len;
	}

	if (self->ocr_stream_flag) {
		hdr_size += 2;
		self->ocr_es_id = mp4_bs_read_int(bs, 16);
	}

	self->size -= hdr_size;

	ret = mp4_read_child_descr_list(descr, bs, es_descr_add_child_descr);

	return ret;
}

static int mp4_es_descr_write(struct mp4_descriptor *descr, mp4_bits_t * bs)
{
	return 0;
}

static int mp4_es_descr_calc_size(struct mp4_descriptor *descr)
{
	return 0;
}

static int mp4_es_descr_dump(struct mp4_descriptor *descr, FILE * fd)
{
	return 0;
}

static int mp4_es_descr_destroy(struct mp4_descriptor *descr)
{
	struct mp4_es_descriptor *self = (struct mp4_es_descriptor *) descr;

	if (!self)
		return -1;

	if (self->url_string)
		free(self->url_string);

	if (self->dec_config_descr)
		self->dec_config_descr->destroy(self->dec_config_descr);

	if (self->sl_config_descr)
		self->sl_config_descr->destroy(self->sl_config_descr);

	if (self->lang_descr)
		self->lang_descr->destroy(self->lang_descr);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

struct mp4_descriptor *mp4_es_descr_create()
{
	struct mp4_es_descriptor *descr;

	descr = (struct mp4_es_descriptor *)
		malloc(sizeof(struct mp4_es_descriptor));
	if (!descr)
		return NULL;
	memset(descr, 0, sizeof(struct mp4_es_descriptor));

	verbose_memleak("create %p...\n", descr);

	descr->read = mp4_es_descr_read;
	descr->write = mp4_es_descr_write;
	descr->calc_size = mp4_es_descr_calc_size;
	descr->destroy = mp4_es_descr_destroy;
	descr->dump = mp4_es_descr_dump;

	return (struct mp4_descriptor *) descr;
}
