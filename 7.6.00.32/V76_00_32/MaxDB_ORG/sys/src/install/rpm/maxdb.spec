###############################################################
# PACKAGE INFORMATION
#
# Notes:
#
# - rpm documentation
#   See www.rpm.org.
#


name:		maxdb
version: 	§RELEASE
release: 	1	
copyright:	(c) 2003-2004 SAP AG, Germany
vendor: 	MySQL AB
packager: 	maxdb@lists.mysql.com
summary: 	MaxDB Sofware
group: 		Applications/Databases
Buildroot:	§BUILDROOT

%description

For more information please see www.mysql.com/maxdb.


#####################################################################
#
#	MaxDB release independent software
#
#
#


%package ind
summary: 	database remote communication server
group: 		Applications/Databases

%description ind
MaxDB Communication Server / Basis Tools

-	remote communication server
-	command line tools for database administration
-	SQLDBC runtime (C++ interface to access MaxDB)	

For more information please see www.mysql.com/maxdb.


%files ind
%dir %attr(-, sdb, sdba) /opt/sdb/programs/bin
%attr(-, sdb, sdba) /opt/sdb/programs/bin/dbanalyzer
%attr(-, sdb, sdba) /opt/sdb/programs/bin/dbmcli
%attr(-, sdb, sdba) /opt/sdb/programs/bin/dbmevtdisp
%attr(-, sdb, sdba) /opt/sdb/programs/bin/dbmgetf
%attr(-, sdb, sdba) /opt/sdb/programs/bin/loadercli
%attr(-, sdb, sdba) /opt/sdb/programs/bin/protconv
%attr(-, sdb, sdba) /opt/sdb/programs/bin/sdbconfig
%attr(-, sdb, sdba) /opt/sdb/programs/bin/sdbinfo
%attr(-, sdb, sdba) /opt/sdb/programs/bin/sqlcli
%attr(-, sdb, sdba) /opt/sdb/programs/bin/sysmon
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_analys
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_cons
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_maketi
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_ping
%attr(6551, sdb, sdba) /opt/sdb/programs/bin/x_server
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_wiz
%attr(-, sdb, sdba) /opt/sdb/programs/bin/x_wizstop
%attr(-, sdb, sdba) /opt/sdb/programs/bin/xsysrc
%attr(-, sdb, sdba) /opt/sdb/programs/bin/xuser
%attr(-, sdb, sdba) /opt/sdb/programs/bin/ps_all
%attr(6511, sdb, sdba) /opt/sdb/programs/bin/getdbroot
%dir %attr(500, sdb, sdba) /opt/sdb/programs/bin/starter
%attr(4510, root, sdba) /opt/sdb/programs/bin/starter/sdbstarter
%attr(-, sdb, sdba) /opt/sdb/programs/env
%attr(-, sdb, sdba) /opt/sdb/programs/etc
%attr(-, sdb, sdba) /opt/sdb/programs/lib
%dir %attr(-, sdb, sdba) /opt/sdb/programs/pgm
%attr(-, sdb, sdba) /opt/sdb/programs/pgm/lserver
%attr(-, sdb, sdba) /opt/sdb/programs/pgm/maketi
%attr(0500, sdb, sdba) /opt/sdb/programs/pgm/niserver
%attr(0500, sdb, sdba) /opt/sdb/programs/pgm/nisslserver
%dir %attr(500, sdb, sdba) /opt/sdb/programs/pgm/starter
%attr(4510, root, sdba) /opt/sdb/programs/pgm/starter/sdbstarter
%attr(0500, sdb, sdba) /opt/sdb/programs/pgm/vserver
%attr(-, sdb, sdba) /opt/sdb/programs/terminfo
%attr(-, sdb, sdba) /var/opt/sdb
%attr(444, root, root) %config  /etc/opt/sdb

%pre ind
groupadd sdba >/dev/null 2>&1
useradd -c "MaxDB user" -g sdba -m sdb >/dev/null 2>&1
usermod -s /bin/false -L -c "MaxDB user" -g sdba sdb >/dev/null 2>&1


ports="
sql6    7210 tcp
"
_i=0
for _arg in $ports
do
    if [ $_i -eq 0 ]; then
	_i=1; _servicename=$_arg; continue
    fi
    if [ $_i -eq 1 ]; then
	_i=2; _port=$_arg; continue
    fi
    _i=0
    _protocol=$_arg
    _serviceline=`awk '/^[ 	]*'$_servicename'[ 	]/ {print $1 "/" $2}' /etc/services 2>&1`
    if [ -z "$_serviceline" ]; then
	echo "$_servicename $_port/$_protocol" >> /etc/services
	if [ $? -ne 0 ]; then
	    echo "entry '$_servicename $_port/$_protocol' could not be added to /etc/services" >&2
	    echo "add the entry to /etc/services manually" >&2
	    exit 1
	fi
    fi
done
exit 0

%post ind

export DBROOT=/opt/sdb/programs

/opt/sdb/programs/bin/x_server start >/dev/null 2>&1
exit 0

%preun ind

if [ "$1" != "0" ]; then
    # do nothing in upgrade case
    exit 0
fi
export DBROOT=/opt/sdb/programs

/opt/sdb/programs/bin/x_server stop >/dev/null  2>&1

_o=`fuser $DBROOT/bin/* $DBROOT/pgm/* 2>/dev/null`
if [ $? -eq 0 ]; then
    if [ `echo $_o | wc -l` -eq 1 -a `echo $_o | grep -c "^$DBROOT/pgm/vserver"` -eq 1 ]; then
	$DBROOT/bin/x_server stop >/dev/null 2>&1
    else
	echo "- aborting (software is in use)" >&2
	exit 1
    fi
fi


rm -rf /var/opt/sdb/data/wrk/.M7600*  >/dev/null 2>&1
rm -rf /var/opt/sdb/data/config/.M7600* >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Databases.ini >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Databases.ini_lock >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Installations.ini >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Installations.ini_lock >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Runtimes.ini >/dev/null 2>&1
rm -f /var/opt/sdb/data/config/Runtimes.ini_lock >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/create_demo_db.log >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/drop_demo_db.log >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/dbmsrv.prt >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/NI_TRACEFILE  >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/niserver*.trace  >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/loader.prt  >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/xserver*.prt  >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/niserver.prot  >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/TST75/datasave >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/TST75/autosave.001 >/dev/null 2>&1
rmdir /var/opt/sdb/data/wrk/TST75 >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/TST76/datasave >/dev/null 2>&1
rm -f /var/opt/sdb/data/wrk/TST76/autosave.001 >/dev/null 2>&1
rmdir /var/opt/sdb/data/wrk/TST76 >/dev/null 2>&1
rmdir /var/opt/sdb/data/pipe >/dev/null 2>&1

rm -f \
    /var/opt/sdb/data/wrk/vserver.prot \
    /var/opt/sdb/data/wrk/root/* \
    /var/spool/sql/ini/SAP_DBTech.ini* \
    > /dev/null 2>&1

rm -rf /var/opt/sdb/data/wrk/irtrace* /var/spool/sql/dbspeed /var/spool/sql/pid /var/spool/sql/ppid >/dev/null 2>&1



ports="
sql6    7210 tcp
"
_i=0
for _arg in $ports
do
    if [ $_i -eq 0 ]; then
	_i=1; _servicename=$_arg; continue
    fi
    if [ $_i -eq 1 ]; then
	_i=2; _port=$_arg; continue
    fi
    _i=0
    _protocol=$_arg
    grep -v '^[ 	]*'$_servicename'[ 	]' /etc/services > /tmp/.$$
    [ $? -eq 0 ] && cp -f /tmp/.$$ /etc/services
    rm -f /tmp/.$$
done
exit 0

%postun ind

exit 0


########################################################
#
#	MaxDB  -  ODBC/JDBC Driver
#
#
#

%package callif
summary: 	Database Client Software
group: 		Applications/Databases
Buildroot:	§BUILDROOT

%description callif
MaxDB ODBC and JDBC Interfaces

-	ODBC driver
-	JDBC driver

For more information please see www.mysql.com/maxdb.

%files callif
%attr(-, sdb, sdba) /opt/sdb/interfaces/odbc
%attr(-, sdb, sdba) /opt/sdb/interfaces/jdbc


%pre callif

groupadd sdba >/dev/null 2>&1
ports="
sql6    7210 tcp
"
_i=0
for _arg in $ports
do
    if [ $_i -eq 0 ]; then
	_i=1; _servicename=$_arg; continue
    fi
    if [ $_i -eq 1 ]; then
	_i=2; _port=$_arg; continue
    fi
    _i=0
    _protocol=$_arg
    _serviceline=`awk '/^[ 	]*'$_servicename'[ 	]/ {print $1 "/" $2}' /etc/services 2>&1`
    if [ -z "$_serviceline" ]; then
	echo "$_servicename $_port/$_protocol" >> /etc/services
	if [ $? -ne 0 ]; then
	    echo "entry '$_servicename $_port/$_protocol' could not be added to /etc/services" >&2
	    echo "add the entry to /etc/services manually" >&2
	    exit 1
	fi
    fi
done
exit 0

%post callif

exit 0

%preun callif

exit 0

##########################################################
#
#	dependent server software
#
#

%package srv76
summary: 	Database Server Software
group: 		Applications/Databases
PreReq:		maxdb-ind >= §RELEASE
Requires:   maxdb-ind >= §RELEASE

%description srv76
MaxDB Database Server

-	database kernel
-	Database Manager Server
-	Replication Manager Server
-	MaxDB start script

For more information please see www.mysql.com/maxdb.


%files srv76
%attr(-, sdb, sdba) /opt/sdb/7600/bin
%attr(-, sdb, sdba) /opt/sdb/7600/env
%attr(-, sdb, sdba) /opt/sdb/7600/etc
%attr(-, sdb, sdba) /opt/sdb/7600/lib
%dir %attr(-, sdb, sdba) /opt/sdb/7600/pgm
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/cons
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmreg
%attr(6551, sdb, sdba) /opt/sdb/7600/pgm/dbmsrv
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmsrvscd
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmshm
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmunreg
%attr(6550, root, sdba) /opt/sdb/7600/pgm/dbmverifypw
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/diagnose
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/kernel
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmstart
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/dbmstop
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/lserver
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/python
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/slowknl
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/sysrc
%dir %attr(500, sdb, sdba) /opt/sdb/7600/pgm/starter
%attr(4510, root, sdba) /opt/sdb/7600/pgm/starter/sdbstarter
%attr(-, sdb, sdba) /opt/sdb/7600/pgm/tracesort
%attr(-, sdb, sdba) /opt/sdb/7600/sap
%attr(775, root, root) %config /etc/init.d/maxdb76

%pre srv76
groupadd sdba >/dev/null 2>&1
exit 0

%post srv76

export DBROOT=/opt/sdb/7600

if [ "$1" != "1" ]; then
	if [ -x $DBROOT/bin/dbmcli ]; then 	
		$DBROOT/bin/dbmcli -s -R $DBROOT inst_unreg $DBROOT >/dev/null 2>&1
	fi
fi

$DBROOT/bin/dbmsrvctl start
if [ $? -ne 0 ]; then
	echo "- could not register the installation" >&2
	exit 1
fi

_o=`$DBROOT/bin/dbmcli -R $DBROOT inst_reg -k $DBROOT`
_test=`echo $_o | grep OK`
if [ "$_test" = "" ]; then
    echo "- could not register the installation: $_o" >&2
    exit 1
fi

exit 0

%preun srv76
if [ "$1" != "0" ]; then
    # do nothing in upgrade case
    exit 0
fi
export DBROOT=/opt/sdb/7600

find $DBROOT/misc -type f -name '*.pyc' -exec rm {} \; >/dev/null 2>&1
find $DBROOT/env -type f -name '*.pyc' -exec rm {} \; >/dev/null 2>&1
find $DBROOT/lib/python1.5 -type f -name '*.pyc' -exec rm {} \; >/dev/null 2>&1

[ -z "$IND_PROG_DBROOT" -a -f /etc/opt/sdb ] && IND_PROG_DBROOT=`grep '^IndepPrograms=' /etc/opt/sdb | sed 's:IndepPrograms=::g'`
[ -z "$IND_PROG_DBROOT" -a -f /var/spool/sql/ini/SAP_DBTech.ini ] && IND_PROG_DBROOT=`grep '^IndepPrograms=' /var/spool/sql/ini/SAP_DBTech.ini | sed 's:IndepPrograms=::g'`

[ ! -z "$IND_PROG_DBROOT" ] && $IND_PROG_DBROOT/bin/x_server start >/dev/null 2>&1

[ -z "$IND_DATA_DBROOT" -a -f /etc/opt/sdb ] && IND_DATA_DBROOT=`grep '^IndepData=' /etc/opt/sdb | sed 's:IndepData=::g'`
[ -z "$IND_DATA_DBROOT" -a -f /var/spool/sql/ini/SAP_DBTech.ini ] && IND_DATA_DBROOT=`grep '^IndepData=' /var/spool/sql/ini/SAP_DBTech.ini | sed 's:IndepData=::g'`

$DBROOT/bin/dbmcli -s -R $DBROOT inst_unreg $DBROOT >/dev/null 2>&1

_o=`fuser $DBROOT/bin/* $DBROOT/pgm/* 2>/dev/null`
if [ $? -eq 0 ]; then
    if [ `echo $_o | wc -l` -eq 1 -a `echo $_o | grep -c "^$DBROOT/pgm/vserver"` -eq 1 ]; then
	$DBROOT/bin/x_server stop >/dev/null 2>&1
    else
	echo "- aborting (software is in use)" >&2
	exit 1
    fi
fi

rm -f $DBROOT/env/_OPTSAPD* $DBROOT/load.prot >/dev/null 2>&1

_dcom_link=$DBROOT/wrk/Registry.dcom

if [ -L $_dcom_link ]; then
	_dcom_file=`ls -l $_dcom_link | sed -e 's/.*-> //'`
	if [ -f $_dcom_file ]; then
		rm -f $_dcom_file >/dev/null 2>&1
	fi
fi
rm -f ${_dcom_link}*  >/dev/null 2>&1

rm -f /var/spool/sql/ini/Registry_dcom.ini >/dev/null 2>&1


if [ ! -z "$IND_DATA_DBROOT" ]; then
    rm -f \
	$IND_DATA_DBROOT/config/_OPTSAPD* \
	$IND_DATA_DBROOT/wrk/_OPTSAPD*/* \
	$IND_DATA_DBROOT/wrk/dbmsrv.prt \
	>/dev/null 2>&1
    rmdir \
	$IND_DATA_DBROOT/wrk/_OPTSAPD* \
	>/dev/null 2>&1
fi


exit 0


###############################################################
#
#	MaxDB SQLDBC
# 
#

#%package	sqldbc
#summary: 	Database Clients Software
#group: 	Applications/Databases


#%description sqldbc
#MaxDB sqldbc

#-	sqldbc 
#-	includes
#-	libs (incl. runtime)

#For more information please see www.mysql.com/maxdb.

#%files sqldbc
#%attr(-, sdb, sdba) /opt/sdb/interfaces/sqldbc
#%pre sqldbc


#ports="
#sql6    7210 tcp
#"
#_i=0
#for _arg in $ports
#do
#    if [ $_i -eq 0 ]; then
#	_i=1; _servicename=$_arg; continue
#    fi
#    if [ $_i -eq 1 ]; then
#	_i=2; _port=$_arg; continue
#    fi
#    _i=0
#    _protocol=$_arg
#    _serviceline=`awk '/^[ 	]*'$_servicename'[ 	]/ {print $1 "/" $2}' /etc/services 2>&1`
#    if [ ! -z "$_serviceline" ]; then
#	for _cl in $_serviceline
#	do
#	    if [ "$_cl" != "$_servicename/$_port/$_protocol" ]; then
#		echo "inconsistent service entry database:" >&2
#		echo "specified entry '$_servicename $_port/$_protocol' does not match retrieved entry:" >&2
#		echo "$_serviceline" >&2
#		exit 1
#	    fi
#	done
#    fi
#    if [ -z "$_serviceline" ]; then
#	echo "$_servicename $_port/$_protocol" >> /etc/services
#	if [ $? -ne 0 ]; then
#	    echo "entry '$_servicename $_port/$_protocol' could not be added to /etc/services" >&2
#	    echo "add the entry to /etc/services manually" >&2
#	    exit 1
#	fi
#   fi
#done
#exit 0

#%post sqldbc

#exit 0

#%preun sqldbc

#exit 0



####################################################################
#
#		MaxDB Perl/Python interface
#
#
#

%package scriptif
summary: 	Database Client Software
group: 	Applications/Databases


%description scriptif
MaxDB Perl and Python Interfaces

-	Perl driver
-	Python driver

For more information please see www.mysql.com/maxdb.

%files scriptif
&if $MACH == I386
%attr(-, sdb, sdba) /opt/sdb/interfaces/perl
&endif
%attr(-, sdb, sdba) /opt/sdb/interfaces/python



%pre scriptif

groupadd sdba >/dev/null 2>&1

ports="
sql6    7210 tcp
"
_i=0
for _arg in $ports
do
    if [ $_i -eq 0 ]; then
	_i=1; _servicename=$_arg; continue
    fi
    if [ $_i -eq 1 ]; then
	_i=2; _port=$_arg; continue
    fi
    _i=0
    _protocol=$_arg
    _serviceline=`awk '/^[ 	]*'$_servicename'[ 	]/ {print $1 "/" $2}' /etc/services 2>&1`
    if [ -z "$_serviceline" ]; then
	echo "$_servicename $_port/$_protocol" >> /etc/services
	if [ $? -ne 0 ]; then
	    echo "entry '$_servicename $_port/$_protocol' could not be added to /etc/services" >&2
	    echo "add the entry to /etc/services manually" >&2
	    exit 1
	fi
    fi
done
exit 0

%post scriptif

exit 0

%preun scriptif

exit 0


#################################################################
#
#
#	MaxDB sample database
#
#

%package testdb76
summary: 	sample database
group: 	Applications/Databases
PreReq:	maxdb-srv76
Requires: maxdb-srv76

%description testdb76
MaxDB Sample Database

-	package creates and immediately starts database instance TST76 (having 20 MB data space and 8 MB log space)
	Please remember to shutdown database before system shutdown.
-	sample database user: TEST, password TEST
-	DBM user: DBM, password DBM
-	database administrator: DBA, password DBA

For more information please see www.mysql.com/maxdb.


%files testdb76
%attr(775, sdb, sdba) /opt/sdb/testdb76

%pre testdb76
groupadd sdb >/dev/null 2>&1
exit 0

%post testdb76
if [ "$1" != "1" ]; then
    # do nothing in upgrade case
    exit 0
fi
export DBROOT=/opt/sdb/7600

IND_PROG_DBROOT=`$DBROOT/bin/dbmcli dbm_getpath IndepProgPath 2>&1 | awk 'BEGIN {ok=0} NR==1 && $1="OK" {ok=1} NR==2 && ok!=0 {print $0}'`
[ -z "$IND_PROG_DBROOT" -a -f /var/spool/sql/ini/SAP_DBTech.ini ] && IND_PROG_DBROOT=`grep '^IndepPrograms=' /var/spool/sql/ini/SAP_DBTech.ini | sed 's:IndepPrograms=::g'`

[ ! -z "$IND_PROG_DBROOT" ] && $IND_PROG_DBROOT/bin/x_server start >/dev/null 2>&1

/opt/sdb/testdb76/create_demo_db.sh > /var/opt/sdb/data/wrk/create_demo_db.log 2>&1
chmod 644 /var/opt/sdb/data/wrk/create_demo_db.log > /dev/null 2>&1

exit 0

%preun testdb76
if [ "$1" != "0" ]; then
    # do nothing in upgrade case
    exit 0
fi
export DBROOT=/opt/sdb/7600

IND_PROG_DBROOT=`$DBROOT/bin/dbmcli dbm_getpath IndepProgPath 2>&1 | awk 'BEGIN {ok=0} NR==1 && $1="OK" {ok=1} NR==2 && ok!=0 {print $0}'`
[ -z "$IND_PROG_DBROOT" -a -f /var/spool/sql/ini/SAP_DBTech.ini ] && IND_PROG_DBROOT=`grep '^IndepPrograms=' /var/spool/sql/ini/SAP_DBTech.ini | sed 's:IndepPrograms=::g'`

[ ! -z "$IND_PROG_DBROOT" ] && $IND_PROG_DBROOT/bin/x_server start >/dev/null 2>&1

IND_DATA_DBROOT=`$DBROOT/bin/dbmcli dbm_getpath IndepDataPath 2>&1 | awk 'BEGIN {ok=0} NR==1 && $1="OK" {ok=1} NR==2 && ok!=0 {print $0}'`
[ -z "$IND_DATA_DBROOT" -a -f /var/spool/sql/ini/SAP_DBTech.ini ] && IND_DATA_DBROOT=`grep '^IndepData=' /var/spool/sql/ini/SAP_DBTech.ini | sed 's:IndepData=::g'`

/opt/sdb/testdb76/drop_demo_db.sh > /var/opt/sdb/data/wrk/drop_demo_db.log 2>&1
chmod 644 /var/opt/sdb/data/wrk/drop_demo_db.log > /dev/null 2>&1



[ ! -z "$IND_DATA_DBROOT" ] && rm -rf $IND_DATA_DBROOT/wrk/sdb >/dev/null 2>&1

exit 0

###############################################################
#
#
#	MaxDB Webtools
#
#
#
#

%package web
summary: 	Database Client Software
group: 		Applications/Databases
PreReq:		maxdb-callif >= §RELEASE
Requires:   maxdb-callif >= §RELEASE


%description web
MaxDB Web Tools

-	modules for Apache Web Server
-	http server for running MaxDB Web Tools without Apache
	(start script is /etc/init.d/maxdbweb)
-	WebDBM, the browser-based MaxDB access for administrating remote and local databases,
	(url: http://<host>:<port>/webdbm)

For more information please see www.mysql.com/maxdb.


%files web
%attr(-, sdb, sdba) /opt/sdb/web
%attr(775, root, root) /etc/init.d/maxdbweb
%attr(775, sdb, sdba) /var/log/sdb
%attr(755, root, sdba) %config /var/spool/sql/ini/WebAgent76.ini


%pre web

groupadd sdba > /dev/null 2>&1

if [ ! -L /usr/spool ]; then
        if [ -d /usr/spool ]; then
                echo "/usr/spool has to be a link to ../var/spool"
                exit 1
        fi
        if [ ! -d /var/spool ];then
                mkdir -p /var/spool > /dev/null 2>&1
        fi
        ln -s ../var/spool /usr/spool
fi

ports="
sql6    7210 tcp
"
_i=0
for _arg in $ports
do
    if [ $_i -eq 0 ]; then
	_i=1; _servicename=$_arg; continue
    fi
    if [ $_i -eq 1 ]; then
	_i=2; _port=$_arg; continue
    fi
    _i=0
    _protocol=$_arg
    _serviceline=`awk '/^[ 	]*'$_servicename'[ 	]/ {print $1 "/" $2}' /etc/services 2>&1`
    if [ -z "$_serviceline" ]; then
	echo "$_servicename $_port/$_protocol" >> /etc/services
	if [ $? -ne 0 ]; then
	    echo "entry '$_servicename $_port/$_protocol' could not be added to /etc/services" >&2
	    echo "add the entry to /etc/services manually" >&2
	    exit 1
	fi
    fi
done
exit 0

%preun web
find /var/log/sdb -type f -name '*.log' -exec rm {} \; >/dev/null 2>&1
rm -f /var/spool/sql/ini/WebAgent76.ini_lock >/dev/null 2>&1
rm -f /opt/sdb/web/pgm/wahttp.pid >/dev/null 2>&1


exit 0
