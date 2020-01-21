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
#ifndef __MP4_INCLUDED__
#define __MP4_INCLUDED__
#include "mp4_util.h"
#include "mp4_file_data_map.h"
#include "mp4_list.h"

struct mp4_moov_box;
struct mp4_mdat_box;
struct mp4_ftyp_box;

/* internal mp4 movie file object */
struct mp4_handle {
	char *file_name;
	uint8_t mode;

	mp4_fdm_t *data_map;

	/* moov box */
	struct mp4_moov_box *moov;
	/* mdat box (one and only one when we store the file) */
	struct mp4_mdat_box *mdat;
	/* file brand (since v2, NULL means mp4 v1) */
	struct mp4_ftyp_box *brand;

	/* link all the root boxes */
	mp4_list_t *root_boxes;

};

#endif /*__MP4_INCLUDED__ */
