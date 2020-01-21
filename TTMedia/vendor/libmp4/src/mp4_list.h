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
#ifndef __MP4_LIST_INCLUDE__
#define __MP4_LIST_INCLUDE__
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct mp4_list mp4_list_t;

mp4_list_t *mp4_list_create();
void mp4_list_destroy(mp4_list_t * list);

uint32_t mp4_list_count(mp4_list_t * list);
int mp4_list_add(mp4_list_t * list, void *item);
int mp4_list_insert(mp4_list_t * list, void *item, uint32_t idx);
int mp4_list_rem(mp4_list_t * list, uint32_t idx);
void *mp4_list_get(mp4_list_t * list, uint32_t idx);
int mp4_list_find(mp4_list_t * list, void *item);
int mp4_list_del_item(mp4_list_t * list, void *item);
void mp4_list_reset(mp4_list_t * list);
void *mp4_list_last(mp4_list_t * list);
int mp4_list_rem_last(mp4_list_t * list);
void *mp4_list_enum(mp4_list_t * list, uint32_t * idx);

#endif							/* __MP4_LIST_INCLUDE__ */
