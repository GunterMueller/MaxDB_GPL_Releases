/****************************************************************************

  module      : vsp36.c

  -------------------------------------------------------------------------

  responsible:   DanielD
 
  special area:  RTE | Basic Runtime Support
 
  description:   Template routines too large for inlining

  first created:2000-06-05  12:53
  last changed: 2000-06-05  17:24

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

#include <limits.h>

#include "gsp00_0.h"
#include "gsp00_2.h"

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
 *  METHODS                                                                  *
 *===========================================================================*/


/**\
------------------------------------ ROUTINE_DEBUG chapter SET
\**/

static void sp36_AddElement (
    unsigned char *data,
    int            elem)
{
    int targetByte = elem / CHAR_BIT;
    unsigned char compareByte = 1 << (elem - (targetByte * CHAR_BIT));
    data [targetByte] |= compareByte;
}

/*----------------------------------------*/
externC void sp36_BuildSet (
    unsigned char *data,
    int maxVal,
    int argc,
    va_list argptr)
{
    int    elem;

    memset (data, '\0', (maxVal + CHAR_BIT) / CHAR_BIT);
    while (argc > 0) {
        elem = va_arg (argptr, int);
#if COMPILEMODE_MSP00 >= QUICK_MSP00
        if ( (0 > elem) || (elem > maxVal) )
          sqlk_rangeviolation (elem, 0, maxVal, __LINE__, __FILE__);
#endif
        sp36_AddElement (data, elem);
        --argc;
    }
}

/*----------------------------------------*/

externC void sp36_BuildSmallSet (
    unsigned char *dataPtr,
    int maxVal,
    int argc,
    va_list argptr)
{
    int    elem;
    unsigned char data;
    unsigned char compareByte;

    data = '\0';
    while (argc > 0) {
        elem = va_arg (argptr, int);
#if COMPILEMODE_MSP00 >= QUICK_MSP00
        if ( (0 > elem) || (elem > maxVal) )
          sqlk_rangeviolation (elem, 0, maxVal, __LINE__, __FILE__);
#endif
        compareByte = 1 << elem;
        data |= compareByte;
        --argc;
    }
    *dataPtr = data;
}

/*----------------------------------------*/

externC void sp36_c2p (
    void * instanceData,
    int    instanceSize,
    const char * str)
{
    int len = strlen (str);

    if (len > instanceSize)
        len = instanceSize;
    memcpy (instanceData, str, len);
    if (len < instanceSize)
        memset ((char *) instanceData + len, ' ', instanceSize - len);
}

/*----------------------------------------*/


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/