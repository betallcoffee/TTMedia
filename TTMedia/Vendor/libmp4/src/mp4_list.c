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
#include "mp4_list.h"
#include "mp4_debug.h"

struct mp4_list {
	void **slots;
	uint32_t entry_count;
};

struct mp4_list *mp4_list_create()
{
	struct mp4_list *list;

	list = (struct mp4_list *) malloc(sizeof(struct mp4_list));
	if (!list)
		return NULL;

	list->slots = NULL;
	list->entry_count = 0;

	return list;
}

void mp4_list_destroy(struct mp4_list *list)
{
	if (!list)
		return;

	if (list->slots)
		free(list->slots);

	free(list);
}

int mp4_list_add(struct mp4_list *list, void *item)
{
	int slots_size;

	if (!list)
		return -1;

	list->entry_count++;
	slots_size = list->entry_count * sizeof(void *);
	list->slots = (void **) realloc(list->slots, slots_size);
	if (!list->slots) {
		list->entry_count = 0;
		return -1;
	}
	list->slots[list->entry_count - 1] = item;

	return 0;
}

uint32_t mp4_list_count(struct mp4_list * list)
{
	return list ? list->entry_count : 0;
}

void *mp4_list_get(struct mp4_list *list, uint32_t idx)
{
	if (!list || (idx >= list->entry_count))
		return NULL;

	return list->slots[idx];
}

void *mp4_list_last(struct mp4_list *list)
{
	if (!list || !list->entry_count)
		return NULL;

	return list->slots[list->entry_count - 1];
}

/*idx is from 0 to entry_count - 1*/
int mp4_list_rem(struct mp4_list *list, uint32_t idx)
{
	uint32_t i;

	if (!list || !list->slots || !list->entry_count)
		return -1;

	i = list->entry_count - idx - 1;
	if (i)
		memmove(&list->slots[idx], &list->slots[idx + 1], sizeof(void *) * i);

	list->slots[list->entry_count - 1] = NULL;
	list->entry_count -= 1;
	list->slots =
		(void **) realloc(list->slots, sizeof(void *) * list->entry_count);
	return 0;
}

int mp4_list_rem_last(struct mp4_list *list)
{
	if (!list || !list->slots || !list->entry_count)
		return -1;

	list->entry_count -= 1;
	list->slots =
		(void **) realloc(list->slots, sizeof(void *) * list->entry_count);
	return 0;
}

/*position is from 0 to entry_count - 1*/
int mp4_list_insert(struct mp4_list *list, void *item, uint32_t position)
{
	uint32_t i;

	if (!list || !item)
		return -1;

	/*if last entry or first of an empty array... */
	if (position >= list->entry_count)
		return mp4_list_add(list, item);

	list->slots =
		(void **) realloc(list->slots,
						  (list->entry_count + 1) * sizeof(void *));
	i = list->entry_count - position;

	memmove(&list->slots[position + 1], &list->slots[position],
			sizeof(void *) * i);
	list->entry_count++;
	list->slots[position] = item;
	return 0;
}

void mp4_list_reset(struct mp4_list *list)
{
	if (list) {
		list->entry_count = 0;
		free(list->slots);
		list->slots = NULL;
	}
}

void *mp4_list_enum(struct mp4_list *list, uint32_t * pos)
{
	void *res = mp4_list_get(list, *pos);

	(*pos)++;

	return res;
}
