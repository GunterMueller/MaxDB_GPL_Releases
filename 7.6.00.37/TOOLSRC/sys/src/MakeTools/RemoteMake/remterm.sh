#!/bin/sh
PATH=/devtool/local/bin:/usr/local/bin:$PATH export PATH
. $1/.iprofile
BASHCALL=/usr/local/bin/bash
cd $1

if [ -f /bin/bash ]
then
        BASHCALL=/bin/bash
fi

if [ -f /devtool/local/bin/bash ]
then
        BASHCALL=/devtool/local/bin/bash
fi

xterm -display $2:0.0 +ls -e  $BASHCALL >/dev/null 2>&1 &
