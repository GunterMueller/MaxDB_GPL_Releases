# :tt/cpan/SAPDB/Install/zlib.com
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

* ascii
zconf.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/zconf.h ascii
zlib.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/zlib.h ascii
zutil.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/zutil.h ascii
deflate.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/deflate.h ascii
infblock.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/infblock.h ascii
infcodes.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/infcodes.h ascii
inffast.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/inffast.h ascii
inffixed.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/inffixed.h ascii
inftrees.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/inftrees.h ascii
infutil.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/infutil.h ascii
trees.h ->$WRK/fast/obj/tt/cpan/SAPDB/Install/zlib/trees.h ascii
