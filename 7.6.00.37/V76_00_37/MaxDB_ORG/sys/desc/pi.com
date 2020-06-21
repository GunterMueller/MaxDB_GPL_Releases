#1996-09-06 Josef, erzeuge varianten fuer sqlca-lib (MT,MD und SM) in WIN32
#1998-04-28 vpi30lic fuer lint entfernt.
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

* inc=vsp001,gpr00.h,vpi00fc check=no concom=yes

&if $MACH=NMP
* -Kschar
&endif
*-sdk GENCPC setbuildinfo ->$SAPDBSDK/incl/
vpi00c inc=vsp001,vsp004 setbuildinfo 

vpi30.c inc=gpr01.h,\
        gpr04.h,\
        vpr01Env.h,\
        vpr01Precom.h,\
        vpr01Module.h,\
        vpr01Con.h,\
        vpr01SQL.h,\
        vpr01EXECSQL.h,\
        vpi10ConnSync.h,\
        vpr06ParseId.h,\
        vpr07c,\
        gpr03.h,\
        gpr03m.h,\
        gpr08.h,\
        vpi00c-d,\
        geo31.h  -Dsql_sqldb -ansi

vpi30orc   -Dsql_oracle

vpi30cac    inc=vpr01Env.h
vpi30cac-d  inc=vpr01Env.h -Mx=-MD
vpi30cac-l  inc=vpr01Env.h -Mx=-ML

vpi00c-d setbuildinfo PI30DYN

vpi90c inc=+vpi90c

vpi01Libpcrl.h inc=gip00.h,SAPDB_Names.h concom=yes

vpi01Libpcrl*.c inc=vpr100.h,heo102.h,heo02x.h,gsp09.h,vpr09Config.h,vpi01Libpcrl.h,gpr00.h,vpi00fc,vpr07c,:precom/sqlinfo.h,:precom/sqlcancel.h,vpr09DynaDesc.h,vpi01GetModuleName.h -ansi concom=yes
vpi01Libpcrl-d.c -MD -DPI01LIBPCR 

vpi10ConnSync.h inc=gpr00.h
vpi10ConnSync.c inc=vpi10ConnSync.h

vpi01DynaDesc.h inc=vpr09DynaDesc.h

vpi01GetModuleName.h inc=
vpi01GetModuleName*.c inc=vpi01GetModuleName.h
vpi01GetModuleName-d.c -MD


