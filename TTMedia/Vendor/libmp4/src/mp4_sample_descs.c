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
#include "mp4_debug.h"

/* read visual_sample_entry field */
int visual_sample_entry_read(struct mp4_box *a, mp4_bits_t * bs)
{
	struct visual_sample_entry_box *box = (struct visual_sample_entry_box *) a;
	int i;

	if (box->size < 78)
		return -1;

	box->size -= 78;
	mp4_bs_read_data(bs, box->reserved, 6);
	box->data_reference_index = mp4_bs_read_u16(bs);
	box->pre_defined = mp4_bs_read_u16(bs);
	box->reserved1 = mp4_bs_read_u16(bs);
	for (i = 0; i < 3; i++)
		box->pre_defined1[i] = mp4_bs_read_u32(bs);
	box->width = mp4_bs_read_u16(bs);
	box->height = mp4_bs_read_u16(bs);
	box->horiz_res = mp4_bs_read_u32(bs);
	box->vert_res = mp4_bs_read_u32(bs);
	box->reserved2 = mp4_bs_read_u32(bs);
	box->frames_count = mp4_bs_read_u16(bs);
	mp4_bs_read_data(bs, box->compressor_name, 32);
	box->compressor_name[32] = 0;
	box->bit_depth = mp4_bs_read_u16(bs);
	box->pre_defined2 = mp4_bs_read_u16(bs);

	verbose_read("width: %d, height: %d, frames_count: %d \n",
				 box->width, box->height, box->frames_count);

	return 0;
}

int audio_sample_entry_read(struct mp4_box *a, mp4_bits_t * bs)
{
	struct audio_sample_entry_box *box = (struct audio_sample_entry_box *) a;

	if (box->size < 28)
		return -1;

	box->size -= 28;
	mp4_bs_read_data(bs, box->reserved, 6);
	box->data_reference_index = mp4_bs_read_u16(bs);

	box->version = mp4_bs_read_u16(bs);
	box->revision = mp4_bs_read_u16(bs);
	box->reserved1 = mp4_bs_read_u32(bs);
	box->channelcount = mp4_bs_read_u16(bs);
	box->samplesize = mp4_bs_read_u16(bs);
	box->pre_defined = mp4_bs_read_u16(bs);
	box->reserved2 = mp4_bs_read_u16(bs);
	box->sampleratehi = mp4_bs_read_u16(bs);
	box->sampleratelo = mp4_bs_read_u16(bs);

	verbose_read("channelcount: %d, samplesize: %d \n",
				 box->channelcount, box->samplesize);
	return 0;
}
