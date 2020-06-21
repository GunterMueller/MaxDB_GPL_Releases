
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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



# Inputfile for QueryRewrite_Test
#
# > View definitions 
#
# Existing tables are T1 ... T10 with columns (K int primary key, A1 int, ..., An int)
#  where Tn has columns up to An
# Definitions may range over more than one line und must stop with an '!' 
# Anything in a line after '#' will be ignore
#
# Tables as well as Views are created in schema QRW by default
# 

CREATE VIEW V1 AS SELECT a1 FROM T1 UNION ALL SELECT a2 FROM T3 WHERE K <> 100 !

CREATE VIEW V2 AS SELECT k FROM T2 WHERE a2>100 INTERSECT SELECT * FROM V1 WHERE a1<100 !
