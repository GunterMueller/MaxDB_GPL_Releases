/*



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





*/
#if defined(IMF)
#include "livecache/lctest/lctesterrors.h"
#else
#include "lctesterrors.h" // nocheck
#endif
#include "lcbasetypes.h" // nocheck
#include "livecachebase.h" // nocheck
#include <string.h>
#include <stdio.h>


void ThrowError(int e) {
  ThrowError(e, -1, -1);
}

void ThrowError(int e, int val1, int val2) {
  char msg[256];
  msg[0] = 0;
  switch (e) {
  case e_NEW_FAILED :
    strcpy(msg, "operator new failed");
    break;
  case e_WRONG_LIST_OID :  
    strcpy(msg, "wrong list oid found");
    break;
  case e_WRONG_VAR_OID :
    strcpy(msg, "wrong var oid found");
    break;
  case e_WRONG_VAR_OBJ_CONTENT :
    strcpy(msg, "wrong var obj content found");
    break;
  case e_CONS_VIEW_ERRO_OID_FOUND :
    strcpy(msg, "error oid in new consisten view found");
    break;
  case e_ABORTED :
    strcpy(msg, "test aborted");
    break;
  case e_INSERTED_KEY_NOT_FOUND :
    strcpy(msg, "inserted key not found");
    break;
  case e_SELF_VERIFY_FAILED :
    strcpy(msg, "self verify failed");
    break;
  case e_VERIFY_FAILED :
    strcpy(msg, "verify failed");
    break;
  case e_WRONG_VAR_OBJECT :
    strcpy(msg, "wrong var obj found");
    break;
  case e_WRONG_LIST_SUM :
    strcpy(msg, "wrong list sum found");
    break;
  case e_WRONG_SUM :
    strcpy(msg, "wrong sum found");
    break;
  case e_DIFF_ITER_KEY :
    strcpy(msg, "difference between iterator and key access");
    break;
  case e_DIFF_ITER_LIST_COUNT :
    strcpy(msg, "difference between iterator and list count");
    break;
  case e_WRONG_PARAM : 
    strcpy(msg, "wrong parameter");
    break;
  case e_SQL_ERROR :
    strcpy (msg, "sql error");  
    break;
  case e_SCHEMA_DROPPED :
    strcpy(msg, "schema dropped");
    break;
  case e_DROP_IN_PROGRESS :
    strcpy(msg, "drop schema in progress");
    break;
  case e_TOO_NEW_OBJECT :
    strcpy(msg, "too new object found");
    break;
  case e_CLEANUP_NOT_CALLED :
    strcpy(msg, "omsCleanUp not called");
    break;
  case e_PACKED_ERROR :
    strcpy(msg, "OmsTypePacked_8_3 conversion error");
    break;
  case e_WRONG_KEY_RANGE_ITER :
    strcpy(msg, "unexpected key found in key range iter");
    break;
  case e_ALLOCATOR_ERROR :
    strcpy(msg, "basic allocator test error");
    break;
  case e_ALREADY_IN_SCHEMA_DIR :
    strcpy(msg, "already in schema directory");
    break;
  default :
    val1 = -1;
    sprintf(msg, "Unknown Error %d", e);
  }
  if (val1 != -1) {
    strcat(msg, " : ");
    char val_c[30];
    sprintf(val_c, ",%d", val1);
    strcat(msg, val_c);
    if (val2 != -1) {
      sprintf(val_c,",%d", val2);
      strcat(msg, ", ");
      strcat(msg, val_c);
    }
  }
  throw DbpError(e, msg);
};