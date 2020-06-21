/*!**************************************************************************

  module:       hbd22.h
  special area: OMS_KeyManagement
  responsible:  UweH
  last changed: 1999-11-05  17:00
  version:      7.2.0.0
  see also: 
  copyright:    Copyright (c) 1998-2005 SAP AG
  description:  Map an object key to an OID, there is no consistent-check done.



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


#ifndef HBD22_H
#define HBD22_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GGG92_H
#   include "ggg92.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif

#ifndef FILEDIR_OMSKEY_HPP
#   include "FileDirectory/FileDir_OmsKey.hpp"  
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

externCpp void 
bd22CreateObjKeyFiles (tgg00_TransContext    &Trans,
                       FileDir_FileNo        &Fileno,
                       tsp00_Int4             NumObjKeyFiles,
                       const LVC_FileNoArray &FileNoArray,
                       Msg_List              &ErrorList);

/*-----------------------------------------------------------------------------------*/

externCpp tsp00_Int4
bd22ObjKeyFileIndex (const tsp00_Int4      KeyLen,
                     void                 *pKey,
                     const tsp00_Uint4     NumHashValues);

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22Rollforward_CreateObjKeyFiles (tgg00_TransContext    &Trans,
                                   FileDir_FileNo        &Fileno,
                                   tsp00_Int4             NumObjKeyFiles,
                                   const LVC_FileNoArray &FileNoArray,
                                   Msg_List              &ErrorList);
/*-----------------------------------------------------------------------------------*/
/* PTS 1107153 AK 17/07/2000 */

externCpp void
bd22GetLeaveCountOfObjKeyFiles (tgg00_TransContext        &Trans,
							    const FileDir_FileNo      &Fileno,
                                FileDir_Oms               &fd_key_smart,
							    tsp00_Int4                &NumKeyLeaves,
                                Msg_List                  &ErrorList);

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22GetOId (tgg00_TransContext  &Trans,
            FileDir_FileNo      &Fileno,
			tsp00_Int4           KeyLen,
			void                *pKey,
			tgg92_KernelOid     &OId,
            Msg_List            &ErrorList);

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22GetOIdForVerify (tgg00_TransContext  &Trans,
                     FileDir_FileNo      &Fileno,
                     tsp00_Int4           KeyLen,
                     void                *pKey,
                     tgg92_KernelOid     &OId,
                     Msg_List            &ErrorList);


/*!***************************************************************************
function   : bd22GetNextOId
description: delivers the next OId which is in the interval between StartKey and Stopkey

             The OId of the object with the smallest key (with respect to a memcmp) in the 
			 interval StartKey and Stopkey is returned
             The parameter bIncludeStartKey  defines whether the lower interval 
			 border is included in the interval or not
             
arguments : Trans            [in/out]  TransContext
            ObjFileNo        [in]      number of the object file 
            KeyLen           [in]      common key length
            pStartKey        [in]      pointer to the key defining the lower boundary of the interval
            bIncludeStartKey [in]      defines whether the lower boundary is included in the interval or not
            pStopKey         [in]      pointer to the key defining the lower boundary of the interval
            pResultKey       [inp]     pointer to the memory to where the result key has to be copied
            ResultOId        [out]     resulting OId
******************************************************************************
externCpp void 
bd22GetNextOId (tgg00_TransContext  &Trans,
				tbd90_ObjFileNo     &ObjFileNo,
				tsp00_Int4           KeyLen,
				void                *pStartKey,
				SAPDB_Bool           bIncludeStartKey, 
				void                *pStopKey,            
				void                *pResultKey,  
				tgg92_KernelOid     &ResultOId);
*** */
/*-----------------------------------------------------------------------------------*/
externCpp void
bd22InsertOId (tgg00_TransContext &Trans,
               FileDir_FileNo     &Fileno,
			   tsp00_Int4          KeyLen,
			   void               *pKey,
			   tgg92_KernelOid     OId,
			   tgg92_KernelOid    &FoundOId,
               Msg_List           &ErrorList);

/*-----------------------------------------------------------------------------------*/

externCpp void 
bd22InsertOIdReCreate (tgg00_TransContext &Trans,
                       FileDir_FileNo     &Fileno,
                       tsp00_Int4          KeyLen,
                       void               *pKey,
                       tgg92_KernelOid     OId,
                       tgg92_KernelOid    &FoundOId,
                       Msg_List           &ErrorList);

/*-----------------------------------------------------------------------------------*/

externCpp void
bd22ReleaseObjKeyFiles (tgg00_TransContext     &Trans,
						FileDir_FileNo         &Fileno,
                        FileDir_Oms            &fd_key_smart,
                        Msg_List               &ErrorList);

/*-----------------------------------------------------------------------------------*/
externCpp void 
bd22RemoveOid (tgg00_TransContext     &Trans,
               FileDir_Oms            &fd_key_smart,
               tsp00_Int4              KeyLen,
			   void                   *pKey,
               tgg92_KernelOid         Oid,
               Msg_List               &ErrorList);

/*---------------------------------------------------------------------------*/

#endif /*HBD22_H*/
