#!/usr/bin/perl
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
#
#    21.06.2001 integrate new setup package for DBanalyzer (DBA)
#
#


# $Header: //prodprojects/vmake/dev/src/_tools/maketools/sys/src/BuildPackage/buildpackage.pl#5 $
#
# $DateTime: 2005/05/25 08:19:10 $
#
# $Change: 560346 $


use Getopt::Long;

%opt_ctrl=(
    'old' => \$opt_old,
    'new' => \$opt_new,
    'apo30a' => \$opt_apo30a,
	'installdir=s' => \$opt_installdir
);


# keep unused arguments in @ARGV
Getopt::Long::Configure ('pass_through');
Getopt::Long::Configure ('noauto_abbrev');

GetOptions(%opt_ctrl);

my $load_path = $0;
$load_path =~ s/[\/\\][^\/\\]+$//;
@INC = ($load_path,@INC);


if($opt_installdir =~ /\S/){
	require BuildPackage::Vars;
}


if($opt_apo30a){
    eval{
        require BuildPackage::APOCOM30A;
    };if($@){
        print STDERR "ERR: $@\n";
        exit -1;
    }
    print "\ndone\n";
    exit 0;
}

require BuildPackage::Old;
require BuildPackage::New;

$opt_old && $opt_new && die("ERR: new and old option set\n");


if($opt_new || $opt_old){
    if($opt_new){
        BuildPackage::New::start(@ARGV);
    }
    else{
        BuildPackage::Old::start(@ARGV);
    }
}
else{
    if(($ENV{'RELVER'} eq 'R74' or $ENV{'RELVER'} eq 'R73') and $ENV{'CORRECTION_LEVEL'} > 2){
        BuildPackage::New::start(@ARGV);
    }
    elsif($ENV{'RELVER'} eq 'R74' and $ENV{'CORRECTION_LEVEL'} == 1 and $ENV{'BUILDPRAEFIX'} > 15){
        BuildPackage::New::start(@ARGV);
    }
    elsif($ENV{'RELVER'} eq 'R74' and $ENV{'CORRECTION_LEVEL'} == 2 and $ENV{'BUILDPRAEFIX'} > 0){
        BuildPackage::New::start(@ARGV);
    }
    elsif($ENV{'RELVER'} gt 'R74')
    {
        BuildPackage::New::start(@ARGV);
    }
    else{
        BuildPackage::Old::start(@ARGV);
    }
}

