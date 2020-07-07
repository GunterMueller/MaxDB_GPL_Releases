#!/bin/bash
#
# @(#)installRemuser.bash 2001-09-13
#
# J. Mensing, SAP AG
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
Usage() {
    echo Syntax: installRemuser
    echo Usage: Install/Check environment setup for remuser
    echo Need preinstalled /rel/TOOL/tool/bin directory, preinstalled remuser 998 group remuser 998
    echo Need lot of space on /rel/...
    echo This can be called directly, but is normally called via imf remuser.mac
}

if [ $# -gt 1 ]; then
    Usage
elif [ ! -d /rel/TOOL/tool/bin ]; then
    echo Missing Tool directory /rel/TOOL/tool/bin
elif [ ! -r /rel/TOOL/tool/bin/SetupMachineEnv.bash ]; then
    echo Missing /rel/TOOL/tool/bin/SetupMachineEnv.bash
elif [ "x$USER" != "xremuser" ]; then
    echo Must be called by remuser only
elif [ ! -r /rel/TOOL/tool/bin/Skeleton ]; then
    echo Missing iprofile Skeleton file
else
#
#  If setup individually .vdnvars is just touched, but this guaranties it exists...
#
    echo Touching .vdnvars
    touch $HOME/.vdnvars
#
#  .rhosts file must exist and be mode 600
#
    if [ ! -r $HOME/.rhosts ]; then
        echo Creating .rhosts file allowing access to everybody
        touch $HOME/.rhosts
        chmod 600 $HOME/.rhosts
        echo "+ +" >$HOME/.rhosts
    fi
#
#  Skeleton File creation
#
    if [ ! -r /rel/Skeleton ]; then
        echo Creating new Skeleton file
        cp /rel/TOOL/tool/bin/Skeleton /rel/Skeleton
    fi

    if [ ! -r /rel/Skeleton6 ]; then
        echo Creating new Skeleton6 file
        cat /rel/TOOL/tool/bin/Skeleton | sed -e "s/VMAKE_OPTION=SOIDpchvMLRgbWX/VMAKE_OPTION=SOIDpchvMLgbWX/" > /rel/Skeleton6
    fi
fi
