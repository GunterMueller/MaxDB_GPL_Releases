
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


use Mail::Sendmail;

# check arguments
if ( @ARGV < 2 ) {	
    print "usage : $0 <duser> [ <duser> ...] <File to send> \n";
    exit
}

do "./mutvar.all";
if ( $@ ) { print "$@"; exit }

# get the list of receiver and expand with domain name
local @MutToList;
while (@ARGV > 1) { push ( @MutToList, shift ); }
map { $_ .= '@sap.com' } @MutToList;

# read file
local $SendFile = shift;
open (INFILE,"<$SendFile") || die "Can't find $SendFile";
local @MutMsg = <INFILE>;
close (INFILE);


# fill sendmail struct
local %mail = (
          To      => join (',', @MutToList),
    	  From    => $ENV{USERNAME}.'@exchange.sap.corp',
	      Message => join ('', @MutMsg),
          Smtp    => 'mail.sap-ag.de',
	      Subject => "$CWD($DBNAME)" );

#send mail
sendmail(%mail) or warn $Mail::Sendmail::error;
