/*!
 * \file    SQL_Column.hpp
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


#ifndef SQL_COLUMN_HPP
#define SQL_COLUMN_HPP



/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef __OMS_DEFINES_HPP
#include "Oms/OMS_Defines.h"
#endif

/* PTS 1127744 */
#include "livecachetypes.h"  /* no check */


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SQL_ColumnDesc;


/************************************************************************/
/* Class SqlCol                                                         */
/************************************************************************/

class AFX_EXT_CLASS SqlCol
{
# if defined (SQLCLS_IMPL)
  friend class SQL_Statement;
# endif
private:
# if defined (SQLCLS_IMPL)
  enum {COLNMX = 33};
  enum {INFOINPUT, INFOOUTPUT, INFOINOUT};
  void sqlColInit();
# endif
  SQL_ColumnDesc * m_desc;
public:
  /* symbolic values returned by sqlVarType()
   * used in: SqlCol( void * p, SqlParmType t, long l, short f = 0 );
   */
  typedef enum {
    VBYTE_DB, VDEC, VINT, VUINT, VFLOAT, VCHAR, VSTDSTRING, VSTRING, VBOOL,
    /* following types will be deleted in the future */
    VUNS2, VUNS4, VINT2, VINT4, VINT8, VFLO4, VFLO8,
    /* new inserted types */
    VCHAR_UNICODE, VSTRING_UNICODE
# if !defined (EXT_CLIENT_SQL)
    /* special VCHAR type for ascii columns in an unicode liveCache, PTS 1121342 */
    , VCHAR_ASCII
# endif
  } SqlParmType;
  /* symbolic values returned by sqlType()
   * NOTYPE means: no DB information for this column
   */
  typedef enum {
    FIXED=0, FLOAT=1, CHAR=2, BYTE=4, LONG=6, LONGBYTE=8,
    DATE=10, TIME=11, TIMESTAMP = 13, BOOLEAN=23, UNICODE=24,
    SMALLINT = 29, INTEGER = 30, VARCHAR = 31, VARBYTE = 33,
    STRINGUNICODE=34, LONGUNICODE=35, NOTYPE = -1
  } SqlDbColType;
  /* symbolic values returned by sqlInOut() */
  typedef enum {IO_IN, IO_OUT, IO_INOUT} SqlDbColIO;
  /* symbolic values returned by sqlNullValue () */
  typedef enum {NOT_NULL, NULL_PERMITTED} SqlDbColNull;
  SqlCol();
  ~SqlCol();
  SqlCol(const SqlCol & right);              /* PTS 1111690     */
# if !defined (LCAPPS_SQLCPP_EXPLSQLCOLCONS) /* PTS 1128431     */
  SqlCol(int & p);                           /* variable        */
  SqlCol(const int & p);
  SqlCol(short & p);                         /* description for */
  SqlCol(const short & p);
  SqlCol(long & p);                          /* different base  */
  SqlCol(const long & p);
  SqlCol(unsigned & p);                      /* types           */
  SqlCol(const unsigned & p);
  SqlCol(unsigned short & p);
  SqlCol(const unsigned short & p);
  SqlCol(unsigned long & p);
  SqlCol(const unsigned long & p);
  SqlCol(OmsTypeInt8 & p);                   /* PTS 1127744 */
  SqlCol(const OmsTypeInt8 & p);             /* PTS 1127744 */
  SqlCol(OmsTypeUInt8 & p);                  /* PTS 1127744 */
  SqlCol(const OmsTypeUInt8 & p);            /* PTS 1127744 */
  SqlCol(float & p);
  SqlCol(const float & p);
  SqlCol(double & p);
  SqlCol(const double & p);
  // # ifndef AIX                             //  PTS 1111912
//# if !defined(AIX) || (__IBMCPP__ >= 500)     //  PTS 1111912
//  SqlCol(bool & p);
//# endif
  SqlCol(bool & p);
  SqlCol(const bool & p);
  SqlCol(char & p);
  SqlCol(const char & p);
  SqlCol(signed char & p);
  SqlCol(const signed char & p);
  SqlCol(unsigned char & p);
  SqlCol(const unsigned char & p);
# else
  explicit SqlCol(int & p);
  explicit SqlCol(const int & p);
  explicit SqlCol(unsigned short & p);
  explicit SqlCol(const unsigned short & p);
  explicit SqlCol(char & p);
  explicit SqlCol(const char & p);
  explicit SqlCol(unsigned char & p);
  explicit SqlCol(const unsigned char & p);
  explicit SqlCol(OmsTypeInt8 & p);          /* PTS 1127744 */
  explicit SqlCol(const OmsTypeInt8 & p);    /* PTS 1127744 */
  explicit SqlCol(OmsTypeUInt8 & p);         /* PTS 1127744 */
  explicit SqlCol(const OmsTypeUInt8 & p);   /* PTS 1127744 */
# endif
  /* for application defined types.                           */
  /* if cv is not NULL, it must have been allocated by new    */
  SqlCol( void * p, SqlParmType t, long l, short f = 0 );
  SqlCol( const void * p, SqlParmType t, long l, short f = 0 ); /* PTS 1128431 */
  int     sqlType() const;             // returns column type (SqlDbColType)
  long    sqlLength() const;           // returns column length
  int     sqlFrac() const;             // returns column fraction length
  int     sqlInOut() const;            // returns column usage (SqlDbColIO)
  int     sqlVarType() const;          // returns variable type (SqlParmType)
  int     sqlNullValue() const;        // returns column mode (SqlDbColNull)
  long    sqlIndicator() const;        // returns indicator;
  int     sqlIsNull() const;           // (TRUE=1) if value returned is NULL
  int     sqlOverflow() const;         // (TRUE=1) if value returned is numeric Overflow
  int     sqlTruncated() const;        // (TRUE=1) if value returned has been truncated
  void    sqlSetNull();                // sets a null value for input
  void    sqlSetLength( long, int=0 ); // sets collength and colfrac
/*# if defined(SUN) || defined(SOLARIS)  PTS 1111690
  const SqlCol & operator= (const SqlCol right);
# else */
  const SqlCol & operator= (const SqlCol & right);
/*#endif  PTS 1111676  */
  void *  operator new (size_t sz);
  void    operator delete (void * p);
  void *  operator new[] (size_t sz);
  void    operator delete[] (void * p);
};


#endif /* SQL_COLUMN_HPP */


