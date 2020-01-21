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
#include "mp4_file.h"
#include "mp4_list.h"
#include "mp4_debug.h"

static int mp4_parse_movie_boxes(struct mp4_handle *mov)
{
	struct mp4_box *a;
	uint64_t total_size = 0;
	int status = 0;
	struct mp4_bits *bs;

	bs = mp4_fdm_get_bs(mov->data_map);
	assert(bs != NULL);

	/*while we have some data, parse our boxes */
	while (mp4_bs_available(bs)) {
		status = mp4_parse_root_box(&a, bs);
		if (!a || status < 0) {
			verbose_read("a: %p, status: %d \n", a, status);
			return status;
		}

		switch (a->type) {
			/*moov box */
		case MP4_BOX_TYPE_moov:
			total_size += a->size;
			if (mov->moov) {
				verbose_read("can't exist more than one moov box\n");
				return -1;
			}

			mov->moov = (struct mp4_moov_box *) a;
			/*set our pointer to the movie */
			mov->moov->mov = mov;

			break;

			/*we only keep the MDAT in READ for dump purposes */
		case MP4_BOX_TYPE_mdat:
			total_size += a->size;
			if (mov->mode == MP4_OPEN_READ) {
				if (!mov->mdat) {
					mov->mdat = (struct mp4_mdat_box *) a;
				} else {
					/* mdat can be zero or any number */
					a->destroy(a);
					a = NULL;
				}
			}
			break;

		case MP4_BOX_TYPE_ftyp:
			total_size += a->size;
			/*one and only one ftyp box */
			if (mov->brand) {
				verbose_read("ftyp box exists already \n");
				a->destroy(a);
				return -1;
			}

			mov->brand = (struct mp4_ftyp_box *) a;
			break;

		default:
			total_size += a->size;
			break;
		}

		/* chain this box with the father and the other at same level */
		if (a)
			mp4_list_add(mov->root_boxes, a);
	}

	/*we need at least moov */
	if (!mov->moov) {
		verbose_read("need at least moov box \n");
		return -1;
	}

	/*we must have movie header */
	if (!mov->moov->mvhd)
		return -1;

	return 0;
}

/* open a mp4 file */
struct mp4_handle *mp4_open_file(const char *file, int mode)
{
	struct mp4_handle *mov;
	int err;

	mov = (struct mp4_handle *) malloc(sizeof(struct mp4_handle));
	if (mov == NULL) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(mov, 0, sizeof(struct mp4_handle));

	mov->file_name = strdup(file);
	mov->mode = mode;

	mov->root_boxes = mp4_list_create();
	if (mov->root_boxes == NULL) {
		verbose_err("create root boxes list failed \n");
		return NULL;
	}

	mov->data_map = mp4_fdm_create(file, MP4_DATA_MAP_READ);
	if (mov->data_map == NULL) {
		verbose_err("data map create failed \n");
		return NULL;
	}

	/* parse the whole movie */
	err = mp4_parse_movie_boxes(mov);
	if (err) {
		/* todo, release resource */
		verbose_err("parse the whole movie failed \n");
		return NULL;
	}

	return mov;
}

void mp4_close_file(struct mp4_handle *mov)
{
	if (!mov)
		return;

	if (mov->data_map)
		mp4_fdm_destroy(mov->data_map);

	mp4_box_list_destroy(mov->root_boxes);

	if (mov->file_name)
		free(mov->file_name);

	free(mov);
}
