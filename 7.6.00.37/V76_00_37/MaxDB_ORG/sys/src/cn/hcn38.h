/*******************************************************************************
  module:       hcn38.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  basic definitions and type for vcn38.cpp and vcn38?.cpp
  last change:  2000-11-29 11:25
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



*******************************************************************************/

#if !defined HCN38_H
#define HCN38_H

#include "hni00b.h"
#include "hni32.h"
#include "hni33.h"
#include "hni34.h"

#if defined (WIN32)
#elif defined (UNIX)
    #include<unistd.h>
#else
    #error hcn38.h only coded for WIN32 and UNIX (define one of them)
#endif

static const char * FunctionStringDelete_cn38="delete";
static const char * FunctionStringRestore_cn38="restore";
static const char * FunctionStringInquire_cn38="inquire";
static const char * FunctionStringBackup_cn38="backup";

#define MAX_STAGING_AREAS_CN38 256
#define MAX_BACKINT_PATH_CHARS_CN38 256

#endif
