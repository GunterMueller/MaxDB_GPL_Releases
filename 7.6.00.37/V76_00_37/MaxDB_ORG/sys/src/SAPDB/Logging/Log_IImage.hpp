/*!
   @file           Log_IImage.hpp
   @author         UweH
   @ingroup        Logging
   @brief          defines the class Log_IImage

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



#ifndef Log_IImage_H
#define Log_IImage_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"

class Data_SplitSpaceWriter;
class Data_SplitSpaceReader;
class Msg_List;

/*!
   @interface      Log_IImage
   @brief          common image interface
 */
class Log_IImage
{

public:
    /// returns the action type
    virtual Log_ActionType GetActionType() const = 0;

    /// returns the action version
    virtual Log_ActionVersion GetPersistentVersion() const = 0;
    
    /// returns the local entry sequence
    virtual Log_EntrySequence GetSequence() const = 0;

    /// the image is written as persistent layout
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const = 0;

    /// the image is read from persistent layout
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader &Reader,
                                       SAPDBErr_MessageList  &errlist) = 0;

    /// the action is written to the trace file
    virtual void WriteToTrace (const char * title = 0) const = 0;

    /// append some info to the errlist
    virtual void AppendContextInfo (Msg_List                  &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;

    /// returns true, if image is an after image.
    virtual bool IsAfterImage () const = 0;
};
#endif  /* Log_IImage_H */
