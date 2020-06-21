/*!================================================================

 module:        Util.cpp

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

#include <stdio.h>
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/tests/Util.h"

void Util_PrintError(const char *str, const IFR_ErrorHndl *err)
{
  static char buf[1024];
  const char * szText = err->getErrorText();
  sp77sprintfUnicode(sp77encodingUTF8, buf, sizeof(buf), "ERROR: %s (%d):%s\n", str,  
                     err->getErrorCode(), err->getErrorText());
  printf(buf);
};

void Util_SPrintError(char *buf, const char *str, const IFR_ErrorHndl *err)
{
    sp77sprintfUnicode(sp77encodingUTF8, buf, 1024, "ERROR: %s (%d):%s\n", str,  
                       err->getErrorCode(), err->getErrorText());
}

/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to TCHAR depending on compile switch -G unicode
    
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

int convertToUCS2(unsigned short *szTCHAR, int maxLenTCHAR, char *szStr) {
  IFR_Bool swapped = (sp77encodingUCS2Swapped == sp77encodingUCS2Native) ? true : false;
  return convertToUCS2(szTCHAR, maxLenTCHAR, swapped, szStr);
}

int convertToUCS2(unsigned short *szTCHAR, int maxLenTCHAR, IFR_Bool swapped, char *szStr)
{
  tsp78ConversionResult ConvRes = sp78_Ok; 
  unsigned int   dlen, slen;
  const tsp77encoding *encoding = (swapped) ? sp77encodingUCS2Swapped : sp77encodingUCS2;
  ConvRes = sp78convertString(encoding, (char*) szTCHAR, maxLenTCHAR, &dlen, true,			     
			     sp77encodingAscii, szStr, strlen(szStr), &slen);
  if (sp78_Ok != ConvRes)
  {
    return (-1);
  }
  return (0);
}

