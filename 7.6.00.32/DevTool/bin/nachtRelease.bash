#!/bin/bash
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
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
#
# nachtRelease.bash
#
# complete night run test script to run a single release
#

Usage()
{
  echo "Syntax:$0 release nacht|weekend [32BitOnly|64BitOnly|32__64Bit]"
  echo "Start compilation and tests normal or at weekend (clears worktree)"
  exit 0
}

if [ $# -lt 2 ]
then
 Usage
fi

User=cron
Release=$1
shift
Mode=$1
shift
Schedule=$1
shift


# kill running day compilation
. /rel/TOOL/tool/bin/SetupMachineEnv.bash ${Release}
if  [ "$Release" = "74" ]; then
  if [ "$BIT32Supported" = "NO" -o "$BIT64Supported" = "NO" ]
  then
      /rel/TOOL/tool/bin/nachtkill.pl -k 74
  else
      /rel/TOOL/tool/bin/nachtkill.pl -k 74_64
  fi
fi

echo Update of remuser tool area
/rel/TOOL/tool/bin/updateRemuserTools.bash

# called twice because of possible update in updateRemuserTools
. /rel/TOOL/tool/bin/SetupMachineEnv.bash ${Release}

# Since this script only creates test releases, build number requests are suppressed
export RELSTAT=TEST

giveup()
{
	echo "$0 terminated by signal....";
	exit 0;
}

cleanupSpace()
{
     #  rm -rf xxx $1_D*/sys/wrk
     #  rm -rf xxx $1_D*/sys/wrk32
     #  rm -rf xxx $1_D*/tmp/*
       rm -rf xxx $1/sys/wrk
       rm -rf xxx $1/sys/wrk32
       rm -rf xxx $1/tmp/*
       rm -rf xxx $1/usr/bin
       rm -rf xxx $1/usr/pgm
       rm -rf xxx $1/usr/incl
       rm -rf xxx $1/test/pc
#
# Keep only the last 10000 lines in allsut.prt
#
       tail -10000 $1/test/kern/allsut.prt > $1/test/allsut.prt
       rm -rf xxx $1/test/kern
       mkdir -p $1/test/kern
       mv $1/test/allsut.prt $1/test/kern/.

#
# Keep only last 10 latest protocols in jtest, remove all
#
       ( cd $1/test/jtest/protocols; rm -rf xxx `ls -t1 | tail +10` )
       mv $1/test/jtest/protocols $1/test/jtest_protocols
       rm -rf xxx $1/test/jtest
       mkdir -p $1/test/jtest
       mv $1/test/jtest_protocols $1/test/jtest/protocols
#
# Keep only last 50 latest protocols in /rel/nachtlauf
#
       ( cd /rel/nachtlauf; rm -rf xxx `ls -t1 | tail +50` )
}

trap "giveup" HUP INT QUIT TERM PIPE

echo =====================================================================
echo "Complete night run mode Release $Release $Mode $Schedule"


if [ "x$Mode" = "xweekend" ]; then

   echo =====================================================================
   echo "Clearing working directories"
   echo =====================================================================
   if [ "$BIT32Supported" = "NO" -o "$BIT64Supported" = "NO" ]
   then
       cleanupSpace /rel/${Release}
   else
       if [ "$Schedule" != "64BitOnly" -a "$BIT32Supported" = "YES" ]
       then
           cleanupSpace /rel/${Release}_32
       fi
       if [ "$Schedule" != "32BitOnly" -a "$BIT64Supported" = "YES" ]
       then
           cleanupSpace /rel/${Release}_64
       fi
   fi
   echo $Release clearing complete
fi

echo =====================================================================
echo "Clearing working directories `date` finished"
echo =====================================================================

if [ "$Schedule" != "64BitOnly" -a "$BIT32Supported" = "YES" ]
then
    echo =====================================================================
    echo "Compilation 32 Bit of releases $Release"
    echo =====================================================================
    $TOOL/bin/nachtCompilation.bash $Mode $Release
fi

if [ "$Schedule" != "32BitOnly" -a "$BIT64Supported" = "YES" ]
then
  echo =====================================================================
    echo "Compilation 64 Bit of releases $Release"
  echo =====================================================================
    $TOOL/bin/nachtCompilation.bash $Mode $Release BIT64
fi

if [ "x$Release" = "x73" ]
then
  echo =====================================================================
  echo "Starting depending nachtRelease 731 script of release $Release"
  echo =====================================================================
  if [ "x$Mode" = "xweekend" ]
  then
    $TOOL/bin/nachtRelease.bash 731 $Mode $Schedule 1>/rel/nachtlauf/`$HOME/bin/today`_731_Weekend 2>&1 &
  else
    if [ "$Schedule" = "32BitOnly" ]
    then
      $TOOL/bin/nachtRelease.bash 731 $Mode $Schedule 1>/rel/nachtlauf/`$HOME/bin/today`_731_32Bit_Nacht 2>&1 &
    else
      if [ "$Schedule" = "64BitOnly" ]
      then
        $TOOL/bin/nachtRelease.bash 731 $Mode $Schedule 1>/rel/nachtlauf/`$HOME/bin/today`_731_64Bit_Nacht 2>&1 &
      else
        $TOOL/bin/nachtRelease.bash 731 $Mode $Schedule 1>/rel/nachtlauf/`$HOME/bin/today`_731_3264Bit_Nacht 2>&1 &
      fi
    fi
  fi
fi

if [ "$Schedule" != "64BitOnly" -a "$BIT32Supported" = "YES" ]
then
  echo =====================================================================
    echo "Testrun 32 Bit"
  echo =====================================================================
    $TOOL/bin/nachtTest.bash $Mode $Release
fi

if [ "$Schedule" != "32BitOnly" -a "$BIT64Supported" = "YES" ]
then
  echo =====================================================================
    echo "Testrun 64 Bit"
  echo =====================================================================
    $TOOL/bin/nachtTest.bash $Mode $Release BIT64
fi
#
exit 0

