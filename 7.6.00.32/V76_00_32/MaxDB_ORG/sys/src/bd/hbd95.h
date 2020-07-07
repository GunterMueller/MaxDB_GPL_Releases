/*!**************************************************************************

  module      : hbd95.h

  -------------------------------------------------------------------------

  responsible : d025251

  special area: rollforward
  description : This header file is the interface between the data storage and the log component
                with respect to roll forward  operations during the recovery 


  last changed: 2001-05-22  17:07
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef HBD95_H
#define HBD95_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "DataAccess/Data_FileTypes.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg92.h"
#include "gbd00.h"
#include "Logging/Log_Types.hpp"
#include "liveCache/LVC_Types.hpp"

#include "FileDirectory/FileDir_Types.hpp"  

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!***************************************************************************
  Chapter: Redo Routines
 ****************************************************************************/


/*!***************************************************************************
function   : bd95Rollforward_CreateObjFile

description: redo of a the operation 'create object file'.

             This function is the counterpart of
             the function bd04CreateObjFile, but whereas  bd04DropObjFile is used during online operations 
             bd95Rollforward_CreateObjFile is used only for the recovery. While bd04CreateObjFile can choose
             the page numbers of the root and the subroots without restrictions bd95Rollforward_CreateObjFile
             must use exsactly the same page numbers which were originally choosen during the online creation of the 
             file. The original page numbers are given in the parameter RootPageNoArray.

arguments  : Trans                     [in] TransContext   
             ObjFileNo                 [in] Number of the file to create
             bFileIsForVarObj          [in] if true the file is for objects of variable length else of fixed length
             ObjBodyLen                [in] length of the object body (but only relevant if bFileIsForVarObj is false)
             ObjKeyLen                 [in] length of the object keys
             bCreateSeveralObjKeyFiles [in] in case that keys are defined for this file (i.e. ObjKeyLen>0)
                                            false means that exactly one key file should be created otherwise
                                            keys should be distributet over more than one key file to allow
                                            a better parallel access to the keys
             RootPageNoArray           [in] array of the page numbers which have to be used for the root 
                                            and the subroot pages           
return value: none
  
******************************************************************************/

externCpp void
bd95Rollforward_CreateObjFile (tgg00_TransContext   &Trans,
                               const FileDir_FileNo &ObjFileNo,
                               Data_FileType         filetype,
                               tsp00_Int4            ObjBodyLen,
                               tsp00_Int4            ObjKeyLen,
                               Data_PageNo           rootPage,
                               LVC_FileNoArray      &filenoArr);

/*!***************************************************************************
function   : bd95Rollforward_DropObjFile

description: redo of a the operation 'drop object file'.

             This function is the counterpart of
             the function bd04DropObjFile, but whereas  bd04DropObjFile is used during online operations 
             bd95RollforwardObj_DelObj is used only for the recovery. While bd04DropObjFile markes a
             object file only as deleted (pages are removed by the garbage collector) bd95RollforwardObj_DelObj
             removes the pages immediately. This is since the GC is not active during the restart

arguments  : Trans           [in]     TransContext   
             ObjFileNo       [in]     Number of the file accomodating the object to change
        
return value: none
  
******************************************************************************/

externCpp void
bd95Rollforward_DropObjFile (tgg00_TransContext   &Trans,
                             const FileDir_FileNo &ObjFileNo);

/*!***************************************************************************
function   : bd95RollforwardObj_DelObj

description: redo of a the operation 'delete object'.

             This function is the counterpart of
             the function bd04DelObj, but whereas  bd04DelObj is used during online operations 
             bd95RollforwardObj_DelObj is used only for the recovery. 

arguments  : Trans           [in]     TransContext   
             OId             [in]     OId (=position) of the object to delete
             ObjVers         [in/out] Version of the before image of the current after image 
                                      object. In principle this ObjVers must match with the
                                      object version of the current object.
             ObjFileNo       [in]     Number of the file accomodating the object to change
             RedoEntrySequence [in]   sequence number of the redo entry which contains the current task to
                                      rollforward an object
             RedoUpdTransNo  [in]     Handle of the transaction which changed the object from the current image to the after image
             ContVarObj      [in]     VarObject is continous object of a sequence of VarObject for one omsVarObject
             bReused         [in]     object has a changed frameversion, becuase it had been "reused" by OMS layer

return value: none
  
******************************************************************************/

externCpp void
bd95RollforwardObj_DelObj (tgg00_TransContext       &Trans,        
                           tgg92_KernelOid           OId,
                           const tgg91_PageRef      &ObjVers,
                           const FileDir_FileNo     &ObjFileNo,
                           const Log_EntrySequence  &RedoEntrySequence,
                           const tgg91_TransNo      &RedoUpdTransNo,
                           SAPDB_Int1                ContVarObj,             /* PTS 1127661 FF */
                           bool                      bReused,                /* PTS 1127661 FF */
                           SAPDBErr_MessageList     &errlist);
/*!***************************************************************************
function   : bd95RollforwardObj_NewObj

description: redo of a the operation 'new object'. 

             This function is the counterpart of
             the function bd04NewObj, but whereas  bd04NewObj is used during online operations 
             bd95RollforwardObj_NewObj is used only for the recovery. 

arguments  : Trans           [in]     TransContext   
             OId             [in]     OId (=position) of the object frame where the new object is to place
             ObjVers         [in/out] Version of the before image of the current after image 
                                      object. In principle this ObjVers must match with the
                                      object version of the current object.
             ObjFileNo       [in]     Number of the file accomodating the object to change
             AfterObjKeyLen  [in]     Length of the key of the after image of the object
             pAfterObjKey    [in]     Pointer to the key of the after image of the object
             RedoEntrySequence [in]   sequence number of the redo entry which contains the current task to
                                      rollforward an object
             RedoUpdTransNo  [in]     Handle of the transaction which changed the object from the current image to the after image
             ContVarObj      [in]     VarObject is continous object of a sequence of VarObject for one omsVarObject
return value: none
                  
******************************************************************************/

externCpp void
bd95RollforwardObj_NewObj (tgg00_TransContext       &Trans,
                           tgg92_KernelOid           OId,
                           const tgg91_PageRef      &ObjVers,
                           const FileDir_FileNo     &ObjFileNo,
                           tsp00_Int4                AfterObjKeyLen,
                           void                     *pAfterObjKey,
                           const Log_EntrySequence  &RedoEntrySequence,
                           const tgg91_TransNo      &RedoUpdTransNo,
                           SAPDB_Int1                ContVarObj,          /* PTS 1127661 FF */
                           SAPDBErr_MessageList     &errlist);


/*!***************************************************************************
function   : bd95Rollforward_UpdObj

description: redo of a the operation 'update object'.

             This function is the counterpart of
             the function bd04UpdateObj, but whereas  bd04UpdateObj is used during online operations 
             bd95Rollforward_UpdObj is used only for the recovery. 

arguments  : Trans           [in]     TransContext   
             OId             [in]     OId (=position) of the object frame which is to update
             ObjVers         [in/out] Version of the before image of the current after image 
                                      object. In principle this ObjVers must match with the
                                      object version of the current object.
             ObjFileNo       [in]     Number of the file accomodating the object to change
             AfterObjLen     [in]     Length of the after image of the object
             pAfterObj       [in]     Pointer to the after image of the object
             RedoEntrySequence [in]   sequence number of the redo entry which contains the current task to
                                      rollforward an object
             RedoUpdTransNo  [in]     Handle of the transaction which changed the object from the current image to the after image
             ContVarObj      [in]     VarObject is continous object of a sequence of VarObject for one omsVarObject
             bReused         [in]     object has a changed frameversion, becuase it had been "reused" by OMS layer
return value: none
           
******************************************************************************/

externCpp  void 
bd95Rollforward_UpdObj (tgg00_TransContext       &Trans,
                        const tgg92_KernelOid    &OId,
                        const tgg91_PageRef      &ObjVers,
                        const FileDir_FileNo     &ObjFileNo,
                        tsp00_Int4                NewObjBodyLen,
                        void                     *pNewObjBody,
                        const Log_EntrySequence  &RedoEntrySequence,
                        const tgg91_TransNo      &RedoUpdTransNo,
                        SAPDB_Int1                ContVarObj,             /* PTS 1127661 FF */
                        bool                      bReused);               /* PTS 1127661 FF */


/*!***************************************************************************
  EndChapter: Redo Routines
 ****************************************************************************/

#endif  /* HBD95_H */
