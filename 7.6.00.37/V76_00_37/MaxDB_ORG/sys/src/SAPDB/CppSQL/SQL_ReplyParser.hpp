/*!
 * \file    SQL_ReplyParser.hpp
 * \author  PeterG (GoertzP)
 * \brief   SQL reply parser
 */

/*

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



*/


#ifndef SQL_REPLYPARSER_HPP
#define SQL_REPLYPARSER_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef HIN204_H
#include "PacketInterface/PIn_ReplyParser.h"
#endif


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SQL_SessionContext;
class SQL_Statement;


/************************************************************************/
/* Class SQL_ReplyParser                                                */
/************************************************************************/

class SQL_ReplyParser : public  PIn_ReplyParser
{
  SQL_SessionContext & m_sessCtxt;
  SQL_Statement &      m_stmt;
public:
  SQL_ReplyParser( SQL_SessionContext & sessionContext,
                   SQL_Statement & stmt );
  virtual int        ParseReplyData(tsp1_packet * m_reply_packet);
  virtual AcceptKind HandleData( int argCount, int dataLen,
                                 const teo00_Byte * dataPtr );
  virtual AcceptKind HandleErrortext( int argCount, int len,
                                      const char * data );
  virtual AcceptKind HandleLongdata( int argCount, int dataLen,
                                     const tsp00_Byte * dataPtr );
  virtual AcceptKind HandleParsid( int argCount, int len,
                                   const tin02_ParseId * data );
  virtual AcceptKind HandleParsidOfSelect( int argCount, int len,
                                           const tin02_ParseId * data );
  virtual AcceptKind HandleResultcount( int argCount, int len,
                                        long data );
  virtual AcceptKind HandleResulttablename( int argCount, int len,
                                            const char * data );
  virtual AcceptKind HandleShortinfo( int argCount, int len,
                                      const tsp1_param_info * data );
};

#endif /* SQL_ReplyParser */

