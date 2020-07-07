
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

* -I$WRK/incl/extern/zlib

deflate.h    dep=zutil.h
infblock.h              
infcodes.h              
inffast.h               
inffixed.h              
inftrees.h              
infutil.h               
trees.h                 
zconf.h                 
zlib.h       dep=zconf.h
zutil.h      dep=zlib.h 

adler32.c    dep=zlib.h
compress.c   dep=zlib.h
crc32.c      dep=zlib.h
deflate.c    dep=deflate.h
gzio.c       dep=zutil.h
infblock.c   dep=zutil.h,infblock.h,inftrees.h,infcodes.h,infutil.h
infcodes.c   dep=zutil.h,inftrees.h,infblock.h,infcodes.h,infutil.h,inffast.h
inffast.c    dep=zutil.h,inftrees.h,infblock.h,infcodes.h,infutil.h,inffast.h
inflate.c    dep=zutil.h,infblock.h
inftrees.c   dep=zutil.h,inftrees.h,inffixed.h
infutil.c    dep=zutil.h,infblock.h,inftrees.h,infcodes.h,infutil.h
trees.c      dep=deflate.h,trees.h
uncompr.c    dep=zlib.h
zutil.c      dep=zutil.h
