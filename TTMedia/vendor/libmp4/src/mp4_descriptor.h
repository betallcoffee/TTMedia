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
#ifndef __MP4_DESCRIPTOR_INCLUDE__
#define __MP4_DESCRIPTOR_INCLUDE__
#include "lib_mp4.h"
#include "mp4_bits.h"
#include "mp4_list.h"

/* ISO/IEC 14496-1:2002-w4848(E) 8.2.1 */
enum {
	ForbiddenZeroTag = 0x0,
	ObjectDescrTag = 0x01,
	InitialObjectDescrTag = 0x02,
	ES_DescrTag = 0x03,
	DecoderConfigDescrTag = 0x04,
	DecSpecificInfoTag = 0x05,
	SLConfigDescrTag = 0x06,
	ContentIdentDescrTag = 0x07,
	SupplContentIdentDescrTag = 0x08,
	IPI_DescrPointerTag = 0x09,
	IPMP_DescrPointerTag = 0x0A,
	IPMP_DescrTag = 0x0B,
	QoS_DescrTag = 0x0C,
	RegistrationDescrTag = 0x0D,
	ES_ID_IncDescrTag = 0x0E,
	ES_ID_RefDescrTag = 0x0F,
	IOD_Tag = 0x10,
	OD_Tag = 0x11,
	IPI_DescrPointerRefTag = 0x12,

	ExtendedProfileLevelDescrTag = 0x13,
	profileLevelIndicationIndexDescrTag = 0x14,

	/* ISO reserved */
	ISO_DESC_RANGE_BEGIN = 0x15,
	ISO_DESC_RANGE_END = 0x3F,

	ContentClassificationDescrTag = 0x40,
	KeyWordDescrTag = 0x41,
	RatingDescrTag = 0x42,
	LanguageDescrTag = 0x43,
	ShortTextualDescrTag = 0x44,
	ExpandedTextualDescrTag = 0x45,
	ContentCreatorNameDescrTag = 0x46,
	ContentCreationDateDescrTag = 0x47,
	OCICreatorNameDescrTag = 0x48,
	OCICreationDateDescrTag = 0x49,
	SmpteCameraPositionDescr = 0x4A,

	/* oci reserved */
	ISO_OCI_EXT_RANGE_BEGIN = 0x4B,
	ISO_OCI_EXT_RANGE_END = 0x5F,

	/* iso reserved */
	ISO_RESERVED_RANGE_BEGIN = 0x60,
	ISO_RESERVED_RANGE_END = 0xBF,

	/* user private reserved */
	USER_PRIVATE_RANGE_BEGI = 0xC0,
	USER_PRIVATE_RANGE_END = 0xFE,

	ForbiddenFFTag = 0xFF
};

#define MP4_BASE_DESCRIPTOR  \
	uint32_t tag; \
	uint32_t size; \
	int (*read)(struct mp4_descriptor *self, mp4_bits_t *bs); \
	int (*write)(struct mp4_descriptor *self, mp4_bits_t *bs); \
	int (*calc_size)(struct mp4_descriptor *self ); \
	int (*dump)(struct mp4_descriptor *self, FILE* fd ); \
	int (*destroy)(struct mp4_descriptor *self )

struct mp4_descriptor {
	MP4_BASE_DESCRIPTOR;
};

struct mp4_default_descriptor {
	MP4_BASE_DESCRIPTOR;
	uint32_t data_size;
	char *data;
};

/* ISO/IEC 14496-1:2002-w4848(E) 8.6.5 */
struct mp4_es_descriptor {
	MP4_BASE_DESCRIPTOR;
	uint16_t es_id;
	uint8_t stream_dependence_flag;	/* 1 bits */
	uint8_t url_flag;			/* 1 bits */
	uint8_t ocr_stream_flag;	/* 1 bits */
	uint8_t stream_priority;	/* 5 bits */

	uint16_t depends_on_es_id;
	uint8_t url_len;
	char *url_string;

	uint16_t ocr_es_id;

	/* TODO: support all descrs in the definition */
	struct mp4_descriptor *dec_config_descr;
	struct mp4_descriptor *sl_config_descr;

	struct mp4_descriptor *lang_descr;
};

/* ISO/IEC 14496-1:2002-w4848(E) 8.6.6 */
struct mp4_dec_config_descriptor {
	MP4_BASE_DESCRIPTOR;

	uint8_t object_type_indication;
	uint8_t stream_type;		/* 6 bits */
	uint8_t up_stream;			/* 1 bits */
	uint8_t reserved;			/* 1 bits */
	uint32_t buffer_size_db;	/* 24 bits */
	uint32_t maxbitrate;
	uint32_t avgbitrate;

	struct mp4_descriptor *dec_specific_info;
	mp4_list_t *profile_level_indication_index_descr;
};

int mp4_parse_descriptor(struct mp4_descriptor **descr, mp4_bits_t * bs);

void mp4_descr_list_destroy(mp4_list_t * box);
int mp4_read_child_descr_list(struct mp4_descriptor *par, mp4_bits_t * bs,
							  int (*add_child) (struct mp4_descriptor * par,
												struct mp4_descriptor * ch));

struct mp4_descriptor *mp4_descriptor_create(uint8_t tag);

struct mp4_descriptor *mp4_default_descr_create();
struct mp4_descriptor *mp4_es_descr_create();
struct mp4_descriptor *mp4_dec_config_descr_create();

#endif							/* __MP4_DESCRIPTOR_INCLUDE__ */
