/*!**************************************************************************

  module      : hbd90.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: 
  description : Declaration of exported functions of modul vbd90.cpp


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

#ifndef HBD90_H
#define HBD90_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GGG92_H
#   include "ggg92.h"
#endif
#ifndef GBD00_H
#   include "gbd00.h"
#endif
#ifndef GBD05_H
#   include "gbd05.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef LOG_TYPES_HPP
#   include "Logging/Log_Types.hpp"
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif
#ifndef LVC_ABSTRACTOBJKEYITERATOR_HPP
#   include "liveCache/LVC_AbstractObjKeyIterator.hpp"
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void 
bd90TransEnd(tgg00_TransContext   &Trans);

/*---------------------------------------------------------------------------*/

externCpp bool
bd90GetKeyedFile (tgg00_TransContext    &Trans,
             FileDir_OmsKey             &fd_key_smart,
             const FileDir_FileNo       &fileId,
             Msg_List                   &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp bool
bd90GetFile(tgg00_TransContext      &Trans,
            FileDir_Oms             &fd_smart,
            const FileDir_FileNo    &fileId,
            bool                     doShareLock,
            int                      line,
            Msg_List                &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
bd90CreateObjFile (cbd05_Current         &Current,
                   tsp00_Int4             ObjBodyLen,
                   tsp00_Int4             ObjKeyLen,
                   tsp00_Int4             NumObjKeyFiles,
                   cbd910_ObjDataNode    &NptrsRoot,
                   Converter_Version     &converterVersion,
                   Msg_List              &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
bd90Rollforward_CreateObjFile (cbd05_Current         &Current,
                               tsp00_Int4             ObjBodyLen,
                               tsp00_Int4             ObjKeyLen,
                               tsp00_Int4             NumObjKeyFiles,
                               Data_PageNo            rootPage,
                               cbd910_ObjDataNode    &NptrsRoot,
                               Converter_Version     &converterVersion,
                               Msg_List              &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void 
bd90DelAllObj (cbd05_Current        &Current,           //[inp\out]
               tsp00_Int4           &NumDelObj,         //[out]
               tgg92_KernelOid      &ErrorOid,
               Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void 
bd90DelObj (cbd05_Current         &Current,             //[inp\out]
            tgg92_KernelOid       &OId,                 //[inp\out] 
            tgg91_PageRef          WantedObjVers,       //[inp]
            SAPDB_Int1             contVarObj,          //[inp] 
            bool                   bOmsLayerReusedObj,  //[inp]
            Msg_List              &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
bd90DropObjFile (cbd05_Current   &Current,
                 Msg_List        &ErrorList);


/*---------------------------------------------------------------------------*/

externC void
bd90GetNumChains (tsp00_Int4  &NumObjPageChains);
 
/*----------------------------------------------------------------------------*/
// PTS 1120478
externCpp void
bd90GetContainerId (cbd05_Current     &Current,            //[in\out]
                    tgg92_KernelOid    OId);               //[in]

/*----------------------------------------------------------------------------*/

externCpp void
bd90GetObjConsistent (cbd05_Current        &Current,            //[inp\out] 
                      tgg92_KernelOid      &OId,                //[inp\out] 
                      tsp00_Int4            ExtObjBodySize,     //[inp= buffersize for object]
                      void                 *pExtObjBody,        //[inp]
                      tsp00_Int4            HashKeySize,        //[inp]
                      void                 *pHashKey,           //[inp]  
                      tsp00_Int4           &ObjBodySize,        //[out=length of the object] 
                      tsp00_Int4           &KeyLen,             //[out]
                      tsp00_Byte*          *pKey,               //[out]
                      tgg91_PageRef        &ResultObjVers,      //[out]
                      tgg91_TransNo        &UpdTransId,         //[out] 
                      tsp00_Int4           &LogReadAccesses,    //[out] 
                      Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
bd90GetObjConsistentWithLock (cbd05_Current        &Current,            //[inp\out] 
                              tgg92_KernelOid      &OId,                //[inp\out] 
                              tsp00_Int4            ExtObjBodySize,     //[inp= buffersize for object]
                              void                 *pExtObjBody,        //[inp]
                              tsp00_Int4            HashKeySize,        //[inp]
                              void                 *pHashKey,           //[inp]  
                              const bool            bShareLock,         //[inp]   
                              tsp00_Int4           &ObjBodySize,        //[out=length of the object] 
                              tsp00_Int4           &KeyLen,             //[out]
                              tsp00_Byte*          *pKey,               //[out]
                              tgg91_PageRef        &ResultObjVers,      //[out]
                              tgg91_TransNo        &UpdTransId,         //[out] 
                              tsp00_Int4           &LogReadAccesses,    //[out] 
                              Msg_List             &ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void
bd90GetObjFileStatistics (cbd05_Current   &Current,
                          tsp00_Int4         &NumAllPages,
                          tsp00_Int4         &NumFreePages,
                          tsp00_Int4         &NumEmptyPages,
                          tsp00_Int4         &NumKeyLeaves,       
                          tsp00_Int4         &Occupancy);

/*---------------------------------------------------------------------------*/

externCpp void
bd90IsObjHistoryUsed (cbd05_Current      &Current,
                      tgg92_KernelOid    &OId,
                      bool                ignoreVersions,   /* PTS 1130354 */
                      bool               &bHistoryIsUsed,
                      SAPDB_UInt8        &OldestTransNo,    /* PTS 1130354 */
                      Msg_List           &ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void
bd90IsObjSelfLocked (cbd05_Current&    Current,
                     tgg92_KernelOid&  OId,
					 bool              bShared,
                     bool&             IsSelfLocked,
                     Msg_List&         ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void
bd90LockObj (cbd05_Current&      Current,
             tgg92_KernelOid&    OId,
             tgg91_PageRef&      WantedObjVers,    
             Msg_List&           ErrorList);

    
/*----------------------------------------------------------------------------*/

externCpp void
bd90LockUpdObj (cbd05_Current&        Current,
                tgg92_KernelOid&      OId,
                tgg91_PageRef&        WantedObjVers,    
                Msg_List&             ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void
bd90LockShareObj (cbd05_Current&      Current,
                  tgg92_KernelOid&    OId,
                  tgg91_PageRef&      WantedObjVers,   
                  Msg_List&           ErrorList);
    
/*----------------------------------------------------------------------------*/

externCpp void 
bd90NewObj (cbd05_Current        &Current,                  // [in\out]
            tsp00_Int4            KeyLen,                   // [in]
            void                 *pKey,                     // [in]
            SAPDB_Int1            bcontObj,
            tgg92_KernelOid      &ResultOId,                // [out]
            tgg91_PageRef        &ResultObjVers,            // [out]
            Msg_List             &ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void 
bd90NextObjConsistent (cbd05_Current        &Current,
                       tgg92_KernelOid      &OId,
                       bool                  bWithObject,
                       tsp00_Int4            ExtObjBodySize,
                       void                 *pExtObjBody, 
                       tsp00_Int4           &ObjBodySize,   // [out=length of the object] 
                       tgg91_PageRef        &ObjVers,
                       tsp00_Int4           &AllLogReadAccesses,  
                       Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void
bd90NextObjConsistentOnThisPage (cbd05_Current           &Current,
                                 FileDir_Oms             &fd_smart,
                                 tgg92_KernelOid         &OId,
                                 tgg91_PageRef           &ResultObjVers,
                                 tgg91_PageRef           &ObjHistRef,             // PTS 1120478 FF
                                 bool                     bWithObject,
                                 bool                     bOnlyHdrVarObjects,     // used fo omsVarObjIter
                                 tsp00_Int4               ExtObjBodySize,
                                 void                    *pExtObjBody,
                                 cbd910_PrimObjDataNode  &NptrsPage,
                                 tsp00_Int4              &ObjBodySize,             //[out=length of the object]
                                 tsp00_Bool              &bNextObjIsFound,
                                 tsp00_Int4              &LogReadAccessesPerPage,  //[out] PTS 1107819 AK 12/09/2000
                                 Msg_List                &ErrorList);

/*!***************************************************************************
function   : bd90NextOidsConsistent
description: see kb04NextOidsConsistent
new function: PTS 1120478 
*****************************************************************************/
externCpp void
bd90NextOidsConsistent (cbd05_Current          &Current,
                        bool                    bOnlyHdrVarObj,
                        const tgg92_KernelOid  &CurrOid,
                        tsp00_Int4             &noOfOid,
                        tgg92_KernelOid        *pOid,
                        tgg91_PageRef          *pObjHistRef,
                        tsp00_Int4             &AllLogReadAccesses,  
                        Msg_List               &ErrorList);

/*!---------------------------------------------------------------------------
function    : bd90NextObjFromKeyIterator
description : see kb04NextObjFromKeyIterator
Interface changed: PTS 1119480 
----------------------------------------------------------------------------*/
externCpp void
bd90NextObjFromKeyIterator (tgg00_TransContext          &Trans,
                            LVC_AbstractObjKeyIterator  &KeyIterator,    
                            bool                         bInclusive,  
                            tsp00_Int4                   objBodySize,
                            tsp00_Int4                   KeyLen,      
                            void                        *pRestartKey, 
                            tsp00_Int4                  &noOfOid,
                            tgg92_KernelOid             *pResultOId, 
                            tgg91_PageRef               *pResultObjVers,
                            tsp00_MoveObj               *pBody,
                            tsp00_Int4                  &LogReadAccesses,
                            Msg_List                    &ErrorList);

/*!---------------------------------------------------------------------------
function    : bd90NextObjFromKeyIterator
description : see kb04NextObjFromKeyIterator
New function since: PTS 1119480 
----------------------------------------------------------------------------*/
externCpp void
bd90PrevObjFromKeyIterator (tgg00_TransContext          &Trans,
                            LVC_AbstractObjKeyIterator  &KeyIterator,    
                            bool                         bInclusive, 
                            tsp00_Int4                   objBodySize,
                            tsp00_Int4                   KeyLen,      
                            void                        *pRestartKey, 
                            tsp00_Int4                  &noOfOid,
                            tgg92_KernelOid             *pResultOId, 
                            tgg91_PageRef               *pResultObjVers,
                            tsp00_MoveObj               *pBody,
                            tsp00_Int4                  &LogReadAccesses,
                            Msg_List                    &ErrorList);

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 
bd90MaxNumChainsPerContainer ();

/*---------------------------------------------------------------------------*/

externCpp void 
bd90ReadAhead (cbd05_Current  &Current);

/*---------------------------------------------------------------------------*/

externC void
bd90ReleaseObjFile (cbd05_Current &Current);
   
/*----------------------------------------------------------------------------*/

externC void
bd90RemoveObjFile (tgg00_TransContext &Trans,
                   FileDir_FileNo &ObjFileNo);

/*----------------------------------------------------------------------------*/

externCpp void
bd90ReuseObj (cbd05_Current        &Current,
              tgg92_KernelOid      &OId,
              tgg91_PageRef        &ResultObjVers,
              tsp00_Int4            KeySize,
              void                 *pKey,
              bool                 &bTryAgain,
              Msg_List             &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void 
bd90RollbackObj (cbd05_Current        &Current,            //[inp\out]
                 tgg00_MessType_Enum   RollbackType,       //[inp]
                 tgg92_KernelOid      &OId,                //[inp]
                 const FileDir_FileNo &ObjFileNo,
                 tgg00_ObjState        NewState,           //[inp]
                 tgg91_PageRef         BeforeObjRef,       //[inp]
                 tgg91_TransNo         BeforeObjUpdVers,   //[inp]
                 tsp00_Int4            BeforeObjSize,      //[inp]
                 void                 *pBeforeObj,         //[inp]
                 SAPDB_Int1            contVarObject,
                 bool                  bOmsLayerReusedObj,
                 Msg_List             &ErrorList);

/*----------------------------------------------------------------------------*/

externCpp void
bd90UnlockObj (cbd05_Current       &Current,
         tgg92_KernelOid           &OId,    
         bool                       bSharedLock);    

/*!***************************************************************************
function   : bd90UpdateObj

description: Replaces the current image of an object by a new one

arguments  : Current          [in/out]  handle of the task context and the fileid
             OId              [in]      OId of the frame containing the object to be updated
             ObjVers          [in/out]  Version of the object which shall be updated. Only if the given Version corresponds to the
                                        Version of the current image of the object the object can be updated otherwise the error 
                                        e_object_dirty is returned
                                        After this operation ObjVers contains the version of the updated object image
             NewObjBodyLen    [in]     Length of the new object image replacing the old image of the object
             pNewObjBody      [in]     Pointer to the new object image
             RedoEntrySequence [in]    if RedoEntrySequence is invalid this update is a normal (online) update
                                       but if it has a valid value this is a rollforward of an update operation during a restart 
                                       which differs from a normal update by a fact that no history data is written and 
                                       by a differnt check whether the update is allowed
             RedoUpdTransNo   [in]     in case that that the operation is a rollforward of an update this is 
                                       update transno of the transaction which originally updated the object othwerwise
                                       this parameter has no meaning
             contObj          [in]     If the object is a var-object, then this flag indicates, whether the
                                       object is a 'head'-object (false) or a 'continuation'-object (true)
             bOmsLayerReusedObj [in]   The oid of the object has been reused from another object, this means
                                       the generation of the object, to be updated, is different from the
                                       generation which has been read from the kernel.
******************************************************************************/

externCpp  void 
bd90UpdateObj (cbd05_Current            &Current,
               tgg92_KernelOid          &OId,
               tgg91_PageRef            &ObjVers,
               tsp00_Int4                NewObjBodyLen,
               void                     *pNewObjBody,
               const Log_EntrySequence  &RedoEntrySequence,
               const tgg91_TransNo      &RedoUpdTransNo,
               SAPDB_Int1                contObj,             // PTS 1124278
               bool                      bOmsLayerReusedObj,  // PTS 1127661
               bool                      bForRollforward,   
               Msg_List                 &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp void 
bd90LockObjContainer( tgg00_TransContext  &Trans
                     ,tgg00_Surrogate     &TableId
                     ,tgg00_LockReqMode   &ReqMode );

/*---------------------------------------------------------------------------*/
externCpp void 
bd902FreeFreePageListCache(tgg00_TransContext   &Trans);

#endif /* HBD90_H */
