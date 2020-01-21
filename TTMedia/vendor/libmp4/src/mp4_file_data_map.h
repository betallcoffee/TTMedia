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
#ifndef __MP4_FILE_DATA_MAP_INCLUDED__
#define __MP4_FILE_DATA_MAP_INCLUDED__
#include "mp4_bits.h"

/*File data map modes*/
enum {
	/*read mode */
	MP4_DATA_MAP_READ,
	/*write mode */
	MP4_DATA_MAP_WRITE,
};

typedef struct mp4_fdm mp4_fdm_t;

mp4_fdm_t *mp4_fdm_create(const char *file_name, uint8_t mode);
void mp4_fdm_destroy(mp4_fdm_t * map);

mp4_bits_t *mp4_fdm_get_bs(mp4_fdm_t * map);
uint32_t mp4_fdm_get_data(mp4_fdm_t * ptr, char *buffer, uint32_t buf_len,
						  uint64_t offset);

#endif							/* __MP4_FILE_DATA_MAP_INCLUDED__ */
