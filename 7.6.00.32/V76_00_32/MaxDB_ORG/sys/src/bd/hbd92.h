/****************************************************************************

  module      : hbd92.h

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: 
  description : Declaration of exported functions of modul vbd92.cpp


  version     : 7.6.0.0
  last changed: 1999-07-28  19:48
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

#ifndef HBD92_H
#define HBD92_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg92.h"
#include "gbd00.h"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "gsp00.h" 
#include "heo670.h"
#include "gbd05.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
#  define _INLINE
#else
#  define _INLINE   inline
#endif
  
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifdef BIT64
//  object frames get 8 byte aligned
#  define BD92ALIGN_OBJ_FRAME(OBJ_FRAME_LEN) \
    (((((OBJ_FRAME_LEN) - 1) >> 3) + 1 ) << 3)

#else
//  object frames get 4 byte aligned
#  define BD92ALIGN_OBJ_FRAME(OBJ_FRAME_LEN)   \
    (((((OBJ_FRAME_LEN) - 1) >> 2) + 1 ) << 2)

#endif

/*---------------------------------------------------------------------------*/
/* PTS 1107145 AK 10/07/2000 */
/* CRS 1108035 AK 13/10/2000 */  
#if COMPILEMODE_MSP00 >= FAST_MEO00 

/* PTS 1114891 FF 2002-03-14 */
#  define BD92CHECK_OBJ_POSITION( FUNC_NAME, TR_ERROR, OBJ_POSITION, NPTR) \
    if ((OBJ_POSITION < NODE_HEADER_MXBD00)       || \
        ((OBJ_POSITION + sizeof(tgg92_ObjHeader)) > BODY_END_BD00)) { \
	      TR_ERROR = e_invalid_oid;\
	      bd92WriteWrongPosToConsole (STATIC_CAST(const char *,FUNC_NAME), \
            OBJ_POSITION, NPTR.np_ptr()->nd_first_obj(), NPTR.np_ptr()->nd_obj_frame_len()); \
          eo670_CTraceStack(); \
	      return; \
       } \
    else if ((OBJ_POSITION < NPTR.np_ptr()->nd_first_obj()) ||  \
        ((OBJ_POSITION + NPTR.np_ptr()->nd_obj_frame_len()) > BODY_END_BD00) || \
        ((OBJ_POSITION - NPTR.np_ptr()->nd_first_obj()) % \
        BD92ALIGN_OBJ_FRAME (NPTR.np_ptr()->nd_obj_frame_len()) != 0)) { \
	      TR_ERROR = e_illegal_object_pos;\
	      bd92WriteWrongPosToVtrace (STATIC_CAST(const char *,FUNC_NAME), \
              OBJ_POSITION, NPTR.np_ptr()->nd_first_obj(), NPTR.np_ptr()->nd_obj_frame_len()); \
	      return; \
	}


/* PTS 1114891 FF 2002-03-14 */
#  define BD92CHECK_OID_POSITION( FUNC_NAME, TR_ERROR, OID, NPTR)            \
    if ((OID.gg92GetPos() < NODE_HEADER_MXBD00)       || \
        ((OID.gg92GetPos() + sizeof(tgg92_ObjHeader)) > BODY_END_BD00)) { \
	      TR_ERROR = e_invalid_oid;\
	      bd92WriteWrongOidToConsole (STATIC_CAST(const char *,FUNC_NAME),OID); \
          eo670_CTraceStack(); \
	      return; \
       } \
    else if ((OID.gg92GetPos() < NPTR.np_ptr()->nd_first_obj()) ||  \
        ((OID.gg92GetPos() + NPTR.np_ptr()->nd_obj_frame_len()) > BODY_END_BD00) || \
        ((OID.gg92GetPos() - NPTR.np_ptr()->nd_first_obj()) % \
        BD92ALIGN_OBJ_FRAME (NPTR.np_ptr()->nd_obj_frame_len()) != 0)) { \
	      TR_ERROR = e_illegal_object_pos;\
	      bd92WriteWrongOidToVtrace (STATIC_CAST(const char *,FUNC_NAME),OID); \
	      bd92WriteWrongPosToVtrace (STATIC_CAST(const char *,FUNC_NAME), \
            OID.gg92GetPos(), NPTR.np_ptr()->nd_first_obj(), NPTR.np_ptr()->nd_obj_frame_len()); \
	      return; \
	}
 
#else

#  define BD92CHECK_OBJ_POSITION( FUNC_NAME, TR_ERROR, OBJ_POSITION, NPTR)

#  define BD92CHECK_OID_POSITION( FUNC_NAME, TR_ERROR, OID,          NPTR)
        
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp bool 
bd92ObjPosIsValid (tgg00_ObjPagePos  ObjPos,
                   tgg00_ObjPagePos  FirstObjPos,
                   tsp00_Uint2       ObjFrameLen);

/*---------------------------------------------------------------------------*/

externC void
bd92VerifyObjFile (tbd_current_tree       &curr,
				   boolean                 bConverterUpdate);

/*----------------------------------------------------------------------------*/

externCpp void
bd92VerifyObjPage (cbd05_Current  &Current,
				   tbd_node_ptrs     &NptrsPage);

/*---------------------------------------------------------------------------*/

externCpp void
bd92VtraceObjKey (tgg00_TransContext    &Trans, 
                  tsp00_C8              &Fileno,
				  tsp00_Int4             KeyLen,
				  void                  *pKey,
				  tgg00_VtraceType_Enum  TraceObject);

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongPosToConsole (const char       *funcName, 
                            tgg00_ObjPagePos  ObjPos,
                            tgg00_ObjPagePos  FirstObjPos,
                            tsp00_Uint2       ObjFrameLen);

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongPosToVtrace (const char       *funcName, 
                           tgg00_ObjPagePos  ObjPos,
                           tgg00_ObjPagePos  FirstObjPos,
                           tsp00_Uint2       ObjFrameLen);

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongOidToConsole (const char *            funcName, 
                            const tgg92_KernelOid   &OId);

/*----------------------------------------------------------------------------*/

externCpp void
bd92WriteWrongOidToVtrace (const char *            funcName, 
                           const tgg92_KernelOid   &OId);

/*----------------------------------------------------------------------------*/

#endif  /* HBD92_H */
