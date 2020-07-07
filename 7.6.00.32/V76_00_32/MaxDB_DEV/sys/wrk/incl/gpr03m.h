/*!================================================================
 module:    gpr03m.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Memory Management

 description: Memory Management Verwaltung

 see:

 -------------------------------------------------------------------

                         Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

 ===================================================================*/

#ifndef __GPR03M_H__
#define __GPR03M_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gsp00.h"
#ifdef USE_MALLOC
#ifndef FREEBSD
#include <malloc.h>
#endif
#else
#include "geo572.h"
#endif
/*==================================================================*
 *  MACROS                                                          *
 *==================================================================*/

#if !defined(externC)
# if defined (__cplusplus)
#  define  externC      extern "C"
# else
#  define  externC
# endif
#endif
#ifdef SAPDB_FAST
#define pr03mAllocat(Size, TraceStr) pr03mAllocatF(Size)
#define pr03mCalloc(num, Size, TraceStr) pr03mCallocF(num, Size)
#define pr03mReallocat(Size, Buffer, TraceStr) pr03mReallocatF(Size, Buffer)
#define pr03mFree(Buffer, TraceStr) pr03mFreeF(Buffer)
#else
#define pr03mAllocat(Size, TraceStr) pr03mAllocatT(Size, TraceStr)
#define pr03mCalloc(num, Size, TraceStr) pr03mCallocT(num, Size, TraceStr)
#define pr03mReallocat(Size, Buffer, TraceStr) pr03mReallocatT(Size, Buffer, TraceStr)
#define pr03mFree(Buffer, TraceStr) pr03mFreeT(Buffer, TraceStr)
#endif

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

externC void* pr03mAllocatT(tsp00_Int4 Size, char *szStr);
externC void* pr03mAllocatF(tsp00_Int4 Size);
externC void pr03mAllocatP(tsp00_Int4 lSize, void** Buffer, tsp00_Name VAR_ARRAY_REF szPStr);

/*!
  Function: pr03mCallocT

  description: Allocates an array in memory with elements initialized to 0.

  arguments:
      num          [in]  number of elements
      lSize        [in]  length in bytes of each element
      szStr        [in]  Tracestring fuer Memorycheck.

  return:          Pointer auf resevierten Speicher oder NULL fuer Fehler.
 */
externC void* pr03mCallocT(tsp00_Int4 num, tsp00_Int4 lSize, char *szStr);
externC void* pr03mCallocF(tsp00_Int4 num, tsp00_Int4 lSize);


externC void pr03mFreeT(void* Buffer, char *szStr);
externC void pr03mFreeF(void* Buffer);
externC void pr03mFreeP(void* Buffer, tsp00_Name VAR_ARRAY_REF szPStr);

externC void* pr03mReallocatT(void* Buffer, tsp00_Int4 Size, char *szStr);
externC void* pr03mReallocatF(void* Buffer, tsp00_Int4 Size);
externC void* pr03mReallocatP(void* Buffer, tsp00_Int4 Size, tsp00_Name VAR_ARRAY_REF szPStr);

externC boolean pr03mValidateAlloc(void* Buffer, char *szStr);

#endif
