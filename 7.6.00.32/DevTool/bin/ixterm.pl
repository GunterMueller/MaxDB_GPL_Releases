#!/usr/bin/perl
#
# ixterm.pl
# creates a child procces and runs a shell in a 'xterm'
# we use environment for session properties, so a childs
# window looks by default like parent window
#
# usage: ixterm [-display <DISPLAY>] [-cr <COLOR>]
#        [-fg <COLOR>] [-bg <COLOR>] [-fn <FONT>]
#        [-sb] [-bash] [-e <CMD>]
#
#	-display <DISPLAY>: X display
#	-cr <COLOR>:        color of caret (cursor)
#   -fg <COLOR>:        forground color
#   -bg <COLOR>:        background color
#   -fn <FONT>:         font
#   -sb:                show scrollbar
#	-bash:              use 'bash' as shell
#   -e <CMD>:			execute command <CMD> in new shell
#
# cba 98/01/13
#     funtional body separated in a perl-module,
#     ixterm.pl is only a wrapper
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


use Env;
push @INC, $ENV{'HOME'}."/bin";

require "ixterm.pm";
ixterm::ixterm (@ARGV);

