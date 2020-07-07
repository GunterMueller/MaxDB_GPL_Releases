/****************************************************************************

  module      : hbd91.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: 
  description : Declaration of exported functions of modul vbd91.cpp


  version     : 7.4.4.1
  last changed: 2003-01-22  19:48
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

#ifndef HBD91_H
#define HBD91_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef GBD910_H
#   include "gbd910.h"
#endif
#ifndef LVC_ERRORTRACEOBJECT_HPP
#   include "liveCache/LVC_ErrorTraceObject.hpp"
#endif
#ifndef LOG_UNDOFILE_HPP
#   include "Logging/Log_UndoFile.hpp"
#endif
#ifndef GC_IGARBAGECOLLECTOR_HPP
#   include "GarbageCollector/GC_IGarbageCollector.hpp" 
#endif
#ifndef FILEDIR_ITERATOR_HPP
#   include "FileDirectory/FileDir_Iterator.hpp"  
#endif
#ifndef LVC_TYPES_HPP
#   include "FileDirectory/FileDir_Types.hpp"  
#endif
#ifndef LVC_TYPES_HPP
#   include "liveCache/LVC_Types.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

 
externCpp void
bd91AddFileToObjFileDir (tgg00_TransContext     &Trans,
						 tgg00_FileId           &FileId,
						 cbd910_ObjDataNode     &NptrsRoot,
                         Converter_Version      &converterVersion,
                         SAPDB_UInt1             numKeyFiles,
                         LVC_RootArray          &FileNoArray,
                         Msg_List               &ErrorList);
/*---------------------------------------------------------------------------*/

externC void
bd91CheckIndex (tgg00_TransContext   &Trans,
                tsp00_C16            &ObjFileNoStr );
 
/*---------------------------------------------------------------------------*/

externCpp void
bd91CheckRetentionTime(tsp00_TaskId  TaskId);

/*---------------------------------------------------------------------------*/

externPascal void bd91CleanUpFileDirCache (
    tgg00_TransContext VAR_VALUE_REF  Trans);

/*---------------------------------------------------------------------------*/

externPascal void bd91DumpGarbColl (
    tgg00_VfFileref       VAR_VALUE_REF  hostfile,
    tsp00_Int4                           buffer_size,
    tsp00_Page            VAR_ARRAY_REF  buffer,
    tsp00_Int4            VAR_VALUE_REF  dump_pno,
    integer               VAR_VALUE_REF  pos,
    tsp00_VfReturn        VAR_VALUE_REF  error,
    tsp00_ErrText         VAR_ARRAY_REF  errtext);

/*---------------------------------------------------------------------------*/

externPascal void bd91DeleteFileDirCache (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/*---------------------------------------------------------------------------*/

externPascal void bd91InfoGCReset ();

/*---------------------------------------------------------------------------*/

externPascal void bd91InfoGCStats (
    tsp00_Int4                           idxGC,
    tsp00_TaskId          VAR_VALUE_REF  TaskId,
    pasbool               VAR_VALUE_REF  active,
    tsp00_8ByteCounter    VAR_VALUE_REF  activateCount,
    tsp00_8ByteCounter    VAR_VALUE_REF  deleteObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistEntryNormal,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistCreateObjFile,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistDeleteObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistDropObjFile,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistLockObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistInsertObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistNewObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relHistUpdateObj,
    tsp00_8ByteCounter    VAR_VALUE_REF  relPageEmpty,
    tsp00_8ByteCounter    VAR_VALUE_REF  relPageDropFile,
    tsp00_8ByteCounter    VAR_VALUE_REF  relDropFile);

/*---------------------------------------------------------------------------*/

externC void bd91InfoContainerRoots (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    FileDir_Iterator          *&pObjFileIter,
    FileDir_Iterator          *&pContObjFileIter,
    tsp00_Addr                           pStackarea,
    tsp00_Int2                           StackareaLen,
    tsp00_C8              VAR_VALUE_REF  ClassId,
    tsp00_Int4            VAR_VALUE_REF  Root,
    tsp00_Int4            VAR_VALUE_REF  ContainerSequelNo,
    tsp00_Int2            VAR_VALUE_REF  NumKeyPartitions,
    pasbool               VAR_VALUE_REF  VarContainer,
    pasbool               VAR_VALUE_REF  DeleteFlag,
    tsp00_Int4            VAR_VALUE_REF  MaxObjBodySize,
    tsp00_Int4            VAR_VALUE_REF  MaxObjPerPage,
    tsp00_Int4            VAR_VALUE_REF  MaxChains,
    tsp00_Int2            VAR_VALUE_REF  UnusedBytesPerPage);

/*---------------------------------------------------------------------------*/

externC void bd91InfoContainerChains (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    FileDir_Iterator                   *&pObjFileIter,
    FileDir_Iterator                   *&pContObjFileIter,
    tsp00_Addr                           pStackarea,
    tsp00_Int2                           StackareaLen,
    tsp00_C8              VAR_VALUE_REF  ClassId,
    tsp00_Int4            VAR_VALUE_REF  ContainerSequelNo,
    tsp00_Int4            VAR_VALUE_REF  ChainId,
    tsp00_Int4            VAR_VALUE_REF  Subroot,
    tsp00_Int4            VAR_VALUE_REF  ObjCount,
    tsp00_Int4            VAR_VALUE_REF  PageCount,
    tsp00_Int4            VAR_VALUE_REF  ActiveGC,
    pasbool               VAR_VALUE_REF  RelEmptyPages);

/*---------------------------------------------------------------------------*/

externC void bd91InfoContainerKeys (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    FileDir_Iterator                   *&pObjFileIter,
    tsp00_Addr                           pStackarea,
    tsp00_Int2                           StackareaLen,
    tsp00_Int2            VAR_VALUE_REF  KeyIdx,
    tsp00_C8              VAR_VALUE_REF  ClassId,
    tsp00_Int4            VAR_VALUE_REF  KeyRoot);

/*---------------------------------------------------------------------------*/

externC void
bd91InitOMSGarbageCollection ();

/*---------------------------------------------------------------------------*/

externPascal void 
bd91InitRWRecreateIndex(void);

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 b91MaxRetentionTime();
 
/*---------------------------------------------------------------------------*/

externC tsp00_Int4 b91MinRetentionTime();

/*---------------------------------------------------------------------------*/

externPascal void bd91FDirNextObjFileId (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    FileDir_Iterator                   *&pObjFileIter,
    tgg00_FileId          VAR_VALUE_REF  ObjFileId);

/*---------------------------------------------------------------------------*/

externC void
bd91RecreateIndex (tgg00_TransContext   &Trans,
                   tsp00_C16            &ObjFileNoStr );

/*---------------------------------------------------------------------------*/
// PTS 1113452 FF 16-Jan-2002

externC void
bd91RemoveObjFilesMarkedForDelete (tgg00_TransContext &Trans);

/*---------------------------------------------------------------------------*/

externC void
bd91StartOMSGarbageCollectionIfRequired (tsp00_TaskId TaskId); 

/*---------------------------------------------------------------------------*/

externC void
bd91StartOMSGarbageCollection (tgg00_TransContext   &Trans);

/*---------------------------------------------------------------------------*/

externC void
bd91EnableOMSHistoryRemoval (tsp00_TaskId  TaskId);

/*---------------------------------------------------------------------------*/

externC void
bd91StartOMSGarbageCollectionAfterSpecialOp (tsp00_TaskId  TaskId);

/*---------------------------------------------------------------------------*/

externC void
bd91StopOMSGarbageCollectionForSpecialOp (tsp00_TaskId  TaskId);

/*---------------------------------------------------------------------------*/

externCpp void
bd91RemoveObjFromFile (cbd05_Current           &Current,
                       cbd910_PrimObjDataNode  &NptrsPage,
					   cbd910_SubRootNode      &NptrsSubRoot,
                       tbd900_ObjFrame         *pObjFrame,
                       tgg92_KernelOid         &OId,
                       const Converter_Version &converterVersion,
                       FileDir_Oms             &fd_smart,
                       Log_UndoFile::Iterator  &historyIterator,
                       LVC_ErrorTraceObject    &ErrorTraceObject,
                       bool                     bNoKeyRemove, // new PTS 1128894 FF 31-MAR-2004
                       Msg_List    &errlist);

/*---------------------------------------------------------------------------*/

externCpp void
bd91RemoveObjFromPage (cbd05_Current           &Current,
					   cbd910_PrimObjDataNode  &NptrsPage,
					   tbd900_ObjFrame         *pObjFrame,
                       tgg92_KernelOid         &OId,
                       const Converter_Version &converterVersion,
                       FileDir_Oms             &fd_smart,
					   LVC_ErrorTraceObject    &ErrorTraceObject,
                       bool                     bNoKeyRemove); // new PTS 1128894 FF 31-MAR-2004

/*---------------------------------------------------------------------------*/

externCpp void
bd91ReleaseUnusedObject (tgg00_TransContext      &Trans,
                         Log_UndoFile::Iterator  &historyIterator,
                         const tgg92_KernelOid   &OId,
                         FileDir_FileNo      ObjFileNo,
                         const tgg91_PageRef     &WantedObjVers,
                         GC_IGarbageCollector    &IGarbColl,
                         Msg_List    &errlist);

/*---------------------------------------------------------------------------*/
// PTS 1119647 FF 2002-DEC-05
externCpp void
bd91DropObjFile (tgg00_TransContext      &Trans,
                         FileDir_FileNo ObjFileNo);

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void bd91TransEnd (tgg00_TransContext    VAR_VALUE_REF  t,
                                pasbool                              commit);

/*---------------------------------------------------------------------------*/
externC void
bd91FreeCaches(tgg00_TransContext &Trans);

/*---------------------------------------------------------------------------*/
extern bool bd91MigrateToNewLockprotocol;       // UH 2003-07-03
extern bool bd91MigrateToDoubleLinkedFreelist;  // UH 2003-07-03

#endif /* HBD91_H */
