/*!**************************************************************************

  module      : hbd902.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: 
  description : Declaration of exported functions of modul vbd902.cpp


  version     : 7.2.0.0
  last changed: 1999-06-22  15:47
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


*****************************************************************************/

#ifndef HBD902_H
#define HBD902_H

#include "SAPDBCommon/SAPDB_C_Cplusplus.h"

class  FileDir_FileNo;
class  FileDir_Oms;
struct tgg00_TransContext;

/*---------------------------------------------------------------------------*/

externCpp void 
bd902LockShareSchema(tgg00_TransContext   &Trans,
                     FileDir_Oms          &fd_smart,
                     const FileDir_FileNo &fileId );

/*---------------------------------------------------------------------------*/

externCpp void 
bd902RemoveLockShareSchemaFlag(tgg00_TransContext   &Trans);

/*---------------------------------------------------------------------------*/

externCpp void 
bd902FreeLockShareSchemaCache(tgg00_TransContext   &Trans);

/*---------------------------------------------------------------------------*/

#endif /* HBD902_H */
