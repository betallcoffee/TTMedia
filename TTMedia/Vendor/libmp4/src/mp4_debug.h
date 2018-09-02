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
#ifndef __MP4_DEBUG_INCLUDE__
#define __MP4_DEBUG_INCLUDE__
#include <assert.h>
#include "lib_mp4.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

extern uint32_t g_verbosity;

#define VERBOSE(exprverbosity, args...)					\
    do {												\
        if ( exprverbosity & g_verbosity ) {			\
			printf("%s:%s: ", __FILE__, __FUNCTION__);	\
            printf(args);								\
        }												\
    } while(0)

#define verbose_err(expr...)      \
    VERBOSE(MP4_DETAILS_ERROR, expr)

#define verbose_warning(expr...)        \
    VERBOSE(MP4_DETAILS_WARNING, expr)

#define verbose_read(expr...)       \
    VERBOSE(MP4_DETAILS_READ,expr)

#define verbose_read_table(expr...) \
    VERBOSE((MP4_DETAILS_READ | MP4_DETAILS_TABLE), expr)

#define verbose_read_sample(expr...)    \
    VERBOSE((MP4_DETAILS_READ | MP4_DETAILS_SAMPLE), expr)

#define verbose_write(expr...)      \
    VERBOSE(MP4_DETAILS_WRITE, expr)

#define verbose_write_table(expr...)    \
    VERBOSE((MP4_DETAILS_WRITE | MP4_DETAILS_TABLE), expr)

#define verbose_write_sample(expr...)   \
    VERBOSE((MP4_DETAILS_WRITE | MP4_DETAILS_SAMPLE), expr)

#define verbose_memleak(expr...)   \
    VERBOSE((MP4_DETAILS_MEMLEAK), expr)

#endif							/* __MP4_DEBUG_INCLUDE__ */
