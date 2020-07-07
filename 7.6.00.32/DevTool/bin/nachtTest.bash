#!/bin/bash

Usage()
{
  echo "Syntax:nachtTest.sh nacht|weekend Release [BIT64]"
  echo "Start test normal(slow) or at weekend (slow/quick/fast)"
  exit 0
}

if [ $# -lt 1 ]; then
 Usage
fi

User=cron
Mode=$1
shift
Release=$1
shift
Want64=$1
shift

. /rel/TOOL/tool/bin/SetupMachineEnv.bash ${Release}

#---------------------------------------------------
#
# Prepare special release handling....
#   instead of having 'release features' distributed around the script
#   please decide here on release specific stuff...
#
if [ "x$Release" = "x72" -o "x$Release" = "x74" -o "x$Release" = "x741" -o "x$Release" = "x742" ]; then
    CallJavaLivecacheTests=YES
else
    CallJavaLivecacheTests=NO
fi
#
if [ "x$Release" = "x74" -o "x$Release" = "x741" -o "x$Release" = "x742" ]; then
    CallJavaRecoveryTests=YES
else
    CallJavaRecoveryTests=NO
fi
#
if [ "x$Release" = "x80" -o "x$Release" = "x731" ]; then
    CallJavaOLTPTests=NO
else
    CallJavaOLTPTests=YES
fi
#
if [ "x$Release" = "x731" ]; then
    CallJavaIFATests=YES
else
    CallJavaIFATests=NO
fi
#
if [ "x$Release" = "x74" -o "x$Release" = "x743" -o "x$Release" = "x742" ]; then
    CallJavaOdbcTests=YES
else
    CallJavaOdbcTests=NO
fi
#
if [ "x$Release" = "x74" -o "x$Release" = "x741" -o "x$Release" = "x72" -o "x$Release" = "x73" ]; then
    CallJavaDBMTests=YES
else
    CallJavaDBMTests=NO
fi
#
if [ "x$Release" = "x73" -o "x$Release" = "x742" -o "x$Release" = "x74" ]; then
    CallUnicodeTests=YES
else
    CallUnicodeTests=NO
fi
#
if [ "x$Release" = "x74" ]; then
    CallJavaUnicodeTests=YES
else
    CallJavaUnicodeTests=NO
fi
#
if [ "x$Release" = "x80" -o "x$Release" = "x731" ]; then
    CallQuickOrFastOnWeekend=NO
else
    CallQuickOrFastOnWeekend=YES
fi
#
if [ "x$Release" = "x80" -o "x$Release" = "x731" ]; then
    CallSlowSutall=NO
else
    CallSlowSutall=YES
fi
if [ "x$Release" = "x80" ]; then
    CallInterfaceTest=YES
else
    CallInterfaceTest=NO
fi
#
#
#---------------------------------------------------
#
# OWN depends on 32/64 support and actual release
#
# RR (i.e. 72,73,74,80) leads to
#   /rel/RR i.e. /rel/73
#      32Bit if 64Bit is not supported (DEC)
#      64Bit if 32Bit is not supported (LINUX)
#   /rel/RR_32
#      32Bit if 64Bit is supported too
#   /rel/RR_64
#      64Bit if 32Bit is supported too
#
if [ "$BIT32Supported" = "NO" -o "$BIT64Supported" = "NO" ]; then
    OWN=/rel/${Release}
    LOG=$TOOL/currentLogin/${Release}.rel
    SUTDBName="SUT${Release}"
else
    RelBuild="`echo $Release | cut -c3-`"
    if [ "x$RelBuild" = "x" ]; then
        if [ "x$Want64" = "xBIT64" ]; then
            OWN=/rel/${Release}_64
            LOG=$TOOL/currentLogin/${Release}_64.rel
            SUTDBName="SUT${Release}_64"
        else
            OWN=/rel/${Release}_32
            LOG=$TOOL/currentLogin/${Release}_32.rel
            SUTDBName="SUT${Release}_32"
        fi
    else
        if [ $RelBuild -lt 10 ]; then
            if [ "x$Want64" = "xBIT64" ]; then
                OWN=/rel/${Release}_64
                LOG=$TOOL/currentLogin/${Release}_64.rel
                SUTDBName="SUT${Release}64"
            else
                OWN=/rel/${Release}_32
                LOG=$TOOL/currentLogin/${Release}_32.rel
                SUTDBName="SUT${Release}32"
            fi
        else
            if [ "x$Want64" = "xBIT64" ]; then
                OWN=/rel/${Release}_64
                LOG=$TOOL/currentLogin/${Release}_64.rel
                SUTDBName="SU${Release}64"
            else
                OWN=/rel/${Release}_32
                LOG=$TOOL/currentLogin/${Release}_32.rel
                SUTDBName="SU${Release}32"
            fi
        fi
    fi
fi

#
# Coding reacts on BIT64 Environment variable set (even if value is 0...)
# So for 32Bit compilation unset BIT64 is needed
#
if [ "x$Want64" = "xBIT64" ]; then
     BIT64=1
     export BIT64
     shift
else
     unset BIT64
fi

Pid=$$
LoginTime=`date`
# Reenable this to get history records
# LOGHISTORY=$HOME/.remuserLogins
LOGHISTORY=/dev/null
/bin/echo at ${LoginTime} started $0 ${User} >>$LOGHISTORY

TERM=vt100
export TERM

giveup()
{
	/bin/echo ${User} logged out at `date` >>$LOGHISTORY
	exit 0;
}

trap "giveup" HUP INT QUIT TERM PIPE

TST=
echo OWN = $OWN

if [ -f $LOG ]; then
 echo "remuser on `hostname` busy."
 cat $LOG
 giveup
fi

cleanup()
{
        rm $LOG
        /bin/echo ${User} logged out at `date` >>$LOGHISTORY
        exit 0
}

/bin/echo "Current User=${User} Pid=${Pid} logged in at ${LoginTime}" >$LOG
trap "cleanup" HUP INT QUIT TERM PIPE
#
# Keep only last 80 latest entries
#
( cd $HOME/nachtlauf; rm -rf xxx `ls -t1 | tail +80` )

/bin/echo "Command:$0" >>$LOG
/bin/echo "Command:$0" >>$LOGHISTORY

DBROOT=$OWN/usr
DEV_STUDIO=$Release
PYTHONPATH=$TOOL/lib/Python
PERL5LIB=$TOOL/bin:$TOOL/lib/perl5:$OWN/usr/misc

export OWN DBROOT PYTHONPATH DEV_STUDIO PERL5LIB

if [ -d $TOOL ]
then
    if [ -r $OWN/.iprofile ]
    then
      . $OWN/.iprofile

      if [ -f $HOME/.vdnvars ]
      then
          . $HOME/.vdnvars
      else
          echo "can't find .vdnvars "
      fi

      TOOLVARS=$TOOL/bin/toolvars.pl export TOOLVARS

      DBTERM=vt100_hp
      export DBTERM

      umask 0002

      cd $OWN
      echo
      echo "==================================================================="
      echo ---  `uname -a`  ---
      echo "==================================================================="

    else
        echo "can't read .iprofile"
    fi
else
    echo "can't find $TOOL"
fi

##################################################
#
# Java based liveCache tests
#
##################################################
echo "==================================================================="
echo "start Java based liveCache tests"
echo "==================================================================="
if [ "$LiveCacheOS" = "YES" ]; then
  if [ "$BIT64" = "1" ]; then
    if [ "x$CallJavaLivecacheTests" = "xYES" ]; then
      echo "---------------"
      cd $OWN/test/jtest

      if [ "x$Mode" = "xweekend" ]; then
        perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -d LC$Release -sr -monitor lctest_weekend
      else
        perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -d LC$Release -sr -monitor lctest_night
      fi
    else
      echo "No Java liveCache tests for release $Release"
    fi
  else
    echo "No Java liveCache tests for 32 Bit releases"
  fi
else
  echo "No liveCache OS"
fi

##################################################
#
# Java based recovery tests
#
##################################################
echo "==================================================================="
echo "start Java based recovery tests"
echo "==================================================================="
if [ "$LiveCacheOS" = "YES" ]; then
  if [ "$BIT64" = "1" ]; then
    if [ "x$CallJavaRecoveryTests" = "xYES" ]; then
      echo "---------------"
      cd $OWN/test/jtest

      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -d LC${Release}R -sr -monitor lctest_recover

    else
      echo "No Java recovery tests for release $Release"
    fi
  else
    echo "No Java recovery tests for 32 Bit releases"
  fi
else
  echo "No liveCache OS"
fi





echo "==================================================================="
echo "start SUT test slow"
echo "==================================================================="
if [ -x $OWN/usr/pgm/slowknl ]; then
  echo "--------------"
  echo "Calling sutall"
  cd $TEST/kern/sut
  x_server
  echo "db_stop"
  dbmcli -d ${SUTDBName} -u SUPERDBA,COLDUSER db_stop
  echo "db_clear"
  dbmcli -d ${SUTDBName} -u SUPERDBA,COLDUSER db_clear
  echo "Calling sutparam"
  sutparam.pl -d ${SUTDBName} -o remuser,Remuser
  if [ "x$CallSlowSutall" = "xNO" ]; then
    echo "NOT Calling sutall for release $Release"
  else
    echo "Calling sutall"
    if [ "x$CallJavaLivecacheTests" = "xNO" ]; then
      rm -f VZLCTEST.*
    fi
    sutall.pl slow slowci ${SUTDBName}
#    python $TOOL/bin/qaccSUT.py $OWN/test/kern/allsut.prt
  fi
else
  echo "No new slowknl found -> sutall slow not started"
fi

if [ "x$Mode" = "xweekend" ]; then
  if [ "x$CallQuickOrFastOnWeekend" = "xNO" ]; then
    echo "sutall (quick/fast) not wanted for now..."
  else
	echo "==================================================================="
	echo "start SUT test quick"
	echo "==================================================================="
    if [ -x $OWN/usr/pgm/quickknl ]; then
      echo "--------------"
      echo "Calling sutparam"
      sutparam.pl -d ${SUTDBName} -o remuser,Remuser
      echo "Calling sutall (quick)"
      sutall.pl quick slowci ${SUTDBName}
#      python $TOOL/bin/qaccSUT.py $OWN/test/kern/allsut.prt
    else
      echo "no quickknl found -> no sutall quick"
    fi
	echo "==================================================================="
	echo "start SUT test fast"
	echo "==================================================================="
    if [ -x $OWN/usr/pgm/kernel ]; then
      echo "--------------"
      echo "Calling sutparam"
      sutparam.pl -d ${SUTDBName} -o remuser,Remuser
      echo "Calling sutall (fast)"
      sutall.pl fast slowci ${SUTDBName}
    else
      echo "no fast kernel found -> no sutall fast"
    fi
    if [ "x$CallUnicodeTests" = "xYES" ]; then
#     Unicode Tests
      echo "--------------"

      if [ -x $OWN/usr/pgm/kernel ]; then
        DBUNICODE=xci
        export DBUNICODE
        DEFCODE=UNICODE
        export DEFCODE
        echo "Calling sutall (DBUNICODE=xci, DEFCODE=UNICODE)"
        sutall.pl fast slowci ${SUTDBName}
#        python $TOOL/bin/qaccSUT.py $OWN/test/kern/allsut.prt
      else
        echo "no fast kernel found -> no UNICODE sutall fast"
      fi

      if [ -x $OWN/usr/pgm/slowknl ]; then
        DBUNICODE=
        export DBUNICODE
        DEFCODE=
        export DEFCODE
        imf -u XRESTORE.cmd
        sutsingle.pl slow slowci ${SUTDBName} XINITDB.vdnts
        sutsingle.pl slow slowci ${SUTDBName} YINIT.vdnts
      else
        echo "no slow kernel found -> no UNICODE sutsingle slow"
      fi
    fi
  fi
fi

##################################################
#
# Java based OLTP tests
#
##################################################
echo "==================================================================="
echo "start Java based OLTP and ODBC tests"
echo "==================================================================="
if [ "x$CallJavaOLTPTests" = "xYES" ]; then
    echo  "----------------"
    echo "Running jtest"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor oltptest
    else
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor -d OL32$Release oltptest
    fi
else
    echo "No Java OLTP tests for release $Release"
fi

if [ "x$CallJavaOdbcTests" = "xYES" ]; then
    echo  "----------------"
    echo "Running jtest"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor odbc
    else
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor -d OD32$Release odbc
    fi
else
    echo "No Java ODBC tests for release $Release"
fi

if [ "x$CallJavaIFATests" = "xYES" ]; then
    echo  "----------------"
    echo "Running jtest"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor ifatest
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor ifatest_unicode
    else
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor ifatest
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor ifatest_unicode
    fi
else
    echo "No Java IFA tests for release $Release"
fi

if [ "x$CallInterfaceTest" = "xYES" ]; then
    echo  "----------------"
    echo "Running Interfacetests"
    cd $OWN
#    if [ "$BIT64" = "1" ]; then
#      perl $OWN/usr/pgm/runtests.pl -d DB74 -n 10.31.165.14 -u DBA,DBA
#    fi
else
    echo "No New Interface Runtime tests for release $Release"
fi


##################################################
#
# Java based UNICODE OLTP tests
#
##################################################
echo "==================================================================="
echo "start Java based UNICODE OLTP tests"
echo "==================================================================="
if [ "x$CallJavaUnicodeTests" = "xYES" ]; then
    echo  "----------------"
    echo "Running jtest -> oltptest_unicode"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor oltptest_unicode
    else
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor -d OL32${Release}U oltptest_unicode
    fi
    echo  "----------------"
    echo "Running jtest -> odbc_unicode"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -sr -monitor odbc_unicode
    else
      perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -sr -monitor -d OD32${Release}U odbc_unicode
    fi

else
    echo "No Java UNICODE tests for release $Release"
fi

##################################################
#
# Java based DBM tests
#
##################################################
echo "==================================================================="
echo "start Java based DBM tests"
echo "==================================================================="
if [ "x$CallJavaDBMTests" = "xYES" ]; then
    echo  "----------------"
    echo "Running jtest"
    cd $OWN/test/jtest

    if [ "$BIT64" = "1" ]; then
        perl $OWN/test/jtest/jtrun.pl -scheduled -bits 64 -monitor DBMTest
    else
        perl $OWN/test/jtest/jtrun.pl -scheduled -bits 32 -monitor DBMTest
    fi

else
    echo "No Java DBM-tests for release $Release"
fi

echo "-------------------------------------------------------------------"
echo Nachtlauf finished `date`
echo "-------------------------------------------------------------------"

cleanup

