# script to call msdev with environment from toolvars
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

use Env;
use Getopt::Std;
use OLE;
use Win32::Process;

getopts('qs');
if ( $opt_q )
{
    # quick
    $ENV{VMAKE_VERSION} = "quick";
    $ENV{VMAKE_DEFAULT} = "quick";
	print ("Start quick ...");
}
elsif ( $opt_s ) {
    # slow
    $ENV{VMAKE_VERSION} = "slow";
    $ENV{VMAKE_DEFAULT} = "quick";
	print ("Start slow ...");
}
else
{
	$ENV{VMAKE_VERSION} = "fast";
	$ENV{VMAKE_DEFAULT} = "fast";
	print ("Start fast ...");
}
do "$TOOLVARS";
die $@ if $@;

$ENV{BUILDPID} = "PID$$";

$ENV{BUILDNUMBER} = "0";
$ENV{BUILDSTRING} = "00-000-000-000";


Win32::Process::Create($msdev_app,
	"c:\\Program Files\\Microsoft Visual Studio\\Common\\MSDev98\\Bin\\msdev.exe",
	"msdev D:\\SAPDevelop\\TOOLSRC\\develop\\sys\\src\\tt\\vmake-project\\vmake_project.dsw",
	1,
    NORMAL_PRIORITY_CLASS,
    ".")|| die "cannot create process\n";



        

#$editor = CreateObject OLE 'MSDEV.APPLICATION'
#			or die "Couldn't create new instance of MsDev App!!";

#$editor->{'WindowState'} = 3;#dsWindowStateNormal
#$editor->{'Visible'} = 1;
#$editor->{'Active'} = 1;
