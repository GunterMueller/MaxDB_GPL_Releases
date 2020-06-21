#
# expat - James Clark's XML Parsing
#
#
#    ========== licence begin LGPL
#    Copyright (C) 2000 SAP AG
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

&if $MACH in [ I386, ALPHA ]
* -DXML_BYTE_ORDER=12
&else
* -DXML_BYTE_ORDER=21
&endif

asciitab.h 	inc=

*hashtable.h 	inc=	PTS 1109175
expat.h         inc=
iasciitab.h 	inc=
latin1tab.h 	inc=
nametab.h 	inc=
utf8tab.h 	inc=
*xmldef.h 	inc=		PTS 1109175
*xmlparse.h 	inc=	PTS 1109175
xmlrole.h 	inc=xmltok.h
xmltok.h 	inc=
xmltok_impl.h 	inc=
xmltok_impl_c.h	inc=
xmltok_ns_c.h 	inc=
config.h    inc=

*hashtable.c 	inc=xmldef.h,hashtable.h	PTS 1109175
xmlparse.c 	inc=xmltok.h,xmlrole.h,expat.h,config.h
xmlrole.c 	inc=xmlrole.h,ascii.h,config.h
xmltok.c 	dep=xmltok_ns_c.h inc=ascii.h,xmltok.h,nametab.h,xmltok_impl.h,asciitab.h,\
utf8tab.h,iasciitab.h,latin1tab.h,xmltok_impl_c.h,config.h
