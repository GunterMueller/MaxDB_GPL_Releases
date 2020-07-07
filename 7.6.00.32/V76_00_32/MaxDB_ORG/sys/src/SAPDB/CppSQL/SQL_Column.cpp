/*!
 * \file    SQL_Column.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Column
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "geo00.h"

#ifndef SQL_COLUMN_HPP
#include "CppSQL/SQL_Column.hpp"
#endif

#ifndef SQL_COLUMNDESC_HPP
#include "CppSQL/SQL_ColumnDesc.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif

#if defined (EXT_CLIENT_SQL)
#include "ggg00.h"
#include "ggg01.h" // error constants
#include "Oms/OMS_DbpBase.hpp"
#else
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#endif


/************************************************************************/
/* Implementation of class Sqlcol                                       */
/************************************************************************/

SqlCol::SqlCol() {
  sqlColInit();
}

/*----------------------------------------------------------------------*/

SqlCol::~SqlCol() {
  delete m_desc;
  m_desc = NULL;
}

/*---------------- PTS 1111690 -----------------------------------------*/

SqlCol::SqlCol(const SqlCol & right) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi = right.m_desc->vfi;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(int & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const int & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(short & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const short & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

&if $OS = UNIX and !defined BIT64 or $OS = WIN32
/*----------------------------------------------------------------------*/

SqlCol::SqlCol(long & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const long & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

&endif
/*----------------------------------------------------------------------*/

SqlCol::SqlCol(unsigned & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const unsigned & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(unsigned short & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const unsigned short & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

&if $OS = UNIX and !defined BIT64 or $OS = WIN32
/*----------------------------------------------------------------------*/

SqlCol::SqlCol(unsigned long & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const unsigned long & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

&endif
/*----------------------------------------------------------------------*/

SqlCol::SqlCol(float & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VFLOAT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const float & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VFLOAT;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(double & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VFLOAT;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const double & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VFLOAT;
  }
}

/*----------------------------------------------------------------------*/

//#ifndef AIX                                   PTS 1111912
#if !defined(AIX) || (__IBMCPP__ >= 500)    //  PTS 1111912
SqlCol::SqlCol(bool & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VBOOL;
  }
}
#endif

/*----- PTS 1128431 ----------------------------------------------------*/

#if !defined(AIX) || (__IBMCPP__ >= 500)
SqlCol::SqlCol(const bool & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VBOOL;
  }
}
#endif

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(signed char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const signed char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol(unsigned char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p; /* PTS 1128431 */
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol(const unsigned char & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VBYTE_DB;
  }
}

/*----- PTS 1127744 ----------------------------------------------------*/

SqlCol::SqlCol(OmsTypeInt8 & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----- PTS 1127744 ----------------------------------------------------*/

SqlCol::SqlCol(const OmsTypeInt8 & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VINT;
  }
}

/*----- PTS 1127744 ----------------------------------------------------*/

SqlCol::SqlCol(OmsTypeUInt8 & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = &p;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----- PTS 1127744 ----------------------------------------------------*/

SqlCol::SqlCol(const OmsTypeUInt8 & p) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vlen   = sizeof(p);
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = &p;
    m_desc->vfi.vtype  = SqlCol::VUINT;
  }
}

/*----------------------------------------------------------------------*/

SqlCol::SqlCol( void * p, SqlCol::SqlParmType t, long l, short f ) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vaddr  = p;
    m_desc->vfi.vaddrc = p; /* PTS 1128431 */
    m_desc->vfi.vtype  = t;
    m_desc->vfi.vlen   = l;
    m_desc->vfi.vfrac  = STATIC_CAST(unsigned char,f);
    /* ++++ PTS 1110644  ++++ */
    switch (t) {
      case SqlCol::VUNS2 :
        m_desc->vfi.vtype = SqlCol::VUINT;
        m_desc->vfi.vlen  = 2;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VUNS4 :
        m_desc->vfi.vtype = SqlCol::VUINT;
        m_desc->vfi.vlen  = 4;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT2 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 2;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT4 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 4;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT8 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 8;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VFLO4 :
        m_desc->vfi.vtype = SqlCol::VFLOAT;
        m_desc->vfi.vlen  = 4;
        break;
      case SqlCol::VFLO8 :
        m_desc->vfi.vtype = SqlCol::VFLOAT;
        m_desc->vfi.vlen  = 8;
        break;
      default :
        break;
    }
    /* ---- PTS 1110644 ---- */
  }
}

/*----- PTS 1128431 ----------------------------------------------------*/

SqlCol::SqlCol( const void * p, SqlCol::SqlParmType t, long l, short f ) {
  sqlColInit();
  if (m_desc) {
    m_desc->vfi.vaddr  = NULL;
    m_desc->vfi.vaddrc = p;
    m_desc->vfi.vtype  = t;
    m_desc->vfi.vlen   = l;
    m_desc->vfi.vfrac  = STATIC_CAST(unsigned char,f);
    switch (t) {
      case SqlCol::VUNS2 :
        m_desc->vfi.vtype = SqlCol::VUINT;
        m_desc->vfi.vlen  = 2;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VUNS4 :
        m_desc->vfi.vtype = SqlCol::VUINT;
        m_desc->vfi.vlen  = 4;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT2 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 2;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT4 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 4;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VINT8 :
        m_desc->vfi.vtype = SqlCol::VINT;
        m_desc->vfi.vlen  = 8;
        m_desc->vfi.vfrac = 0;
        break;
      case SqlCol::VFLO4 :
        m_desc->vfi.vtype = SqlCol::VFLOAT;
        m_desc->vfi.vlen  = 4;
        break;
      case SqlCol::VFLO8 :
        m_desc->vfi.vtype = SqlCol::VFLOAT;
        m_desc->vfi.vlen  = 8;
        break;
      default :
        break;
    }
  }
}

/*----------------------------------------------------------------------*/

void SqlCol::sqlColInit() {
  m_desc = new SQL_ColumnDesc;
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlType() const {
  if (m_desc) {
    return m_desc->sfi.sp1i_data_type;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

long SqlCol::sqlLength() const {
  if (m_desc) {
    return m_desc->sfi.sp1i_length;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlFrac() const {
  if (m_desc) {
    return m_desc->sfi.sp1i_frac;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlInOut() const {
  if (m_desc) {
    return m_desc->sfi.sp1i_io_type;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlVarType() const {
  if (m_desc) {
    return m_desc->vfi.vtype;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlNullValue() const {
  if (m_desc) {
    if (m_desc->sfi.sp1i_mode.includes(sp1ot_mandatory)) {
      return NOT_NULL;
    }
    else {
      return NULL_PERMITTED;
    }
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

long SqlCol::sqlIndicator() const {
  if (m_desc) {
    return m_desc->vfi.ind;
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlIsNull() const {
  if (m_desc) {
    return (-1 == m_desc->vfi.ind);
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlOverflow() const {
  if (m_desc) {
    return (-2 == m_desc->vfi.ind);
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

int SqlCol::sqlTruncated() const {
  if (m_desc) {
    return (m_desc->vfi.ind > 0);
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

/*#if defined( SUN ) || defined ( SOLARIS )  PTS 1111690 
const SqlCol & SqlCol::operator= (const SqlCol right)
#else */
const SqlCol & SqlCol::operator= (const SqlCol & right)
/*#endif              PTS 1111676 */
{
  if (m_desc) {
    m_desc->vfi = right.m_desc->vfi;
  }
  return *this;
}

/*----------------------------------------------------------------------*/

void SqlCol::sqlSetNull() {
  if (m_desc) {
    m_desc->vfi.ind = -1;
  }
}

/*----------------------------------------------------------------------*/

void SqlCol::sqlSetLength( long len, int frac ) {
  if (m_desc) {
    m_desc->vfi.vlen = len;
    m_desc->vfi.vfrac = (unsigned char)frac;
  }
}

/*----------------------------------------------------------------------*/

void * SqlCol::operator new (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void * p = malloc(sz);
    if (!p) {
      throw DbpError( e_new_failed, "new failed" );
    }
    return p;
# else
#   if defined (OMSTST)
      co10_GetKernelInterface()->TestBadAlloc();
#   endif
    //SAPDBMem_IRawAllocator * m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    return pSessionContext->allocate(sz);   
# endif
}

/*----------------------------------------------------------------------*/

void SqlCol::operator delete (void * p) {
# if defined (EXT_CLIENT_SQL)
    free(p);
# else
    //SAPDBMem_IRawAllocator * m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    pSessionContext->deallocate(p);
# endif
}

/*----------------------------------------------------------------------*/

void * SqlCol::operator new[] (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void * p = malloc(sz);
    if (!p) {
      throw DbpError( e_new_failed, "new failed" );
    }
    return p;
# else
#   if defined (OMSTST)
      co10_GetKernelInterface()->TestBadAlloc();
#   endif
    //SAPDBMem_IRawAllocator * m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    return pSessionContext->allocate(sz);
# endif
}

/*----------------------------------------------------------------------*/

void SqlCol::operator delete[] (void * p) {
# if defined (EXT_CLIENT_SQL)
    free(p);
# else
    //SAPDBMem_IRawAllocator * m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    pSessionContext->deallocate(p);
# endif
}

