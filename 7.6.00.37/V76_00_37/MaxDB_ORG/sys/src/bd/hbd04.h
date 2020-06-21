/*!**************************************************************************

  module      : hbd04.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: kb-bd interface for the bd-funtions of the liveCache
  description : BD FileSystemInterface4: Operations for processing object files ..


  version     : 7.6.0.0
  last changed: 2003-10-02  15:53
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

#ifndef HBD04_H
#define HBD04_H


/*****************************************************
 *     MD5: '\321\206\245\336<\016\310\273\203\011\355\230\367xf\324'
 *****************************************************/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GGG91_H
#   include "ggg91.h"
#endif
#ifndef GGG92_H
#   include "ggg92.h"
#endif
#ifndef GSP00_H
#   include "gsp00.h"
#endif
#ifndef GGG00_H
#   include "ggg00.h"
#endif
#ifndef LOG_TYPES_HPP
#   include "Logging/Log_Types.hpp"
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif
#ifndef FILEDIR_FILENO_HPP
#   include "FileDirectory/FileDir_FileNo.hpp"
#endif
#ifndef FILEDIR_TYPES_HPP
#   include "FileDirectory/FileDir_Types.hpp"
#endif
#ifndef DATA_TYPES_HPP
#   include "DataAccess/Data_Types.hpp"
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!***************************************************************************
  Notes: Conventions

  description: input and output parameter 
  
               The parameter Trans of the type TransContext is called an input
               parameter in the following text. However, it contains also an important
               output parameter which is trError_gg00 defining the error state.
*/ 

/*!***************************************************************************
  Chapter: Interface Routines
 ****************************************************************************/

/*!***************************************************************************
function   : bd04CreateKeyIterator
description: Creates an iterator running over a predefined key range
             
       This function creates a key iterator which runs over all objects with
			 keys between the given start and stop keys. The iterator is returned via a
			 void pointer. If the pointer to the start key  is empty and the start key len is zero
			 the iterator starts with the smallest key. If the stop key pointer is nil and the stop key len
			 is zero the iterator stops at the largest key. The iterator can be incremented resp. decremented 
       using the functions kb04NextObjFromKeyIterator resp. kb04PrevObjFromKeyIterator. 

arguments : Trans              [in/out] TransContext
            ObjFile            [in]   Id of the file to scan
            KeyLen             [in]   common length of the start and stop key 
            pStartKey          [in]   pointer to the start key
            pLowerKey          [in]   pointer to the lower boundary of the interval
            pUpperKey          [in]   pointer to the upper boundary of the interval
            pKeyIterator       [out]  pointer to the key range iterator
            bAscendingKeyOrder [in]   Should the iterator run from the smallest entry to the largest (=true)
                                      or vice versa (=false)
            ErrorList          [in/out] list to hold error describing information
******************************************************************************/ 
externCpp void
bd04CreateKeyIterator (tgg00_TransContext &Trans,
                       tgg00_FileId       &ObjFile,
                       tsp00_Int4          KeyLen,
                       void               *pStartKey,
                       void               *pLowerKey,             
                       void               *pUpperKey,             
                       void               *&pKeyIterator,
                       bool                bAscendingKeyOrder,   
                       Msg_List           &ErrorList);

/*!***************************************************************************
function   : bd04CreateObjFile
description: Creates an object file

             A complete object file including all auxiliary structures is created.
             If the input ObjKeyLen is larger than zero a key container (key tree)
             is formed too.

arguments  : Trans      [in]                TransContext
             ObjFileId  [in,out]            Id of the file to create. In the input file id two parameters
                                            have to be set already the objfileno and the objfiletype all other
                                            members of the file id are set by the function call 
             ObjBodyLen [in]                Length of the objects to be stored in the file
             ObjKeyLen  [in]                Length of the object keys
             bCreateSeveralObjKeyFiles [in] in case that keys are defined for this file (i.e. ObjKeyLen>0)
                                            false means that exactly one key file should be created otherwise
                                            keys should be distributet over more than one key file to allow
                                            a better parallel access to the keys
             bCreateFileIsOnline   [in]     if this is true the create file takes place during the normal online work,
                                            that means that the system can choose the page numbers of the root and subroot
                                            page free, however if the function is called during a restart the file has to 
                                            be recreated with exactly the same root and subroot pages as it was created 
                                            online. These page numbers are given via the next parameter.
             RootPageNoArray      [in]      if the previous parameter bCreateFileIsOnline is false this array conatins
                                            the page numbers which have to be used as root and subroots for the new file
                                  [out]     if the previous parameter bCreateFileIsOnline is true this is an out parameter which 
                                            returns the page numbers of the root and subroots which have to be written to the log
******************************************************************************/
externC void
bd04CreateObjFile (tgg00_TransContext   &Trans,
                   tgg00_FileId         &ObjFileId,
                   tsp00_Int4            ObjBodyLen,
                   tsp00_Int4            ObjKeyLen,
                   tsp00_Bool            bCreateSeveralObjKeyFiles,
                   tsp00_Bool            bCreateFileIsOnline,
                   Data_PageNo           rootPage,
                   tsp00_Uint1           contIndex,
                   LVC_FileNoArray      &FileNoArray,
                   Msg_List             &ErrorList);

/*!***************************************************************************
function   : bd04DelAllObj
description: All objects of a file are deleted

             All objects of the file given by the object file id are deleted. 
             If there is any object which cannot be deleted since the current 
             image does not belong to the consistent view of the current 
             transaction the error e_object_dirty is returned. In principle 
             this operations works like the following command b04DelObj 
             which is applied to all objects of a file.

arguments  : Trans     [in]     TransContext
             ObjFileId [in]     Id of the file accommodating the objects to delete
             NumDelObj [out]    Number of all objects deleted
             ErrorOid  [out]    in case of error, OID in use when error occurred
******************************************************************************/

externCpp void
bd04DelAllObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tsp00_Int4           &NumDelObj,
               tgg92_KernelOid      &ErrorOid,
               Msg_List             &ErrorList);

/*!***************************************************************************
function   : bd04DelObj
description: Deletes a single object
             
             The object defined by the OId and the WantedObjVers is deleted.
             If the current image stored in the object frame given by the OId does not contain the object 
             with the wanted version the error e_object_dirty is returned. 

arguments  : Trans         [in]      TransContext
             ObjFileId     [in,out]  Id of the file accommodating the object to be deleted
                                     If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                     the root and the fileno of the file containing the wanted object
                                     is returned in the fileId.
             OId           [in]      Id of the object frame containing the object to be deleted
             WantedObjVers [in]      Since a frame can contain a number of different objects by the time
                                     a unique definition of an object requires beside the OId also a kind
                                     of time stamp. WantedObjVers is such a time stamp (actually it is the log position).  
             contVarObj    [in]      omsVarObjects bigger than about 8 kb are split by the OMS layer into 
                                     a sequence of VarObjects. To be able to implement an iterator for 
                                     omsVarObjects all VarObjects are marked as continous VarObject
                                     Only the very fisrt VarObject isn't maarked as continous VarObject
             bOmsLayerReusedObj [in] The oid of the object has been reused from another object, this means
                                     the generation of the object, to be updated, is different from the
                                     generation which has been read from the kernel.
******************************************************************************/

externCpp void
bd04DelObj (tgg00_TransContext   &Trans,
            tgg00_FileId         &ObjFileId,
            tgg92_KernelOid       OId,
            tgg91_PageRef         WantedObjVers,
            SAPDB_Int1            contVarObj,  
            bool                  bOmsLayerReusedObj, 
            Msg_List             &ErrorList);

/*!***************************************************************************
function   : b04DestroyKeyIterator
description: Releases all resources held by the iterator
             
arguments : Trans            [in/out]  TransContext
            pKeyIterator     [in]      pointer to the key range iterator
******************************************************************************/ 
externCpp void
bd04DestroyKeyIterator (tgg00_TransContext &Trans,
                        void              *&pKeyIterator);

/*!***************************************************************************
function   : bd04DropObjFile
description: Drops a complete file
             
             The complete object file including all auxiliary structures are removed
             asynchronously. The command does not consider whether all objects to be deleted
             belong to the consistent view of the current transaction. To avoid destruction of 
             consistent views of other transactions this command should be performed only if there
             exists no other open transaction.

arguments : Trans         [in]      TransContext
            ObjFileId     [in]      Id of the file to be deleted

******************************************************************************/                     
externCpp void
bd04DropObjFile (tgg00_TransContext    &Trans,
                 tgg00_FileId          &ObjFileId,
                 Msg_List              &ErrorList);

/*!***************************************************************************
function   : bd04GetContainerId
description: see kb04GetContainerId
Interface changed: PTS 1120478 
*****************************************************************************/
externCpp void
bd04GetContainerId (tgg00_TransContext  &Trans,              //[inp/out]  
                    tgg00_FileId        &ObjFileId,          //[inp]
                    tgg92_KernelOid     &Oid);               //[inp/out] 

/*!***************************************************************************
function   : bd04GetObjConsistent
description: Reads a consistent image of an object via a OId

             This function returns the image of an object defined by the given OId. The returned image must go
             together with the consistent view of the current transaction.

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object
                                      is returned in the fileId. 
             OId             [in,out] Id of the object frame containing the wanted object.
                                      For the description of the out value see HashKeySize.
             ExtObjBodySize  [in]     Size of the destination memory where the found object image has to be copied
             pExtObjBody     [in]     Memory position to which the object has to be copied
             HashKeySize     [in]     HashKeySize > 0 displays that the OId was found via the object hash searching for
                                      the object with the key given by HashKeySize and pHashKey.
                                      If HashKeySize is not zero it is checked whether the Hashkey at the memory position 
                                      pHashKey is really equal to the key of the object with the given OId.
                                      If it is not the error e_wrong_object_version returned. If the keys are equal 
                                      the frame version which is a member of the OId is set correctly since
                                      it cannot be stored in the hash due to the reuse of keyed object frames.
             pHashKey        [in]     Memory position of the key assumed to be the key of the object
             bDoLock         [in]     if bDoLock is true the object is locked (if possible) by the current transaction
             ObjBodySize     [out]    real size of the object
             KeyLen          [out]    real key size of the object
             pKey            [out]    Memory position within the copied object where to find the object key
             ResultObjVers   [out]    time stamp (=log position)of the consistent object image 
             LogReadAccesses [out]    number of jumps back in the history necessary to find the
                                      consistent image of the object (see PTS 1107819 AK 12/09/2000)
******************************************************************************/

externCpp void
bd04GetObjConsistent (tgg00_TransContext    &Trans,              
                      tgg00_FileId          &ObjFileId,          
                      tgg92_KernelOid       &OId,                 
                      tsp00_Int4             ExtObjBodySize,     
                      void                  *pExtObjBody,        
                      tsp00_Int4             HashKeySize,        
                      void                  *pHashKey,           
                      const bool             bDoLock,            
                      const bool             bShareLock,            
                      tsp00_Int4            &ObjBodySize,       
                      tsp00_Int4            &KeyLen,            
                      tsp00_Byte*           *pKey,               
                      tgg91_PageRef         &ResultObjVers, 
                      tgg91_TransNo         &UpdTransNo,
                      tsp00_Int4            &LogReadAccesses,  
                      Msg_List              &ErrorList);

/*!***************************************************************************
function   : bd04GetObjFileStatistics
description: Statistic date of a file are collected and returned.

arguments : Trans         [in]      TransContext
            ObjFileId     [in]      Id of the file to be investigated
            NumAllPages   [out]     Number of all pages the file consists of
            NumFreePages  [out]     Number of all pages which contain empty object frames
            NumEmptyPages [out]     Number of all pages which contain only empty frames, i.e. 
                                    number of empty pages
            NumKeyLeaves  [out]     Number of leaves of the belonging key tree                   (see PTS 1107153 AK 17/07/2000 )
            Occupancy     [out]     Average occupation of the pages with not empty object frames (see PTS 1107153 AK 17/07/2000 )
******************************************************************************/


externCpp void
bd04GetObjFileStatistics (tgg00_TransContext  &Trans,
                          tgg00_FileId        &ObjFileId,
                          tsp00_Int4          &NumAllPages,
                          tsp00_Int4          &NumFreePages,
                          tsp00_Int4          &NumEmptyPages,
                          tsp00_Int4          &NumKeyLeaves,        
                          tsp00_Int4          &Occupancy);          

/*!***************************************************************************
function   : bd04GetObjWithKeyConsistent
description: Reads a consistent image of an object via a key

             This function returns the image of an object defined by a given key. The returned image must go
             together with the consistent view of the current transaction.   

arguments  : Trans           [in]      TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                       the root and the fileno of the file containing the wanted object is returned in the fileId. 
             ExtObjBodySize  [in]      Size of the destination memory where the found object image has to be copied
             pExtObjBody     [in]      Memory position to which the object has to be copied
             KeyLen          [in]      Length of the key for which the appropriate object has to be searched
             pKey            [in]      memory position of the search key
             bDoLock         [in]      if bDoLock is true the object is locked (if possible) by the current transaction
             ResultOId       [out]     Id of the object frame containing the object with the given key 
             ResultObjVers   [out]     time stamp (=log position)of the consistent object image 
             LogReadAccesses [out]     number of jumps back in the history necessary to find the
                                       consistent image of the object (see PTS 1107819 AK 12/09/2000)
******************************************************************************/

externCpp void
bd04GetObjWithKeyConsistent (tgg00_TransContext    &Trans,            //[inp/out]
                             tgg00_FileId          &ObjFileId,        //[inp]
                             tsp00_Int4             ExtObjBodySize,   //[inp]
                             void                  *pExtObjBody,      //[inp] 
                             tsp00_Int4            &KeyLen,           //[inp]
                             void                  *pKey,             //[inp]
                             const bool             bDoLock,          //[inp] 
                             const bool             bShareLock,       //[inp] 
                             tgg92_KernelOid       &ResultOid,        //[out]
                             tgg91_PageRef         &ResultObjVers,    //[out]
                             tgg91_TransNo         &UpdTransId,       //[out]
                             tsp00_Int4            &LogReadAccesses,  //[out] 
                             Msg_List              &ErrorList);

/*----------------------------------------------------------------------------*/

externCpp bool  
bd04GetNewFileId(tgg00_TransContext &Trans, FileDir_FileNo &objFileNo);

/*---------------------------------------------------------------------------*/

externCpp void 
bd04vStateFileversion (tgg00_TransContext &Trans,
                       tgg00_FileId       &versionTree,
                       Msg_List           &ErrorList);

/*!***************************************************************************
function   : bd04IsObjHistoryUsed

description: Checks if any history image is used by any consistent view

             This function investigates whether there is any consistent view which needs the 
             history of the wanted object to get a consistent image of the object.
             (see also CRS 1106284 AK 11-03-2000 )

arguments  : Trans           [in]      TransContext
             ObjFileId       [in,out]  Id of the file accommodating the wanted object.
                                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                       the root and the fileno of the file containing the wanted object is returned in the fileId. 
             OId             [in]      Id of the object frame containing the wanted object.
             ignoreVersions  [in]      true  == ignore consistent views held because of omsVersions
                                       false == consider all existing consistent views
             bHistoryIsUsed  [out]     If this boolean is true a consistent view exists which needs the history of the
                                       object to get the appropriate consistent image of the object.
             OldestTransNo   [out]     TransID (trTransId_gg00) of transaction holding the oldest consistent view

******************************************************************************/

externCpp void
bd04IsObjHistoryUsed (tgg00_TransContext &Trans,
                      tgg00_FileId       &ObjFileId,
                      tgg92_KernelOid     OId,
                      bool                ignoreVersions,   /* PTS 1130354 */
                      bool               &bHistoryIsUsed,
                      SAPDB_UInt8        &OldestTransNo,    /* PTS 1130354 */
                      Msg_List           &ErrorList);

/*!***************************************************************************
function   : bd04IsObjSelfLocked

description: Checks if an object is locked by the current transaction.

             If the investigated object is locked by the current transaction the parameter 
             IsSelfLocked returns true otherwise false. (see also PTS 1001745 UH 23-11-1998)

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId. 
             OId             [in]     Id of the object frame containing the wanted object.
			 bShared         [in]     false = check if excl. locked, true check if shared locked
             IsSelfLocked    [out]    is true if the object is locked by the current transaction otherwise it is false 
******************************************************************************/

externCpp void
bd04IsObjSelfLocked (tgg00_TransContext& Trans,
                     tgg00_FileId&       ObjFileId,
                     tgg92_KernelOid&    OId,
					 bool                bShared,
                     bool&               IsSelfLocked,
                     Msg_List&           ErrorList);

/*!***************************************************************************
function   : bd04LockObj

description: Locks an object

             If an object cannot be locked since the wanted version does not correspond to the current image
             of the object the error e_object_dirty is returned. 

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId. 
             OId             [in]     Id of the object frame containing the wanted object.
             WantedObjVers   [in]     Since a frame can contain a number of different objects by the time
                                      a unique definition of an object requires beside the OId also a kind
                                      of time stamp. WantedObjVers is such a time stamp (actually it is the log position).  
******************************************************************************/
externCpp void
bd04LockObj (tgg00_TransContext& Trans,
             tgg00_FileId&       ObjFileId,
             tgg92_KernelOid&    OId,
             tgg91_PageRef&      WantedObjVers,
             Msg_List&           ErrorList);

/*!***************************************************************************
function   : bd04LockUpdObj

description: Locks an object

             If an object cannot be locked since the wanted version does not correspond to the current image
             of the object the error e_object_dirty is returned. 

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId. 
             OId             [in]     Id of the object frame containing the wanted object.
             WantedObjVers   [in]     Since a frame can contain a number of different objects by the time
                                      a unique definition of an object requires beside the OId also a kind
                                      of time stamp. WantedObjVers is such a time stamp (actually it is the log position).  
******************************************************************************/

externCpp void
bd04LockUpdObj (tgg00_TransContext&   Trans,
                tgg00_FileId&         ObjFileId,
                tgg92_KernelOid&      OId,
                tgg91_PageRef&        WantedObjVers,
                Msg_List             &ErrorList);


/*!***************************************************************************
function   : bd04LockShareObj

description: Locks an object in share mode

             If an object cannot be locked since the wanted version does not correspond to the current image
             of the object the error e_object_dirty is returned. 

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId. 
             OId             [in]     Id of the object frame containing the wanted object.
             WantedObjVers   [in]     Since a frame can contain a number of different objects by the time
                                      a unique definition of an object requires beside the OId also a kind
                                      of time stamp. WantedObjVers is such a time stamp (actually it is the log position).  
******************************************************************************/
externCpp void
bd04LockShareObj (tgg00_TransContext& Trans,
                  tgg00_FileId&       ObjFileId,
                  tgg92_KernelOid&    OId,
                  tgg91_PageRef&      WantedObjVers,
                  Msg_List&           ErrorList);


/*!***************************************************************************
function   : bd04NewObjKey

description: Inserts a new OId with a given key into the key container

             If the key to be inserted exists already in the key container, the belonging
             OId is returned via the parameter ReUsedOId and ReUsedObjVers.

arguments  : Trans         [in]  TransContext
             ObjFileId     [in]  Id of the file accommodating the object with the new OId.
             KeyLen        [in]  Length of the key 
             pKey          [in]  memory position of the object key to be inserted
             OId           [in]  OId to be inserted
             ReUsedOId     [out] If the key to be inserted exists already in the key container, the belonging
                                 OId is returned via the parameter ReUsedOId. 
             ReUsedObjVers [out] If the key to be inserted exists already in the key container, the version
                                 of the belonging OId is returned via the parameter ReUsedObjVers. 
******************************************************************************/

externCpp void
bd04NewObjKey (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tsp00_Int4            KeyLen,
               void                 *pKey,
               tgg92_KernelOid       OId,
               tgg92_KernelOid      &ReUsedOId,
               tgg91_PageRef        &NewObjVers,
               Msg_List             &ErrorList);

/*!***************************************************************************
function   : bd04NewObj

description: Reserves an empty object frame for new object and returns it's OId. 

             If the object owns a key this key is written into the object frame.

arguments  : Trans         [in]  TransContext
             ObjFileId     [in]  Id of the file accommodating the object with the new OId.
             KeyLen        [in]  Length of the key of the object to be inserted
             pKey          [in]  Memory position of the key to be inserted presuming KeyLen > 0
             contObj       [in]  If the object is a var-object, then this flag indicates, whether the
                                 object is a 'head'-object (false) or a 'continuation'-object (true)
             ResultOId     [out] OId of the reserved object frame
             ResultObjVers [out] Version (position of the log entry) of the new object 

******************************************************************************/
externCpp void
bd04NewObj (tgg00_TransContext     &Trans,
            tgg00_FileId           &ObjFileId,
            tsp00_Int4              KeyLen,
            void                   *pKey,
            SAPDB_Int1              contObj,
            tgg92_KernelOid        &ResultOId,
            tgg91_PageRef          &ResultObjVers,
            Msg_List               &ErrorList);

/*!***************************************************************************
function   : bd04NextObjConsistent

description: An object and it's OId following a given input OId are returned.

arguments  : Trans           [in]     TransContext
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileId is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId.
                                      Do to so the input OId mus not be nil.
             CurrOId         [in,out] As input CurrOId defines the position from where the search for the next object should be started.
                                      If an object frame with a consistent object image is found it's OId is returned via CurrOId too. 
                                      If CurrOId as input parameter is nil the very first object of the file is returned.  
             bWithObject     [in]     If bWithObject is true not only the OId but also the object is returned via the following parameter 
                                      ExtObjBodySize and pExtObjBody
             ExtObjBodySize  [in]     Size of the destination memory where the found object image has to be copied
             pExtObjBody     [in]     Memory position to which the object has to be copied
             ObjBodySize     [out]    Length of the object
             ResultObjVers   [out]    Since a frame can contain a number of different objects by the time
                                      a unique definition of an object requires beside the OId also a kind
                                      of time stamp. ResultObjVers a time stamp (actually it is the log position) of the image
                                      of the object found following the input OId.
             LogReadAccesses [out]    Number of jumps back in the history necessary to find the
                                      consistent image of the object (see PTS 1107819 AK 12/09/2000)
******************************************************************************/ 

externCpp void
bd04NextObjConsistent (tgg00_TransContext     &Trans,
                       tgg00_FileId           &ObjFileId,
                       tgg92_KernelOid        &CurrOId,          //[inp,out]
                       bool                    bWithObject,      
                       tsp00_Int4              ExtObjBodySize,   //[inp=size of buffer supplied for the object]
                       void                   *pExtObjBody,
                       tsp00_Int4             &ObjBodySize,      //[out=real size of object]
                       tgg91_PageRef          &ResultObjVers,    
                       tsp00_Int4             &LogReadAccesses,  //[out] 
                       Msg_List               &ErrorList);


/*!***************************************************************************
function   : bd04NextOidsConsistent
description: see kb04NextOidsConsistent
new function: PTS 1120478 
*****************************************************************************/

externCpp void
bd04NextOidsConsistent (tgg00_TransContext     &Trans,
                        tgg00_FileId           &ObjFileId,
                        bool                    bOnlyHdrVarObj,
                        const tgg92_KernelOid  &CurrOid, 
                        tsp00_Int4             &noOfOid,
                        tgg92_KernelOid        *pOid,
                        tgg91_PageRef          *pObjVers,
                        tsp00_Int4             &LogReadAccesses,  
                        Msg_List               &ErrorList);

/*!***************************************************************************
function   : bd04NextObjFromKeyIterator
description: see kb04NextObjFromKeyIterator
Interface changed: PTS 1119480 
*****************************************************************************/

externCpp void
bd04NextObjFromKeyIterator (tgg00_TransContext   &Trans,
                            void                 *pUntypedKeyIterator,    
                            bool                  bInclusive, 
                            tsp00_Int4            objBodySize,
                            tsp00_Int4            KeyLen,        
                            void                 *pRestartKey, 
                            tsp00_Int4           &noOfOid,
                            tgg92_KernelOid      *pResultOId, 
                            tgg91_PageRef        *pObjVers,
                            tsp00_MoveObj        *pBody,
                            tsp00_Int4           &LogReadAccesses,
                            Msg_List             &ErrorList);

/*!***************************************************************************
function   : bd04PrevObjFromKeyIterator
description: see kb04PrevObjFromKeyIterator
Interface changed: PTS 1119480 
*****************************************************************************/

externCpp void
bd04PrevObjFromKeyIterator (tgg00_TransContext   &Trans,
                            void                 *pUntypedKeyIterator,    
                            bool                  bInclusive,  
                            tsp00_Int4            objBodySize,
                            tsp00_Int4            KeyLen,        
                            void                 *pRestartKey, 
                            tsp00_Int4           &noOfOid,
                            tgg92_KernelOid      *pResultOId, 
                            tgg91_PageRef        *pObjVers,
                            tsp00_MoveObj        *pBody,
                            tsp00_Int4           &LogReadAccesses,
                            Msg_List             &ErrorList);

/*!***************************************************************************
function   : bd04OmsReadAheadParticipant

description: Reads all pages of a file.

             This function reads all pages of an file. It is called at the restart of the
             data base to get all pages into the data cache. This avoids user I/O's caused
             by an application.

arguments  : Trans       [in]     TransContext
             ObjFileId   [in,out] Id of the file to be read
******************************************************************************/

externC void
bd04OmsReadAheadParticipant (tgg00_TransContext  &Trans,
                             tgg00_FileId       &ObjFileId);

/*!***************************************************************************
function   : bd04CancelOmsVersion

description: Tries to release all Resources held by an OmsVersion

arguments  : ConsistViewNo [in] consistent view, which had been assigned to OmsVersion
******************************************************************************/
externC void
bd04CancelOmsVersion (tsp00_TaskId   TaskId,
                      tgg91_TransNo &ConsistViewNo);


/*!***************************************************************************
function   : bd04RollbackObj

description: Changes the state of an object frame back to the state it had before

arguments  : Trans           [in]     TransContext   
             ObjFileNo       [in]     Number of the file accomodating the object to change
             RollbackType    [in]     Kind of operation (new, delete, insert, update) to be rolled back
             OId             [in]     OId (=position) of the object frame which shall be changed
             NewState        [in]     State (free, occupied, reserved, feot) in which the current state hast the be transformed
             BeforeObjVers   [in]     Version of the before image of the current object 
             ObjUpdTransNo   [in]     Handle of the transaction which created the before image of the current object
             BeforeObjSize   [in]     Length of the before image of the object
             pBeforeObj      [in]     Pointer to the before image of the object
             contVarObject   [in]     If the object is a var-object, then this flag indicates, whether the
                                      object is a 'head'-object (false) or a 'continuation'-object (true)
             bOmsLayerReusedObj [in] The oid of the object has been reused from another object, this means
                                     the generation of the object, to be updated, is different from the
                                     generation which has been read from the kernel.

******************************************************************************/

externCpp void
bd04RollbackObj (tgg00_TransContext  &Trans,
                 tgg00_MessType_Enum  RollbackType,
                 tgg92_KernelOid      OId,
                 FileDir_FileNo       Fileno,     
                 tgg00_ObjState       NewState,
                 tgg91_PageRef        BeforeObjVers,
                 tgg91_TransNo        ObjUpdTransNo,
                 tsp00_Int4           BeforeObjSize,
                 void                *pBeforeObj,
                 SAPDB_Int1           contVarObject,
                 bool                 bOmsLayerReusedObj);

/*!***************************************************************************
function   : bd04RollbackDropObjContainer

description: Rollback omsDropContainer

arguments  : Trans           [in]     TransContext   
             Fileno          [in]     Fileno of the container
******************************************************************************/

externCpp void
bd04RollbackDropObjContainer (tgg00_TransContext   &Trans,
                              const FileDir_FileNo &Fileno);

/*!***************************************************************************
function   : bd04RollbackDropObjContainer

description: Rollback omsDropContainer

arguments  : Trans           [in]     TransContext   
             Fileno          [in]     Fileno of the container
******************************************************************************/

externCpp void
bd04RollbackCreateObjContainer (tgg00_TransContext   &Trans,
                                const FileDir_FileNo &Fileno);

/*!***************************************************************************
function   : bd04UnlockObj

description: Removes the lock on an object held by the current transaction


arguments  : Trans           [in]     TransContext   
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileId is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId.
             OId             [in]     OId of the frame containing the object to be unlocked
             bSharedLock     [in]     Unlock an exclusive (false) or a share (true) lock
******************************************************************************/

externCpp void
bd04UnlockObj (tgg00_TransContext   &Trans,
               tgg00_FileId         &ObjFileId,
               tgg92_KernelOid       OId,
               bool                  bSharedLock);

/*!***************************************************************************
function   : bd04UpdateObj

description: Replaces the current image of an object by a new one


arguments  : Trans           [in]     TransContext   
             ObjFileId       [in,out] Id of the file accommodating the wanted object.
                                      If the fileId is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
                                      the root and the fileno of the file containing the wanted object is returned in the fileId.
             OId             [in]     OId of the frame containing the object to be updated
             ObjVers         [in/out] Version of the object which shall be updated. Only if the given Version corresponds to the
                                      Version of the current image of the object the object can be updated otherwise the error 
                                      e_object_dirty is returned.
             After this operation ObjVers contains the version of the updated object image
             NewObjBodySize   [in]    Length of the new object image replacing the old image of the object
             pNewObjBody      [in]    Pointer to the new object image
             contObj          [in]    If the object is a var-object, then this flag indicates, whether the
                                      object is a 'head'-object (false) or a 'continuation'-object (true)
             bOmsLayerReusedObj [in]  The oid of the object has been reused from another object, this means
                                      the generation of the object, to be updated, is different from the
                                      generation which has been read from the kernel.
******************************************************************************/

externCpp void
bd04UpdateObj (tgg00_TransContext    &Trans,
               tgg00_FileId          &ObjFileId,
               tgg92_KernelOid        OId,
               tgg91_PageRef         &ObjVers,
               tsp00_Int4             NewObjBodySize,
               void                  *pNewObjBody,
               SAPDB_Int1             contObj,  
               bool                   bOmsLayerReusedObj,
               Msg_List              &ErrorList);

/*---------------------------------------------------------------------------*/

/*!
  EndChapter: Interface Routines
 */
#endif /* HBD04_H */
