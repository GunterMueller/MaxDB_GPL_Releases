
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

sql_execute grant INSERT,UPDATE on SAPR3.DBATL    to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAML    to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAOBJL  to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAPHAL  to PUBLIC

sql_execute grant SELECT        on SAPR3.DBAFID   to PUBLIC
sql_execute grant SELECT        on SAPR3.DBAGRP   to PUBLIC
sql_execute grant SELECT        on SAPR3.DBATRIAL to PUBLIC
sql_execute grant SELECT        on SAPR3.DBAERR   to PUBLIC

sql_execute grant INSERT,UPDATE on SAPR3.DBASPAL  to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAREOL  to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAEXTL  to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBABARL  to PUBLIC
sql_execute grant INSERT,UPDATE on SAPR3.DBAOPTL  to PUBLIC
