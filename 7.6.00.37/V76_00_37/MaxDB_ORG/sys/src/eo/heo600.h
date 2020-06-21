/*!
  @file           heo600.h
  @author         JoegM
  @special area   Unicode Kernel LZU Extensions
  @brief          Contains all unicode extensions of kernel LZU needed
  @List of implemented functionsvmessageUnicode

  vopmsgUnicode
  @first created  000-08-04  19:47

\if EMIT_LICENCE

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



\endif
*/



#ifndef HEO600_H
#define HEO600_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp03.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @Description    Unicode variant of vopmsg


  Accepted for msgEncoding are csp_unicode, csp_unicode_swap, csp_ascii and csp_utf8.
  The function trusts the caller, that he uses a 7-Bit ASCII (or UTF8) msgLabel...
  Size of msgText in vmessage was 40 characters (tsp00_C40). Since UCS2 characters
  needed twice as much, tsp00_C80 allows the same amount of text in UCS2.
   @param          msgEncoding [in] Encoding of msgText
   @param          msgPrio [in] Message priority
   @param          msgType [in] Message type (info, warning or error)
   @param          msgNo [in] Message number
   @param          msgLabel [in] 7-Bit ASCII (or UTF8) fixed size label
   @param          msgText [in] Space padded msgText

 */

externC void vmessageUnicode( 
              tsp00_Int4              msgEncoding ,
              tsp3_priority           msgPrio ,
              tsp3_msg_type           msgType ,
              tsp00_Int4              msgNo ,
              tsp00_C8  VAR_ARRAY_REF msgLabel ,
              tsp00_C80 VAR_ARRAY_REF msgText );

/*!
   @Description    Unicode variant of vopmsg


  The additional arguments npar1,npar2 and spar are no longer supported.
  All vopmsg calls use '0','0',blank_string anyhow, which means to ignore them...
  Accepted encodings are csp_unicode,csp_unicode_swap,csp_ascii,csp_utf8.
  Size of msgText in vopmsg was 64 characters. Since there was no tsp00_C128
  the next higher size was choosen, which is 132...
   @param          msgEncoding [in] Encoding of msgText
   @param          msgPrio [in] Message priority
   @param          msgNo [in] Message number
   @param          msgText [in] Space padded msgText

 */

externC void vopmsgUnicode( tsp00_Int4               msgEncoding ,
                            tsp00_Int2               msgPrio ,
                            tsp00_Int2               msgNo ,
                            tsp00_C132 VAR_ARRAY_REF msgText );

#endif  /* HEO600_H */
