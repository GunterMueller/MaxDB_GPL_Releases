# @(#)   desc/su.com   7.4   02/06/02
#
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

* ->$TEST/jtest/sut/

* ascii noversion unpack=no concom=no

regex(.*\.diffscr)  concom=yes
regex(.*\.pl)       concom=yes exec
regex(.*\.py)       concom=yes exec

:su/COLUMNS_1024.vdnts ascii concom=yes unpack=no ->$OWN/test/kern/sut/ 
:su/SYSDD80.vdnts ascii concom=yes unpack=no ->$OWN/test/kern/sut/
:su/TRIGGER.vdnts ascii concom=yes unpack=no ->$OWN/test/kern/sut/
:su/UNICODE.vdnts ascii concom=yes unpack=no ->$OWN/test/kern/sut/


