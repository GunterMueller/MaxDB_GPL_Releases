# @(#)   desc/wd.com
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

* -DWNT -D_EN_
*c -DEAPI

# Webagent API
gwd00.h inc=heo02.h,heo102.h,SAPDB_Types.h,hwd01wadef.h,hwd10mem.h,hsp77.h

hwd01wadef.h    concom=yes ->$INSTROOT/incl/sapdbwa_define.h

hwd02waapi.h    concom=yes dep=hwd03waapi.h,hwd04wases.h,hwd01wadef.h,hwd06waerr.h ->$INSTROOT/incl/sapdbwa.hpp

hwd03waapi.h    concom=yes dep=hwd05wases.h,hwd01wadef.h,hwd06waerr.h ->$INSTROOT/incl/sapdbwa.h

hwd04wases.h    concom=yes ->$INSTROOT/incl/sapdbwa_session.hpp

hwd05wases.h    concom=yes ->$INSTROOT/incl/sapdbwa_session.h

hwd06waerr.h    concom=yes ->$INSTROOT/incl/sapdbwa_error.h

hwd07dbc.h inc=hwd06waerr.h,hwd08dbc.h
hwd08dbc.h inc=hwd01wadef.h,hwd06waerr.h
hwd09util.h inc=gwd00.h
vwd09util.c inc=hwd09util.h,SAPDB_string.h
hwd10mem.h inc=hwd01wadef.h
vwd10mem.c inc=geo102.h,hwd10mem.h
hwd11strseq.h inc=gwd00.h,hwd03waapi.h
vwd11strseq.c inc=hwd11strseq.h,vpr09DynaDesc.h
hwd12hpool.h inc=gwd00.h
vwd12hpool.c inc=vpr09DynaDesc.h,hwd12hpool.h,hwd27excl.h
hwd15text.h inc=gsp00,hwd01wadef.h
vwd15text.c inc=hwd15text.h
hwd20control.h inc=gwd00.h,hwd05wases.h,hwd21req.h,hwd22rep.h,hwd26err.h
vwd20control.c inc=hwd09util.h,hwd15text.h,hwd20control.h,hwd23wa.h,\
                   hwd25log.h,hwd27excl.h,hwd30ses.h,hwd32hses.h,\
                   hwd37wses.h,hwd39sqlconn.h,hwd40user.h,hwd91mime.h,hwd96wacom.h,\
                   Reg_Registry.h,heo670.h
hwd21req.h inc=gwd00.h,hwd03waapi.h,hwd26err.h,hwd27excl.h,hwd28dict.h
vwd21req.c inc=hwd12hpool.h,hwd21req.h,SAPDBCommon/SAPDB_string.h
hwd22rep.h inc=gwd00.h,hwd03waapi.h,hwd26err.h,hwd28dict.h
vwd22rep.c inc=hwd12hpool.h,hwd20control.h,hwd22rep.h,hwd29date.h,hwd91mime.h
hwd23wa.h inc=gwd00.h,hwd03waapi.h,hwd26err.h,hwd37wses.h,hwd40user.h
vwd23wa.c inc=hwd12hpool.h,hwd03waapi.h,hwd20control.h,hwd23wa.h,hwd32hses.h,hwd40user.h,SAPDB_string.h
hwd25log.h inc=gwd00.h
vwd25log.c inc=hwd25log.h,hwd27excl.h
hwd26err.h inc=gwd00.h,hwd06waerr.h,hwd25log.h
vwd26err.c inc=hwd26err.h,hwd03waapi.h,hsp81.h,hsp83.h,SAPDB_string.h
hwd27excl.h inc=gwd00.h,hwd26err.h
vwd27excl.c inc=heo07.h,hwd27excl.h
hwd28dict.h inc=gwd00.h,hwd11strseq.h
vwd28dict.c inc=hwd28dict.h
hwd29date.h inc=gwd00.h
vwd29date.c inc=hwd29date.h,SAPDB_string.h
hwd30ses.h inc=gwd00.h,hwd03waapi.h,hwd26err.h,hwd31conn.h
vwd30ses.c inc=hwd09util.h,hwd30ses.h,hwd35res.h
hwd31conn.h inc=gwd00.h,hwd05wases.h,hwd26err.h
vwd31conn.c inc=hwd30ses.h,hwd31conn.h,hwd38data.h,hwd39sqlconn.h
hwd32hses.h inc=gwd00.h,hwd05wases.h,hwd26err.h,hwd30ses.h
vwd32hses.c inc=hwd20control.h,hwd30ses.h,hwd31conn.h,hwd32hses.h,hwd33hconn.h,SAPDB_string.h
hwd33hconn.h inc=gwd00.h,hwd05wases.h,hwd26err.h,hwd31conn.h
vwd33hconn.c inc=hwd20control.h,hwd31conn.h,hwd33hconn.h
hwd34dbc.h inc=hwd08dbc.h
vwd34dbc.c inc=vpa00sqlod32.h,gwd00.h,hwd26err.h,hwd27excl.h,hwd31conn.h,hwd34dbc.h,hwd38data.h,hsp81.h,hsp83.h,SAPDB_string.h
hwd35res.h inc=gwd00.h,vpr09DynaDesc.h,hwd27excl.h,heo07.h
vwd35res.c inc=hwd35res.h
hwd36num.h inc=gwd00.h
vwd36num.c inc=vpr09DynaDesc.h,hwd27excl.h,hwd36num.h
hwd37wses.h inc=gwd00.h,hwd03waapi.h
vwd37wses.c inc=hwd20control.h,hwd27excl.h,hwd28dict.h,hwd37wses.h,SAPDB_string.h
hwd38data.h inc=gwd00.h,hwd01wadef.h
vwd38data.c inc=hwd38data.h
hwd39sqlconn.h inc=gwd00.h,hwd26err.h
vwd39sqlconn*.cpp  inc=hwd39sqlconn.h dep=SQL_ExtClientInterface.hpp -except
vwd39sqlconn-libsqlcls_static.cpp  -DEXT_CLIENT_SQL_STATIC_LINK

hwd40user.h inc=gwd00.h,hwd03waapi.h,hwd21req.h,hwd22rep.h,hwd26err.h,\
                hwd30ses.h
vwd40user.c inc=heo02x.h,hwd09util.h,hwd25log.h,hwd40user.h,hwd41cpp.h
hwd41cpp.h inc=heo02x.h,hwd03waapi.h,hwd25log.h
vwd41cpp.cpp inc=gwd00.h,heo670.h,hwd02waapi.h,hwd20control.h,hwd41cpp.h -except
vwd51ns.c inc=gwd00.h,hwd03waapi.h,hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h,SAPDB_string.h -I$OWN/tool/incl/netscape
vwd52ms.c inc=gwd00.h,hwd03waapi.h,hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h,SAPDB_string.h
vwd53ap.c inc=gwd00.h,hwd03waapi.h,hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h -I$OWN/tool/incl/apache
vwd54cgi.c inc=hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h
vwd55ap2.c inc=gwd00.h,hwd03waapi.h,hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h -I$OWN/tool/incl/apache_2.0
vwd56fcgi.c inc=fcgi_config.h,fcgi_stdio.h,\
            hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h
vwd59sim.c inc=gwd00.h,hwd03waapi.h,hwd20control.h,hwd21req.h,hwd22rep.h,hwd28dict.h,hwd29date.h
vwd61dllmain.c inc=hwd20control.h
# Test user dlls
vwd80test.c inc=hwd03waapi.h,hwd05wases.h,heo102.h
vwd81test.cpp inc=hwd02waapi.h,hwd04wases.h,geo102.h
# dbfs access dll
vwd82fs.c inc=heo102.h,hwd03waapi.h,hwd05wases.h,hwd20control.h,hwd101dbfs.h,hwd91mime.h

vwd83echo.c inc=hwd01wadef.h,hwd03waapi.h,hwd20control.h
vwd85dbctest.c inc=hwd08dbc.h,hwd101dbfs.h
vwd86fstest.c inc=hwd08dbc.h,hwd101dbfs.h,SAPDB_string.h
vwd86dbm.cpp inc=geo102.h,hcn14.h,hwd02waapi.h
vwd87wses.cpp inc=geo102.h,hwd02waapi.h
hwd88testutil.h inc=hwd04wases.h,hwd89testapi.h,hwd100dbfs.h,hwd101dbfs.h
vwd88testutil.cpp inc=hwd88testutil.h,SAPDB_string.h
hwd89testapi.h inc=gwd00.h,hwd21req.h,hwd22rep.h,hwd28dict.h
vwd89testapi.cpp inc=hwd89testapi.h,hwd20control.h
vwd88.cpp inc=hwd89testapi.h

# mime type handling
hwd91mime.h inc=gwd00.h
vwd91mime.c inc=hwd91mime.h,hwd28dict.h,SAPDB_string.h

# com interface
vwd95wacom.idl unpack=no noobjcopy -DMIDL_PASS uncond inc= \
                obj=vwd95wacom.idl.o \
               -header=$WRK/incl/vwd95wacom.h
               extdep=$WRK/incl/vwd95wacom.h,vwd95wacom_i.c \
               -keep_tlb -keep_h -keep_c
vwd95wacom.h inc=
hwd96wacom.h inc=gwd00.h,hwd03waapi.h,hwd21req.h,hwd22rep.h,hwd26err.h,\
                 hwd30ses.h
vwd96wacom.cpp dep=vwd95wacom.idl inc=hwd97stdafx.h,heo07.h,hwd09util.h,\
               hwd20control.h,hwd25log.h,hwd27excl.h,hwd96wacom.h,\
               hwd98ustring.h -except
hwd97stdafx.h dep=vwd95wacom.idl inc=vwd95wacom.h
hwd98ustring.h inc=
vwd98ustring.cpp inc=gwd00.h

# DB-Filesystem API
hwd100dbfs.h inc=hwd04wases.h,hwd07dbc.h,hwd101dbfs.h
hwd101dbfs.h inc=gwd00.h,hwd05wases.h,hwd06waerr.h,hwd08dbc.h
vwd101dbfs.c inc=hwd20control.h,hwd26err.h,hwd35res.h,hwd101dbfs.h,\
                 hwd102fdesc.h,hwd103inode.h,hwd104fsdesc.h,hwd106pool.h,SAPDB_string.h
hwd102fdesc.h inc=gwd00.h,hwd101dbfs.h,hwd103inode.h,hwd105dynbuf.h,\
                  hwd106pool.h
vwd102fdesc.c inc=hwd102fdesc.h,hwd103inode.h
hwd103inode.h inc=gwd00.h,hwd26err.h,hwd29date.h,hsp81.h,hsp83.h,SAPDB_string.h
vwd103inode.c inc=hwd103inode.h,SAPDB_string.h
hwd104fsdesc.h inc=gwd00.h,hwd26err.h,hwd101dbfs.h,hwd103inode.h,\
                   hwd105dynbuf.h,hwd106pool.h
vwd104fsdesc.c inc=hwd104fsdesc.h,hwd111getinode.h,hwd112insinode.h,\
                   hwd113delinode.h,hwd114seldir.h,hwd115container.h,\
                   hwd117reninode.h,hwd118moveinode.h,hwd119copy.h
hwd105dynbuf.h inc=gsp00,hpa102odcompr.h,hwd01wadef.h
vwd105dynbuf.c inc=gwd00.h,hwd105dynbuf.h,SAPDB_string.h
hwd106pool.h inc=hwd33hconn.h,hwd34dbc.h
vwd106pool.c inc=gwd00.h,hwd05wases.h,hwd32hses.h,hwd36num.h,hwd106pool.h
hwd109ssofs.h inc=hwd101dbfs.h
vwd109ssofs.c inc=hwd109ssofs.h

hwd111getinode.h inc=gwd00.h,hwd26err.h,hwd103inode.h
vwd111getinode.c inc=hwd104fsdesc.h,hwd111getinode.h,SAPDB_string.h
hwd112insinode.h inc=gwd00.h,hwd26err.h,hwd103inode.h,hwd104fsdesc.h
vwd112insinode.c inc=hwd29date.h,hwd104fsdesc.h,hwd112insinode.h,SAPDB_string.h
hwd113delinode.h inc=gwd00.h,hwd26err.h,hwd103inode.h,hwd104fsdesc.h
vwd113delinode.c inc=hwd113delinode.h,SAPDB_string.h
hwd114seldir.h inc=gwd00.h,hwd26err.h,hwd101dbfs.h,hwd103inode.h
vwd114seldir.c inc=hwd29date.h,hwd104fsdesc.h,hwd114seldir.h
hwd115container.h inc=gwd00.h,hwd26err.h,hwd103inode.h,hwd104fsdesc.h
vwd115container.c inc=hpa102odcompr.h,hwd104fsdesc.h,hwd115container.h,SAPDB_string.h
hwd117reninode.h inc=gwd00.h,hwd26err.h,hwd103inode.h
vwd117reninode.c inc=hwd117reninode.h,hwd104fsdesc.h
hwd118moveinode.h inc=gwd00.h,hwd26err.h,hwd103inode.h
vwd118moveinode.c inc=hwd104fsdesc.h,hwd118moveinode.h
hwd119copy.h inc=gwd00.h,hwd26err.h,hwd103inode.h,hwd104fsdesc.h
vwd119copy.c inc=hwd119copy.h,SAPDB_string.h

vwd120format.c inc=hwd101dbfs.h,hwd128help.h
vwd121mkdir.c inc=hwd101dbfs.h,hwd128help.h
vwd122rmdir.c inc=hwd101dbfs.h,hwd128help.h
vwd123cpin.c inc=hwd101dbfs.h,hwd128help.h
vwd124cpout.c inc=hwd101dbfs.h,hwd128help.h
vwd125ls.c inc=hwd101dbfs.h,hwd128help.h
vwd126rm.c inc=hwd101dbfs.h,hwd128help.h
vwd127mv.c inc=hwd101dbfs.h,hwd128help.h
hwd128help.h inc=gwd00.h
vwd128help.c inc=hwd128help.h
vwd129cp.c inc=hwd101dbfs.h,hwd128help.h
vwd130ln.c inc=hwd101dbfs.h,hwd128help.h

hwd150lock.h inc=hwd101dbfs.h,hwd103inode.h
vwd150lock.c inc=hwd150lock.h

# inetd
hwd201sock.h inc=gsp00,gwd00.h
vwd201sock.c inc=hwd201sock.h,hwd202msg.h
hwd203util.h inc=hwd201sock.h
vwd203util.c inc=hwd203util.h
hwd204zstring.h inc=gwd00.h,SAPDB_string.h
vwd204zstring.cpp inc=hwd204zstring.h
hwd205fs.h inc=geo102.h,hwd204zstring.h,hwd207stream.h
hwd206dbfs.h inc=gwd00.h,geo102.h,hwd05wases.h,hwd101dbfs.h,hwd204zstring.h,hwd205fs.h
vwd206dbfs.cpp inc=hwd26err.h,hwd206dbfs.h,hwd208err.h
hwd207stream.h inc=geo102.h
hwd208err.h inc=hwd26err.h,hwd204zstring.h
vwd208err.cpp inc=hwd208err.h
vwd210inetd.cpp inc=geo102.h,hwd04wases.h,hwd20control.h,hwd32hses.h,\
                    hwd201sock.h,hwd203util.h,hwd206dbfs.h,hwd220ftp.h
hwd211service.h inc=hwd201sock.h,hwd211service.h
vwd211service.cpp inc=hwd211service.h
hwd220ftp.h inc=gwd00.h,hwd205fs.h,hwd207stream.h,hwd211service.h
vwd220ftp.cpp inc=hwd203util.h,hwd220ftp.h,hwd221ftpcmd.h
hwd221ftpcmd.h inc=geo102.h,hwd201sock.h,hwd204zstring.h,SAPDB_string.h
vwd221ftpcmd.cpp inc=hwd203util.h,hwd221ftpcmd.h

# fcgi
fcgiappmisc.h inc=fcgiapp.h
fcgios.h inc=fcgi_config.h
fcgi_stdio.h inc=fcgiapp.h
fcgimisc.h inc=fcgi_config.h
fcgi_config.h inc=fcgi_config_64.h
fcgi_config_64.h inc=

fcgiapp.c inc=fcgi_config.h,fcgimisc.h,fcgiappmisc.h,fastcgi.h,fcgios.h,\
              fcgiapp.h
fcgi_stdio.c inc=fcgi_config.h,fcgi_stdio.h,fcgiapp.h,fcgios.h
os_unix.c inc=fcgimisc.h,fcgi_config.h,fastcgi.h,fcgiapp.h,fcgiappmisc.h,fcgios.h

##################
# Registry
##################
#hwd09Registry.h        inc=SAPDB_Types.h
#vwd09Registry.c        inc=hwd09Registry.h

WA.reg.0w           ascii ->$INSTROOT/config/WA.reg         unpack=no concom=no
WebAgent76.ini.0w   setbuildinfo ascii ->$INSTROOT/config/WebAgent76.ini unpack=no concom=no
