/*!
 * \file    SQL_ColumnDesc.hpp
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


#ifndef SQL_COLUMNDESC_HPP
#define SQL_COLUMNDESC_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef VSP001_H
#include "vsp001.h"
#endif


//#define     MIN_REPLY_SIZE_CO522    1024


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class PIn_Part;
class SQL_SessionContext;
class SQL_Statement;


/************************************************************************/
/* Class SQL_ColumnDesc                                                 */
/************************************************************************/

class SQL_ColumnDesc {
public:
  tsp1_param_info sfi; 
  struct sql_var_info {
    long          ind;
    long          vlen;
    void *        vaddr;
    const void *  vaddrc; /* PTS 1128431 */
    unsigned char vfrac;
    unsigned char vtype;
  } vfi;
  short long_idx;
  SQL_ColumnDesc();
  const  SQL_ColumnDesc & operator= (const SQL_ColumnDesc & right);
  void   assignVar(const SQL_ColumnDesc & right);
  int    addParmToBuffer( PIn_Part * part,
                          SQL_SessionContext & sess,
                          SQL_Statement & stmt,
                          int idx );
  int    addOpenLongDataToBuffer( PIn_Part * part,
                                  SQL_SessionContext & sess,
                                  SQL_Statement & stmt,
                                  int idx );
  int    addOpenLongDescriptor( PIn_Part * part,
                                SQL_SessionContext & sess,
                                SQL_Statement & stmt, 
                                int idx );
  int    addOpenLongDescriptorMass( PIn_Part * part,
                                    SQL_SessionContext & sess,
                                    SQL_Statement & stmt, 
                                    int idx );
  int    getParmFromBuffer( const teo00_Byte * dataPtr, 
                            SQL_SessionContext & sess,
                            SQL_Statement & stmt,
                            int idx );
  int    getParmLongDescFromBuffer( const tsp00_Byte * dataPtr, 
                                    SQL_SessionContext & sess,
                                    SQL_Statement & stmt,
                                    int idx );
  int    getOpenLongDataFromBuffer( const tsp00_Byte * dataPtr, 
                                    SQL_SessionContext & sess,
                                    SQL_Statement & stmt,
                                    int idx );
  void * operator new (size_t sz);
  void   operator delete (void * p);
  int    getIOType() {return sfi.sp1i_io_type;} /* PTS 1123241 */
protected:
  int addParmVBool( PIn_Part * part, SQL_SessionContext & sess,
                    SQL_Statement & stmt, int idx );
  int addParmVChar( PIn_Part * part, SQL_SessionContext & sess,
                    SQL_Statement & stmt, int idx );
  int addParmVNum( PIn_Part * part, SQL_SessionContext & sess,
                   SQL_Statement & stmt, int idx );
  int addParmVSTDString( PIn_Part * part, SQL_SessionContext & sess,
                         SQL_Statement & stmt, int idx );
  int getParmVBool( const teo00_Byte * dataPtr, SQL_SessionContext & sess,
                    SQL_Statement & stmt, int idx );
  int getParmVChar( const teo00_Byte * dataPtr, SQL_SessionContext & sess,
                    SQL_Statement & stmt, int idx );
  int getParmVNum( const teo00_Byte * dataPtr, SQL_SessionContext & sess,
                   SQL_Statement & stmt, int idx );
  int getParmVSTDString( const teo00_Byte * dataPtr, 
                         SQL_SessionContext & sess, 
                         SQL_Statement & stmt, int idx );
};


#endif

