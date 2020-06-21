# @(#)Scripting/Resource.com
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2002-2005 SAP AG
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

sdb__init__.py ascii ->$INSTROOT/runtime/python/sdb/__init__.py
sdb_python__init__*.py ascii ->$INSTROOT/runtime/python/sdb/python%-/__init__.py
sdb__init__2.py ascii ->$INSTROOT/lib/python2.3/sdb/__init__.py

sql.py ascii ->$INSTROOT/runtime/python/sdb/
dbm.py ascii ->$INSTROOT/runtime/python/sdb/
loader.py ascii ->$INSTROOT/runtime/python/sdb/

sapdb__init__.py ascii output=$INSTROOT/runtime/python/sapdb/__init__.py,$INSTROOT/runtime/python/maxdb/__init__.py
dbmtop.py ascii output=$INSTROOT/runtime/python/sapdb/dbm.py,$INSTROOT/runtime/python/maxdb/dbm.py
loadertop.py ascii output=$INSTROOT/runtime/python/sapdb/loader.py,$INSTROOT/runtime/python/maxdb/loader.py
sqltop.py ascii output=$INSTROOT/runtime/python/sapdb/sql.py,$INSTROOT/runtime/python/maxdb/sql.py
dbapitop.py ascii output=$INSTROOT/runtime/python/sapdb/dbapi.py,$INSTROOT/runtime/python/maxdb/dbapi.py

README_python.txt ascii ->$INSTROOT/runtime/python/README.txt
README_perl.txt ascii ->$INSTROOT/runtime/perl/README.txt

python*.exp ascii <-$TOOL/incl/extern/python/sdk%-/python.exp


