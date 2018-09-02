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
#include "mp4_debug.h"

/* add the child box to parent */
static int stbl_add_child_box(struct mp4_box *p, struct mp4_box *c)
{
	struct mp4_stbl_box *par = (struct mp4_stbl_box *) p;

	switch (c->type) {
	case MP4_BOX_TYPE_stsd:
		if (par->stsd)
			return -1;
		par->stsd = c;
		break;

	case MP4_BOX_TYPE_stts:
		if (par->stts)
			return -1;
		par->stts = c;
		break;

	case MP4_BOX_TYPE_ctts:
		if (par->ctts)
			return -1;
		par->ctts = c;
		break;

	case MP4_BOX_TYPE_stsc:
		if (par->stsc)
			return -1;
		par->stsc = c;
		break;

	case MP4_BOX_TYPE_stsz:
	case MP4_BOX_TYPE_stz2:
		if (par->stsz)
			return -1;
		par->stsz = c;
		break;

	case MP4_BOX_TYPE_stco:
	case MP4_BOX_TYPE_co64:
		if (par->co64)
			return -1;
		par->co64 = c;
		break;

	case MP4_BOX_TYPE_stss:
		if (par->stss)
			return -1;
		par->stss = c;
		break;

	case MP4_BOX_TYPE_stsh:
		if (par->stsh)
			return -1;
		par->stsh = c;
		break;

	case MP4_BOX_TYPE_padb:
		if (par->padb)
			return -1;
		par->padb = c;
		break;

		/* todo */
	default:
		/* unknown box, destroy it */
		c->destroy(c);
		break;
	}

	return 0;
}

static int stbl_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	return mp4_read_child_box_list(box, bs, stbl_add_child_box);
}

static int stbl_box_write(struct mp4_box *box, mp4_bits_t * bs)
{
	return 0;
}

static int stbl_box_calc_size(struct mp4_box *box)
{
	return 0;
}

static int stbl_box_dump(struct mp4_box *box, FILE * fd)
{
	return 0;
}

static int stbl_box_destroy(struct mp4_box *box)
{
	struct mp4_stbl_box *self = (struct mp4_stbl_box *) box;

	if (!self)
		return -1;

	if (self->stsd)
		self->stsd->destroy(self->stsd);

	if (self->stts)
		self->stts->destroy(self->stts);

	if (self->ctts)
		self->ctts->destroy(self->ctts);

	if (self->stsc)
		self->stsc->destroy(self->stsc);

	if (self->stsz)
		self->stsz->destroy(self->stsz);

	if (self->co64)
		self->co64->destroy(self->co64);

	if (self->stss)
		self->stss->destroy(self->stss);

	if (self->stsh)
		self->stsh->destroy(self->stsh);

	if (self->padb)
		self->padb->destroy(self->padb);

	verbose_memleak("destroy %p...\n", self);

	free(self);
	return 0;
}

static uint32_t stbl_get_max_sample_size(struct mp4_box *box)
{
	struct mp4_stbl_box *stbl;
	struct mp4_stsz_box *stsz;
	uint32_t max_sample_size = 0;
	int i;

	stbl = (struct mp4_stbl_box *) box;
	if (!stbl)
		return 0;

	stsz = (struct mp4_stsz_box *) stbl->stsz;
	if (!stsz)
		return 0;

	/* constant sample size */
	if (stsz->sample_size)
		return stsz->sample_size;

	for (i = 0; i < stsz->sample_count; i++) {
		if (stsz->entry_size[i] > max_sample_size)
			max_sample_size = stsz->entry_size[i];
	}

	return max_sample_size;
}

static uint32_t stbl_get_sample_size(struct mp4_box *box, uint32_t sample_id)
{
	struct mp4_stbl_box *stbl;
	struct mp4_stsz_box *stsz;

	stbl = (struct mp4_stbl_box *) box;
	if (!stbl)
		return 0;

	stsz = (struct mp4_stsz_box *) stbl->stsz;
	if (!stsz)
		return 0;

	/* constant sample size */
	if (stsz->sample_size)
		return stsz->sample_size;

	if (sample_id >= stsz->sample_count)
		return 0;

	return stsz->entry_size[sample_id];
}

static int stbl_get_sample_times(struct mp4_box *box, uint32_t sample_id,
								 uint64_t * duration, uint64_t * dts)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stts_box *stts;
	struct mp4_stts_entry *entry;
	uint32_t i, entry_count;
	uint32_t sid = 0;
	uint32_t sample_count, sample_delta;
	uint64_t elapsed = 0;

	stts = (struct mp4_stts_box *) stbl->stts;
	if (!stts) {
		verbose_err("stts is NULL \n");
		return -1;
	}

	entry_count = mp4_list_count(stts->entry_list);
	for (i = 0; i < entry_count; i++) {
		entry = mp4_list_get(stts->entry_list, i);
		sample_count = entry->sample_count;
		sample_delta = entry->sample_delta;

		if (sample_id <= sid + sample_count - 1) {
			if (dts) {
				*dts = sample_id - sid;
				*dts *= sample_delta;
				*dts += elapsed;
			}

			if (duration) {
				*duration = sample_delta;
			}

			return 0;
		}

		sid += sample_count;
		elapsed += sample_count * sample_delta;
	}

	return 0;
}

static int stbl_get_sample_cts_offset(struct mp4_box *box, uint32_t sample_id,
									  uint32_t * cts_offset)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_ctts_box *ctts;
	struct mp4_ctts_entry *entry;
	uint32_t i, entry_count;
	uint32_t sid = 0;
	uint32_t sample_count, sample_offset;

	/* default is 0 */
	if (cts_offset)
		*cts_offset = 0;

	ctts = (struct mp4_ctts_box *) stbl->ctts;
	if (!ctts) {
		return 0;
	}

	entry_count = mp4_list_count(ctts->entry_list);
	verbose_read("entry_count: %d \n", entry_count);

	for (i = 0; i < entry_count; i++) {
		entry = mp4_list_get(ctts->entry_list, i);
		sample_count = entry->sample_count;
		sample_offset = entry->sample_offset;

		if (sample_id <= sid + sample_count - 1) {
			if (cts_offset) {
				*cts_offset = sample_offset;
			}
			break;
		}

		sid += sample_count;
	}

	return 0;
}

static int stbl_get_sample_file_offset(struct mp4_box *box, uint32_t sample_id,
									   uint64_t * offset)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stsc_box *stsc;
	struct mp4_stsz_box *stsz;
	struct mp4_stco_box *stco;
	struct mp4_co64_box *co64;
	uint32_t chunk_id, first_sample_id;
	uint32_t sample_offset_within_chunk;
	uint64_t chunk_offset;

	stsc = (struct mp4_stsc_box *) stbl->stsc;
	if (!stsc) {
		return -1;
	}

	stsz = (struct mp4_stsz_box *) stbl->stsz;
	if (!stsz) {
		return -1;
	}

	if (!stbl->co64)
		return -1;

	/* get sample to chunk info */
	stsc->get_sample_info((struct mp4_box *) stsc, sample_id,
						  &chunk_id, &first_sample_id);
	/* get sample offset within chunk */
	stsz->get_sample_offset_in_chunk((struct mp4_box *) stsz, sample_id,
									 first_sample_id,
									 &sample_offset_within_chunk);
	/* get chunk offset */
	if (stbl->co64->type == MP4_BOX_TYPE_co64) {
		co64 = (struct mp4_co64_box *) stbl->co64;
		chunk_offset = co64->get_chunk_offset(stbl->co64, chunk_id);
	} else {
		stco = (struct mp4_stco_box *) stbl->co64;
		chunk_offset = stco->get_chunk_offset(stbl->co64, chunk_id);
	}

	if (offset)
		*offset = chunk_offset + sample_offset_within_chunk;

	verbose_read_table("sample_id: %d, chunk_id: %d, first_sample_id: %d, "
					   "sample_offset_within_chunk: %d, offset: %lld\n",
					   sample_id, chunk_id, first_sample_id,
					   sample_offset_within_chunk, chunk_offset);
	return 0;
}

static int stbl_get_sample_sync_flag(struct mp4_box *box, uint32_t sample_id,
									 uint8_t * is_sync)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stss_box *stss;
	uint32_t i, entry_count;
	uint32_t sample_number;

	if (is_sync)
		*is_sync = 0;

	stss = (struct mp4_stss_box *) stbl->stss;
	if (!stss) {
		if (is_sync)
			*is_sync = 1;
		return 0;
	}

	entry_count = stss->entry_count;
	for (i = 0; i < entry_count; i++) {
		sample_number = stss->sample_number[i];

		/* sample_id is from 0 */
		if (sample_id + 1 == sample_number) {
			if (is_sync)
				*is_sync = 1;
			break;
		}
	}

	return 0;
}

static int stbl_get_visual_info(struct mp4_box *box, uint32_t * width,
								uint32_t * height)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stsd_box *stsd;
	struct mp4_box *entry;
	struct visual_sample_entry_box *visual;
	char type_fourcc[5];

	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	switch (entry->type) {
	case MP4_BOX_TYPE_mp4v:
	case MP4_BOX_TYPE_avc1:
		visual = (struct visual_sample_entry_box *) entry;
		*width = visual->width;
		*height = visual->height;
		return 0;
	default:
		mp4_fourcc_to_str(entry->type, type_fourcc, 5);
		verbose_read_table("unsupport type: %s \n", type_fourcc);
		return -1;
	}

	return 0;
}

static int stbl_get_audio_info(struct mp4_box *box, uint32_t * sample_rate,
							   uint32_t * channels, uint8_t * bits_per_sample,
							   uint8_t * version)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stsd_box *stsd;
	struct mp4_box *entry;
	struct audio_sample_entry_box *audio;

	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	switch (entry->type) {
	case MP4_BOX_TYPE_mp4a:
		audio = (struct audio_sample_entry_box *) entry;
		if (sample_rate)
			*sample_rate = audio->sampleratehi;
		if (channels)
			*channels = audio->channelcount;
		if (bits_per_sample)
			*bits_per_sample = audio->samplesize;
		if (version)
			*version = audio->version;
		return 0;
	default:
		verbose_read_table("unsupport type: %d \n", entry->type);
		return -1;
	}

	return 0;
}

int stbl_get_h264_seq_pic_hdrs(struct mp4_box *box,
							   uint8_t * nr_of_seq_hdr,
							   char ***seq_hdr,
							   uint16_t ** seq_hdr_size,
							   uint8_t * nr_of_pic_hdr,
							   char ***pic_hdr, uint16_t ** pic_hdr_size)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stsd_box *stsd;
	struct mp4_box *entry;
	struct mp4_avc1_box *avc1;
	struct mp4_avcC_box *avcC;
	struct avc_dec_cfg_record *avc_config;
	int count, i;
	struct avc_parameter_sets *sets;

	verbose_read("enter\n");
	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	if (entry->type == MP4_BOX_TYPE_avc1) {
		avc1 = (struct mp4_avc1_box *) entry;
		avcC = (struct mp4_avcC_box *) avc1->avcC;
		if (!avcC) {
			verbose_read_table("NO avcC box\n");
			return -1;
		}
	} else {
		verbose_read_table("not h264 track\n");
		return -1;
	}

	avc_config = avcC->avc_config;
	if (!avc_config) {
		verbose_err("avc config is NULL \n");
		return -1;
	}

	/* ok, get the seq and pic info now */
	count = mp4_list_count(avc_config->sequence_parameter_sets_nalu);
	*nr_of_seq_hdr = count;

	*seq_hdr = avc_config->pp_seq_hdr;
	*seq_hdr_size = avc_config->p_seq_hdr_size;

	verbose_read("nr_of_seq_hdr: %d \n", count);

	for (i = 0; i < count; i++) {
		sets = mp4_list_get(avc_config->sequence_parameter_sets_nalu, i);

		verbose_read("seq_size: %d \n", sets->size);
		avc_config->pp_seq_hdr[i] = sets->data;
		avc_config->p_seq_hdr_size[i] = sets->size;
	}

	count = mp4_list_count(avc_config->picture_parameter_sets_nalu);
	*nr_of_pic_hdr = count;
	verbose_read("nr_of_pic_hdr: %d \n", count);

	*pic_hdr = avc_config->pp_pic_hdr;
	*pic_hdr_size = avc_config->p_pic_hdr_size;

	for (i = 0; i < count; i++) {
		sets = mp4_list_get(avc_config->picture_parameter_sets_nalu, i);

		verbose_read("pic_size: %d \n", sets->size);
		avc_config->pp_pic_hdr[i] = sets->data;
		avc_config->p_pic_hdr_size[i] = sets->size;
	}

	return 0;
}

int stbl_get_h264_dec_config_info(struct mp4_box *box,
								  uint8_t * nalu_size, uint8_t * cfg_ver,
								  uint8_t * profile_ind, uint8_t * profile_comp,
								  uint8_t * level_ind)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stsd_box *stsd;
	struct mp4_box *entry;
	struct mp4_avc1_box *avc1;
	struct mp4_avcC_box *avcC;
	struct avc_dec_cfg_record *avc_config;

	if (!stbl)
		return -1;

	stsd = (struct mp4_stsd_box *) stbl->stsd;
	if (!stsd)
		return -1;

	/* get the first entry only */
	entry = mp4_list_get(stsd->sample_entry_boxes, 0);
	if (!entry)
		return -1;

	if (entry->type == MP4_BOX_TYPE_avc1) {
		avc1 = (struct mp4_avc1_box *) entry;
		avcC = (struct mp4_avcC_box *) avc1->avcC;
		if (!avcC) {
			verbose_read_table("NO avcC box\n");
			return -1;
		}
	} else {
		verbose_read_table("not h264 track\n");
		return -1;
	}

	avc_config = avcC->avc_config;
	if (!avc_config) {
		verbose_err("avc config is NULL \n");
		return -1;
	}

	/* ok, get all the dec info */
	if (nalu_size)
		*nalu_size = avc_config->nal_unit_size;
	if (cfg_ver)
		*cfg_ver = avc_config->configuration_version;
	if (profile_ind)
		*profile_ind = avc_config->avc_profile_indication;
	if (profile_comp)
		*profile_comp = avc_config->profile_compatibility;
	if (level_ind)
		*level_ind = avc_config->avc_level_indication;

	return 0;
}

int stbl_find_sample_from_time(struct mp4_box *box, uint64_t when,
							   uint32_t * sample_id, uint32_t * prev_sample_id)
{
	struct mp4_stbl_box *stbl = (struct mp4_stbl_box *) box;
	struct mp4_stts_box *stts;
	uint32_t i, j, count;
	struct mp4_stts_entry *ent;
	uint64_t current_dts = 0;
	uint32_t current_sample_id = 0;

	/* can't find the sample */
	*sample_id = 0xFFFFFFFF;
	*prev_sample_id = 0xFFFFFFFF;

	if (!stbl)
		return -1;

	stts = (struct mp4_stts_box *) stbl->stts;
	if (!stts)
		return -1;

	count = mp4_list_count(stts->entry_list);
	for (i = 0; i < count; i++) {
		ent = (struct mp4_stts_entry *) mp4_list_get(stts->entry_list, i);
		for (j = 0; j < ent->sample_count; j++) {
			if (current_dts >= when)
				goto done;
			current_sample_id += 1;
			current_dts += ent->sample_delta;
		}
	}

	verbose_read_table("sample_id: 0x%x\n", *sample_id);
	/* can't find the sample */
	return 0;

  done:

	verbose_read_table("current_dts: %lld, when: %lld, current_sample_id: %d \n",
		   current_dts, when, current_sample_id);
	if (current_dts == when) {
		(*sample_id) = current_sample_id;
	} else {
		if (current_sample_id != 0) {
			(*prev_sample_id) = current_sample_id - 1;
		} else {
			(*prev_sample_id) = 0;
		}
	}

	return 0;
}

struct mp4_box *stbl_box_create()
{
	struct mp4_stbl_box *self;

	self = (struct mp4_stbl_box *) malloc(sizeof(struct mp4_stbl_box));
	if (!self) {
		verbose_err("malloc failed \n");
		return NULL;
	}
	memset(self, 0, sizeof(struct mp4_stbl_box));

	verbose_memleak("create %p...\n", self);

	self->destroy = stbl_box_destroy;
	self->read = stbl_box_read;
	self->write = stbl_box_write;
	self->calc_size = stbl_box_calc_size;
	self->dump = stbl_box_dump;
	self->get_max_sample_size = stbl_get_max_sample_size;
	self->get_sample_size = stbl_get_sample_size;
	self->get_sample_times = stbl_get_sample_times;
	self->get_sample_cts_offset = stbl_get_sample_cts_offset;
	self->get_visual_info = stbl_get_visual_info;
	self->get_audio_info = stbl_get_audio_info;
	self->get_sample_sync_flag = stbl_get_sample_sync_flag;
	self->get_sample_file_offset = stbl_get_sample_file_offset;
	self->get_h264_seq_pic_hdrs = stbl_get_h264_seq_pic_hdrs;
	self->get_h264_dec_config_info = stbl_get_h264_dec_config_info;
	self->find_sample_from_time = stbl_find_sample_from_time;

	return (struct mp4_box *) self;
}
