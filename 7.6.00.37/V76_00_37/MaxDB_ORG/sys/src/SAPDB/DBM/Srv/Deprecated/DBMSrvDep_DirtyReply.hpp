/*! 
    \file    DBMSrvDep_DirtyReply.hpp
    \author  MarcW
    \ingroup decreprecated classes of the DBMServer

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

*/

#if !defined(DBMSrvDep_DirtyReply_hpp)
#define DBMSrvDep_DirtyReply_hpp

#include "DBM/Srv/DBMSrv_Reply.hpp"

/*!
    @brief a DBMSrv_Reply, that has the constant maximal length of 
*/
class DBMSrvDep_DirtyReply: public DBMSrv_Reply {
public:
    DBMSrvDep_DirtyReply( char * replyData, int  * replyLength)
        : DBMSrv_Reply(replyData, replyLength, DBMSrvDep_DirtyReply::REPLYLENGTHMAX){
    }

    static const int REPLYLENGTHMAX;
};

#endif //DBMSrvDep_DirtyReply_hpp
