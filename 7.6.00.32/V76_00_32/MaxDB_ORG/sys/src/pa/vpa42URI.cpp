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
#include <string.h>
#include "SAPDB/RunTime/Communication/RTEComm_URIBuilder.hpp"
#include "vpa40DBC.h"

extern "C" {

char* pa42GetURI (UCHAR *servernode, sqldbname serverdb, char encrypt)
{
  RTEComm_URIBuilder::URIBuildRC rc;
  RTEComm_URIBuilder dbURI;
  Msg_List msgList;
  char *p;

  rc = dbURI.BuildDatabaseURI ((SAPDB_UTF8 const *) serverdb, (SAPDB_Char const *)servernode,
                               msgList, 1);

  if (rc != RTEComm_URIBuilder::NoError)
    return NULL;

  p = (char*) malloc (dbURI.GetURILength()+1);

  if (p != NULL)
    strcpy (p, dbURI.GetURI());

  return p;

}

}
