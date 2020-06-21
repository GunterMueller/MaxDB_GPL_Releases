/**************************************************************************/
/*!

  @file         Log_IDiagnose.hpp
  @author       MartinB
  @ingroup      Logging
  @brief        Interface for diagnosis of the Log_Volume. The Implementation of this Interface is
                done in Log_Volume.
  @since        2003-03-18
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


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


#ifndef LOG_IDIAGNOSE_HPP
#define LOG_IDIAGNOSE_HPP

#include "Logging/Log_Page.hpp"

/*!
   @interface    Log_IDiagnose
   @brief        Interface for diagnosis of the Log_Volume. The Implementation of this Interface is
                 done in Log_Volume.
*/
class Log_IDiagnose
{
public:
    /// returns the singleton for single-page access
    static Log_IDiagnose& Instance();
    /// Allocates a pageframe to a given Log_Page. Remember to release the memory later!
    /// Returns false, if the method fails
    virtual bool AllocateLogPageframe(Log_Page & logPage) = 0;
    /// Deallocates a pageframe to a given Log_Page
    virtual void ReleaseLogPageframe(Log_Page & logPage) = 0;
    /// Reads the specified page from the Log_Volume into the frame. The Volume is attached if neccessary.
    /// Returns false, if the method fails
    virtual void ReadLogPage(const tsp00_TaskId taskid,
                                   Log_Page & logPage,
                             const Log_RawDeviceOffset offset) = 0;
    /// Writes the specified page onto the Log_Volume. The Volume is attached if neccessary.
    /// Returns false, if the method fails
    virtual void WriteLogPage(const tsp00_TaskId taskid,
                                    Log_Page & logPage,
                              const Log_RawDeviceOffset offset) = 0;
};
#endif  /* LOG_IDIAGNOSE_HPP */

