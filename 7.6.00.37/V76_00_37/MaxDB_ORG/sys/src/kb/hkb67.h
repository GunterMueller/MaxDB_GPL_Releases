/*!***************************************************************************

  module      : vkb67.cpp
  author      : JuergenA, UweH
  responsible : UweH
  special area: KB_object_handling_interface
  last changed: 2000-12-20
  Copyright (c) 2000-2005 SAP AG
  see also    :
  description : 



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


*****************************************************************************/

#ifndef HKB67_H
#define HKB67_H

#include "Logging/Log_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"


/*---------------------------------------------------------------------------*/
externCpp void
kb67CreateContainer (tgg00_TransContext   &Trans,
                     tgg00_FileId         &FileId,
                     tbd_node_ptrs        &Nptrs,
                     Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
kb67DelUpdObj (tgg00_TransContext   &Trans,
               tgg00_MessType_Enum   WantedMessType,
               tgg00_FileId         &ObjFile,
               tgg92_KernelOid       Oid,
               tsp00_Int4            ObjKeySize,
               tgg91_PageRef         WantedObjVers,
               tgg92_ObjHeader      &ObjHeader,     
               tsp00_Int4            ObjBodySize,
               void                 *pObjBody,
               SAPDB_Int1            contVarObject,        
               bool                  bOmsLayerReusedObj,  
               Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/
externCpp void
kb67DropContainer (tgg00_TransContext   &Trans,
                   FileDir_FileNo       &contHndl,
                   Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
kb67DropObjHistory (tgg00_TransContext    &trans,
                    const tgg92_ObjHeader &objHeader,
                    const tgg92_KernelOid &oid,
                    Msg_List              &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
kb67GetBeforeImage (tgg00_TransContext   &Trans,     
                    tgg92_KernelOid       Oid,
                    tgg91_PageRef         FirstBeforeRef,
                    bool                  bWithObject,
                    tsp00_Int4            KeyLen,
                    tsp00_Int4           &ObjBodyLen,
                    tsp00_Int4            ObjBodySize,
                    void                 *pObjBody,
                    tgg00_ObjFrameVers   &FrameVers,
                    tgg91_PageRef        &ResultObjVers,
                    tgg91_PageRef        &ObjHistRef,         
                    tgg91_TransNo        &HistUpdTransId,         // PTS 1137480
                    tsp00_Int4           &LogReadAccesses,
                    Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
kb67GetObjConsistent(tgg00_TransContext   &Trans,
                     tgg92_KernelOid       Oid,
                     tgg00_ObjState        ObjState,
                     tgg00_ObjTransInfo   &ObjTransInfo,
                     bool                  bWithObj,
                     tsp00_Int4            KeyLen,
                     tsp00_Int4           &ObjBodyLen,
                     tsp00_Int4            ObjBodySize,
                     void                 *pObjBody,
                     tgg00_ObjFrameVers   &FrameVers,
                     tgg91_PageRef        &ResultObjVers,
                     tgg91_PageRef        &ObjHistRef,        
                     tgg91_TransNo        &HistUpdTransId,         // PTS 1137480
                     tsp00_Int4           &LogReadAccesses,
                     Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp bool
kb67IsTransActive(tgg92_ObjHeader &ObjHeader);

/*---------------------------------------------------------------------------*/

/* PTS 1106284 UH 07-04-2000 new */
externCpp bool
kb67IsObjHistoryUsed (tgg00_TransContext &Trans,
                      tgg92_KernelOid    &Oid,
                      tgg00_ObjTransInfo &ObjTransInfo,
                      bool                ignoreVersions,       /* PTS 1130354 */
                      SAPDB_UInt8         &OldestTransNo);      /* PTS 1130354 */

/*---------------------------------------------------------------------------*/

externCpp void
kb67LockObj (tgg00_TransContext  &Trans,
             tgg00_FileId        &ObjFile,
             tgg92_KernelOid      Oid,
             tgg91_PageRef        WantedObjVers,
             tgg00_ObjState_Enum  ObjState,
             tgg00_ObjTransInfo  &ObjTransInfo);

/*---------------------------------------------------------------------------*/

externCpp void
kb67LockShareObj (tgg00_TransContext  &Trans,
                  tgg00_FileId        &ObjFile,
                  tgg92_KernelOid      Oid,
                  tgg91_PageRef       WantedObjVers,
                  tgg00_ObjState_Enum  ObjState,
                  tgg00_ObjTransInfo  &ObjTransInfo);

/*-----------------------------------------------------------------------------------*/

externCpp void
kb67LockUpdateObj (tgg00_TransContext   &Trans,
                   tgg00_FileId         &ObjFile,
                   tgg92_KernelOid       Oid,
                   tgg91_PageRef         WantedObjVers,
                   tgg92_ObjHeader      &ObjHeader,
                   tgg00_ObjTransInfo   &ObjTransInfo,
                   Msg_List             &ErrorList);
    
/*---------------------------------------------------------------------------*/

externCpp void
kb67NewObj (tgg00_TransContext   &Trans,
            tgg00_FileId         &ObjFile,
            tgg92_KernelOid       Oid,
            tgg92_ObjHeader      &ObjHeader,     
            tsp00_Int4            ObjBodySize,
            tsp00_Int4            ObjKeySize,
            SAPDB_Int1            bcontObj,
            Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp tsp00_Bool
kb67RollforwardObj(tgg92_ObjHeader          &ObjHeader,
                   const tgg91_TransNo      &RedoUpdTransNo, 
                   const Log_EntrySequence  &RedoEntrySequence,
                   const tgg91_PageRef      &ObjBeforeRef);

/*---------------------------------------------------------------------------*/

externCpp void
kb67UpdObj (tgg00_TransContext       &Trans,
            tgg00_FileId             &ObjFile,
            tgg92_KernelOid           OId,
            tsp00_Int4                ObjKeySize,
            tgg91_PageRef             ObjVers,
            tgg92_ObjHeader          &ObjHeader,     
            tsp00_Int4                ObjBodySize,
            void                     *pObjBody,
            const Log_EntrySequence  &RedoEntrySequence,
            const tgg91_TransNo      &RedoUpdTransNo,
            SAPDB_Int1                contVarObject,      
            bool                      bOmsLayerReusedObj,
            Msg_List                 &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
kb67WriteDeleteObjectAfterImage (tgg00_TransContext    &trans,
                                 FileDir_FileNo        &contHndl,
                                 const tgg92_KernelOid &oid,
                                 const tgg91_PageRef   &version,
                                 SAPDB_Int1             contVarObj,          
                                 bool                   bOmsLayerReusedObj,
                                 Msg_List              &ErrorList);


#endif /*HKB67_H*/
