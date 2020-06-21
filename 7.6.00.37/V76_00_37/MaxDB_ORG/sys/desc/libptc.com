#
# ptoc Support Library (libptc.a)
#
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

*                       inc=:libptc/h00vars.h,:libptc/libpc.h
:libptc/h00vars.h		inc=:libptc/whoami.h
:libptc/whoami.h		inc=
:libptc/libpc.h         inc=
:libptc/Version.c		inc=
:libptc/sql__1r.c
:libptc/sql__atan.c		inc=:libptc/libpc.h
:libptc/sql__ca.c		inc=:libptc/libpc.h 
:libptc/sql__caseerr.c	inc=:libptc/libpc.h
:libptc/sql__catchae.c	inc=:libptc/libpc.h
:libptc/sql__ckl.c		inc=
:libptc/sql__clock.c	inc=
:libptc/sql__close.c
:libptc/sql__closea.c
:libptc/sql__closep.c
:libptc/sql__compare.c	inc=:libptc/libpc.h
:libptc/sql__cos.c		inc=:libptc/libpc.h
:libptc/sql__date.c		inc=:libptc/libpc.h
:libptc/sql__del.c
:libptc/sql__dfn.c
:libptc/sql__dispse.c
:libptc/sql__endof.c
:libptc/sql__endol.c
:libptc/sql__env.c
:libptc/sql__er.c
:libptc/sql__erq.c
:libptc/sql__exit.c
:libptc/sql__exp.c		inc=:libptc/libpc.h
:libptc/sql__expp.c 	inc=:libptc/libpc.h
:libptc/sql__fflush.c
:libptc/sql__flp.c
:libptc/sql__ger.c		inc=:libptc/libpc.h
:libptc/sql__get.c
:libptc/sql__gn.c
:libptc/sql__goto.c		inc=:libptc/libpc.h
:libptc/sql__gtl.c		inc=:libptc/libpc.h
:libptc/sql__ic.c
:libptc/sql__intr.c
:libptc/sql__lel.c		inc=:libptc/libpc.h
:libptc/sql__lexe.c
:libptc/sql__ln.c		inc=:libptc/libpc.h
:libptc/sql__lor.c
:libptc/sql__ltl.c		inc=:libptc/libpc.h
:libptc/sql__max.c
:libptc/sql__multi.c	inc=:libptc/libpc.h
:libptc/sql__new.c
:libptc/sql__nil.c
:libptc/sql__nlf.c
:libptc/sql__nname.c
:libptc/sql__npa.c
:libptc/sql__nre.c		inc=:libptc/libpc.h
:libptc/sql__pack.c		inc=:libptc/libpc.h
:libptc/sql__pcnst.c
:libptc/sql__peer.c		inc=:libptc/libpc.h
:libptc/sql__perrorp.c	inc=:libptc/libpc.h
:libptc/sql__pmain.c
:libptc/sql__pr.c
:libptc/sql__pred.c		inc=:libptc/libpc.h
:libptc/sql__psetadd.c	inc=:libptc/libpc.h
:libptc/sql__psetco.c
:libptc/sql__psetge.c	inc=:libptc/libpc.h
:libptc/sql__psetgt.c	inc=:libptc/libpc.h
:libptc/sql__psetle.c	inc=:libptc/libpc.h
:libptc/sql__psetlt.c	inc=:libptc/libpc.h
:libptc/sql__psetm.c	inc=:libptc/libpc.h
:libptc/sql__pstrcat.c	inc=:libptc/libpc.h
:libptc/sql__pstrcom.c	inc=:libptc/libpc.h
:libptc/sql__pstrcpy.c	inc=:libptc/libpc.h
:libptc/sql__put.c
:libptc/sql__pvars.c	inc=:libptc/libpc.h
:libptc/sql__rd.c
:libptc/sql__readln.c
:libptc/sql__reset.c
:libptc/sql__rewrite.c
:libptc/sql__rgv.c
:libptc/sql__round.c	inc=:libptc/libpc.h
:libptc/sql__scnst.c
:libptc/sql__setadd.c	inc=:libptc/libpc.h
:libptc/sql__setco.c
:libptc/sql__setmin.c	inc=:libptc/libpc.h
:libptc/sql__setrang.c	inc=:libptc/libpc.h
:libptc/sql__setsub.c	inc=:libptc/libpc.h
:libptc/sql__sin.c		inc=:libptc/libpc.h
:libptc/sql__sqrt.c		inc=:libptc/libpc.h
:libptc/sql__srand.c
:libptc/sql__stopp.c
:libptc/sql__strlen.c	inc=:libptc/libpc.h
:libptc/sql__succ.c		inc=:libptc/libpc.h
:libptc/sql__sync.c
:libptc/sql__time.c		inc=:libptc/libpc.h
:libptc/sql__trunc.c	inc=:libptc/libpc.h
:libptc/sql__ucmp.c		inc=:libptc/libpc.h
:libptc/sql__uncs.c
:libptc/sql__unget.c
:libptc/sql__unpack.c	inc=:libptc/libpc.h
:libptc/sql__ut.c
:libptc/sql__writeln.c
