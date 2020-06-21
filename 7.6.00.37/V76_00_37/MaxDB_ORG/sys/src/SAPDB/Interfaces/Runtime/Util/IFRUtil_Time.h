/*!
  @file           IFRUtil_Time.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Supplemental functions for getting the current time
                These are normally RTE functions, but currently not
                available in 'Research' tree.

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFRUTIL_TIME_H
#define IFRUTIL_TIME_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_Types.h"
// Use this if RTE is there 

#ifdef HAVE_RTESYS_MICROTIME
#  include <RunTime/Sytem/RTESys_MicroTime.h> /*nocheck*/
#endif

IFR_BEGIN_NAMESPACE

/**
 * Wrapper around commonly used time measurement functions. These map to
 * RTE functions, when available and present.
 */
class IFRUtil_Time
{
public:
    /**
     * Writes a time stamp in ISO format into the given string.
     * The string must have room for at least 27 characters, including 
     * the terminating zero byte.
     * @param dest The destination string.
     */
    static void currentTimeStamp(char *dest);
    
    /**
     * Returns a time stamp that has a resolution in micro seconds.
     * The returned number may or may not have a relation to the 
     * real-world time and shall only be used for relative measurement.
     * @return A time stamp in micro second resolution (1 is 1 micro second).
     */
    static IFR_UInt8 currentMicroSeconds();
};


IFR_END_NAMESPACE

#endif  /* IFRUTIL_TIME_H */
