# @(#)   desc/fast/os.com   6.1.2   08/15/96
#
#
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

&if $OS = OS2
*                       trace=none check=no -OPT="-O -Op"
*-u       -DUSER        trace=none check=no -OPT="-O -Op"
*-s       -DSERVER      trace=none check=no -OPT="-O -Op"
*-k       -DKERNEL   -Y trace=none check=no -OPT="-O -Op"
*-c       -DCOMPO       trace=none check=no -OPT="-O -Op"
*-w       -DWINCOMPO    trace=none check=no -OPT="-O -Op"
&else
*                       trace=none check=no
*-u       -DUSER        trace=none check=no
*-s       -DSERVER      trace=none check=no
*-k       -DKERNEL   -Y trace=none check=no
*-c       -DCOMPO       trace=none check=no
*-w       -DWINCOMPO    trace=none check=no
*-x       -DASLIBRARY   trace=none check=no
&endif


vos01tc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp009c,vos00c,vos00mc                                   # TEST
vos01ic   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc                                           # INSTALL
vos01cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc                                           # SystemRC
vos96cc   -DSERVER -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00xsc -DRUN_AS_SERVICE        # SERVER
vos96oc   -DSERVER -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00xsc -DRUN_AS_SERVICE        # SERVER
vos96tc   -DSERVER -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00xsc -DRUN_AS_SERVICE        # SERVER
vos96trc  -DSERVER -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00xsc -DRUN_AS_SERVICE        # SERVER

vos87cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc -x=ntcpc                           # DBTIMES
vos88cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc -x=ntcpc                           # DBPARAMS
vos89cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00kc                                    # DEVINIT  OS/2 only
vos90cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc -DRUN_AS_SERVICE                          # START
vos91cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00kc,vos00mc -DRUN_AS_SERVICE           # STOP
vos92c-c  -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,veo00xc,veo002c,vos00kc,vos00mc,vos00cnc           # CONS
vos92c-m  -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,veo00xc,veo002c,vos00kc,vos00mc,vos00cnc -DRUN_AS_SYSMON -x=ntcpc # SYSMON
vos93cc   -DCOMPO  -uc -ansi inc=vos00rxc                                                                                 # SQLREXX  OS/2 only
vos94cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp009c,vos00c,vos00mc,veo005c                            # SERVICE  Win32 only

vos97cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp009c,vos00c,vos00mc                                   # SHOW
vos98cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc                                    # PUTPARAM
vos99cc   -DCOMPO  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc                                    # GETPARAM

# RTE COMPONENTS
vos60c-c  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc

# WINDOWS COMPONENTS
vos60c-w  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc

# COMMON ASM.
vos76a

#
#  TCP/IP
#
vos40lc   -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00vc,vos00mc                                         # Windows NT only
vos40c-n  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00vc,vos00cmc,vos00vc  -DNO_SOCKET
vos40c-1  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00vc,vos00cmc,vos00vc                                # Windows NT only

&if $OS = OS2
vos40c-2  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc  -Gt -Gs -Wppc- -INCL=%TCP200_INCLUDE% # OS/2
&else
vos40c-2  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00vc  -DNO_SOCKET_DELAY             # Windows NT only
&endif

#
# SERVER
#
vos33oc   -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos42oc   -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos01c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos02c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00vc
vos09c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c
vos23c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp009c,vos00c,vos00cmc,vos00mc
vos03c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos33c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos41c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos42c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos43c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos44c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c
vos46c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c
vos47c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c
vos49c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos50c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos57c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos60c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos80c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c
vos96c-s  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos97c-s  -uc -ansi

#
#  USER
#
vos01c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos02c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc,vos00vc
vos06c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos03c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos09c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos11c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos23c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vsp009c
vos41c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos42c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos43c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos44c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos46c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos47c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos49c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos50c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos57c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos60c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos33c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos80c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos90c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos94c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vsp009c,vos00c,vos00mc,veo005c
vos96c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos97c-u  -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc

vos01uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos01uc-t -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc  -DWINTERM
vos02uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos03uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc
vos03uc-t -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc  -DWINTERM
vos04u                     inc=veo00c,vos00spc,vsp00,vsp002c,vsp004c,vos00c
vos04uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00dsc,vos00mc  -DVIO_KBD
vos05uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos06uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos06uc-t -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc  -DWINTERM
vos07uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos08uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos09uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00dsc,vos00mc    # Windows NT only
vos10uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos11uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc,
vos12uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos13uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc,veo004c  -DUSE_REGISTRY_DATABASE
vos14uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos17uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00cmc
vos18uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos21uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c
vos24uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos40uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos40uc-t -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc  -DWINTERM
vos57uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos69uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c,vos00mc
vos80uc   -DUSER -uc -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vos00c



#
#  KERNEL
#
vos01c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc
vos02c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00vc
vos03c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00vc
vos06c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos11c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos23c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vsp009c
vos33c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00kc
vos41c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos42c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00cmc,vos00mc,vos00kc
vos43c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos44c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00kc
vos46c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00kc
vos47c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00kc
vos49c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos50c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos60c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos97c-k  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc

vos01kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc

# COM Layer
# COM Layer needs MS-VC++ Version 5 therefore vos35kc is compiled without 
#           DCOM support. vos36kc, vos37kc, vos38kc need to be included
#           into sqlkern.lib and vos35kc has to be compiled without
#           defining NO_DCOM to get COM support for UDT's within the kernel.
vos35kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos36kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp001c,vos00dc -D_WIN32_DCOM 
vos37kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp001c,vos00dc -D_WIN32_DCOM  
vos38kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp001c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc,vos00dc -D_WIN32_DCOM 
vos38kc-d -Y -uc -ansi inc=vsp00c,vsp001c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc,vos00dc -D_WIN32_DCOM 

vos50kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc,vos00cmc
vos51kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp001c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos52kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos53kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos54kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos55kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos56kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos57kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos58kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos59kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos60kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos61kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos62kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos63kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos64kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos65kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos66kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mc,vos00kc
vos68kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos69kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos70kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos71ka   -DKERNEL
vos71kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos72kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos73kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos74kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos79kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos80kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos80koc  -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc -DWINDOWCOMPAT
vos80knc  -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos81kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos82kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cnc
vos83kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos84kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc,vos00cmc
vos85kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc
vos88kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,veo002c,vos00kc
vos89kc   -DKERNEL -Y -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,vos00mkc,vos00kc,vos00cmc

# control components
vos901c        -ansi inc=vsp00c,vsp003c,vsp009c,vsp002c,veo00c,vos00spc,vos00c
vos902c        -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vos00c,veo00xc,veo002c,vos00kc,vos00mc,vos00cnc,vsp009c,veo005c
vos903c        -ansi inc=vsp00c,vsp009c
vos905c -DUSER -ansi inc=vsp00c,vsp002c,vsp004c,veo00c,vos00spc,vsp009c,vos00c,vos00cmc,vos00mc
vos906c        -ansi inc=vsp00c,vsp001c,vsp002c,veo00c,vos00spc,vsp009c,vos00c,vos00cmc,vos00mc,vos00xsc

vos92c-x d -DCTRLCOMP  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp004c,vos00c,veo00xc,veo002c,vos00kc,vos00mc,vos00cnc,vsp009c,veo005c
vos94c-x d -DCTRLCOMP  -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp004c,vsp009c,vos00c,vos00mc,veo005c

vos97cc-x d -DCTRLCOMP -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp004c,vsp009c,vos00c,vos00mc,veo005c
vos97cc-s d -DCTRLCOMP -uc -ansi inc=vsp00c,vsp002c,veo00c,vos00spc,vsp004c,vsp009c,vos00c,vos00mc,veo005c -DRUN_AS_SERVICE
