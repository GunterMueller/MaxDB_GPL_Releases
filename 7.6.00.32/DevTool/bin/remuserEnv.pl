#!/usr/bin/perl
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#
# no more cutted lines
$ENV{CMD_OUTPUT_LENGTH}=0;
# new perforce server
$ENV{P4USER}="remuser";
$ENV{P4PORT}="perforce3006.wdf.sap.corp:3006";
#
$ENV{ADDEDPATH}="/rel/TOOL/tool/bin:$ENV{HOME}/bin:/rel/DevTool/local/bin";
    $ENV{PATH}="$ENV{ADDEDPATH}:/usr/bin:/opt/SUNWspro/bin:/usr/ccs/bin:/etc:/usr/openwin/bin:/usr/local/bin";
    $ENV{LD_LIBRARY_PATH}="/opt/SUNWspro/lib:/usr/ccs/lib";
$No64Bit=1;
$No32Bit=0;
$ENV{USER}="remuser";
$MIRROROS="UNIX/SOLARIS_I386";
$MIRRORNEEDED="YES";
$RSH="rsh";
#
# End of SUN specific
#
#####################################################################
#####################################################################
#
# DEC specific
#
$BASHCALL="bash -noprofile -rcfile $ENV{HOME}/bashrc.local";
$ENV{PATH}="$ENV{ADDEDPATH}:/sbin:/usr/sbin:/usr/bin:/usr/ccs/bin:/usr/bin/X11:/usr/local/bin:/usr/local/kde/bin:/usr/opt/jre/bin";
$No64Bit=0;
$No32Bit=1;
$MIRROROS="UNIX/DEC_64";
$MIRRORNEEDED="YES";
$RSH="rsh";
#
# End of DEC specific
#
#####################################################################
#####################################################################
#
# SNI specific
#
$BASHCALL="bash --noprofile --rcfile $ENV{HOME}/bashrc.local";
$ENV{PATH}="$ENV{ADDEDPATH}:/usr/bin:/etc:/usr/ucb:/usr/ccs/bin:/usr/bin/X11:/sbin:/usr/local/bin";
$No64Bit=0;
$No32Bit=0;
$MIRROROS="UNIX/RLX_64";
$MIRRORNEEDED="NO";
$RSH="rsh";
#
# End of SNI specific
#
#####################################################################
#####################################################################
#
# LINUX specific
#
$BASHCALL="bash -noprofile -rcfile $ENV{HOME}/bashrc.local";
$ENV{PATH}="$ENV{ADDEDPATH}:/usr/sbin:/sbin:/usr/bin:/usr/local/bin:/bin:/usr/bin:/usr/X11R6/bin:/usr/X11R6/bin:/usr/local/jdk1.2.2/bin";
$ENV{LD_LIBRARY_PATH}="/usr/local/lib";
$No64Bit=1;
$No32Bit=0;
$MIRROROS="UNIX/LINUX_32";
$MIRRORNEEDED="NO";
$RSH="rsh";
#
# End of LINUX specific
#
#####################################################################
#####################################################################
#
# LINUX specific
#
$BASHCALL="bash -noprofile -rcfile $ENV{HOME}/bashrc.local";
$ENV{PATH}="$ENV{ADDEDPATH}:/usr/sbin:/sbin:/usr/bin:/usr/local/bin:/bin:/usr/bin:/usr/X11R6/bin:/usr/X11R6/bin:/usr/local/jdk1.2.2/bin";
$ENV{LD_LIBRARY_PATH}="/usr/local/lib";
$No64Bit=0;
$No32Bit=1;
$MIRROROS="UNIX/LINUX_64";
$MIRRORNEEDED="NO";
$RSH="rsh";
#
# End of LINUX specific
#
#####################################################################
#####################################################################
#
# HPUX specific
#
$BASHCALL="bash --noprofile --rcfile $ENV{HOME}/bashrc.local";
$ENV{USER}="remuser";
$ENV{PATH}="$ENV{ADDEDPATH}:/usr/bin:/usr/bin/X11:/usr/local/bin:/opt/aCC/bin:/opt/aCC/lbin:/opt/Python/bin:/opt/langtools/bin:/opt/java/bin";
#$ENV{LD_LIBRARY_PATH}="";
$ENV{LM_LICENSE_FILE}="/etc/license.dat";
$No64Bit=0;
$No32Bit=0;
$MIRROROS="UNIX/HP11_64";
$MIRRORNEEDED="YES";
$RSH="remsh";
#
# End of HPUX specific
#
#####################################################################
$BASHCALL="bash -noprofile -rcfile $ENV{HOME}/bashrc.local";
$ENV{USER}="remuser";
$ENV{PATH}="$ENV{ADDEDPATH}:$ENV{PATH}";
$RSH="rsh";

if ( ( $IgmeVersion eq "7.4.03.develop" )
  && ( $MIRRORNEEDED eq "YES" ) )
{
    $MIRRORHOST="ls0106.wdf.sap.corp";
    $MIRRORUSER="remuser";
    $MIRRORPATH="/sapmnt/ls0106/b/LC/SapdbDevPackage/$MIRROROS";
}

do "$ENV{HOME}/localEnvironment.pl";
1;
