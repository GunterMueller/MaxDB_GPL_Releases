#
# expat - James Clark's XML Parsing
#
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

ascii.h 			inc=
asciitab.h 			inc=
expat.h				inc=:idl/expat/expat_external.h
expat_external.h	inc=
iasciitab.h 		inc=
internal.h			inc=
latin1tab.h 		inc=
macconfig.h			inc=
nametab.h 			inc=
utf8tab.h 			inc=
winconfig.h			inc=
xmlrole.h 			inc=:idl/expat/xmltok.h
xmltok.h 			inc=
xmltok_impl.h 		inc=
config.h			inc=

xmltok_impl.c		inc=
xmltok_ns_c.h		inc=

xmlparse.c 			inc=:idl/expat/expat.h,:idl/expat/config.h,:idl/expat/internal.h,\
						:idl/expat/xmltok.h,:idl/expat/xmlrole.h
xmlrole.c 			inc=:idl/expat/xmlrole.h,:idl/expat/ascii.h,:idl/expat/config.h,\
						:idl/expat/internal.h,:idl/expat/expat_external.h
xmltok.c 			inc=:idl/expat/config.h,:idl/expat/expat_external.h,:idl/expat/internal.h,\
						:idl/expat/xmltok.h,:idl/expat/nametab.h,:idl/expat/xmltok_impl.h,\
						:idl/expat/ascii.h,:idl/expat/xmltok_impl.c,:idl/expat/asciitab.h,\
						:idl/expat/utf8tab.h,:idl/expat/iasciitab.h,:idl/expat/latin1tab.h,\
						:idl/expat/xmltok_ns_c.h
