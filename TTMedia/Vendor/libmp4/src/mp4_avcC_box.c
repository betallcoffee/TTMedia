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
#include <stdio.h>
#include "mp4_boxes.h"
#include "mp4_list.h"
#include "mp4_debug.h"

static struct avc_dec_cfg_record *avc_dec_cfg_alloc()
{
	struct avc_dec_cfg_record *avc_cfg;

	avc_cfg =
		(struct avc_dec_cfg_record *) malloc(sizeof(struct avc_dec_cfg_record));
	if (!avc_cfg)
		return NULL;
	memset(avc_cfg, 0, sizeof(struct avc_dec_cfg_record));

	avc_cfg->sequence_parameter_sets_nalu = mp4_list_create();
	if (!avc_cfg->sequence_parameter_sets_nalu) {
		verbose_err("list create failed \n");
		return NULL;
	}

	avc_cfg->picture_parameter_sets_nalu = mp4_list_create();
	if (!avc_cfg->picture_parameter_sets_nalu) {
		verbose_err("list create failed \n");
		return NULL;
	}

	return avc_cfg;

}

static void avc_dec_cfg_free(struct avc_dec_cfg_record *avc_cfg)
{
	struct avc_parameter_sets *sets;

	if (!avc_cfg)
		return;

	if (avc_cfg->pp_seq_hdr)
		free(avc_cfg->pp_seq_hdr);

	if (avc_cfg->p_seq_hdr_size)
		free(avc_cfg->p_seq_hdr_size);

	if (avc_cfg->pp_pic_hdr)
		free(avc_cfg->pp_pic_hdr);

	if (avc_cfg->p_pic_hdr_size)
		free(avc_cfg->p_pic_hdr_size);

	while (mp4_list_count(avc_cfg->sequence_parameter_sets_nalu)) {
		sets = (struct avc_parameter_sets *)
			mp4_list_get(avc_cfg->sequence_parameter_sets_nalu, 0);

		mp4_list_rem(avc_cfg->sequence_parameter_sets_nalu, 0);
		if (sets->data)
			free(sets->data);
		free(sets);
	}
	mp4_list_destroy(avc_cfg->sequence_parameter_sets_nalu);

	while (mp4_list_count(avc_cfg->picture_parameter_sets_nalu)) {
		sets = (struct avc_parameter_sets *)
			mp4_list_get(avc_cfg->picture_parameter_sets_nalu, 0);

		mp4_list_rem(avc_cfg->picture_parameter_sets_nalu, 0);
		if (sets->data)
			free(sets->data);
		free(sets);
	}

	mp4_list_destroy(avc_cfg->picture_parameter_sets_nalu);

	free(avc_cfg);
}

static int avcC_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_avcC_box *self = (struct mp4_avcC_box *) box;
	struct avc_dec_cfg_record *avc_config;
	int count;
	int i;

	if (self->avc_config)
		avc_dec_cfg_free(self->avc_config);

	self->avc_config = avc_dec_cfg_alloc();
	if (!self->avc_config) {
		verbose_err("avc dec cfg alloc failed \n");
		return -1;
	}
	avc_config = self->avc_config;

	avc_config->configuration_version = mp4_bs_read_u8(bs);
	avc_config->avc_profile_indication = mp4_bs_read_u8(bs);
	avc_config->profile_compatibility = mp4_bs_read_u8(bs);
	avc_config->avc_level_indication = mp4_bs_read_u8(bs);
	mp4_bs_read_int(bs, 6);		/* reserved bit(6) */
	avc_config->nal_unit_size = 1 + mp4_bs_read_int(bs, 2);
	mp4_bs_read_int(bs, 3);		/* reserved bit(3) */

	verbose_read("version: %d, profile_indication: %d, "
				 "profile_compatibility: %d, "
				 "avc_level_indication: %d, nal_unit_size: %d \n",
				 avc_config->configuration_version,
				 avc_config->avc_profile_indication,
				 avc_config->profile_compatibility,
				 avc_config->avc_level_indication, avc_config->nal_unit_size);

	count = mp4_bs_read_int(bs, 5);
	verbose_read("nr_of_seq_hdr: %d \n", count);

	/* for get seq inf use */
	avc_config->pp_seq_hdr = (char **) malloc(count * sizeof(char *));
	avc_config->p_seq_hdr_size =
		(uint16_t *) malloc(count * sizeof(uint16_t *));

	for (i = 0; i < count; i++) {
		struct avc_parameter_sets *sets;

		sets = (struct avc_parameter_sets *)
			malloc(sizeof(struct avc_parameter_sets));
		if (!sets) {
			verbose_err("malloc failed \n");
			return -1;
		}
		memset(sets, 0, sizeof(struct avc_parameter_sets));

		sets->size = mp4_bs_read_int(bs, 16);
		sets->data = (char *) malloc(sizeof(char) * sets->size);
		if (!sets->data) {
			verbose_err("malloc failed \n");
			return -1;
		}
		mp4_bs_read_data(bs, sets->data, sets->size);
		mp4_list_add(avc_config->sequence_parameter_sets_nalu, sets);
	}

	count = mp4_bs_read_int(bs, 8);
	verbose_read("nr_of_pic_hdr: %d \n", count);

	avc_config->pp_pic_hdr = (char **) malloc(count * sizeof(char *));
	avc_config->p_pic_hdr_size =
		(uint16_t *) malloc(count * sizeof(uint16_t *));

	for (i = 0; i < count; i++) {
		struct avc_parameter_sets *sets;

		sets = (struct avc_parameter_sets *)
			malloc(sizeof(struct avc_parameter_sets));
		if (!sets) {
			verbose_err("malloc failed \n");
			return -1;
		}
		sets->size = mp4_bs_read_int(bs, 16);
		sets->data = (char *) malloc(sizeof(char) * sets->size);
		if (!sets->data) {
			verbose_err("malloc failed \n");
			return -1;
		}
		mp4_bs_read_data(bs, sets->data, sets->size);
		mp4_list_add(avc_config->picture_parameter_sets_nalu, sets);
	}

	return 0;
}

static int avcC_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int avcC_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int avcC_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int avcC_box_destroy(struct mp4_box *box)
{
	struct mp4_avcC_box *self = (struct mp4_avcC_box *) box;

	if (!self)
		return -1;

	if (self->avc_config)
		avc_dec_cfg_free(self->avc_config);

	verbose_memleak("destroy %p...\n", self);

	free(self);

	return 0;
}

struct mp4_box *avcC_box_create()
{
	struct mp4_avcC_box *self;

	self = (struct mp4_avcC_box *) malloc(sizeof(struct mp4_avcC_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_avcC_box));

	verbose_memleak("create %p...\n", self);

	self->destroy = avcC_box_destroy;
	self->read = avcC_box_read;
	self->write = avcC_box_write;
	self->calc_size = avcC_box_calc_size;
	self->dump = avcC_box_dump;

	return (struct mp4_box *) self;
}
