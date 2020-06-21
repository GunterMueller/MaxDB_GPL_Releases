/*!===========================================================================

  module:       StudioTD_Global.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Global definitions for the table defintion class.

  created:      2000-05-11

  updated:      2000-07-07

  ------------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/

#ifndef STUDIOTD_GLOBAL_H
#define STUDIOTD_GLOBAL_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
const char SEP_LEVEL_1[] = { 1, '\0' };
const char SEP_LEVEL_2[] = { 2, '\0' };
const char SEP_LEVEL_3[] = { 3, '\0' };
const char SEP_LEVEL_4[] = { 4, '\0' };
const char SEP_LEVEL_5[] = { 5, '\0' };


#if defined(_WIN32)
#define StudioTD_NEW_LINE "\r\n"
const short StudioTD_NEW_LINE_LEN = 2;
#else
#define StudioTD_NEW_LINE "\n"
const short StudioTD_NEW_LINE_LEN = 1;
#endif

#define SEP          StudioTD_NEW_LINE"//"StudioTD_NEW_LINE

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define CASE(s)                  ((casesensitive) ? s : UpperCase(s))
#define SET_ATTRIBUTE(att,value) if (att != value)           \
                                   setAlterType (AlterChange); \
                                 att = value;
#define SET_DIRTYFLAG(att,value) if (att != value)           \
                                   mDirty = true;

/*!===========================================================================
  declaration: StudioTD_Mode
 
  description: 
    Mode type for distinguishing different situation in which the table
    definition class can be used.

  possible values:
    CreateNew - Create a new table from scratch
    CreateFromTable - Use an existing table as template
    CreateFromQuery - Use an existing query as template
    AlterTable - Change an existing table definition
    OpenDraft - Open a stored draft that was previously stored in the database
============================================================================*/
typedef enum _studiotd_mode
{
  CreateNew,
  CreateFromTable,
  CreateFromQuery,
  AlterTable,
  OpenDraft
  
} StudioTD_Mode;


/*!===========================================================================
  declaration: StudioTD_AlterType
 
  description: 
    Type of modification of a table definition.

  possible values:
    AlterNoChange - Table definition was not changed yet
    AlterAdd - Some definitions have been added
    AlterRemove - Some definitions have been removed
    AlterChange - Some definitions have been changed
============================================================================*/
typedef enum _studiotd_altertype
{
  AlterNoChange,
  AlterAdd,
  AlterRemove,
  AlterChange
  
} StudioTD_AlterType;


/*!===========================================================================
  declaration: StudioTD_String
 
  description: 
    Type of a string in a table definition.

============================================================================*/
typedef Tools_DynamicUTF8String   StudioTD_String;

#endif   /* STUDIOTD_GLOBAL_H */
