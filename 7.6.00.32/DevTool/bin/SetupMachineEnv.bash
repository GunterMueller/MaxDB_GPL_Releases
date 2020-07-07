Usage()
{
  echo "Syntax: SetupMachineEnv <Release>"
  echo "set machine environment for <Release>"
  exit 0
}

if [ $# -lt 1 ]; then
 Usage
fi

# Release 72,73,74,80,...
Release=$1

# no more cutted lines....
CMD_OUTPUT_LENGTH=0
export CMD_OUTPUT_LENGTH

# new perforce server
P4PORT="perforce3006.wdf.sap.corp:3006"
export P4PORT

#########################################################################
# On all UNIX remuser platforms /rel/TOOL must be either a symbolic link
# to the TOOL directory or it is the TOOL directory...
TOOL=/rel/TOOL/tool
export TOOL

PATH=${TOOL}/bin:${HOME}/bin:/rel/DevTool/local/bin

#####################################################################
#
# Needed platform dependend environment
#
# PATH  The path information
# BIT32SUPPORTED YES or NO
# BIT64SUPPORTED YES or NO
# LiveCacheOS YES or NO
# MAXSUTDURATION
#




#####################################################################
PATH=${PATH}:/usr/sbin:/sbin:/usr/bin:/usr/local/bin:/bin:/usr/bin:/usr/X11R6/bin:/usr/X11R6/bin:.:$HOME/IBMJava2-13/jre/bin
LD_LIBRARY_PATH=/usr/local/lib
export PATH LD_LIBRARY_PATH

BIT32Supported=YES
export BIT32Supported

BIT64Supported=NO
export BIT64Supported

LiveCacheOS=YES
export LiveCacheOS

MAXSUTDURATION=1800
export MAXSUTDURATION

MIRROROS=UNIX/LINUX_32
MIRRORNEEDED=NO

#####################################################################

RSH=rsh

####################################################################
# DevPackage mirror establishing
# (temporary solution...)

if [ "x$Release" = "x74" -a "x$MIRRORNEEDED" = "xYES" ]; then
    MIRRORHOST=ls0106.wdf.sap-ag.de
    MIRRORUSER=remuser
    MIRRORPATH=/sapmnt/ls0106/b/LC/SapdbDevPackage/$MIRROROS
fi

if [ -r $HOME/localEnvironment ]; then
    . $HOME/localEnvironment $Release
fi
