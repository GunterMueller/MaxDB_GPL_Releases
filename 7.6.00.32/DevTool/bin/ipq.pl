#!/usr/bin/perl
# ipq           [-old | -h{0-9}] target
#
# @(#)ipq  		1999-07-27
#
# Displays protocol of "target".
#
# changes:
# cba 1999/07/27
# body moved to iprot.pm ADIS CR 1103480
#
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

package ipq;

use Env;
use Carp;
use iprot;

iprot ("ipq", @ARGV);

