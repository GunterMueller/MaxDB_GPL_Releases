# sys/src/SAPDB/webdav_j/webdav_j.com
#    ========== licence begin  GPL
#    Copyright (c) 2005 SAP AG
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

data_sources.xml ascii ->$DBROOT/servlets/webdav/data-sources.xml
webdavcontext.xml ascii ->$DBROOT/servlets/webdav/webdavcontext.xml
WebDAVDB_ds.xml ascii ->$DBROOT/servlets/webdav/WebDAVDB-ds.xml
README.txt ascii ->$DBROOT/servlets/webdav/Readme.txt
webdavServletMaxDB_1.1.ear binary ->$DBROOT/servlets/webdav/webdavServletMaxDB-1.1.ear
webdavServletMaxDB_1.1.war binary ->$DBROOT/servlets/webdav/webdavServletMaxDB-1.1.war
webdavServletMaxDB_1.1.xml ascii ->$DBROOT/servlets/webdav/webdavServletMaxDB-1.1.xml
