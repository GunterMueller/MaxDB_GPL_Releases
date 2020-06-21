#
# Python/vmake.com
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

regex(.*[.]py$) ascii ->$TOOL/lib/Python/vmake/

oref.py     ascii exec ->$TOOL/bin/
objHull.py  ascii exec ->$TOOL/bin/
irename.py  ascii exec ->$TOOL/bin/
incusec.py  ascii exec ->$TOOL/bin/
imkdep.py   ascii exec ->$TOOL/bin/
parcel.py   ascii exec ->$TOOL/bin/
checkConcom.py   ascii exec ->$TOOL/bin/
wdfprot.py  ascii exec ->$TOOL/bin/
getLcProtocols.py ascii exec ->$TOOL/bin/
minimizeIncludes.py ascii exec ->$TOOL/bin/

