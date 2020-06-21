/*!================================================================

 module:        Util.h

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Example: Utility for testing

 ------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




 ===================================================================*/

#include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"

void Util_PrintError(const char *str, const IFR_ErrorHndl *err);
void Util_SPrintError(char *buf, const char *str, const IFR_ErrorHndl *err);
/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to UCS2
    
  Arguments:
    szTCHAR     [out] output buffer (TCHAR)
    maxLenTCHAR [in]  maximum length of szTCHAR
    szStr       [in]  null terminated input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertToUCS2(unsigned short *szTCHAR, int maxLenTCHAR, char *szStr);

/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to UCS2
    
  Arguments:
    szTCHAR     [out] output buffer (TCHAR)
    maxLenTCHAR [in]  maximum length of szTCHAR
    swapped     [in]  set to true if the byte order of the output buffer 
                      should be in swapped byte order
    szStr       [in]  null terminated input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertToUCS2(unsigned short *szTCHAR, int maxLenTCHAR, IFR_Bool swapped, char *szStr);
