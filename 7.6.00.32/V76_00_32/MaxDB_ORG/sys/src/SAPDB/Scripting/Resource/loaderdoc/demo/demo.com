# 
#
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

ISO_8859_1.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_2.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_3.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_4.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_5.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_6.TXT      ascii unpack=no ->$OWN/usr/env/
ISO_8859_7.TXT      ascii unpack=no ->$OWN/usr/env/
loader.cfg          ascii unpack=no ->$OWN/usr/config/
transport.py        ascii unpack=no ->$OWN/usr/lib/python2.3/sdb/loader/
transport.par       ascii unpack=no ->$OWN/usr/lib/test/sdb/loader/test/
transport.cfg       ascii unpack=no ->$OWN/usr/lib/test/sdb/loader/test/
regex(*)            ascii unpack=no ->$OWN/usr/lib/test/sdb/loader/demo/
regex(*)            ascii unpack=no ->$OWN/test/sdb/loader/demo/


