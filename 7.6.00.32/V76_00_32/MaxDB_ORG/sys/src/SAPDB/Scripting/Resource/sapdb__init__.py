#
#
#    ========== licence begin  GPL
#    Copyright (c) 2003-2005 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end

__all__ = [
    'sql', 'dbm', 'loader', 'dbapi',
    ]


#
# compatibility for those doing 'import sapdb'
#
def connect (*args):
    _loadGlobals ()
    return apply (sql.connect, args)

def utilitySession (*args):
    _loadGlobals ()
    return apply (sql.utilitySession, args)

def _loadGlobals ():
    global sql, CommunicationError, SQLError
    global SapDB_LongReader, SapDB_Prepared, SapDB_ResultSet, SapDB_Session
    global _buildInfo, connect, utilitySession
    import sql
    CommunicationError = sql.CommunicationError
    SQLError = sql.SQLError
    SapDB_LongReader = sql.SapDB_LongReader
    SapDB_Prepared = sql.SapDB_Prepared
    SapDB_ResultSet = sql.SapDB_ResultSet
    SapDB_Session = sql.SapDB_Session
    _buildInfo = sql._buildInfo
    connect = sql.connect

