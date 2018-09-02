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
#include "mp4_boxes.h"
#include "mp4_debug.h"

struct mp4_box *mp4_box_create(uint32_t type)
{
	struct mp4_box *box;

	switch (type) {
	case MP4_BOX_TYPE_free:
	case MP4_BOX_TYPE_skip:
		box = free_box_create();
		break;
	case MP4_BOX_TYPE_ftyp:
		box = ftyp_box_create();
		break;
	case MP4_BOX_TYPE_mdat:
		box = mdat_box_create();
		break;
	case MP4_BOX_TYPE_moov:
		box = moov_box_create();
		break;
	case MP4_BOX_TYPE_mvhd:
		box = mvhd_box_create();
		break;
	case MP4_BOX_TYPE_trak:
		box = trak_box_create();
		break;
	case MP4_BOX_TYPE_tkhd:
		box = tkhd_box_create();
		break;
	case MP4_BOX_TYPE_tref:
		box = tref_box_create();
		break;
	case MP4_BOX_TYPE_edts:
		box = edts_box_create();
		break;
	case MP4_BOX_TYPE_elst:
		box = elst_box_create();
		break;
	case MP4_BOX_TYPE_mdia:
		box = mdia_box_create();
		break;
	case MP4_BOX_TYPE_mdhd:
		box = mdhd_box_create();
		break;
	case MP4_BOX_TYPE_hdlr:
		box = hdlr_box_create();
		break;
	case MP4_BOX_TYPE_minf:
		box = minf_box_create();
		break;
	case MP4_BOX_TYPE_vmhd:
		box = vmhd_box_create();
		break;
	case MP4_BOX_TYPE_smhd:
		box = smhd_box_create();
		break;
	case MP4_BOX_TYPE_hmhd:
		box = hmhd_box_create();
		break;
	case MP4_BOX_TYPE_nmhd:
		box = nmhd_box_create();
		break;
	case MP4_BOX_TYPE_dinf:
		box = dinf_box_create();
		break;
	case MP4_BOX_TYPE_dref:
		box = dref_box_create();
		break;
	case MP4_BOX_TYPE_stbl:
		box = stbl_box_create();
		break;
	case MP4_BOX_TYPE_stsd:
		box = stsd_box_create();
		break;
	case MP4_BOX_TYPE_stts:
		box = stts_box_create();
		break;
	case MP4_BOX_TYPE_ctts:
		box = ctts_box_create();
		break;
	case MP4_BOX_TYPE_stsc:
		box = stsc_box_create();
		break;
	case MP4_BOX_TYPE_stsz:
		box = stsz_box_create();
		break;
	case MP4_BOX_TYPE_stz2:
		box = stz2_box_create();
		break;
	case MP4_BOX_TYPE_stco:
		box = stco_box_create();
		break;
	case MP4_BOX_TYPE_co64:
		box = co64_box_create();
		break;
	case MP4_BOX_TYPE_stss:
		box = stss_box_create();
		break;
	case MP4_BOX_TYPE_stsh:
		box = stsh_box_create();
		break;
	case MP4_BOX_TYPE_padb:
		box = padb_box_create();
		break;
	case MP4_BOX_TYPE_udta:
		box = udta_box_create();
		break;
	case MP4_BOX_TYPE_url:
		box = url_box_create();
		break;
	case MP4_BOX_TYPE_urn:
		box = urn_box_create();
		break;
	case MP4_BOX_TYPE_mp4v:
		box = mp4v_box_create();
		break;
	case MP4_BOX_TYPE_mp4a:
		box = mp4a_box_create();
		break;
	case MP4_BOX_TYPE_mp4s:
		box = mp4s_box_create();
		break;
	case MP4_BOX_TYPE_esds:
		box = esds_box_create();
		break;
	case MP4_BOX_TYPE_rtp:
		box = rtp_box_create();
		break;
	case MP4_BOX_TYPE_tims:
		box = tims_box_create();
		break;
	case MP4_BOX_TYPE_tsro:
		box = tsro_box_create();
		break;
	case MP4_BOX_TYPE_snro:
		box = snro_box_create();
		break;
	case MP4_BOX_TYPE_avc1:
		box = avc1_box_create();
		break;
	case MP4_BOX_TYPE_avcC:
		box = avcC_box_create();
		break;
	case MP4_BOX_TYPE_btrt:
		box = btrt_box_create();
		break;
	case MP4_BOX_TYPE_m4ds:
		box = m4ds_box_create();
		break;
	default:
		box = unknown_box_create();
		break;
	}

	if (box)
		box->type = type;

	return box;
}

int mp4_fourcc_to_str(uint32_t type, char *buf, int buf_size)
{
	uint32_t i;
	int ch;

	if (buf == NULL || buf_size < 5)
		return -1;

	for (i = 0; i < 4; i++, buf++) {
		ch = type >> (8 * (3 - i)) & 0xff;
		if (isprint(ch))
			*buf = ch;
		else
			*buf = '.';
	}

	*buf = 0;

	return 0;
}

int mp4_parse_box(struct mp4_box **box, mp4_bits_t * bs)
{
	uint32_t type, hdr_size;
	uint64_t size, start, end;
	char type_fourcc[5];
	char uuid[16];
	int ret = 0;
	struct mp4_box *new_box;

	if ((bs == NULL) || (box == NULL))
		return -1;

	*box = NULL;
	start = mp4_bs_get_position(bs);

	size = (uint64_t) mp4_bs_read_u32(bs);
	type = mp4_bs_read_u32(bs);
	hdr_size = 8;

	mp4_fourcc_to_str(type, type_fourcc, sizeof(type_fourcc));
	verbose_read("size: %llu, type: %s \n", size, type_fourcc);

	/* extend large size */
	if (size == 1) {
		size = mp4_bs_read_u64(bs);
		hdr_size += 8;
		verbose_read("size 64: %llu\n", size);
	} else if (size == 0) {
		/* box extend to the end of file */
		size = mp4_bs_available(bs) + 8;
	}

	/* extended type */
	if (type == MP4_BOX_TYPE_uuid) {
		memset(uuid, 0, 16);
		mp4_bs_read_data(bs, uuid, 16);
		hdr_size += 16;
	}

	/* check size valid */
	if (size < hdr_size) {
		verbose_err("box size(%lld) < box header size(%d) \n", size, hdr_size);
		return -1;
	}

	/* create the box based on the type */
	new_box = mp4_box_create(type);
	if (!new_box) {
		verbose_err("box create failed \n");
		return -1;
	}

	if (!new_box->type)
		new_box->type = type;

	end = mp4_bs_available(bs);
	if (size - hdr_size > end) {
		new_box->size = size - hdr_size - end;
		*box = new_box;
		return -1;
	}

	if (type == MP4_BOX_TYPE_uuid) {
		memcpy(new_box->uuid, uuid, 16);
	}

	/* box data size */
	new_box->size = size - hdr_size;
	ret = new_box->read(new_box, bs);
	if (ret) {
		verbose_err("read failed \n");
		new_box->destroy(new_box);
		*box = NULL;
		return ret;
	}

	/* restore the whole box size */
	new_box->size = size;
	end = mp4_bs_get_position(bs);

	*box = new_box;

	return ret;
}

int mp4_parse_root_box(struct mp4_box **box, mp4_bits_t * bs)
{
	int ret;
	uint64_t start;

	/*at least we can get the box size and type */
	if (mp4_bs_available(bs) < 8) {
		verbose_err("invalid mp4 file \n");
		return -1;
	}

	start = mp4_bs_get_position(bs);
	ret = mp4_parse_box(box, bs);
	if (ret) {
		verbose_err("mp4_parse_box failed \n");
		mp4_bs_seek(bs, start);
		if (*box)
			(*box)->destroy(*box);
		*box = NULL;
		return -1;
	}

	return ret;
}

void mp4_box_list_destroy(mp4_list_t * box)
{
	uint32_t count, i;
	struct mp4_box *a;

	if (!box)
		return;

	count = mp4_list_count(box);
	for (i = 0; i < count; i++) {
		a = (struct mp4_box *) mp4_list_get(box, i);

		if (a)
			a->destroy(a);
	}

	mp4_list_destroy(box);
}

int mp4_read_child_box_list(struct mp4_box *par, mp4_bits_t * bs,
							int (*add_child) (struct mp4_box * par,
											  struct mp4_box * ch))
{
	int ret;
	struct mp4_box *box;

	while (par->size) {
		ret = mp4_parse_box(&box, bs);
		if (ret) {
			if (box)
				box->destroy(box);
			return ret;
		}

		if (par->size < box->size) {
			if (box)
				box->destroy(box);
			return -1;
		}

		par->size -= box->size;
		ret = add_child(par, box);
		if (ret) {
			box->destroy(box);
			return ret;
		}
	}

	return 0;
}

int mp4_full_box_read(struct mp4_box *box, mp4_bits_t * bs)
{
	struct mp4_full_box *self = (struct mp4_full_box *) box;

	if (self->size < 4)
		return -1;

	self->version = mp4_bs_read_u8(bs);
	self->flags = mp4_bs_read_u24(bs);
	self->size -= 4;

	return 0;
}
