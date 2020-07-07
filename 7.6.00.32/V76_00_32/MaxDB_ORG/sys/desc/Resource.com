# @(#)   desc/fast/Resource.com   7.2.0   1999-07-07
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

# --- kernel.exe,quickknl.exe and slowknl.exe resources
#
ntkern.ico		 binary 
ntcold.ico		 binary
ntwarm.ico		 binary
ntstop.ico		 binary
ntstate.ico		 binary
ntkern.rc		 binary
ntkern_bin.mc	 binary 
kernel.rc	         	 inc=gsp100.h,ntkern.ico,ntcold.ico,ntwarm.ico,ntstop.ico,ntstate.ico,ntkern.rc,ntkern_bin.mc
kernel-omstst.rc		 inc=gsp100.h,ntkern.ico,ntcold.ico,ntwarm.ico,ntstop.ico,ntstate.ico,ntkern.rc,ntkern_bin.mc -DOMSTST
quickknl.rc		       inc=gsp100.h,ntkern.ico,ntcold.ico,ntwarm.ico,ntstop.ico,ntstate.ico,ntkern.rc,ntkern_bin.mc
slowknl.rc		       inc=gsp100.h,ntkern.ico,ntcold.ico,ntwarm.ico,ntstop.ico,ntstate.ico,ntkern.rc,ntkern_bin.mc



#
# --- resources
#
query.rc         inc=gsp100.h
load.rc			     inc=gsp100.h
dit.rc           inc=gsp100.h
backrest.rc      inc=gsp100.h
cr_param.rc      inc=gsp100.h
diagnose.rc      inc=gsp100.h
livecachedbg.rc  inc=gsp100.h
livecachesink.rc inc=gsp100.h
oci.rc           inc=gsp100.h
param.rc         inc=gsp100.h
plrun.rc         inc=gsp100.h
prtprot.rc       inc=gsp100.h
liboms.rc        inc=gsp100.h
slowci.rc        inc=gsp100.h
sqlfilter.rc     inc=gsp100.h
sqlpl.rc         inc=gsp100.h
sql_run.rc       inc=gsp100.h
tplrun.rc        inc=gsp100.h
userx.rc         inc=gsp100.h
vttest.rc		     inc=gsp100.h
tracesort.rc		 inc=gsp100.h
libhsscopy.rc        inc=gsp100.h

xtcpupd.ico		   binary
xtcpupd.rc		   inc=gsp100.h,vox31x,xtcpupd.ico

#
# --- preccompiler resources
#
pc3.rc	   	inc=gsp100.h,vmt90c
libpcr.rc	inc=gsp100.h,vmt90c

#
# --- ODBC resources
#
sapdb90.bmp		 binary
sqlod32.rc		 inc=gsp100.h,vpa00dlc,vpa00txc,sapdb90.bmp

sqlsp32.rc		 inc=gsp100.h,vpa00dlc
sqlodext.rc      inc=gsp100.h
odcompr.rc       inc=gsp100.h
#
# --- WebAgent resources
#
wapi.rc          inc=gsp100.h
dbfsapi.rc       inc=gsp100.h
wapiap.rc        inc=gsp100.h
wapims.rc        inc=gsp100.h
wapins.rc        inc=gsp100.h
wacgi.rc         inc=gsp100.h
dbformat.rc      inc=gsp100.h
dbcpin.rc        inc=gsp100.h
dbcpout.rc       inc=gsp100.h
dbcp.rc          inc=gsp100.h
dbmkdir.rc       inc=gsp100.h
dbrmdir.rc       inc=gsp100.h
dbmv.rc          inc=gsp100.h
dbls.rc          inc=gsp100.h
dbrm.rc          inc=gsp100.h
wafs.rc          inc=gsp100.h


#
# --- RTE resources
#
systemrc.rc      inc=gsp100.h
cons.rc			     inc=gsp100.h
service.rc		   inc=gsp100.h
service_clu.rc	 inc=gsp100.h
serv.ico		     binary
serv_clu.rc  	   inc=gsp100.h,serv.ico
serv.rc			     inc=gsp100.h,serv.ico
putparam.rc		   inc=gsp100.h
getparam.rc		   inc=gsp100.h
show.rc			     inc=gsp100.h
stop.rc			     inc=gsp100.h
stop_clu.rc       inc=gsp100.h
start.rc			     inc=gsp100.h
start_clu.rc      inc=gsp100.h
strt.rc			     inc=gsp100.h
strt_clu.rc      inc=gsp100.h
sqlrun.rc		     inc=gsp100.h
sqluser74n.rc		 inc=gsp100.h
sqlinst.rc	     inc=gsp100.h 
sqlmsg_bin.mc    binary 
sqlmsg.rc		     inc=gsp100.h,sqlmsg_bin.mc

#
# --- RTE resources (sqltcp)
#
sqltcp.rc		   inc=gsp100.h
sqltcpn.rc		 inc=gsp100.h
sqltcp1.rc		 inc=gsp100.h
sqltcp2.rc		 inc=gsp100.h

#
# --- DBM resources
#
dbmsrv.rc        inc=gsp100.h dep=DBM_Resource.rc
dbmsrv_clu.rc    inc=gsp100.h dep=DBM_Resource.rc
dbmcli.rc        inc=gsp100.h dep=DBM_Resource.rc
dbmspecial.rc    inc=gsp100.h dep=DBM_Resource.rc
dbmapi.rc        inc=gsp100.h dep=DBM_Resource.rc
dbmgetf.rc       inc=gsp100.h dep=DBM_Resource.rc

#
# --- samples
#
versample.rc     inc=gsp100.h


# --- microsoft cluster server resources
#
sapdbmscsman.rc	 inc=gsp100.h
sapdbmscsman_.rc inc=gsp100.h
sapdbmscsres.rc  inc=gsp100.h
sapdbmscsex.rc   inc=gsp100.h


#
# --- tools
#
setserviceport.rc inc=gsp100.h
