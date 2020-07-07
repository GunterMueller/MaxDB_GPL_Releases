/*!========================================================================

  @file         vta321.cpp
  @ingroup      slowci
  @author       ElkeZ

  @brief        Pascal interface

  @since        2003-10-14  13:18
  @sa           

  ==========================================================================

  \if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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




  \endif
 ============================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include  "gta00.h"
#include  "hgg17.h"
#include  "hta320.h"
//
// next two to be excluded if 'new' messagelist can be used without old one
#include  "geo200.h"
#include  "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"
#include  "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  DECLARATIONS                                                             *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


externC void t321_GetError 
(
    tta_user_proc_globals  *pUpg,
    tsp1_part              *pError
) 
{
    SAPDBErr_MessageList    messageList;
    SAPDBErr_MessageList    tmpmessageList;
    SAPDB_Byte             *pMsgText;
    SAPDB_Bool              wasTruncated;

    
	pMsgText = REINTERPRET_CAST(SAPDB_Byte*, pError) + sizeof (pError->sp1p_part_header());
    if (messageList.Load (pError->sp1p_buf_len(), pMsgText, wasTruncated, tmpmessageList))
    {
        SAPDBErr_MessageList * pNextMsg;
        SAPDBErr_MessageList * pCurrMsg = &messageList;
        SAPDB_Char const     * pFileName;
        SAPDB_UTF8 const     * PErrorText;
        SAPDB_UInt4            lineNo;
        SAPDB_UInt4            neededSize;
        char                 * constText  = "in line       of file";
        char                   outline [182];

        do 
            {
            pNextMsg   = pCurrMsg->NextMessage();
            pFileName  = pCurrMsg->FileName();
            lineNo     = pCurrMsg->LineNumber();

            /*
            if (tasim_unicode == pUpg->term_simulation)
            {
            // messagelist will be transferred using UTF8, it has to be converted
            }
            */

            pUpg->prot.ln = pUpg->term->blankline;
            PErrorText = pCurrMsg->MessageWithArguments (sizeof(outline), outline, neededSize);
            if (neededSize > sizeof(pUpg->prot.ln)) 
                {
                neededSize = sizeof(pUpg->prot.ln);
                }
            SAPDB_MemCopyNoCheck (pUpg->prot.ln, &outline, neededSize);
            t320prot (*pUpg, neededSize);

            pUpg->prot.ln = pUpg->term->blankline;
            SAPDB_MemCopyNoCheck (pUpg->prot.ln, constText, strlen(constText)); 
            g17int4to_line (lineNo, false, 5, 9, pUpg->prot.ln);
            SAPDB_MemCopyNoCheck (&pUpg->prot.ln[22], pFileName, strlen(pFileName)); 
            t320prot (*pUpg, 22+strlen(pFileName));

            if (pNextMsg != NULL)   
                {
                pCurrMsg = pNextMsg;
                pNextMsg = pNextMsg->NextMessage();
                }
            else
                pCurrMsg = NULL;

            } while (pCurrMsg != NULL);

    }

}


/*---------------------------------------------------------------------------*/
