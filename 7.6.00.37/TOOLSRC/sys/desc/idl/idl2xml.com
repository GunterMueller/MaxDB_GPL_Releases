# @(#)   desc/IdlChecker.com   73.03.xx  5-JUN-2000
#
#
#regex(.*\.c$) -I$WRK/incl/idl/idl2xml
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


&if $OSSPEC in [ WIN32 ]
* -DMS_DOS
&endif

pminit.c     inc=pen00.h,mam.h,rte.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmdesc.h,pmstck.h  
pmlprim.c    inc=pen00.h,mam.h,rte.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmsmsg.h  
pmmadm.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmradm.h,pms32.h  
pmprim.c    inc=pen00.h,mam.h,rte.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmdesc.h,pmstck.h,pmprim.h,pmsmsg.h,pmpset.h  
pmradm.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmradm.h 
pmser.c    inc=pms00.h,pmser.h,pmsmsg.h  
pmsprm.c    inc=pen00.h,pms00.h,pmsprm.h,pmsmsg.h  
pmstck.c    inc=pen00.h,mam.h,rte.h,pms00.h,pmexec.h,pmmadm.h,pmradm.h,pmfram.h,pmdesc.h,pmstck.h,pmsprm.h,pmsmsg.h,pmprim.h  
pmsvb.c    inc=pms.h  
interfaceinfo.c    inc=pen00.h,pms.h,rte.h,pmincl.h,interfaceinfo.h,interfacetree.h  
interfacetree.c    inc=pen00.h,rte.h,interfacetree.h  
pmaprx.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmdesc.h,pmstck.h,pmsmsg.h  
pmasso.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmdesc.h,pmstck.h,pmsmsg.h   
pmcall.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmfram.h,pmdesc.h,pmstck.h,pmsmsg.h   
pmdesc.c    inc=pms00.h,pmdesc.h  
pmexec.c    inc=pen00.h,mam.h,rte.h,pms.h,pms00.h,pmexec.h,pmmadm.h,pmradm.h,pmfram.h,pmdesc.h,pmprim.h,pmstck.h,pmlprim.h,pmsmsg.h 
pmflat.c    inc=pen00.h,mam.h,rte.h,pms00.h,pmexec.h,pmmadm.h,pmradm.h,pmfram.h,pmdesc.h,pmstck.h,pmprim.h,pmsmsg.h,pmbufr.h  
rtearg.c    inc=pen00.h,rte.h  
rtebio.c    inc=pen00.h,rte.h,rtebio.h  
rteblo.c    inc=pen00.h,rte.h,rtebio.h  
rtecvt.c    inc=pen00.h,rte.h  
rtefil.c    inc=pen00.h,rte.h 
rtestd.c    inc=pen00.h,rte.h 
