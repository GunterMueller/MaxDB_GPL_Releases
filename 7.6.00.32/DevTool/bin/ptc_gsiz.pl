#!/usr/bin/perl
# ptc_gsiz      *** included shell script
#
# @(#)ptc_gsiz  6.1.2  1995-09-20
#
# Included in shmlink and shmprelink.
# Create gsize.c
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

use Env;
use ICopy;

unlink "allsize";

if ( $SHRGLOB ) {
	open(FILE_IN, "gsize") || die "Can't open gsize: $!"; # gsize created by vmake
	open(FILE_OUT, ">>allsize") || die "Can't open allsize: $!";
	while(<FILE_IN>) {
		chomp;
		open(GSIZE, $_) || die "Can't open $_: $!";
		print FILE_OUT <GSIZE>;
		close(GSIZE);
	}
	close(FILE_IN);
	close(FILE_OUT);
} else {
	$OUTFILE =~ /(.*)\.[fqso]$/, $BASE = $1 ne "" ? $1 : $OUTFILE;
	open(PIPE_IN, "$TOOL/pgm/vmake -NU $BASE |") || die "Can't open pipe $TOOL/pgm/vmake -nU $BASE: $!";
    while (<PIPE_IN>) {
		chomp;
        if ( /^v\w+\d\d+.*shrglob/ )
		{
			next if /^v\w+00|^g\w+\d+/;
            s/debug|shrglob//;
            /^(v\w\w\d\d+[^\s]*)\s+(fast|quick|slow)/;
			local($output) = "$OWN/sys/wrk/$2/gsize/$1.gsz";
            local($fname) ="sys/wrk/$2/gsize/$1.gsz";
            # &gar
            if ( ! -f "$output")
			{
              local @vmakepath = ICopy::_GetPathList($VMAKE_PATH);
              local $curlevel=1;  # without $OWN
              $output="";
              do
              {
                 if (ICopy::_get_path_status($vmakepath[$curlevel]) eq 'local' )
                 {
                    if ( ($abs_path, $rel_path ) = ICopy::GetFilePath($fname, $curlevel, 1, 1) )
                    {
                       $output="$abs_path/$rel_path";
                    }
                 }
                 $curlevel++;
              }
              while (($curlevel<=$#vmakepath) && ($output eq ""));
            }
            if ($output eq "")
            {
              warn "Warning : $fname not found\n";
            }
            else
            {
              push @tmplist, $output unless /[ca]\.gsz/ ;
            }
        }
    }
 	close(PIPE_IN);
    foreach $i ( @tmplist ) {

        if ( -f $i ) {
			open(FILE_IN, "<$i") || die "Can't open $i: $!";
			open(FILE_OUT, ">>allsize") || die "Can't open allsize: $!";
			print FILE_OUT <FILE_IN>;
			close(FILE_IN); close(FILE_OUT);
        } else { warn "Warning : $i not found\n" }
    }
}

system("$TOOL/Posix/touch allsize");  # if not exists
system("$TOOL/Posix/sort -u allsize > gsize.siz");
system("$TOOL/pgm/ptc_ginit gsize.siz > gsize.c") == 0
	or die "Error while executing \"$TOOL/pgm/ptc_ginit gsize.siz > gsize.c\"\n".
	"message: $!\n";


