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
#include "mp4_file_data_map.h"
#include "mp4_debug.h"

struct mp4_fdm {
	uint8_t mode;
	mp4_bits_t *bs;
	FILE *stream;
};

mp4_fdm_t *mp4_fdm_create(const char *file_name, uint8_t mode)
{
	mp4_fdm_t *map = NULL;
	uint8_t bs_mode;

	if (file_name == NULL) {
		verbose_err("file name is NULL \n");
		return map;
	}

	map = (mp4_fdm_t *) malloc(sizeof(mp4_fdm_t));
	if (map == NULL) {
		verbose_err("malloc failed \n");
		return map;
	}
	memset(map, 0, sizeof(mp4_fdm_t));

	map->mode = mode;
	switch (mode) {
	case MP4_DATA_MAP_READ:
		map->stream = fopen(file_name, "rb");
		bs_mode = MP4_BITS_READ;
		break;
	case MP4_DATA_MAP_WRITE:
		map->stream = fopen(file_name, "wb");
		bs_mode = MP4_BITS_WRITE;
		break;
	default:
		free(map);
		return NULL;
	}

	if (map->stream == NULL) {
		verbose_err("open file(%s) failed \n", file_name);
		free(map);
		return NULL;
	}

	map->bs = mp4_bs_create_from_file(map->stream, bs_mode);
	if (map->bs == NULL) {
		verbose_err("bs create from file failed \n");
		fclose(map->stream);
		free(map);
		return NULL;
	}

	verbose_memleak("create %p...\n", map);

	return map;
}

void mp4_fdm_destroy(mp4_fdm_t * map)
{
	if (!map)
		return;

	if (map->bs)
		mp4_bs_destroy(map->bs);

	if (map->stream)
		fclose(map->stream);

	verbose_memleak("destroy %p...\n", map);
	free(map);
}

mp4_bits_t *mp4_fdm_get_bs(mp4_fdm_t * map)
{
	return map->bs;
}

uint32_t mp4_fdm_get_data(mp4_fdm_t * ptr, char *buffer, uint32_t buf_len,
						  uint64_t offset)
{
	uint32_t bytes_read;

	if (offset > mp4_bs_get_size(ptr->bs))
		return 0;

	bytes_read = mp4_bs_pread_data(ptr->bs, buffer, buf_len, offset);

	return bytes_read;
}
