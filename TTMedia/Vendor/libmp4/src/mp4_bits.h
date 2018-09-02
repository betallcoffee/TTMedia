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
#ifndef __MP4_BITS_INCLUDED__
#define __MP4_BITS_INCLUDED__
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>

#define _XOPEN_SOURCE 500
#include <unistd.h>

/*the default size for new streams allocation...*/
#define BS_MEM_BLOCK_ALLOC_SIZE		250

enum {
	MP4_BITS_READ,
	MP4_BITS_WRITE,
	MP4_BITS_FILE_READ,
	MP4_BITS_FILE_WRITE,
	/*private mode if we own the buffer */
	MP4_BITS_WRITE_DYN
};

typedef struct mp4_bits mp4_bits_t;

mp4_bits_t *mp4_bs_create(char *buffer, uint64_t size, uint32_t mode);
mp4_bits_t *mp4_bs_create_from_file(FILE * f, uint32_t mode);
void mp4_bs_destroy(mp4_bits_t * bs);
uint32_t mp4_bs_read_int(mp4_bits_t * bs, uint32_t nBits);
uint64_t mp4_bs_read_long_int(mp4_bits_t * bs, uint32_t nBits);
float mp4_bs_read_float(mp4_bits_t * bs);
double mp4_bs_read_double(mp4_bits_t * bs);
uint32_t mp4_bs_read_data(mp4_bits_t * bs, char *data, uint32_t nbBytes);
uint32_t mp4_bs_read_u8(mp4_bits_t * bs);
uint32_t mp4_bs_read_u16(mp4_bits_t * bs);
uint32_t mp4_bs_read_u24(mp4_bits_t * bs);
uint32_t mp4_bs_read_u32(mp4_bits_t * bs);
uint64_t mp4_bs_read_u64(mp4_bits_t * bs);
uint32_t mp4_bs_read_u32_le(mp4_bits_t * bs);
uint16_t mp4_bs_read_u16_le(mp4_bits_t * bs);
void mp4_bs_write_int(mp4_bits_t * bs, int32_t value, int32_t nBits);
void mp4_bs_write_long_int(mp4_bits_t * bs, int64_t value, int32_t nBits);
void mp4_bs_write_float(mp4_bits_t * bs, float value);
void mp4_bs_write_double(mp4_bits_t * bs, double value);
uint32_t mp4_bs_write_data(mp4_bits_t * bs, char *data, uint32_t nbBytes);
void mp4_bs_write_u8(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u16(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u24(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u32(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u64(mp4_bits_t * bs, uint64_t value);
void mp4_bs_write_u32_le(mp4_bits_t * bs, uint32_t value);
void mp4_bs_write_u16_le(mp4_bits_t * bs, uint32_t value);
void mp4_bs_set_eos_callback(mp4_bits_t * bs, void (*EndOfStream) (void *par),
							 void *par);
uint8_t mp4_bs_align(mp4_bits_t * bs);
uint64_t mp4_bs_available(mp4_bits_t * bs);
void mp4_bs_get_content(mp4_bits_t * bs, char **output, uint32_t * outSize);
void mp4_bs_skip_bytes(mp4_bits_t * bs, uint64_t nbBytes);
int mp4_bs_seek(mp4_bits_t * bs, uint64_t offset);
uint32_t mp4_bs_peek_bits(mp4_bits_t * bs, uint32_t numBits,
						  uint32_t byte_offset);
uint8_t mp4_bs_bits_available(mp4_bits_t * bs);
uint64_t mp4_bs_get_position(mp4_bits_t * bs);
uint64_t mp4_bs_get_size(mp4_bits_t * bs);
uint64_t mp4_bs_get_refreshed_size(mp4_bits_t * bs);

/* used by sample read */
uint32_t mp4_bs_pread_data(struct mp4_bits *bs, char *data,
						   uint32_t nbBytes, uint64_t offset);

#endif		/*__MP4_BITS_INCLUDED__ */
