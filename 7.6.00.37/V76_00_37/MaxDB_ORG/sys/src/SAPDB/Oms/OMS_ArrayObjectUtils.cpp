/*!
* \file    OMS_ArrayObjectUtils.cpp
* \author  ReinerSi
* \brief   utils for array objects
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

#include "Oms/OMS_ArrayObjectUtils.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*======================================================================*/

char OmsArrayUtils::getHexCode( char val )
{
  switch( val )
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
    return val + '0'; 
  default: 
    return val + ( 'A' - 10 ); 
  }
}

char* OmsArrayUtils::convertToHex( char* buf, OmsTypeInt8 val )
{
  int i=18;
  buf[i--] = '\0'; 
  while( i > 1 )
  {
    buf[i--] = getHexCode(((char)(val & 15)));
    val = val >> 4; 
  } 
  buf[i--] = 'x'; 
  buf[i] = '0'; 
  return buf; 
}

void OmsArrayUtils::writeInt8( OmsHandle& h, OmsTypeInt4 fh, const OmsTypeInt8 val )
{
  char buf[256];
  buf[255] = '\0';  
  int off = 255; 
  OmsTypeInt8 tmp_val = val; 
  while( tmp_val > 0 )
  {
    --off; 
    buf[ off ] = 48 + char( tmp_val % 10 ); 
    tmp_val = tmp_val / 10; 
  }
  if( off == 255 )
  {
    --off; 
    buf[ off ] = '0'; 
  }
  h.dbpWriteMessageFile( fh, "%s", &buf[off] ); 
}

void OmsArrayUtils::tmemcpy( void* tgt, const void* src, OmsTypeUInt4 cnt )
{
  SAPDB_MemCopyNoCheck( tgt, src, cnt ); 
}

void OmsArrayUtils::tmemset( void* tgt, OmsTypeByte val, OmsTypeUInt4 cnt )
{
  SAPDB_MemFillNoCheck( tgt, val, cnt ); 
}

void OmsArrayUtils::tstrcat(  char* tgt, OmsTypeUInt4 len, const char* s )
{
  if( strlen( s ) < ( len - strlen( tgt ) - 1 ) )
  {
    strcat( tgt, s ); 
  }
}