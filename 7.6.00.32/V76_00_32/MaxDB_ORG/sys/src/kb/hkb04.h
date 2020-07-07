/****************************************************************************

  module      : hkb04.h

  -------------------------------------------------------------------------

  author      : JuergenA
  responsible : UweH

  special area: KB_object_handling_interface
  description : 


  last changed: 1999-06-22  15:48
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


#ifndef HKB04_H
#define HKB04_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg92.h"

#include "Oms/OMS_ContainerHandle.hpp"
#include "FileDirectory/FileDir_Types.hpp"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void
kb04CreateObjFile (tgg00_TransContext	&Trans,
                   tgg00_FileId 	    &ObjFile,
                   tsp00_Int4       	 ObjBodyLen,
                   tsp00_Int4            ObjKeyLen,
				   tsp00_Bool            bCreateSeveralObjKeyFiles);

/*!***************************************************************************
function   : kb04CreateKeyIterator
description: Creates an iterator running over a predefined key range
             
       This function creates a key iterator which runs over all objects with
			 keys between the given start and stop keys. The iterator is returned via a
			 void pointer. If the pointer to the start key  is empty and the start key len is zero
			 the iterator starts with the smallest key. If the stop key pointer is nil and the stop key len
			 is zero the iterator stops at the largest key. The iterator can be incremented resp. decremented 
       using the functions kb04NextObjFromKeyIterator resp. kb04PrevObjFromKeyIterator. 

arguments : Trans              [in/out] TransContext
            ObjFileId          [in]   Id of the file to scan
            KeyLen             [in]   common length of the start and stop key 
            pStartKey          [in]   pointer to the start key
            pLowerKey          [in]   pointer to the lower boundary of the interval
            pUpperKey          [in]   pointer to the upper boundary of the interval
            pKeyIterator       [out]  pointer to the key range iterator
            bAscendingKeyOrder [in]   Should the iterator run from the smallest entry to the largest (=true)
                                      or vice versa (=false)
******************************************************************************/ 
externCpp void
kb04CreateKeyIterator (tgg00_TransContext &Trans,
				       tgg00_FileId		  &ObjFile,
				       tsp00_Int4	       KeyLen,
				       void		          *pStartKey,
				       void		          *pLowerKey,             // PTS 1119480
				       void		          *pUpperKey,             // PTS 1119480
				       void            *&pKeyIterator,
               bool              bAscendingKeyOrder);   // PTS 1119480

/*---------------------------------------------------------------------------*/

externCpp void
kb04DeleteAllObj (tgg00_TransContext &Trans,
                  tgg00_FileId		 &ObjFile,
                  tsp00_Int4         &NumDelObj,
                  tgg92_KernelOid    &ErrorOid);

/*---------------------------------------------------------------------------*/

externCpp void
kb04DeleteObj (tgg00_TransContext &Trans,
               tgg00_FileId       &ObjFile,
               tgg92_KernelOid     Oid,
               tgg91_PageRef       WantedObjVers,
               bool                bOmsLayerReusedObj,
               bool                bContVarObject);    // PTS 1124278               


/*!***************************************************************************
function   : kb04DestroyKeyIterator
description: Releases all resources held by the iterator
             
arguments : Trans            [in/out]  TransContext
            pKeyIterator     [in]      pointer to the key range iterator
******************************************************************************/ 
externCpp void
kb04DestroyKeyIterator (tgg00_TransContext &Trans,
				        void              *&pKeyIterator);

/*!***************************************************************************
function   : kb04DropObjFile
description: Drops a complete file
             
             The complete object file including all auxiliary structures are removed
             asynchronously. The command does not consider whether all objects to be deleted
             belong to the consistent view of the current transaction. To avoid destruction of 
             consistent views of other transactions this command should be performed only if there
             exists no other open transaction.

arguments : Trans         [in/out]  TransContext
            ObjFileId     [in]      Id of the file to be deleted

******************************************************************************/                     
externCpp void
kb04DropObjFile (tgg00_TransContext    &Trans,
                 tgg00_FileId          &ObjFileId);

/*---------------------------------------------------------------------------*/

externCpp void
kb04EndConsistentTrans (tgg00_TransContext &Trans);


/*---------------------------------------------------------------------------*/

// PTS 1120478
externCpp void
kb04GetContainerId (tgg00_TransContext &Trans,
                    tgg00_FileId       &ObjFile,     
                    tgg92_KernelOid     Oid);

/*---------------------------------------------------------------------------*/

externCpp bool 
kb04GetNewFileId(tgg00_TransContext  &Trans, 
                 FileDir_FileNo      &fileno);

/*---------------------------------------------------------------------------*/

externCpp void 
kb04vStateFileversion (tgg00_TransContext &Trans,
                       tgg00_FileId&       versionTree);

/*---------------------------------------------------------------------------*/

externCpp void
kb04GetObjConsistent (tgg00_TransContext &Trans,
                      tgg00_FileId       &ObjFile,     
                      tgg92_KernelOid     Oid,
                      tsp00_Int4          ObjBodySize,     //[inp=buffer size for object] 
                      void               *pObjBody,        //[inp]
                      bool                bDoLock,         //[inp] 
                      bool                bShared,         //[inp]
                      tsp00_Int4         &ObjBodyLen,      //[out=real object length]
                      tsp00_Int4         &LogReadAccesses, //[out] PTS 1107890 TA 13/09/2000
                      tgg91_PageRef      &ResultObjVers,   //[out]
                      tgg91_TransNo      &UpdTransId);     //[out]

/*---------------------------------------------------------------------------*/

externCpp void
kb04GetObjWithKeyConsistent (tgg00_TransContext	 &Trans,
                             tgg00_FileId	       &ObjFile,
                             tsp00_Int4		        ObjBodySize,
                             void			           *pObjBody,
                             tsp00_Int4	          KeyLen,
                             void		             *pKey,
                             bool                 bDoLock,         //[inp] 
                             bool                 bShared,         //[inp]
                             tsp00_Int4          &LogReadAccesses, //[out] PTS 1107890 TA 13/09/2000
                             tgg92_KernelOid     &ResultOid,
                             tgg91_PageRef       &ResultObjVers,
                             tgg91_TransNo       &UpdTransId);     //[out]

/*---------------------------------------------------------------------------*/

/* PTS 1106284 UH 07-04-2000 new */
externCpp void
kb04IsObjHistoryUsed (tgg00_TransContext &Trans,
					  tgg00_FileId       &ObjFile,
					  tgg92_KernelOid     Oid,
					  bool                ignoreVersions,		/* PTS 1130354 */
					  bool               &bHistoryIsUsed,
					  SAPDB_UInt8        &OldestTransNo);   /* PTS 1130354 */

/*---------------------------------------------------------------------------*/

/* PTS 1001745 UH 23-11-1998 new */
externCpp void
kb04IsObjSelfLocked (tgg00_TransContext &Trans,
                     tgg00_FileId       &ObjFile,
                     tgg92_KernelOid     Oid,
                     bool               &IsSelfLocked,
                     bool                bShared);

/*---------------------------------------------------------------------------*/

externCpp void
kb04LockObj (tgg00_TransContext &Trans,
             tgg00_FileId       &ObjFile,    
             tgg92_KernelOid     Oid,
             tgg91_PageRef       WantedObjVers);

/*---------------------------------------------------------------------------*/

/* PTS 1109298 UH 2001-01-26 new */
externCpp void
kb04LockUpdObj (tgg00_TransContext &Trans,
                tgg00_FileId       &ObjFile,    
                tgg92_KernelOid     Oid,
                tgg91_PageRef       WantedObjVers);

/*---------------------------------------------------------------------------*/

externCpp void
kb04LockShareObj (tgg00_TransContext &Trans,
             tgg00_FileId       &ObjFile,    
             tgg92_KernelOid     Oid,
             tgg91_PageRef       WantedObjVers);

/*---------------------------------------------------------------------------*/


externCpp void
kb04NewObj (tgg00_TransContext &Trans,
            tgg00_FileId       &ObjFile,
            tsp00_Int4          KeyLen,
            void               *KeyColumn,
            bool                bContVarObject,    // PTS 1124278               
            tgg92_KernelOid    &ResultOid,
            tgg91_PageRef      &ResultObjVers,
            tgg91_TransNo      &UpdTransId);    

/*---------------------------------------------------------------------------*/

//externCpp void
//kb04NextObjConsistent (tgg00_TransContext &Trans,
//                       tgg00_FileId       &ObjFile,    
//                       tgg92_KernelOid    &CurrOid,
//                       bool                bWithObject,
//                       tsp00_Int4          ObjBodySize,
//                       void               *pObjBody,
//                       tsp00_Int4&        LogReadAccesses, // PTS 1107890 TA 13/09/2000
//                       tgg91_PageRef      &ResultObjVers);

/*!***************************************************************************
function   : kb04NextOidsConsistent
description: returns the next oid's starting behind the given oid 

arguments : Trans            [in/out] TransContext
            ObjFile          [in]   Id of the file to scan
            currOid          [in]   OID which was read last; NIL if first call
            noOfOid          [in/out] in: maximal number of oids which can be returned (=size of array pResultOId)
                                    out: number of oids which have been read (buckets [0..noOfOid-1] of the array
                                         pResultOId contain valid oids)
            pOid             [out]  array with OIds of the next objects in the iterator scope
            pObjVers         [out]  array with 'Log-OIds' of the objects. Later these oids can be used to read the 
                                    objects without the need to access the history chain 
            LogReadAccesses  [out]  total number of read accesses to the log to find the next consistent object

New function: PTS 1120478 

******************************************************************************/ 
externCpp void
kb04NextOidsConsistent (tgg00_TransContext    &Trans,
                       tgg00_FileId           &ObjFile,	
                       bool                    bOnlyHdrVarObj,
                       const tgg92_KernelOid  &currOid,
                       tsp00_Int4             &noOfOid,
                       tgg92_KernelOid        *pOid,
                       tgg91_PageRef          *pObjVers,
                       tsp00_Int4             &LogReadAccesses);

/*!***************************************************************************
function   : kb04NextObjFromKeyIterator
description: returns the oid's of the next largest objects from the KeyIterator starting from the given restart key 
             
      Before calling this function the iterator must have been created using the function
      kb04CreateKeyIterator.
			If the this function is called consecutively the complete scope of the iterator can be scaned.
			After the last object was delivered the iterator sets the error state in the TransactionContect
		  to e_no_next_object and returns a nil result oid.


arguments : Trans            [in/out] TransContext
            pUntypedKeyIterator [in] pointer to the key range iterator
            bInclusive       [in]   if true and if there is an object with a key matching the restart key, then
                                    the oid of this object is included in the return list. 
                                    Otherwise such an object in excluded
            KeyLen           [in]   length of the keys 
            pRestartKey      [in]   key which is used to find the position the iterator should start reading from
            noOfOid          [in/out] in: maximal number of oids which can be returned (=size of array pResultOId)
                                    out: number of oids which have been read (buckets [0..noOfOid-1] of the array
                                         pResultOId contain valid oids)
            pResultOId       [out]  array with OIds of the next objects in the iterator scope
            pObjVers         [out]  array with 'Log-OIds' of the objects. Later these oids can be used to read the 
                                    objects without the need to access the history chain 
            LogReadAccesses  [out]  total number of read accesses to the log to find the next consisten object

Interface changed: PTS 1119480 

******************************************************************************/ 
externCpp void
kb04NextObjFromKeyIterator (tgg00_TransContext &Trans,
                            void               *pUntypedKeyIterator,    
                            bool                bInclusive,   
                            tsp00_Int4          objBodySize,
                            tsp00_Int4          KeyLen,          
                            void               *pRestartKey,  
                            tsp00_Int4         &noOfOid,
                            tgg92_KernelOid    *pResultOId, 
                            tgg91_PageRef      *pObjVers,
                            tsp00_MoveObj      *pBody,
                            tsp00_Int4         &LogReadAccesses);

/*!***************************************************************************
function   : kb04PrevObjFromKeyIterator
description: returns the oid's of the next smallest objects from the KeyIterator starting from the given restart key 
             
      Before calling this function the iterator must have been created using the function
      kb04CreateKeyIterator.
			If the this function is called consecutively the complete scope of the iterator can be scaned.
			After the last object was delivered the iterator sets the error state in the TransactionContect
		  to e_no_next_object and returns a nil result oid.


arguments : Trans            [in/out] TransContext
            pUntypedKeyIterator [in] pointer to the key range iterator
            bInclusive       [in]   if true and if there is an object with a key matching the restart key, then
                                    the oid of this object is included in the return list. 
                                    Otherwise such an object in excluded
            KeyLen           [in]   length of the keys 
            pRestartKey      [in]   key which is used to find the position the iterator should start reading from
            noOfOid          [in/out] in: maximal number of oids which can be returned (=size of array pResultOId)
                                    out: number of oids which have been read (buckets [0..noOfOid-1] of the array
                                         pResultOId contain valid oids)
            pResultOId       [out]  array with OIds of the next objects in the iterator scope
            pObjVers         [out]  array with 'Log-OIds' of the objects. Later these oids can be used to read the 
                                    objects without the need to access the history chain 
            LogReadAccesses  [out]  total number of read accesses to the log to find the next consisten object

New function since: PTS 1119480 

******************************************************************************/ 
externCpp void
kb04PrevObjFromKeyIterator (tgg00_TransContext &Trans,
                            void               *pUntypedKeyIterator,    
                            bool                bInclusive,   
                            tsp00_Int4          objBodySize,
                            tsp00_Int4          KeyLen,          
                            void               *pRestartKey,  
                            tsp00_Int4         &noOfOid,
                            tgg92_KernelOid    *pResultOId, 
                            tgg91_PageRef      *pObjVers,
                            tsp00_MoveObj      *pBody,
                            tsp00_Int4         &LogReadAccesses);

/*---------------------------------------------------------------------------*/

externCpp void
kb04StartConsistentTrans (tgg00_TransContext &Trans,
						  bool                isOmsVersion); /* PTS 1130354 */


/*---------------------------------------------------------------------------*/

externCpp void
kb04UnlockObj (tgg00_TransContext &Trans,
               tgg00_FileId       &ObjFile, 
               tgg92_KernelOid     Oid);

/*---------------------------------------------------------------------------*/

externCpp void
kb04UnlockSharedObj (tgg00_TransContext &Trans,
                     tgg00_FileId       &ObjFile, 
                     tgg92_KernelOid     Oid);

/*---------------------------------------------------------------------------*/


externCpp void
kb04UpdateObj (tgg00_TransContext &Trans,
               tgg00_FileId       &ObjFile,
               tgg92_KernelOid     Oid,
               tgg91_PageRef      &ObjVers,
               tsp00_Int4          NewBodySize,
               void               *pNewBody,
               bool                contObj,   // PTS 1124278
               bool                bOmsLayerReusedObj);   // PTS 1127661

/*---------------------------------------------------------------------------*/

externCpp void
kb04SetTransactionComment (tgg00_TransContext &Trans,
                            tsp00_Int2         commentLength,
                            SAPDB_Char*        pComment); 

/*---------------------------------------------------------------------------*/

externCpp void
kb04LockContainer(tgg00_TransContext &Trans,
                  const tgg00_FileId &containerId);

/*---------------------------------------------------------------------------*/

#endif /* HKB04_H */
