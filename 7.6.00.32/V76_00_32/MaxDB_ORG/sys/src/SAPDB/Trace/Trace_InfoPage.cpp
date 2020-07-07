/*!**************************************************************************

  module       : Trace_InfoPage.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Handling of trace entries

  description : Function to create a trace info page from C

  last changed: 2000-12-19  8:43
  see also    : example.html ...
  first created:2000-12-18  20:31

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Trace/Trace_InfoPage.h"
#include "Trace/Trace_InfoPage.hpp"
#include "geo50_0.h"
#include "geo002.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

bool Trace_InfoPage::GetInfoPage(tsp00_PageAddr &infoPage)
{
  bool isValid = ( KGS && KGS->traceBufferSpace );

  if ( isValid)  
  {
    infoPage  = KGS->traceBufferSpace;
  }               
  return isValid;
}

/*------------------------------------------------------------*/

bool Trace_InfoPage::GetUktTraceBuffer(SAPDB_Int4 uktId,
                                       tsp00_PageAddr &firstPage,
                                       SAPDB_Int4 & sizeInPages)
{
  bool isValid = ( KGS && KGS->traceBufferSpace );

#if defined(_WIN32)
#  define FIRST_UKT pFirstUKTCtrl
#else
#  define FIRST_UKT first_kp
#endif

  isValid = ( uktId >= 1 && uktId <= (tsp00_Int4)KGS->ulNumOfUKTs );

  if ( isValid )
  {
    firstPage   = (KGS->FIRST_UKT + (uktId - 1) )->traceBuffer;
    sizeInPages = (KGS->FIRST_UKT + (uktId - 1) )->traceBufferSizeInPages;
  }

 return isValid;
}

/*------------------------------------------------------------*/

externC void Trace_InfoPageSetup()
{
  Trace_InfoPage::Layout info;
  
  info.pageSize = XPARAM(ulDBPageSize);
  info.uktCount = KGS->ulNumOfUKTs;
  for ( tsp00_Int4 uktIndex = 0; uktIndex < info.uktCount; uktIndex++ )
  {
#if defined(_WIN32)
    info.uktTraceSizeInPages[uktIndex] = KGS->pFirstUKTCtrl[uktIndex].traceBufferSizeInPages;
#else
    info.uktTraceSizeInPages[uktIndex] = KGS->first_kp[uktIndex].traceBufferSizeInPages;
#endif
  }

  Trace_InfoPage infoPage;
  infoPage.AssignNewPageAndFormat((tsp00_PageAddr)KGS->traceBufferSpace, info);

  // Now we can flag need for saving on crash
  KGS->traceNeedsSaving = true;
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
