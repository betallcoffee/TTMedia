//
//  TTMP4Private.h
//  TTMediaExample
//
//  Created by liang on 25/7/2021.
//  Copyright © 2021 tina. All rights reserved.
//

#ifndef TTMP4Private_h
#define TTMP4Private_h

#include "TTMP4.h"

enum {
    /* 14496-12 */
    MP4_BOX_TYPE_co64 = MP4_FOURCC('c', 'o', '6', '4'),
    MP4_BOX_TYPE_cprt = MP4_FOURCC('c', 'p', 'r', 't'),
    MP4_BOX_TYPE_ctts = MP4_FOURCC('c', 't', 't', 's'),

    MP4_BOX_TYPE_dinf = MP4_FOURCC('d', 'i', 'n', 'f'),
    MP4_BOX_TYPE_dref = MP4_FOURCC('d', 'r', 'e', 'f'),

    MP4_BOX_TYPE_edts = MP4_FOURCC('e', 'd', 't', 's'),
    MP4_BOX_TYPE_elst = MP4_FOURCC('e', 'l', 's', 't'),

    MP4_BOX_TYPE_free = MP4_FOURCC('f', 'r', 'e', 'e'),
    MP4_BOX_TYPE_ftyp = MP4_FOURCC('f', 't', 'y', 'p'),

    MP4_BOX_TYPE_hdlr = MP4_FOURCC('h', 'd', 'l', 'r'),
    MP4_BOX_TYPE_hmhd = MP4_FOURCC('h', 'm', 'h', 'd'),

    MP4_BOX_TYPE_mdat = MP4_FOURCC('m', 'd', 'a', 't'),
    MP4_BOX_TYPE_mdhd = MP4_FOURCC('m', 'd', 'h', 'd'),
    MP4_BOX_TYPE_mdia = MP4_FOURCC('m', 'd', 'i', 'a'),
    MP4_BOX_TYPE_mehd = MP4_FOURCC('m', 'e', 'h', 'd'),
    MP4_BOX_TYPE_meta = MP4_FOURCC('m', 'e', 't', 'a'),
    MP4_BOX_TYPE_minf = MP4_FOURCC('m', 'i', 'n', 'f'),
    MP4_BOX_TYPE_moov = MP4_FOURCC('m', 'o', 'o', 'v'),
    MP4_BOX_TYPE_mvex = MP4_FOURCC('m', 'v', 'e', 'x'),
    MP4_BOX_TYPE_mvhd = MP4_FOURCC('m', 'v', 'h', 'd'),

    MP4_BOX_TYPE_nmhd = MP4_FOURCC('n', 'm', 'h', 'd'),

    MP4_BOX_TYPE_padb = MP4_FOURCC('p', 'a', 'd', 'b'),

    MP4_BOX_TYPE_rtp = MP4_FOURCC('r', 't', 'p', ' '),

    MP4_BOX_TYPE_sbpg = MP4_FOURCC('s', 'b', 'p', 'g'),
    MP4_BOX_TYPE_sdtp = MP4_FOURCC('s', 'd', 't', 'p'),
    MP4_BOX_TYPE_sgpd = MP4_FOURCC('s', 'g', 'p', 'd'),
    MP4_BOX_TYPE_smhd = MP4_FOURCC('s', 'm', 'h', 'd'),
    MP4_BOX_TYPE_skip = MP4_FOURCC('s', 'k', 'i', 'p'),
    MP4_BOX_TYPE_stbl = MP4_FOURCC('s', 't', 'b', 'l'),
    MP4_BOX_TYPE_stco = MP4_FOURCC('s', 't', 'c', 'o'),
    MP4_BOX_TYPE_stdp = MP4_FOURCC('s', 't', 'd', 'p'),
    MP4_BOX_TYPE_stsc = MP4_FOURCC('s', 't', 's', 'c'),
    MP4_BOX_TYPE_stsd = MP4_FOURCC('s', 't', 's', 'd'),
    MP4_BOX_TYPE_stsh = MP4_FOURCC('s', 't', 's', 'h'),
    MP4_BOX_TYPE_stss = MP4_FOURCC('s', 't', 's', 's'),
    MP4_BOX_TYPE_stsz = MP4_FOURCC('s', 't', 's', 'z'),
    MP4_BOX_TYPE_stz2 = MP4_FOURCC('s', 't', 'z', '2'),
    MP4_BOX_TYPE_stts = MP4_FOURCC('s', 't', 't', 's'),
    MP4_BOX_TYPE_snro = MP4_FOURCC('s', 'n', 'r', 'o'),
    MP4_BOX_TYPE_subs = MP4_FOURCC('s', 'u', 'b', 's'),

    MP4_BOX_TYPE_tims = MP4_FOURCC('t', 'i', 'm', 's'),
    MP4_BOX_TYPE_tkhd = MP4_FOURCC('t', 'k', 'h', 'd'),
    MP4_BOX_TYPE_trex = MP4_FOURCC('t', 'r', 'e', 'x'),
    MP4_BOX_TYPE_tref = MP4_FOURCC('t', 'r', 'e', 'f'),
    MP4_BOX_TYPE_trak = MP4_FOURCC('t', 'r', 'a', 'k'),
    MP4_BOX_TYPE_tsro = MP4_FOURCC('t', 's', 'r', 'o'),

    MP4_BOX_TYPE_udta = MP4_FOURCC('u', 'd', 't', 'a'),
    MP4_BOX_TYPE_url = MP4_FOURCC('u', 'r', 'l', ' '),
    MP4_BOX_TYPE_urn = MP4_FOURCC('u', 'r', 'n', ' '),
    MP4_BOX_TYPE_uuid = MP4_FOURCC('u', 'u', 'i', 'd'),

    MP4_BOX_TYPE_vmhd = MP4_FOURCC('v', 'm', 'h', 'd'),

    /* 14496-14 */
    MP4_BOX_TYPE_esds = MP4_FOURCC('e', 's', 'd', 's'),
    MP4_BOX_TYPE_iods = MP4_FOURCC('i', 'o', 'd', 's'),

    MP4_BOX_TYPE_mp4a = MP4_FOURCC('m', 'p', '4', 'a'),
    MP4_BOX_TYPE_mp4s = MP4_FOURCC('m', 'p', '4', 's'),
    MP4_BOX_TYPE_mp4v = MP4_FOURCC('m', 'p', '4', 'v'),

    /* 14496-15 */
    MP4_BOX_TYPE_avc1 = MP4_FOURCC('a', 'v', 'c', '1'),
    MP4_BOX_TYPE_avcC = MP4_FOURCC('a', 'v', 'c', 'C'),
    MP4_BOX_TYPE_avcp = MP4_FOURCC('a', 'v', 'c', 'p'),
    MP4_BOX_TYPE_btrt = MP4_FOURCC('b', 't', 'r', 't'),
    MP4_BOX_TYPE_m4ds = MP4_FOURCC('m', '4', 'd', 's'),

    /* todo */
    MP4_BOX_TYPE_todo = MP4_FOURCC('t', 'o', 'd', 'o')
};


#endif /* TTMP4Private_h */
