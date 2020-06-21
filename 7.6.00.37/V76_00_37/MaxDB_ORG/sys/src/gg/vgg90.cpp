/* @lastChanged: "1998-12-23  11:37"

  * @filename:   vgg90.cpp
  * @purpose:    "GG_kernel_templates"
  * @release:    7.1.0.0
  * @see:        ""
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

#include "ggg90.h"

/*===========================================================================*
 *  TRACE FUNCTIONS                                                          *
 *===========================================================================*/

void gg90StrToLine (const _TCHAR *StrVal,
                    int          &LineLen,
                    tsp00_Line   &Line)
{
    int len = (int)_tcslen (StrVal) * sizeof (_TCHAR);
    
    if (LineLen + len <= sizeof (Line))
    {
        memcpy (Line + LineLen, StrVal, len);
        LineLen += len;
    }
}

/*----------------------------------------------------------------------------*/

void gg90IntToLine (tsp00_Int4  IntVal,
                     int        &LineLen,
                     tsp00_Line &Line)
{
    int    len;
    _TCHAR AuxStr[20];
    
    _itot (IntVal, AuxStr, 10);
    
    len = (int)_tcslen (AuxStr) * sizeof (_TCHAR);
    
    if (LineLen + len <= sizeof (Line))
    {
        memcpy (Line + LineLen, AuxStr, len);
        LineLen += len;
    }
}

/*----------------------------------------------------------------------------*/

void gg90HexToLine (unsigned char  CharVal,
                     int           &LineLen,
                     tsp00_Line    &Line)
{
    int    len;
    _TCHAR AuxStr[5];
    
    _itot (CharVal, AuxStr, 16);
    
    len = (int)_tcslen (AuxStr);
    
    if (len == 1 && LineLen + sizeof (_TCHAR) < sizeof (Line))
    {
        Line [LineLen++] = _T('0');
    }
    
    if (LineLen + len * sizeof (_TCHAR) <= sizeof (Line))
    {
        for (int i = 0; i < len; i++)
        {
            AuxStr [i] = _totupper (AuxStr [i]);
        }
        
        memcpy (Line + LineLen, AuxStr, len * sizeof (_TCHAR));
        LineLen += len;
    }
}


/*===========================================================================*
 *  I N S T A N T I A T I O N                                                *
 *===========================================================================*/

/*
* Instantiate templates so other modules
*    have no need for the definition
*    of member functions
*/

#if defined(WIN32)
template class tgg90_Cint<8>;
#endif
