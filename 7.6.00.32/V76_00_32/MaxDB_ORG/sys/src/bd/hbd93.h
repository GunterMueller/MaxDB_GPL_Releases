/****************************************************************************

  module      : hbd93.h

  -------------------------------------------------------------------------

  responsible : AlexanderK

  special area: logging ...
  description : description ...


  version     : 7.2.0.0
  last changed: 1999-06-24  18:57
  see also    : example.html ...

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

#ifndef HBD93_H
#define HBD93_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd900.h"
#include "gbd910.h"

#include "Logging/Log_Types.hpp"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void
bd93InitContObjPage (cbd05_Current       &Current,
					 tbd_node_ptrs       &NptrsPage,
					 tsp00_Int4           ContObjDistance);

/*---------------------------------------------------------------------------*/

externCpp void
bd93GetVarObjBody (cbd05_Current          &Current,            //[inp\out] 
                   FileDir_OmsVar         &fd_smart,
				   const tgg92_KernelOid  &OId,                //[inp] 
				   tsp00_Int4              ExtObjBodySize,     //[inp]
				   void                   *pExtObjBody,        //[inp]
				   tbd900_ObjFrame        *pObjFrame           //[inp]
				   );

/*---------------------------------------------------------------------------*/

externCpp void  
bd93LogDelVarObj (cbd05_Current         &Current,
                  tgg92_KernelOid        OId, 
                  tgg91_PageRef          WantedObjVers,      
                  tbd_nodeptr            pPage, 
                  tbd900_ObjFrame       *pObjFrame,
                  SAPDB_Int1             contVarObj,         
                  bool                   bOmsLayerReusedObj,    
                  SAPDBErr_MessageList  &ErrorList);

/*---------------------------------------------------------------------------*/

externCpp SAPDB_UInt1
bd93GetNumOfContFiles (void);       

/*---------------------------------------------------------------------------*/

externCpp unsigned short
bd93GetContFileBodySizes (unsigned short index);          // [inp]

/*---------------------------------------------------------------------------*/


externCpp void
bd93RemoveContObj (cbd05_Current           &Current,        //[inp]
                   const Converter_Version &converterVersion,
                   FileDir_OmsVar          &fd_smart,
				   tgg92_KernelOid         &OId,            //[inp]    
				   const tsp00_Int4         ContObjFileNo,  //[inp]     
				   const tsp00_PageNo       ContObjPno,     //[inp] 
				   const tgg00_ObjPagePos   ContObjPos);    //[inp]

/*---------------------------------------------------------------------------*/

externCpp void
bd93UpdateVarObjBody (cbd05_Current           &Current,
                      const Converter_Version &converterVersion,
                      FileDir_OmsVar          &fd_smart,
                      tgg92_KernelOid          OId,
                      tgg91_PageRef            ObjVers,
                      cbd910_PrimObjDataNode  &NptrsAnchor,
                      tbd900_ObjFrame         *&pObjFrame, 
                      tsp00_Int4               NewObjBodySize,
                      void                    *pNewObjBody,
                      bool                     bRollBack,
                      bool                    &bTooManyEmptyPagesInContFile,
                      tsp00_Int4              &NewContObjFileNo,
                      tsp00_Int4              &ContObjFileNo,
                      const Log_EntrySequence &RedoEntrySequence,
                      const tgg91_TransNo     &RedoUpdTransNo,
                      SAPDB_Int1               contObj,           
                      bool                     bOmsLayerReusedObj,
                      SAPDBErr_MessageList    &ErrorList);

/*---------------------------------------------------------------------------*/

#endif  /* HBD93_H */
