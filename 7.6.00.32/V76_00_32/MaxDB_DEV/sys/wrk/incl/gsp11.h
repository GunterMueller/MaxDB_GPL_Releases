/****************************************************************************

  module      : gsp11.h

  -------------------------------------------------------------------------

  responsible : FrankH

  special area: xparam
  description : description ...


  last changed: 1999-04-19  11:01
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




*****************************************************************************/


#ifndef GSP11_H
#define GSP11_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define XP_FIRST_VERSION_SP11      1
#define XP_MAX_VERSION_SP11        10
#define XP_ACT_VERSION_SP11         0
#define XP_REMOVE_ALL_VERSION_SP11 -1

#define XP_CLOSE_NO_FLUSH_SP11   0

#define XP_OPEN_READONLY_SP11    0x01
#define XP_OPEN_READ_WRITE_SP11  0x02
#define XP_OPEN_EXISTING_SP11    0x04
#define XP_OPEN_CREATE_SP11      0x08
#define XP_OPEN_DIRECT_SP11      0x10
#define XP_OPEN_BUFFERED_SP11    0x20
#define XP_OPEN_BIN_SP11         0x40

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef enum tsp11_XpKey_Enum {
    xp_int2_type_sp11,
    xp_int4_type_sp11,
    xp_c8_type_sp11,
    xp_c18_type_sp11,
    xp_c24_type_sp11,
    xp_c40_type_sp11,
    xp_c64_type_sp11,
    xp_crypt_type_sp11
} tsp11_XpKey_Enum;

typedef ENUM_TYPE_MSP00 ( tsp11_XpKey, tsp11_XpKey_Enum, xp_crypt_type_sp11, tsp00_Uint1);

typedef enum tsp11_XpReturnCode
{
    xp_ok_sp11,
    xp_key_not_found_sp11,
    xp_eof_sp11,
    xp_cannot_change_online_sp11,
    xp_no_change_permission_sp11,
    xp_type_mismatch_sp11,
    xp_not_ok_sp11
} tsp11_XpReturnCode;

typedef tsp00_XpKeyType            tsp11_XpKeyType;
typedef tsp00_XpKeyTypec           tsp11_XpKeyTypec;

/*
typedef ENUM_TYPE_MEO00 ( tsp11_XpReturnCode, tsp11_XpReturnCode_Enum, xp_not_ok_sp11, tsp00_Uint1);
*/

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GSP11_H */
