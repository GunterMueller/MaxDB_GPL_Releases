:

#
#   FreeBSD portions added by Kai Mosebach,
#   For more informations see : http://www.komadev.de/sapdb
#

# Linken eines ODBC-Anwendungsprogramms mit dem Datenbank-Laufzeitsystem
# -------------------------------------------
# $1 :      Der Name der Datei, die das Hauptprogramm enthaelt, ist $1.o
#           Das lauffaehige Programm wird in die Datei $1 geschrieben.
# $2...$n : Dateien, die mit dem Hauptprogramm gebunden werden sollen.
#           Fuer jeden Parameter $i wird zuerst nach einer Datei $i.o ,
#           dann nach $i.a gesucht. In beiden Faellen kann es sich um
#           sequentielle Objektdateien oder Bibliotheken handeln.
#

if test $# = 0
then echo Usage: odbclnk [-b] [linkopts] file [file...]
     exit
fi

LNKVER=`basename $0`
export LNKVER

set -- `getopt A:bD:de:L:l:gMNno:rSsT:tu:VXxy:z $*`

options=


for i
do
   if [ $i = '--' ]
   then
     break
   fi
   if [ $i = "-b" ]
   then
      LNKVER="${LNKVER} -b"
   else
      options=$options' '$1
   fi
   shift
done

shift
LIB=$DBROOT/lib
for i in `echo "$LNKVER"`
do
    case "$i" in
    -b )
	DYN="$i"
	;;
    * )
	LNK="$i"
	;;
    esac
done

# PGM-libary
OEXT=.o
LEXT=.a
EEXT=
DEXT=.so

PCODBCLIB="$DBROOT/lib/libsqlod"
 &if $OSSPEC in [ FREEBSD ]
   ADDLIBS=" -L$DBROOT/lib -lsqlptc"
 &else $OSSPEC in [ OSF1, LINUX ]
   ADDLIBS=" -L$DBROOT/lib -lsqlptc -lpthread"
 &endif
if [ "$DYN" = "-b" ]
then
   PCODBCLIB="$PCODBCLIB$DEXT"
   if [ -x $DBROOT/bin/sqllist.rte ]
   then
	PCSQLDB=`$DBROOT/bin/sqllist.rte `
   fi
   PCSQLDB="$PCODBCLIB $ADDLIBS $PCSQLDB"
else
   PCODBCLIB="$PCODBCLIB$LEXT"
   if [ -x $DBROOT/bin/sqllist.rte ]
   then
	PCSQLDB=`$DBROOT/bin/sqllist.rte `
   fi
   PCSQLDB="$PCODBCLIB $ADDLIBS $PCSQLDB"
fi

exec=$1$EEXT
files=''
if   [ -f $1$OEXT ]
then
   main=$1$OEXT
fi

shift

for i
do
   if   [ -f $i$OEXT ]
   then
      files=$files' '$i$OEXT
   elif [ -f $i$LEXT ]
   then
      files=$files' '$i$LEXT
   elif [ -f $i.c ]
   then
      files=$i.c
   else
      files=$files' '$i
   fi
done

if [ -f $exec ] ; then  rm $exec; fi
if [ "$PCSQLDB" ]
then
   if [ `uname -s` = TARGON31 ]
   then
      if [ -f /unix -a ! -f /etc/conf/bin/idinstall ]
      then                        # TOS
         CC=cc
      else                        # SINIX
         CC=m30cc
      fi
   else
     if [ `uname -s` = OSF1 ]
     then
       CC=cxx
     else
       CC=cc
     fi
   fi
   echo $CC -o $exec $options $main $files $PCSQLDB
	$CC -o $exec $options $main $files $PCSQLDB
else
   echo use sqllist
fi
echo odbclnk $exec finished
