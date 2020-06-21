/*!
 * \file    SQL_ColumnDesc.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Column description
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

#ifndef HIN201_H
#include "PacketInterface/PIn_Part.h"
#endif

#ifndef SQL_STATEMENT_HPP
#include "CppSQL/SQL_Statement.hpp"
#endif

#include "hsp40.h"
#include "hsp41.h"
#include "hsp81.h"

/* PTS 1139211 */
#ifndef HSP77_H
#include "hsp77.h"
#endif

#include "livecachetypes.h"  /* no check */
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h" /* PTS 1127744 */

#if defined (EXT_CLIENT_SQL)
#include "ggg00.h"
#include "Oms/OMS_DbpBase.hpp"
#else
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#endif


/************************************************************************/
/* Helper functions                                                     */
/************************************************************************/

/*----- PTS 1127744 ----------------------------------------------------*/

static void CppSQL_VDNNumberToInt8( tsp00_MoveObjPtr pNumber,
                                    int              ioLen,   /* sp1i_in_out_len from short field info */
                                    OmsTypeInt8    & result,
                                    tsp00_NumError & err ) {

    IFR_Int8 * pRes = reinterpret_cast<IFR_Int8*>(&result);
    IFR_Retcode ret = IFRUtil_VDNNumber::numberToInt8( reinterpret_cast<unsigned char*>(pNumber),
                                                       *pRes,
                                                       ioLen );

    if (IFR_OK == ret) {
      err.becomes(num_ok);
    }
    else {
      err.becomes(num_trunc);
    }

}

/*----- PTS 1127744 ----------------------------------------------------*/

static void CppSQL_VDNNumberToUInt8( tsp00_MoveObjPtr pNumber,
                                     int              ioLen,   /* sp1i_in_out_len from short field info */
                                     OmsTypeUInt8   & result,
                                     tsp00_NumError & err) {

    IFR_UInt8 * pRes = reinterpret_cast<IFR_UInt8*>(&result);
    IFR_Retcode ret = IFRUtil_VDNNumber::numberToUInt8( reinterpret_cast<unsigned char*>(pNumber),
                                                        *pRes,
                                                        ioLen );

    if (IFR_OK == ret) {
      err.becomes(num_ok);
    }
    else {
      err.becomes(num_trunc);
    }

}


/*----- PTS 1127744 ----------------------------------------------------*/

static void CppSQL_Int8ToVDNNumber( OmsTypeInt8      source,
                                    tsp00_MoveObjPtr pNumber,
                                    int              len,     /* number of digits (sp1i_length from short field info) */
                                    tsp00_NumError & err ) {

    IFR_Int8 * pSource = reinterpret_cast<IFR_Int8*>(&source);
    IFR_Retcode ret = IFRUtil_VDNNumber::int8ToNumber( *pSource, 
                                                       reinterpret_cast<unsigned char*>(pNumber),
                                                       len );

    if (IFR_OK == ret) {
      err.becomes(num_ok);
    }
    else {
      err.becomes(num_trunc);
    }

}

/*----- PTS 1127744 ----------------------------------------------------*/

static void CppSQL_UInt8ToVDNNumber( OmsTypeUInt8     source,
                                     tsp00_MoveObjPtr pNumber,
                                     int              len,     /* number of digits (sp1i_length from short field info) */
                                     tsp00_NumError & err) {

    IFR_UInt8 * pSource = reinterpret_cast<IFR_UInt8*>(&source);
    IFR_Retcode ret = IFRUtil_VDNNumber::uint8ToNumber( *pSource,
                                                        reinterpret_cast<unsigned char*>(pNumber),
                                                        len );

    if (IFR_OK == ret) {
      err.becomes(num_ok);
    }
    else {
      err.becomes(num_trunc);
    }

}


/************************************************************************/
/* Implementation of class SQL_ColumnDesc                               */
/************************************************************************/

SQL_ColumnDesc::SQL_ColumnDesc() {
  memset( &sfi, '\0', sizeof(sfi) );
  vfi.ind    = 0;
  vfi.vlen   = 0;
  vfi.vaddr  = NULL;
  vfi.vaddrc = NULL; /* PTS 1128431 */
  vfi.vfrac  = 0;
  vfi.vtype  = SqlCol::VBYTE_DB;
  long_idx   = -1;
}

/*----------------------------------------------------------------------*/

const SQL_ColumnDesc & SQL_ColumnDesc::operator= (const SQL_ColumnDesc & right) {
  sfi      = right.sfi;
  vfi      = right.vfi;
  long_idx = right.long_idx;
  return *this;
}

/*----------------------------------------------------------------------*/

void SQL_ColumnDesc::assignVar(const SQL_ColumnDesc & right) {
  vfi = right.vfi;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addParmToBuffer( PIn_Part * part,
                                     SQL_SessionContext & sess,
                                     SQL_Statement & stmt,
                                      int idx  ) {
  int  rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; //PTS 1139211

  switch (vfi.vtype) {
    case SqlCol::VBYTE_DB :
      switch (sfi.sp1i_data_type) {
        case dfixed :
          rc = addParmVNum( part, sess, stmt, idx );
          break;
        case dcha :
        case dche :
        case dchb :
          rc = addParmVChar( part, sess, stmt, idx ); 
          break;
        case dboolean :
          rc = addParmVBool( part, sess, stmt, idx );
          break;
        default :
          // PTS 1139211
          // sprintf( buf1, "Prm:%d", idx);
          // sprintf( buf2, "VT:%x", vfi.vtype);                   /* PTS 1111983 %d -> %x */
          // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
          sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx);
          sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype);                   /* PTS 1111983 %d -> %x */
          sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
          sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
          rc = 0;
          break;
      }
      break;
    case SqlCol::VDEC :
    case SqlCol::VUINT :
    case SqlCol::VINT :
    case SqlCol::VFLOAT :
      rc = addParmVNum( part, sess, stmt, idx );
      break;
# if !defined (EXT_CLIENT_SQL)
    case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
    case SqlCol::VCHAR :
    case SqlCol::VSTRING :
    case SqlCol::VCHAR_UNICODE :
    case SqlCol::VSTRING_UNICODE :
      rc = addParmVChar( part, sess, stmt, idx );
      break;
    case SqlCol::VSTDSTRING :
      rc = addParmVSTDString( part, sess, stmt, idx );
      break;
    case SqlCol::VBOOL :
      rc = addParmVBool( part, sess, stmt, idx );
      break;
    default :
      // PTS 1139211
      // sprintf( buf1, "Prm:%d", idx );
      // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
      // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
      sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
      sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
      sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
      sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
      rc = 0;
      break;
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addOpenLongDataToBuffer( PIn_Part * part,
                                             SQL_SessionContext & sess,
                                             SQL_Statement & stmt,
                                              int idx ) {
  int  rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; // PTS 1139211

  switch (sfi.sp1i_data_type) {
    case dstra :
    case dstrb :
    case dstruni :
    case dlonguni :
      switch (vfi.vtype) {
# if !defined (EXT_CLIENT_SQL)
        case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
        case SqlCol::VCHAR :
        case SqlCol::VSTRING :
        case SqlCol::VCHAR_UNICODE :
        case SqlCol::VSTRING_UNICODE :
          rc = addParmVChar( part, sess, stmt, idx );
          break;
        case SqlCol::VSTDSTRING :
          rc = addParmVSTDString( part, sess, stmt, idx );
          break;
        default :
          // PTS 1139211
          // sprintf( buf1, "Prm:%d", idx );
          // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
          // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
          sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
          sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
          sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
          sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
          rc = 0;
      }
      break;
    default :
      break;
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addOpenLongDescriptor( PIn_Part * part,
                                           SQL_SessionContext & sess,
                                           SQL_Statement & stmt,
                                           int idx ) {
  int  rc = 1;
  char defChar;
  SQL_LongDesc * longDescPtr;

  longDescPtr = stmt.getLongDescPtr(long_idx);
  longDescPtr->dsc.ld_valpos() = 0;
  longDescPtr->dsc.ld_vallen() = vfi.vlen - longDescPtr->varPos;

  switch (sfi.sp1i_data_type) {
    case dstra :
      defChar = csp_ascii_blank;
      break;
    case dstrb :
      defChar = csp_defined_byte;
      break;
    case dstruni :
    case dlonguni :
      defChar = csp_defined_byte;
      longDescPtr->dsc.ld_vallen() = (vfi.vlen - longDescPtr->varPos) * 2;
    default :
      break;
  }
  part->AddParameterArg ( &longDescPtr->dsc, longDescPtr->bufPos, 
                          sfi.sp1i_length, sfi.sp1i_in_out_len , defChar );
  return rc;
}

/*----- PTS 1123241 ----------------------------------------------------*/

int SQL_ColumnDesc::addOpenLongDescriptorMass( PIn_Part * part,
                                               SQL_SessionContext & sess,
                                               SQL_Statement & stmt,
                                               int idx ) {
  int  rc = 1;
  char defChar;
  SQL_LongDesc * longDescPtr;

  longDescPtr = stmt.getLongDescPtrMass(idx);
  longDescPtr->dsc.ld_valpos() = 0;
  longDescPtr->dsc.ld_vallen() = vfi.vlen - longDescPtr->varPos;

  switch (sfi.sp1i_data_type) {
    case dstra :
      defChar = csp_ascii_blank;
      break;
    case dstrb :
      defChar = csp_defined_byte;
      break;
    case dstruni :
    case dlonguni :
      defChar = csp_defined_byte;
      longDescPtr->dsc.ld_vallen() = (vfi.vlen - longDescPtr->varPos) * 2;
    default :
      break;
  }
  part->AddParameterArg ( &longDescPtr->dsc, longDescPtr->bufPos, 
                          sfi.sp1i_length, sfi.sp1i_in_out_len , defChar );
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmFromBuffer( const teo00_Byte * dataPtr,
                                       SQL_SessionContext & sess,
                                       SQL_Statement & stmt,
                                       int idx ) {
  int  rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; // PTS 1139211

  vfi.ind = 0;     /* PTS 1112804 */

  /* PTS 1128431 */
  if (NULL == vfi.vaddr) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) );
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) );
    sess.setRtError( e_no_writable_buffer, buf1, buf2, buf3 );
    rc = 0;
    return rc;
  }

  switch (vfi.vtype) {
    case SqlCol::VBYTE_DB :
      switch (sfi.sp1i_data_type) {
        case dfixed :
          rc = getParmVNum( dataPtr, sess, stmt, idx );
          break;
        case dcha :
        case dche :
        case dchb :
          rc = getParmVChar( dataPtr, sess, stmt, idx );
          break;
        case dboolean :
          rc = getParmVBool( dataPtr, sess, stmt, idx );
          break;
      default :
        // PTS 1139211
        // sprintf( buf1, "Prm:%d", idx );
        // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
        // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
        sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
        sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
        sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
        sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
        rc = 0;
      }
      break;
    case SqlCol::VDEC :
    case SqlCol::VUINT :
    case SqlCol::VINT :
    case SqlCol::VFLOAT :
      rc = getParmVNum( dataPtr, sess, stmt, idx );
      break;
# if !defined (EXT_CLIENT_SQL)
    case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
    case SqlCol::VCHAR :
    case SqlCol::VSTRING :
    case SqlCol::VCHAR_UNICODE :
    case SqlCol::VSTRING_UNICODE :
      rc = getParmVChar( dataPtr, sess, stmt, idx );
      break;
    case SqlCol::VSTDSTRING :
      rc = getParmVSTDString( dataPtr, sess, stmt, idx );
      break;
    case SqlCol::VBOOL :
      rc = getParmVBool( dataPtr,sess, stmt, idx );
      break;
    default :
      // PTS 1139211
      // sprintf( buf1, "Prm:%d", idx );
      // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
      // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
      sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
      sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
      sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
      sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
      rc = 0;
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmLongDescFromBuffer( const tsp00_Byte * dataPtr,
                                               SQL_SessionContext & sess,
                                               SQL_Statement & stmt,
                                               int idx ) {
  int rc = 1;

  switch (sfi.sp1i_data_type) {
    case dstra :
    case dstrb :
    case dstruni :
    case dlonguni :
      if (*(dataPtr + sfi.sp1i_bufpos() - 1) == (teo00_Byte)csp_undef_byte) {
        vfi.ind = -1;
      }
      else {
        SAPDB_MemCopyNoCheck( stmt.getLongDescPtr(long_idx), dataPtr + sfi.sp1i_bufpos(),
                sfi.sp1i_length );
      }
      break;
    default :
      break;
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getOpenLongDataFromBuffer( const tsp00_Byte * dataPtr,
                                               SQL_SessionContext & sess,
                                               SQL_Statement & stmt,
                                               int idx ) {
  int rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; // PTS 1139211
  SQL_LongDesc * longDescPtr = stmt.getLongDescPtr(long_idx);

  /* PTS 1128431 */
  if (NULL == vfi.vaddr) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) );
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) );
    sess.setRtError( e_no_writable_buffer, buf1, buf2, buf3 );
    rc = 0;
    return rc;
  }

  switch (sfi.sp1i_data_type) {
    case dstra :
    case dstrb :
      switch (vfi.vtype) {
        case SqlCol::VCHAR_UNICODE :
        case SqlCol::VSTRING_UNICODE :
          {
            /* local variables */
            tsp81_UCS2Char UCS2Blank;
            tsp81_UCS2Char UCS2NULL;
            int            UCS2Swap;
            unsigned int   converted_len;
            int            len;
            /* assignment to local variables */
            UCS2Blank.s = 32;
            UCS2NULL.s = 0;
            UCS2Swap = ( UCS2Blank.c[0] ? 1 : 0 );
            len = longDescPtr->dsc.ld_vallen()*2;
            /* actual coding */
            if (len > (vfi.vlen - longDescPtr->varPos)) {
              sess.setWarnTrunc();
              vfi.ind = longDescPtr->dsc.ld_maxlen();
              len = vfi.vlen - longDescPtr->varPos;
            }
            sp81ASCIItoUCS2( (tsp81_UCS2Char *)((char*)vfi.vaddr + longDescPtr->varPos),
                             len, UCS2Swap , &converted_len,
                             (tsp00_Byte*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                             longDescPtr->dsc.ld_vallen() );
            longDescPtr->varPos = longDescPtr->varPos + converted_len * 2;
            if ( (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
                 (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
              unsigned int pad_len = vfi.vlen - (longDescPtr->varPos);
              if (pad_len > 0) {
                if (vfi.vtype == SqlCol::VSTRING_UNICODE) {
                  sp81UCS2strncpy( (tsp81_UCS2Char*)((char*)vfi.vaddr + longDescPtr->varPos),
                                   &UCS2NULL, 1 );
                }
                else {
                  memset( (char*)vfi.vaddr + (len*2), ' ', pad_len );
                }
              }
            }
          }
          break;
        default :
          SAPDB_MemCopyNoCheck( (char*)vfi.vaddr + longDescPtr->varPos,
                  dataPtr + longDescPtr->dsc.ld_valpos() - 1,
                  longDescPtr->dsc.ld_vallen() );
          longDescPtr->varPos = longDescPtr->varPos + longDescPtr->dsc.ld_vallen();
      }
      break;
    case dstruni :
    case dlonguni :
      {
        /* local variables */
        tsp81_UCS2Char UCS2Blank;
        tsp81_UCS2Char UCS2NULL;
        int            UCS2Swap;
        unsigned int   converted_len;
        int            len;
        /* assignment to local variables */
        UCS2Blank.s = 32;
        UCS2NULL.s = 0;
        UCS2Swap = ( UCS2Blank.c[0] ? 1 : 0 );
        /* actual coding */
        switch (vfi.vtype) {
          case SqlCol::VCHAR_UNICODE :
          case SqlCol::VSTRING_UNICODE :
            sp81UCS2strncpy( REINTERPRET_CAST(tsp81_UCS2Char*,(tsp00_Byte*)vfi.vaddr + longDescPtr->varPos),
                             (tsp81_UCS2Char*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                             longDescPtr->dsc.ld_vallen()/2 );
            longDescPtr->varPos = longDescPtr->varPos + longDescPtr->dsc.ld_vallen();
            if ( (vm_alldata == longDescPtr->dsc.ld_valmode() ) ||
                 (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
              unsigned int pad_len = vfi.vlen - (longDescPtr->varPos) / 2;
              if (pad_len > 0) {
                if (vfi.vtype == SqlCol::VSTRING_UNICODE) {
                  sp81UCS2strncpy( (tsp81_UCS2Char*)((char*)vfi.vaddr + longDescPtr->varPos),
                                   &UCS2NULL, 1 );
                }
                else {
                  unsigned int len = (longDescPtr->varPos)/2;
                  while (pad_len > 0) {
                    sp81UCS2strncpy( (tsp81_UCS2Char*)((char*)vfi.vaddr + (len*2)),
                                     &UCS2Blank, 1 );
                    pad_len--;
                    len++;
                  }
                }
              }
            }
            break;
          default :
            len = longDescPtr->dsc.ld_vallen() / 2;
            if (len > (vfi.vlen - longDescPtr->varPos)) {
              sess.setWarnTrunc();
              vfi.ind = longDescPtr->dsc.ld_maxlen();
              len = vfi.vlen - longDescPtr->varPos;
            }
            sp81UCS2toASCII( (tsp00_Byte*)vfi.vaddr + longDescPtr->varPos,
                             longDescPtr->dsc.ld_vallen(),
                             &converted_len,
                             (tsp81_UCS2Char*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                             len, UCS2Swap );
            /* PTS 1116580 FF 2002-07-22 check if convertion was possible */
            if (converted_len < (unsigned int)len) {
              vfi.ind = converted_len + 1;
              // PTS 1139211
              // sprintf( buf1, "Prm:%d", idx );
              // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
              // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
              sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
              sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
              sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
              sess.setRtError( e_uni_to_ascii_conv_failed,
                               buf1, buf2, buf3 );
              rc = 0;
            }
            longDescPtr->varPos = longDescPtr->varPos + converted_len;
            if ( (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
                 (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
              unsigned int pad_len = vfi.vlen - longDescPtr->varPos;
              if (pad_len > 0) {
                if (vfi.vtype == SqlCol::VSTRING) {
                  memset((char*)vfi.vaddr + longDescPtr->varPos, '\0', 1);
                }
                else {
                  memset ((char*)vfi.vaddr + longDescPtr->varPos, ' ', pad_len);
                }
              }
            }
        }
      }
  }
  return rc;
}

/*----------------------------------------------------------------------*/

void * SQL_ColumnDesc::operator new (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void * p = malloc(sz);
    if (!p) {
      throw DbpError( e_new_failed, "new failed" );
    }
    return p;
# else
#   if defined(OMSTST)
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

void SQL_ColumnDesc::operator delete (void * p) {
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

int SQL_ColumnDesc::addParmVBool( PIn_Part * part,
                                  SQL_SessionContext & sess,
                                  SQL_Statement & stmt,
                                  int idx ) {
  int  rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; // PTS 1139211

  switch (sfi.sp1i_data_type) {
    case dboolean :
      if (vfi.ind < 0) {
        part->AddParameterArg ( NULL, sfi.sp1i_bufpos(), 0,
                                sfi.sp1i_in_out_len , csp_defined_byte );
      }
      else {
        if (*(bool*)vfi.vaddrc) { /* PTS 1128431 */
          buf1[0] = true;
        }
        else {
          buf1[0] = false;
        }
        part->AddParameterArg ( buf1, sfi.sp1i_bufpos(), 1, 
                                sfi.sp1i_in_out_len , csp_defined_byte );
      }
      break;
    default :
      rc = 0;
  }

  if (0 == rc) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addParmVChar( PIn_Part * part,
                                  SQL_SessionContext & sess,
                                  SQL_Statement & stmt,
                                  int idx ) {
  int              rc = 1;
  int              len = 0;
  tgg00_BasisError rtError;
  tsp81_UCS2Char   UCS2Blank;
  tsp81_UCS2Char   UCS2NULL;
  int              UCS2Swap;
  tsp00_DataType   data_type;
  char             buf1[32];
  char             buf2[6];
  char             buf3[7]; // PTS 1139211
  char defChar;

  UCS2Blank.s = 32;
  UCS2NULL.s = 0;
  UCS2Swap =  ( UCS2Blank.c[0] ? 1 : 0 );
  
  data_type.becomes(sfi.sp1i_data_type);
  if (stmt.isUnicode()) {
    switch (sfi.sp1i_data_type) {
      case dstra :
        data_type.becomes(dstruni);
        break;
      case dcha :
        data_type.becomes(dunicode);
        break;
      case dvarchara :
        data_type.becomes(dvarcharuni);
        break;
      default :
        break;
    }
  }

  switch (data_type) {
    case dstruni :
    case dlonguni :
      break;
    case dstra :
      defChar = csp_ascii_blank;
      break;
    case dstrb :
      defChar = csp_defined_byte;
      break;
  case dvarcharb :
  case dchb : 
  case dunicode :
  case dvarcharuni :
    defChar = csp_defined_byte;
    switch (vfi.vtype) {
      case SqlCol::VBYTE_DB :
        if ( (data_type == dvarcharb) || (data_type == dchb) ) {
          len = 1;
        }
        else {
          rtError = e_incompatible_datatype; 
          rc = 0;
        }
        break;
# if !defined (EXT_CLIENT_SQL)
      case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
      case SqlCol::VCHAR :
        for ( len = vfi.vlen; 
              (len > 0) && (*((char*)vfi.vaddrc + len - 1) == csp_defined_byte); /* PTS 1128431 */
              --len );
        break;
      case SqlCol::VSTRING :
        len = (int)strlen((char*)vfi.vaddrc); /* PTS 1128431 */
        break;
      case SqlCol::VCHAR_UNICODE :
        for ( len = vfi.vlen;
              (len > 0) && !sp81UCS2strncmp( (tsp81_UCS2Char*)((char*)vfi.vaddrc + (len - 1)*2), &UCS2NULL, 1 ); /* PTS 1128431 */
              --len );
        break;
      case SqlCol::VSTRING_UNICODE :
        len = sp81UCS2strlen((tsp81_UCS2Char*)vfi.vaddrc); /* PTS 1128431 */
        break;
    }
    break;
  default :
    defChar = csp_ascii_blank;
    switch ( vfi.vtype ) {
      case SqlCol::VBYTE_DB :
        len = 1;
        break;
# if !defined (EXT_CLIENT_SQL)
      case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
      case SqlCol::VCHAR :
        for ( len = vfi.vlen; 
              (len > 0) && (*((char*)vfi.vaddrc + len - 1) == csp_ascii_blank); /* PTS 1128431 */
              --len );
        break;
      case SqlCol::VSTRING :
        for ( len = (int)strlen((char*)vfi.vaddrc);  /* PTS 1128431 */
              (len > 0) && (*((char*)vfi.vaddrc + len - 1) == csp_ascii_blank); /* PTS 1128431 */
              --len );
        break;
      case SqlCol::VCHAR_UNICODE :
        for ( len = vfi.vlen;
              (len > 0) && !sp81UCS2strncmp( (tsp81_UCS2Char *)((char*)vfi.vaddrc + (len - 1)*2), &UCS2NULL, 1 ); /* PTS 1128431 */
              --len );
        break;
      case SqlCol::VSTRING_UNICODE :
        len = sp81UCS2strlen((tsp81_UCS2Char*)vfi.vaddrc); /* PTS 1128431 */
        break;
    }
    break;
  }

  if (stmt.isUnicode()) {
    switch (sfi.sp1i_data_type) {
      case dstra :
      case dcha :
      case dvarchara :
        len = len * 2;
        break;
      default :
        break;
    }
  }

  if (rc) {
    switch (data_type) {
      case dvarcharb :
      case dchb :
      case dcha :
      case ddate :
      case dtime :
      case dtimestamp :
      case dvarchara :
        switch ( vfi.vtype ) {
          /* case SqlCol::VBYTE_DB : PTS 1124672 */
# if !defined (EXT_CLIENT_SQL)
          case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
            if ( vfi.ind >= 0 && sess.isUnicodeInstance() ) {
              for ( int j = 0; j < len ; j++ ) {
                if ( *((char*)vfi.vaddrc + j) & 0x80 ) { /* PTS 1128431 */
                  rtError = e_nonascii_char;
                  rc = -(j + 1); /* PTS 1130629 */
                  break;
                }
              }
              if (rc < 0) break; /* PTS 1130629 */
            }
# endif
          case SqlCol::VBYTE_DB : /* PTS 1124672 */
          case SqlCol::VCHAR :
          case SqlCol::VSTRING :
            if (vfi.ind < 0) {
              part->AddParameterArg ( NULL, sfi.sp1i_bufpos(), 0,
                                      sfi.sp1i_in_out_len, defChar );
            }
            else {
              if (len > sfi.sp1i_length) {
                sess.setWarnTrunc();
                len = sfi.sp1i_length;
              }
              part->AddParameterArg ( vfi.vaddrc, sfi.sp1i_bufpos(), len, /* PTS 1128431 */
                                      sfi.sp1i_in_out_len , defChar );
            }
            break;
          case SqlCol::VCHAR_UNICODE :
          case SqlCol::VSTRING_UNICODE :
            if (vfi.ind < 0) {
              part->AddParameterArg ( NULL, sfi.sp1i_bufpos(), 0,
                                      sfi.sp1i_in_out_len, defChar );
            }
            else {
              if (len > sfi.sp1i_length) {
                sess.setWarnTrunc();
                len = sfi.sp1i_length;
              }
              part->AddParameterArgUCS2 ( vfi.vaddrc, sfi.sp1i_bufpos(), /* PTS 1128431 */
                                          len, 0, 1, UCS2Swap,
                                          sfi.sp1i_in_out_len,
                                          UCS2Blank );
            }
            break;
        }
        break;
      case dstra :
      case dstrb :
        {
          SQL_LongDesc * longDescPtr;
          longDescPtr = stmt.getLongDescPtr(long_idx);
          if (vfi.ind < 0) {
            vfi.vlen = 0;
            longDescPtr->varPos = 0;
          }

          /* terminate long column input stream ? */
          if (-1 == stmt.getFixBufLength()) {
            len = 0;
            longDescPtr->dsc.ld_valmode().becomes(vm_last_putval);
          }
          else {
            int remainingBytes = part->BytesRemaining();
            //          - MIN_REPLY_SIZE_CO522;
            /* 
            * When called byco521CSqlStmt::addOpenLongDataInput()
            * the following 'if-statement' is always true.
            * It must be clear, that the descriptors for all long-columns
            * which haven't been handled yet, fit into the buffer
            * (max. index - actual index) * sizeof(descriptor + defined Byte)
            */
            if (stmt.getFixBufLength() > part->Length()) {
              remainingBytes = remainingBytes - 
                (stmt.getFixBufLength() - part->Length()) -
                ( (stmt.getLongColNum() - longDescPtr->dsc.ld_used_in_ak()[0]) *
                  (sizeof(tsp00_LongDescriptor) + 1) );
            }
            switch ( vfi.vtype ) {
              case SqlCol::VCHAR_UNICODE :
              case SqlCol::VSTRING_UNICODE :
                len = vfi.vlen - longDescPtr->varPos/2;
                break;
              default :
                len = vfi.vlen - longDescPtr->varPos;
            }
            if (len > remainingBytes) {
              if ((sizeof(tsp00_LongDescriptor) + 1) >= remainingBytes) {
                len = 0;
                longDescPtr->dsc.ld_valmode().becomes(vm_nodata);
              }
              else {
                len = remainingBytes;
                longDescPtr->dsc.ld_valmode().becomes(vm_datapart);
              }
            }
            else {
              if (0 == longDescPtr->varPos) {
                longDescPtr->dsc.ld_valmode().becomes(vm_alldata);
              }
              else {
                if (len > 0) {
                  longDescPtr->dsc.ld_valmode().becomes(vm_lastdata);
                }
              }
            }
            if (!longDescPtr->varPos) {
              longDescPtr->dsc.ld_maxlen() = vfi.vlen;
            }
          }

          if (len) {
            if (stmt.getFixBufLength() > part->Length()) {
              part->ExtendLength(stmt.getFixBufLength() - part->Length());
            }
            longDescPtr->dsc.ld_valpos() = part->Length() + 1;
            longDescPtr->dsc.ld_vallen() = len;
            switch ( vfi.vtype ) {
              case SqlCol::VCHAR_UNICODE :
              case SqlCol::VSTRING_UNICODE :
                part->AddDataUCS2ToAscii( (char*)vfi.vaddrc + longDescPtr->varPos, /* PTS 1128431 */
                                          len, UCS2Swap );
                break;
              default :
                part->AddData( (char*)vfi.vaddrc + longDescPtr->varPos, len ); /* PTS 1128431 */
            }
          }
          else {
            longDescPtr->dsc.ld_valpos() = 0;
            longDescPtr->dsc.ld_vallen() = 0;
          }

          if (vfi.ind < 0) {
            part->AddParameterArg( NULL, longDescPtr->bufPos, 
                                   sfi.sp1i_length, sfi.sp1i_in_out_len,
                                   defChar );
          }
          else {
            part->AddParameterArg( &longDescPtr->dsc, longDescPtr->bufPos,
                                   sfi.sp1i_length, sfi.sp1i_in_out_len,
                                   defChar );
          }

          switch (vfi.vtype) {
            case SqlCol::VCHAR_UNICODE :
            case SqlCol::VSTRING_UNICODE :
              longDescPtr->varPos = longDescPtr->varPos + len*2;
              break;
            default :
              longDescPtr->varPos = longDescPtr->varPos + len;
          }
        } 
        break;
      case dstruni :
      case dlonguni :
        {
          SQL_LongDesc * longDescPtr;
          longDescPtr = stmt.getLongDescPtr(long_idx);
          if (vfi.ind < 0) {
            vfi.vlen = 0;
           longDescPtr->varPos = 0;
          }

          /* terminate long column input stream ? */
          if (-1 == stmt.getFixBufLength()) {
            len = 0;
            longDescPtr->dsc.ld_valmode().becomes(vm_last_putval);
          }
          else {
            int remainingBytes = part->BytesRemaining();
            /* 
            * When called byco521CSqlStmt::addOpenLongDataInput()
            * the following 'if-statement' is always true.
            * It must be clear, that the descriptors for all long-columns
            * which haven't been handled yet, fit into the buffer
            * (max. index - actual index) * sizeof(descriptor + defined Byte)
            */
            if (stmt.getFixBufLength() > part->Length()) {
              remainingBytes = remainingBytes -
                (stmt.getFixBufLength() - part->Length()) -
                ( (stmt.getLongColNum() - longDescPtr->dsc.ld_used_in_ak()[0]) * 
                  (sizeof(tsp00_LongDescriptor) + 1) );
            }
            remainingBytes = remainingBytes & (int)-2;
            switch (vfi.vtype) {
              case SqlCol::VCHAR_UNICODE :
              case SqlCol::VSTRING_UNICODE :
                len = (vfi.vlen - (longDescPtr->varPos/2));
                break;
              default :
                len = vfi.vlen - longDescPtr->varPos;
            }
            if (len > remainingBytes/2) {
              if ((sizeof(tsp00_LongDescriptor) + 1) >= remainingBytes) {
                len = 0;
                longDescPtr->dsc.ld_valmode().becomes(vm_nodata);
              }
              else {
                len = remainingBytes/2;
                longDescPtr->dsc.ld_valmode().becomes(vm_datapart);
              }
            }
            else {
              if (0 == longDescPtr->varPos) {
                longDescPtr->dsc.ld_valmode().becomes(vm_alldata);
              }
              else {
                if (len > 0) {
                  longDescPtr->dsc.ld_valmode().becomes(vm_lastdata);
                }
              }
            }
            if (!longDescPtr->varPos) {
              longDescPtr->dsc.ld_maxlen() = vfi.vlen * 2;
            }
          }

          if (len) {
            if (stmt.getFixBufLength() > part->Length()) {
              part->ExtendLength(stmt.getFixBufLength() - part->Length());
            }
            longDescPtr->dsc.ld_valpos() = part->Length() + 1;
            longDescPtr->dsc.ld_vallen() = len * 2;
            switch ( vfi.vtype ) {
              case SqlCol::VCHAR_UNICODE :
              case SqlCol::VSTRING_UNICODE :
                part->AddData ( (char*)vfi.vaddrc + longDescPtr->varPos, /* PTS 1128431 */
                                len*2 );
                break;
              default :
                part->AddDataAsciiToUCS2 ( (char*)vfi.vaddrc + longDescPtr->varPos, /* PTS 1128431 */
                                           len, UCS2Swap );
            }
          }
          else {
            longDescPtr->dsc.ld_valpos() = 0;
            longDescPtr->dsc.ld_vallen() = 0;
          }

          if (vfi.ind < 0) {
            part->AddParameterArg ( NULL, longDescPtr->bufPos, 
                                    sfi.sp1i_length, sfi.sp1i_in_out_len,
                                    csp_defined_byte );
          }
          else {
            part->AddParameterArg ( &longDescPtr->dsc, longDescPtr->bufPos,
                                    sfi.sp1i_length, sfi.sp1i_in_out_len,
                                    csp_defined_byte );
          }

          switch (vfi.vtype) {
            case SqlCol::VCHAR_UNICODE :
            case SqlCol::VSTRING_UNICODE :
              longDescPtr->varPos = longDescPtr->varPos + len * 2;
              break;
            default :
              longDescPtr->varPos = longDescPtr->varPos + len;
          }
        }
        break;
      case dunicode :
      case dvarcharuni :
        switch (vfi.vtype) {
          case SqlCol::VBYTE_DB :
# if !defined (EXT_CLIENT_SQL)
          case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
          case SqlCol::VCHAR :
          case SqlCol::VSTRING :
            if (vfi.ind < 0) {
              part->AddParameterArgUCS2( NULL, sfi.sp1i_bufpos(), 0, 0, 0,
                                         UCS2Swap, sfi.sp1i_in_out_len,
                                         UCS2Blank );
            }
            else {
              if (len > sfi.sp1i_length) {
                sess.setWarnTrunc();
              len = sfi.sp1i_length;
              }
              part->AddParameterArgUCS2( vfi.vaddrc, sfi.sp1i_bufpos(), len, /* PTS 1128431 */
                                         1, 0, UCS2Swap, sfi.sp1i_in_out_len,
                                         UCS2Blank );
            }
            break;
          case SqlCol::VCHAR_UNICODE :
          case SqlCol::VSTRING_UNICODE :
            if (vfi.ind < 0) {
              part->AddParameterArgUCS2( NULL, sfi.sp1i_bufpos(), 0, 0, 0,
                                         UCS2Swap, sfi.sp1i_in_out_len,
                                         UCS2Blank );
            }
            else {
              if (len > sfi.sp1i_length) {
                sess.setWarnTrunc();
                len = sfi.sp1i_length;
              }
              part->AddParameterArgUCS2( vfi.vaddrc, sfi.sp1i_bufpos(), len, /* PTS 1128431 */
                                         0, 0, UCS2Swap, sfi.sp1i_in_out_len,
                                         UCS2Blank );
            }
            break;
        }
        break;
      default :
        rtError = e_incompatible_datatype;
        rc = 0;
        break;
    }
  }

  if (0 >= rc) { /* PTS 1130629 */
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( rtError, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addParmVNum( PIn_Part * part,
                                 SQL_SessionContext & sess,
                                 SQL_Statement & stmt,
                                 int idx ) {
  int              rc = 1;
  tsp00_NumError   num_err;
  tgg00_BasisError rtError;
  char             buf1[32];
  char             buf2[6];
  char             buf3[7]; // PTS 1139211

  switch (sfi.sp1i_data_type) {
    case dfixed :
    case dfloat :
    case dvfloat :
    case dsmallint :
    case dinteger :
      {
        if (vfi.ind < 0) {
          part->AddParameterArg( NULL, sfi.sp1i_bufpos(), 0, 
                                 sfi.sp1i_in_out_len , csp_defined_byte );
          num_err.becomes(num_ok);
        }
        else {
          if (vfi.vtype  == SqlCol::VDEC) {
            s41pdec( buf1, 1, sfi.sp1i_length, sfi.sp1i_frac,
                     *(tsp00_OldPascalString<20>*) vfi.vaddrc, /* PTS 1128431 */
                     vfi.vlen*2-1, vfi.vfrac, num_err );
          }
          else {
            switch(vfi.vlen) {
              case 1 :
                {
                  switch (vfi.vtype) {
                    case SqlCol::VBYTE_DB :
                    case SqlCol::VINT :
                      {
                        tsp00_Int2 l_int2 = *(const signed char*) vfi.vaddrc; /* PTS 1128431 */
                        s41psint( buf1, 1, sfi.sp1i_length, 0,
                                l_int2, num_err );
                      }
                      break;
                    case SqlCol::VUINT :
                      {
                        unsigned short l_uint2 = *(const unsigned char*)vfi.vaddrc; /* PTS 1128431 */
                        s41psuns( buf1, 1, sfi.sp1i_length, 0, 
                                  l_uint2, num_err );
                      }
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                      break;
                  }
                }
                break;
              case 2 :
                {
                  switch (vfi.vtype) {
                    case SqlCol::VINT :
                      s41psint( buf1, 1, sfi.sp1i_length, 0,
                                *(const tsp00_Int2*)vfi.vaddrc, num_err ); /* PTS 1128431 */
                      break;
                    case SqlCol::VUINT :
                      s41psuns( buf1, 1, sfi.sp1i_length, 0,
                               *(const unsigned short*) vfi.vaddrc, num_err); /* PTS 1128431 */
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                      break;
                  }
                }
                break;
              case 4 :
                {
                  switch (vfi.vtype) {
                    case SqlCol::VINT :
                      s41plint( buf1, 1, sfi.sp1i_length, 0,
                                *(const tsp00_Int4*) vfi.vaddrc, num_err); /* PTS 1128431 */
                      break;
                    case SqlCol::VUINT :
                      s41pluns( buf1, 1, sfi.sp1i_length, 0,
                                *(const tsp00_Uint4*) vfi.vaddrc, num_err); /* PTS 1128431 */
                      break;
                    case SqlCol::VFLOAT :
                      s41plrel( buf1, 1, sfi.sp1i_length, csp_float_frac, 
                                (double)*(const float*)vfi.vaddrc, num_err); /* PTS 1128431 */
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                  }
                }
                // s41p4int( ++buf, 1, *(const tsp00_Int4*)va, num_err );
                break;
              case 8 :
                {
                  switch (vfi.vtype) {
#                   if defined (BIT64)
                      case SqlCol::VINT :
                        s41p8int( buf1, 1, sfi.sp1i_length, 0,
                                  *(const OmsTypeInt64*) vfi.vaddrc, /* PTS 1128431 */
                                  num_err );
                        break;
                      case SqlCol::VUINT :
                        s41p8int( buf1, 1, sfi.sp1i_length, 0,
                                  *(const tsp00_Longint*) vfi.vaddrc, /* PTS 1128431 */
                                  num_err );
                        break;
#                   else /* PTS 1127744 */
                      case SqlCol::VINT :
                        CppSQL_Int8ToVDNNumber( *(const OmsTypeInt8*)vfi.vaddrc,
                                                (tsp00_MoveObj*)buf1, sfi.sp1i_length, num_err );
                        break;
                      case SqlCol::VUINT :
                        CppSQL_UInt8ToVDNNumber( *(const OmsTypeUInt8*)vfi.vaddrc,
                                                 (tsp00_MoveObj*)buf1, sfi.sp1i_length, num_err );
                        break;
#                   endif
                      case SqlCol::VFLOAT :
                        s41plrel( buf1, 1, sfi.sp1i_length, csp_float_frac,
                                  *(const double*) vfi.vaddrc, num_err ); /* PTS 1128431 */
                        break;
                      default :
                        rtError = e_incompatible_datatype;
                        rc = 0;
                  }
                }
                break;
              default :
                num_err.becomes(num_incompatible);
            }
          }
          part->AddParameterArg( buf1, sfi.sp1i_bufpos(), sfi.sp1i_in_out_len,
                                 sfi.sp1i_in_out_len , csp_defined_byte );
        }
        switch (num_err) {
          case num_ok :
            rc = 1;
            break;
          case num_trunc :
          case num_overflow :
            rtError = e_inp_number_overflow;
            rc = 0;
            break;
          case num_invalid :
            rtError = e_inp_number_invalid;
            rc = 0; break;
          case num_incompatible :
            rtError = e_incompatible_datatype;
            rc = 0;
            break;
        }
      }
      break;
    default :
      rtError = e_incompatible_datatype;
      rc = 0;
      break;
  }

  if (0 == rc) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( rtError, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::addParmVSTDString( PIn_Part * part,
                                       SQL_SessionContext & sess,
                                       SQL_Statement & stmt,
                                       int idx ) {
  return 0;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmVBool( const teo00_Byte * dataPtr,
                                  SQL_SessionContext & sess,
                                  SQL_Statement & stmt,
                                  int idx ) {
  int  rc = 1;
  char buf1[32];
  char buf2[6];
  char buf3[7]; // PTS 1139211

  switch (sfi.sp1i_data_type) {
    case dboolean :
      if (*(dataPtr + sfi.sp1i_bufpos() - 1) == 
          (teo00_Byte)csp_undef_byte) {
        vfi.ind = -1;
      }
      else {
        if (*(dataPtr + sfi.sp1i_bufpos())) {
          *(bool*)vfi.vaddr = true;
        }
        else {
          *(bool*)vfi.vaddr = false;
        }
      }
      break;
    default :
      rc = 0;
  }

  if (0 == rc) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( e_incompatible_datatype, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmVChar( const teo00_Byte * dataPtr,
                                  SQL_SessionContext & sess,
                                  SQL_Statement & stmt,
                                  int idx ) {
  int              rc = 1;
  int              len;
  tgg00_BasisError rtError;
  tsp81_UCS2Char   UCS2Blank;
  tsp81_UCS2Char   UCS2NULL;
  int              UCS2Swap;
  tsp00_DataType   data_type;
  char             buf1[32];
  char             buf2[6];
  char             buf3[7]; // PTS 1139211
  char             defChar;

  UCS2Blank.s = 32;
  UCS2NULL.s = 0;
  UCS2Swap =  ( UCS2Blank.c[0] ? 1 : 0 );

  data_type.becomes(sfi.sp1i_data_type);
  if (stmt.isUnicode()) {
    switch (sfi.sp1i_data_type) {
      case dstra :
        data_type.becomes(dstruni);
        break;
      case dcha :
        data_type.becomes(dunicode);
        break;
      case dvarchara :
        data_type.becomes(dvarcharuni);
        break;
      default :
        break;
    }
  }

  switch (data_type) {
    case dstruni :
    case dlonguni :
      break;
    case dstra :
      defChar = csp_ascii_blank;
      break;
    case dstrb :
      defChar = csp_defined_byte;
      break;
    case dvarcharb :
    case dchb :
      defChar = csp_defined_byte;
      /* PTS 1111983 
         for ( len=sfi.sp1i_length; (len > 0) && (*(dataPtr + sfi.sp1i_bufpos() + len - 1) == csp_ascii_blank); --len);*/
      for ( len = sfi.sp1i_length;
            (len > 0) && (*(dataPtr + sfi.sp1i_bufpos() + len - 1) == csp_defined_byte);
            --len);
      break;
    case dunicode :
    case dvarcharuni :
      for ( len = sfi.sp1i_length*2;
            (len > 0) && !sp81UCS2strncmp( (tsp81_UCS2Char*)(dataPtr + sfi.sp1i_bufpos() + len - 2), &UCS2Blank, 1 );
            len = len -2 );
      if (len) {
        len = len / 2;    /* len == number of UCS2-characters */
      }
      break;
    default :
      defChar = csp_ascii_blank;
      for ( len = sfi.sp1i_length;
            (len > 0) && (*(dataPtr + sfi.sp1i_bufpos() + len - 1) == csp_ascii_blank);
            --len);
      break;
  }

  /* PTS 1116580 FF
  if (stmt.isUnicode()) {
    switch (sfi.sp1i_data_type) {
      case dstra :
      case dcha :
      case dvarchara :
        len = len * 2;
        break;
      default :
        break;
    }
  }
  */

  switch (data_type) {
    case dvarcharb :
    case dchb :
    case dcha :
    case ddate :
    case dtime :
    case dtimestamp :
    case dvarchara :
      switch (vfi.vtype) {
        case SqlCol::VBYTE_DB :
# if !defined (EXT_CLIENT_SQL)
        case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
        case SqlCol::VCHAR :
        case SqlCol::VSTRING :
          if ((char)csp_undef_byte == *((char*)dataPtr + sfi.sp1i_bufpos() - 1)) {
            vfi.ind = -1;
          }
          else {
            switch (vfi.vtype) {
              case SqlCol::VBYTE_DB :
                *(char*)vfi.vaddr = *((char*)dataPtr + sfi.sp1i_bufpos());
                /* if (1 != len) { PTS 1111983 */
                if (1 < len) {
                  rtError = e_incompatible_datatype;
                  rc = 0;
                }
                break;
# if !defined (EXT_CLIENT_SQL)
              case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
                if (sess.isUnicodeInstance()) {
                  for ( int j = 0; j < len ; j++ ) {
                    if ( *((char*)dataPtr + sfi.sp1i_bufpos() + j) & 0x80 ) {
                      rtError = e_nonascii_char;
                      rc = -(j + 1); /* PTS 1130629 */
                      break;
                    }
                  }
                  if ( (rc < 0) && (-rc - 1 >= vfi.vlen) ) { /* PTS 1130629 */
                    rc = 0;
                  }
                }
# endif
              case SqlCol::VCHAR :
                if (len > vfi.vlen) {
                  sess.setWarnTrunc();
                  vfi.ind = len;
                  len = vfi.vlen;
                }
                SAPDB_MemCopyNoCheck( vfi.vaddr, dataPtr + sfi.sp1i_bufpos(), len );
                if (len < vfi.vlen) {
                  memset( (char*)vfi.vaddr + len, defChar, 
                          vfi.vlen - len );
                }
                break;
              case SqlCol::VSTRING :
                if (len >= vfi.vlen) {
                  sess.setWarnTrunc();
                  vfi.ind = len;
                  len = vfi.vlen - 1;
                }
                SAPDB_MemCopyNoCheck( vfi.vaddr, dataPtr + sfi.sp1i_bufpos(), len );
                *((char*)vfi.vaddr + len) = '\0';
                break;
            }
          }
          break;
        case SqlCol::VCHAR_UNICODE :
        case SqlCol::VSTRING_UNICODE :
          if ((char)csp_undef_byte == 
              *((char*)dataPtr + sfi.sp1i_bufpos() - 1)) {
            vfi.ind = -1;
          }
          else {
            unsigned int converted_len;
            switch ( vfi.vtype ) {
              case SqlCol::VSTRING_UNICODE :
                if (len >= vfi.vlen) {
                  sess.setWarnTrunc();
                  vfi.ind = len;
                  len = vfi.vlen - 1;
                }
                sp81ASCIItoUCS2( REINTERPRET_CAST(tsp81_UCS2Char*,vfi.vaddr),
                                 len, UCS2Swap , &converted_len,
                                 (tsp00_Byte*)(dataPtr + sfi.sp1i_bufpos()),
                                 len );
                /* PTS 1116580 FF 2002-07-09 */
                for (len = converted_len; len < ( vfi.vlen ); len++ ) {
                  *((short*)vfi.vaddr + len) = 0;
                }
                break;
              case SqlCol::VCHAR_UNICODE :
                if (len > vfi.vlen) {
                  sess.setWarnTrunc();
                  vfi.ind = len;
                  len = vfi.vlen;
                }
                sp81ASCIItoUCS2( REINTERPRET_CAST(tsp81_UCS2Char*,vfi.vaddr),
                                 len, UCS2Swap , &converted_len,
                                 (tsp00_Byte*)(dataPtr + sfi.sp1i_bufpos()),
                                 len );
                /* PTS 1116580 FF 2002-07-09 */
                for ( len = converted_len; len < ( vfi.vlen ); len++ ) {
                  *((short*)vfi.vaddr + len) = UCS2Blank.s;
                }
                break;
            }
          }
      }
      break;
    case dstra :
    case dstrb :
      if ((char)csp_undef_byte == *((char*)dataPtr + sfi.sp1i_bufpos() - 1)) {
        vfi.ind = -1;
      }
      else {
        SQL_LongDesc * longDescPtr = stmt.getLongDescPtr(long_idx);
        char colIdx = longDescPtr->dsc.ld_used_in_ak()[0];
        SAPDB_MemCopyNoCheck( &longDescPtr->dsc, dataPtr + longDescPtr->bufPos,
                sizeof(longDescPtr->dsc) );
        longDescPtr->dsc.ld_used_in_ak()[0] = colIdx;
        if ( (vm_datapart == longDescPtr->dsc.ld_valmode()) ||
             (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
             (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
          switch (vfi.vtype) {
            case SqlCol::VCHAR_UNICODE :
            case SqlCol::VSTRING_UNICODE :
              {
                unsigned int converted_len;
                UCS2Blank.s = 32;
                UCS2NULL.s = 0;
                UCS2Swap =  ( UCS2Blank.c[0] ? 1 : 0 );
                len = longDescPtr->dsc.ld_vallen();
                if (len*2 > (vfi.vlen - longDescPtr->varPos)) {
                  sess.setWarnTrunc();
                  vfi.ind = longDescPtr->dsc.ld_maxlen();
                  len = (vfi.vlen - longDescPtr->varPos) / 2;
                }
                sp81ASCIItoUCS2( (tsp81_UCS2Char*)((char*)vfi.vaddr + longDescPtr->varPos),
                                 len, UCS2Swap , &converted_len,
                                 (tsp00_Byte*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                                 len );
                longDescPtr->varPos = longDescPtr->varPos + converted_len*2;
                if ( (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
                     (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
                  unsigned int pad_len = vfi.vlen - (longDescPtr->varPos);
                  if (pad_len > 0) {
                    if (vfi.vtype == SqlCol::VSTRING_UNICODE) {
                      sp81UCS2strncpy ( (tsp81_UCS2Char*)((char*)vfi.vaddr + longDescPtr->varPos),
                                         &UCS2NULL, 1);
                    }
                    else {
                      memset( (char*)vfi.vaddr + longDescPtr->varPos,
                              ' ', pad_len);
                    }
                  }
                }
              }
              break;
            default :
              SAPDB_MemCopyNoCheck( (char*)vfi.vaddr + longDescPtr->varPos,
                      dataPtr + longDescPtr->dsc.ld_valpos() - 1,
                      longDescPtr->dsc.ld_vallen() );
              longDescPtr->varPos = longDescPtr->varPos + 
                                    longDescPtr->dsc.ld_vallen();
              if ( (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
                   (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
                unsigned int pad_len = vfi.vlen - (longDescPtr->varPos);
                if (pad_len > 0) {
                  if (vfi.vtype == SqlCol::VSTRING) {
                    memset( (char*)vfi.vaddr + longDescPtr->varPos,
                            '\0', 1);
                  }
                  else {
                    memset( (char*)vfi.vaddr + longDescPtr->varPos,
                            ' ', pad_len);
                  }
                }
              }
          }
        }
      }
      break;
    case dstruni :
    case dlonguni :
      if ((char)csp_undef_byte == *((char*)dataPtr + sfi.sp1i_bufpos() - 1)) {
        vfi.ind = -1;
      }
      else {
        unsigned int converted_len;
        SQL_LongDesc * longDescPtr = stmt.getLongDescPtr(long_idx);
        char colIdx = longDescPtr->dsc.ld_used_in_ak()[0];
        SAPDB_MemCopyNoCheck( &longDescPtr->dsc, dataPtr + longDescPtr->bufPos,
                sizeof(longDescPtr->dsc));
        longDescPtr->dsc.ld_used_in_ak()[0] = colIdx;
        if ( (vm_datapart == longDescPtr->dsc.ld_valmode()) ||
             (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
             (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
          switch (vfi.vtype) {
            case SqlCol::VCHAR_UNICODE :
            case SqlCol::VSTRING_UNICODE :
              sp81UCS2strncpy( REINTERPRET_CAST(tsp81_UCS2Char*,(tsp00_Byte*)vfi.vaddr + longDescPtr->varPos),
                               (tsp81_UCS2Char*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                               longDescPtr->dsc.ld_vallen()/2 );
              longDescPtr->varPos = longDescPtr->varPos + 
                                    longDescPtr->dsc.ld_vallen();
              if ( (vm_alldata == longDescPtr->dsc.ld_valmode()) ||
                   (vm_lastdata == longDescPtr->dsc.ld_valmode()) ) {
                unsigned int pad_len = vfi.vlen - longDescPtr->varPos/2;
                if (pad_len > 0) {
                  if (vfi.vtype == SqlCol::VSTRING_UNICODE) {
                    sp81UCS2strncpy ( (tsp81_UCS2Char*)((char*)vfi.vaddr + longDescPtr->varPos),
                                      &UCS2NULL, 1);
                  }
                  else {
                    unsigned int len = longDescPtr->varPos / 2;
                    while (pad_len > 0) {
                      sp81UCS2strncpy ( (tsp81_UCS2Char*)((char*)vfi.vaddr + (len*2)),
                                        &UCS2Blank, 1 );
                      pad_len--;
                      len++;
                    }
                  }
                }
              }
              break;
            default :
              len = longDescPtr->dsc.ld_vallen() / 2;
              if (len > (vfi.vlen - longDescPtr->varPos)) {
                sess.setWarnTrunc();
                vfi.ind = longDescPtr->dsc.ld_maxlen();
                len = vfi.vlen - longDescPtr->varPos;
              }
              sp81UCS2toASCII( (tsp00_Byte*)vfi.vaddr + longDescPtr->varPos,
                               this->vfi.vlen, &converted_len,
                               (tsp81_UCS2Char*)(dataPtr + longDescPtr->dsc.ld_valpos() - 1),
                               len, UCS2Swap );
              /* PTS 1116580 FF 2002-07-22 check if convertion was possible */
              if (converted_len < (unsigned int)len) {
                vfi.ind = converted_len + 1;
                rtError = e_uni_to_ascii_conv_failed;
                rc = 0;
              }
              longDescPtr->varPos = longDescPtr->varPos + converted_len;
          }
        }
      }
      break;
    case dunicode :
    case dvarcharuni :
      if ((char)csp_undef_byte == *((char*)dataPtr + sfi.sp1i_bufpos() - 1)) {
        vfi.ind = -1;
      }
      else {
        unsigned int converted_len;
        unsigned int pad_len;
        switch (vfi.vtype) {
# if !defined (EXT_CLIENT_SQL)
          case SqlCol::VCHAR_ASCII : /* PTS 1121342 */
# endif
          case SqlCol::VCHAR :
            if (len > vfi.vlen) {
              sess.setWarnTrunc();
              vfi.ind = len;
              len = vfi.vlen;
            }
            sp81UCS2toASCII( (tsp00_Byte*)vfi.vaddr, vfi.vlen, &converted_len,
                             (tsp81_UCS2Char*)(dataPtr + sfi.sp1i_bufpos()),
                             len, UCS2Swap );
            /* PTS 1116580 FF 2002-07-22 check if convertion was possible */
            if (converted_len < (unsigned int)len) {
              vfi.ind = converted_len + 1;
              rtError = e_uni_to_ascii_conv_failed;
              rc = 0;
            }
            if (converted_len < (unsigned int)vfi.vlen) {
              memset( (char*)vfi.vaddr + converted_len, csp_ascii_blank,
                      vfi.vlen - converted_len );
            }
            break;
          case SqlCol::VSTRING :
            if (len >= vfi.vlen) {
              sess.setWarnTrunc();
              vfi.ind = len;
              len = vfi.vlen - 1;
            }
            sp81UCS2toASCII( (tsp00_Byte*)vfi.vaddr, vfi.vlen, &converted_len,
                             (tsp81_UCS2Char *)(dataPtr + sfi.sp1i_bufpos()),
                             len, UCS2Swap );
            *((char*)vfi.vaddr + converted_len) = '\0';
            /* PTS 1116580 FF 2002-07-22 check if convertion was possible */
            if ( converted_len < (unsigned int)len ) {
              vfi.ind = converted_len + 1;
              rtError = e_uni_to_ascii_conv_failed;
              rc = 0;
            }
            break;
          case SqlCol::VCHAR_UNICODE :
            if (len > vfi.vlen) {
              sess.setWarnTrunc();
              vfi.ind = len;
              len = vfi.vlen;
            }
            sp81UCS2strncpy( REINTERPRET_CAST(tsp81_UCS2Char*,vfi.vaddr),
                             (tsp81_UCS2Char*)(dataPtr + sfi.sp1i_bufpos()),
                             len );
            pad_len = vfi.vlen - len;
            while (pad_len > 0) {
              sp81UCS2strncpy ( (tsp81_UCS2Char*)((char*)vfi.vaddr + (len*2)),
                                &UCS2Blank, 1);
              pad_len--;
              len++;
            }
            break;
          case SqlCol::VSTRING_UNICODE :
            if (len >= vfi.vlen) {
              sess.setWarnTrunc();
              vfi.ind = len;
              len = vfi.vlen - 1;
            }
            sp81UCS2strncpy( REINTERPRET_CAST(tsp81_UCS2Char*,vfi.vaddr),
                             (tsp81_UCS2Char*)(dataPtr + sfi.sp1i_bufpos()),
                             len );
            if (len < vfi.vlen -2) {
              sp81UCS2strncpy ((tsp81_UCS2Char*)((char*)vfi.vaddr + (len*2)), &UCS2NULL, 1);
            }
            break;
        }
      }
      break;
    default :
      rtError = e_incompatible_datatype;
      rc = 0;
      break;
  }

  if (rc <= 0) { /* PTS 1130629 */
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( rtError, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmVNum( const teo00_Byte * dataPtr,
                                 SQL_SessionContext & sess,
                                 SQL_Statement & stmt,
                                 int idx ) {
  int              rc = 1;
  tsp00_NumError   num_err;
  tgg00_BasisError rtError;
  char             buf1[32];
  char             buf2[6];
  char             buf3[7]; // PTS 1139211

  switch (sfi.sp1i_data_type) {
    case dfixed :
    case dfloat :
    case dvfloat :
    case dsmallint :
    case dinteger :
      {
        if (*( dataPtr + sfi.sp1i_bufpos() - 1) == 
            (teo00_Byte)csp_undef_byte) {
          vfi.ind = -1;
          num_err.becomes(num_ok);
        }
        else {
          if (vfi.vtype  == SqlCol::VDEC) {
            s40gdec( (void*)dataPtr, sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                     *(tsp00_OldPascalString<20>*) vfi.vaddr,
                     vfi.vlen*2-1, vfi.vfrac, num_err );
          }
          else {
            switch(vfi.vlen) {
              case 1 :
                {
                  switch ( vfi.vtype ) {
                    case SqlCol::VBYTE_DB :
                    case SqlCol::VINT :
                      {
                        tsp00_Int2 l_int2;
                        s40gsint( (unsigned char*)dataPtr,
                                  sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                  l_int2, num_err);
                        if ( (l_int2 < -128) || (l_int2 > 127) ) {
                          num_err.becomes(num_overflow);
                        }
                        else {
                          *(signed char*)vfi.vaddr = (signed char)l_int2;
                        }
                      }
                      break;
                    case SqlCol::VUINT :
                      {
                        short l_uint2;
                        s40gsuns( (unsigned char*)dataPtr,
                                  sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                  l_uint2, num_err );
                        if ( (l_uint2 < 0) || (l_uint2 > 255) ) {
                          num_err.becomes(num_overflow);
                        }
                        else {
                          *(unsigned char*)vfi.vaddr = (unsigned char)l_uint2;
                        }
                      }
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                  }
                }
                break;
              case 2 :
                {
                  switch (vfi.vtype) {
                    case SqlCol::VINT :
                      s40gsint( (unsigned char*)dataPtr,
                                sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                *(tsp00_Int2*) vfi.vaddr, num_err );
                      break;
                    case SqlCol::VUINT :
                      s40gsuns( (unsigned char*)dataPtr,
                                sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                *(short*) vfi.vaddr, num_err );
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                  }
                }
                break;
              case 4 :
                {
                  switch (vfi.vtype) {
                    case SqlCol::VINT :
                      s40glint( (unsigned char*)dataPtr,
                                sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                *(tsp00_Int4*) vfi.vaddr, num_err );
                      break;
                    case SqlCol::VUINT :
                      s40gluns( (unsigned char*)dataPtr,
                                sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                *(tsp00_Int4*)vfi.vaddr, num_err );
                      break;
                    case SqlCol::VFLOAT :
                      s40gsrel( (unsigned char*)dataPtr,
                                sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                *(float*)vfi.vaddr, num_err );
                      break;
                    default :
                      rtError = e_incompatible_datatype;
                      rc = 0;
                  }
                }
                break;
              case 8 :
                {
                  switch (vfi.vtype) {
#                   if defined(BIT64)
                      case SqlCol::VINT :
                        s40g8int( (unsigned char*)dataPtr,
                                  sfi.sp1i_bufpos() + 1 , sfi.sp1i_length,
                                  *(tsp00_Longint*)vfi.vaddr, num_err );
                        break;
                      case SqlCol::VUINT :
                        s40g8int( (unsigned char*)dataPtr,
                                  sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                  *(tsp00_Longint*)vfi.vaddr, num_err );
                        break;
#                   else /* PTS 1127744 */
                      case SqlCol::VINT :
                        CppSQL_VDNNumberToInt8( (tsp00_MoveObj*)(dataPtr + sfi.sp1i_bufpos()),
                                                sfi.sp1i_in_out_len,
                                                *(OmsTypeInt8*)vfi.vaddr,
                                                num_err );
                        break;
                      case SqlCol::VUINT :
                        CppSQL_VDNNumberToUInt8( (tsp00_MoveObj*)(dataPtr + sfi.sp1i_bufpos()),
                                                 sfi.sp1i_in_out_len,
                                                 *(OmsTypeUInt8*)vfi.vaddr,
                                                 num_err );
                        break;
#                   endif
                      case SqlCol::VFLOAT :
                        s40glrel( (unsigned char*)dataPtr,
                                  sfi.sp1i_bufpos() + 1, sfi.sp1i_length,
                                  *(double*)vfi.vaddr, num_err );
                        break;
                      default :
                        rtError = e_incompatible_datatype;
                        rc = 0;
                  }
                }
                break;
              default :
                num_err.becomes(num_incompatible);
            }
          }
        }
        switch (num_err) {
          case num_ok :
            rc = 1;
            break;
          case num_trunc :
          case num_overflow :
            rtError = e_inp_number_overflow;
            rc = 0;
            break;
          case num_invalid :
            rtError = e_inp_number_invalid;
            rc = 0;
            break;
          case num_incompatible :
            rtError = e_incompatible_datatype;
            rc = 0;
            break;
        }
      }
      break;
    default :
      rtError = e_incompatible_datatype;
      rc = 0;
  }

  if (0 == rc) {
    // PTS 1139211
    // sprintf( buf1, "Prm:%d", idx );
    // sprintf( buf2, "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    // sprintf( buf3, "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sp77sprintf( buf1, sizeof(buf1), "Prm:%d", idx );
    sp77sprintf( buf2, sizeof(buf2), "VT:%x", vfi.vtype );                  /* PTS 1111983 %d -> %x */
    sp77sprintf( buf3, sizeof(buf3), "DBT:%x", (int)(sfi.sp1i_data_type) ); /* PTS 1111983 %d -> %x */
    sess.setRtError( rtError, buf1, buf2, buf3 );
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_ColumnDesc::getParmVSTDString( const teo00_Byte * dataPtr,
                                       SQL_SessionContext & sess,
                                       SQL_Statement & stmt,
                                       int idx ) {
  return 0;
}

