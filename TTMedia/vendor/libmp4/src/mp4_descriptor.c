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

struct mp4_descriptor *mp4_descriptor_create(uint8_t tag)
{
	struct mp4_descriptor *descr = NULL;

	switch (tag) {
	case ES_DescrTag:
		descr = mp4_es_descr_create();
		break;
	case DecoderConfigDescrTag:
		descr = mp4_dec_config_descr_create();
		break;
	case ObjectDescrTag:
	case InitialObjectDescrTag:
	case DecSpecificInfoTag:
	default:
		descr = mp4_default_descr_create();
		break;
	}

	return descr;
}

int mp4_parse_descriptor(struct mp4_descriptor **descr, mp4_bits_t * bs)
{
	uint32_t val, size, hdr_size;
	uint8_t tag;
	struct mp4_descriptor *tmp_desc;
	int ret;

	if (!bs) {
		*descr = NULL;
		return -1;
	}

	tag = mp4_bs_read_int(bs, 8);
	hdr_size = 1;

	size = 0;
	do {
		val = mp4_bs_read_int(bs, 8);
		hdr_size++;
		size <<= 7;
		size |= val & 0x7F;
	} while (val & 0x80);

	verbose_read("tag: %d, size: %d, hdr_size: %d\n", tag, size, hdr_size);

	tmp_desc = mp4_descriptor_create(tag);
	if (!tmp_desc) {
		*descr = NULL;
		return -1;
	}

	tmp_desc->tag = tag;
	tmp_desc->size = size;
	ret = tmp_desc->read(tmp_desc, bs);
	if (ret) {
		verbose_err("read failed \n");
		tmp_desc->destroy(tmp_desc);
		*descr = NULL;
		return -1;
	}

	/* restore the whole descr size */
	tmp_desc->size = size + hdr_size;

	*descr = tmp_desc;
	return 0;
}

void mp4_descr_list_destroy(mp4_list_t * list)
{
	uint32_t count, i;
	struct mp4_descriptor *descr;

	if (!list)
		return;

	count = mp4_list_count(list);
	for (i = 0; i < count; i++) {
		descr = (struct mp4_descriptor *) mp4_list_get(list, i);
		if (descr)
			descr->destroy(descr);
	}

	mp4_list_destroy(list);
}

int mp4_read_child_descr_list(struct mp4_descriptor *par, mp4_bits_t * bs,
							  int (*add_child) (struct mp4_descriptor * par,
												struct mp4_descriptor * ch))
{
	int ret;
	struct mp4_descriptor *descr;

	while (par->size) {
		ret = mp4_parse_descriptor(&descr, bs);
		if (ret) {
			verbose_err("parse descriptor failed \n");
			if (descr)
				descr->destroy(descr);
			return ret;
		}

		if (par->size < descr->size) {
			verbose_err("par->size(%d) < descr->size(%d) failed \n",
						par->size, descr->size);
			if (descr)
				descr->destroy(descr);
			return -1;
		}

		par->size -= descr->size;
		ret = add_child(par, descr);
		if (ret) {
			verbose_err("add child failed \n");
			descr->destroy(descr);
			return ret;
		}
	}

	return 0;
}
