/*!*********************************************************************

    @file         Proc_SQL.hpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#ifndef PROC_SQL_HPP
#define PROC_SQL_HPP


class SQLMan_Context;
class SAPDBErr_MessageList;

/*!

  @class Proc_SQL
  @brief Access to tables to configure UDE server

 */
class Proc_SQL
{
public:
    static bool selectUdeServerConfig (
        SQLMan_Context * context,
        const char     * languageString,
        char           * serverKind,
        char           * commandLine,
        char           * extraInfo,
        int            * sessionLimit,
        SAPDBErr_MessageList * errInfo);

private:
    Proc_SQL ();
    ~Proc_SQL ();

};


#endif
