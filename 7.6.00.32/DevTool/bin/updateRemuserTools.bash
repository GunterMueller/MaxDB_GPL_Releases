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
# updateRemuserTools.bash
#
# update remuser tool area /rel/TOOL/...
#

if [ ! -d /rel/TOOL/tool/currentLogin ]; then
    mkdir /rel/TOOL/tool/currentLogin
fi

LOG=/rel/TOOL/tool/currentLogin/TOOL

cleanup()
{
        rm $LOG
        exit 0
}

trap "cleanup" HUP INT QUIT TERM PIPE

set -o noclobber # prevent over writing from files
if ! /bin/echo "Current User=${User} Pid=${Pid} logged in at ${LoginTime}" >$LOG; then
 set +o noclobber # make files over writeable again
 while [ -f $LOG ]; do
  echo "tool make in process."
  sleep 60
 done
 exit 0;
fi
set +o noclobber # make files over writeable again

if ls -1 /rel/TOOL/tool/currentLogin/*.rel; then
  echo "another make in process."
  cleanup
fi

. /rel/TOOL/tool/bin/SetupMachineEnv.bash TOOL
. go TOOL
unset DEPOT
imf alltools
imf vmake
imf igme
imf remuser

cleanup
