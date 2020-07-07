/*!
  @file           Data_RecordAccessHeader.h
  @author         ThomasA
  @ingroup        File
  @brief          general methods to access members of a record header

\if EMIT_LICENCE

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



\endif
*/


#ifndef DATA_RECORDHEADERACCESS_H
#define DATA_RECORDHEADERACCESS_H

#if defined (__cplusplus)
#define EXTERN_PASCAL externPascal
#else
#define EXTERN_PASCAL
#endif


#define DEBUG 1
#if defined(DEBUG)
EXTERN_PASCAL int Data_GetRecLen(void* pRec);
EXTERN_PASCAL int Data_GetKeyLen(void* pRec);
EXTERN_PASCAL int Data_GetVarcolOffset(void* pRec);
EXTERN_PASCAL int Data_GetVarcolCount(void* pRec);
#else
/*!
   @brief returns the length of a record lying in the data cache. Since the persistent record
          header may be substituded by a swizzeled pointer, this may require an access to the
          transient representation of the header.
*/

#define IS_SWIZZLED_POINTER(R) (((unsigned char*) R)[1] & 0x80)

#define Data_GetRecLen(R) ((IS_SWIZZLED_POINTER(R)) ? Data_DeRefRecLen((tgg00_Rec*) R) : ((tgg00_Rec*) R)->integer.C_2.recLen_gg00)
/*!
   @brief returns the key length of a record lying in the data cache. Since the persistent record
          header may be substituded by a swizzeled pointer, this may require an access to the
          transient representation of the header.
*/

#define Data_GetKeyLen(R) ((IS_SWIZZLED_POINTER(R)) ? Data_DeRefKeyLen((tgg00_Rec*) R) : ((tgg00_Rec*) R)->integer.C_2.recKeyLen_gg00) 

/*!
   @brief returns the offset of columns of variable length of a record lying in the data cache. 
          Since the persistent record header may be substituded by a swizzeled pointer, this may require an access to the
          transient representation of the header.
*/

#define Data_GetVarcolOffset(R) ((tgg00_Rec*) R)->integer.C_2.recVarcolOffset_gg00()

/*!
   @brief returns the number of columns of variable length of a record lying in the data cache. 
          Since the persistent record header may be substituded by a swizzeled pointer, this may require an access to the
          transient representation of the header.
*/

#define Data_GetVarcolCount(R) ((tgg00_Rec*) R)->integer.C_2.recVarcolCnt_gg00()

#endif

EXTERN_PASCAL int Data_DeRefKeyLen(tgg00_Rec* pRec);

EXTERN_PASCAL int Data_DeRefRecLen(tgg00_Rec* pRec);

#endif