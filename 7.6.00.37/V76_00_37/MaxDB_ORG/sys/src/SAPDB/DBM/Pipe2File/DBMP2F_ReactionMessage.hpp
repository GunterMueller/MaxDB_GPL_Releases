/*******************************************************************************
  module:       DBMP2F_ReactionMessage.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements messages as reactions for pipe2file
  last change:  2002-06-25 07:42
  version:      7.4.3
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG



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




*******************************************************************************/

#if !defined(_DBMP2F_ReactionMessage_hpp)
#define _DBMP2F_ReactionMessage_hpp


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Pipe2File/DBMP2F_Reaction.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_ReactionMessage
// -----------------------------------------------------------------------------

class DBMP2F_ReactionMessage: public DBMP2F_Reaction
{
  public:
    DBMP2F_ReactionMessage(const char * TheMessage);

    int React(void);
    int React(const char * Info, int & Answer);
    int React(teo00_Uint8 Info, time_t TimeInfo);

  private:
    const char * Message;
};

#endif
