/* @lastChanged: "1998-02-24 10:15"

  * @filename:   hak263.h
  * @purpose:    "Stream Handle Functions"
  * @release:    7.1.0.0
  * @see:        "-.-"
  *
  * @Copyright (c) 1998-2005 SAP AG"


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


*/

#ifndef HAK_263_H
#define HAK_263_H

/* ========================================================================== */
/* ==========                   INCLUDES                           ========== */
/* ========================================================================== */


#include "Oms/OMS_Defines.h"
#include "livecachetypes.h"     /* no check */
#include "vak001.h"


/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

struct ak263_StreamHandle {
    void*                 rowDef;
    OmsTypeABAPTabHandle  tabHandle;
};

/* ========================================================================== */
/* ==========                EXPORT FUNCTION DECLARATION           ========== */
/* ========================================================================== */

externC
void ak263CheckHandle (tak_all_command_glob& acv,
            tsp00_Int4&          st_ix,
            ak263_StreamHandle*  phandle,
            int                  handleLen,
            bool                 isABAP,
            tgg00_BasisError&    e,
            tsp00_Int4           paramNo,
            tsp00_KnlIdentifier& methodName);

externC
tsp00_Int4 ak263EvalHandleLength (tsp00_Int4 ColCount);

externC
void ak263NumberToInt8 (
           tsp00_MoveObjPtr                   pNumber,
           int                                inOutLength,
           tsp00_8ByteCounter  VAR_VALUE_REF  result,
           tsp00_NumError      VAR_VALUE_REF  e);

externC
void ak263NumberToUInt8 (
           tsp00_MoveObjPtr                   pNumber,
           int                                inOutLength,
           tsp00_8ByteCounter  VAR_VALUE_REF  result,
           tsp00_NumError      VAR_VALUE_REF  e);

externC
void ak263Int8ToNumber (
     tsp00_8ByteCounter                 source,
     tsp00_MoveObjPtr                   pNumber,
     int                                inOutLength,
     tsp00_NumError      VAR_VALUE_REF  e);

externC
void ak263UInt8ToNumber (
     tsp00_8ByteCounter                 source,
     tsp00_MoveObjPtr                   pNumber,
     int                                inOutLength,
     tsp00_NumError      VAR_VALUE_REF  e);

externC
void ak263WriteConversionError (tak_all_command_glob& acv,
                                ak263_StreamHandle*   phandle,
                                int                   paramNo,
                                tsp00_Int4            tabHandle,
                                tsp00_KnlIdentifier&  methodName);

externC void ak263WriteTranslationError (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  methodName,
    integer                              paramNo,
    integer                              errorOffset,
    integer                              errorChar);
 
#endif
