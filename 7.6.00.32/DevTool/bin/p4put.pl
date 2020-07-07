# 20.11.2000 h.b.
#
# execute p4 submit with a given description text
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
use Getopt::Std;
use ICopy;

chdir ($TMP);

my $tempfile = "p4put.dat";

if (!getopts ('hd:f:P:c:') or ($opt_h) or (!$opt_d) or (@ARGV < 1)) {

  print "usage: p4put -d <description string> <file> [<file> ...]\n\n";
  exit 1;
}
my $P4Exec = "p4";
$P4Exec = "$P4Exec -P $opt_P " if ($opt_P); 
$P4Exec = "$P4Exec -c $opt_c " if ($opt_c); 

open (PIPE_IN, "$P4Exec change -o |") or die "can't pipe output from p4: $!\n";
open (FILE_OUT, ">$tempfile") or die "can't open $tempfile\n";

while (<PIPE_IN>) {

    s/<enter description here>/$opt_d/;
    
    print FILE_OUT $_;
    
    last if (/^Files:$/);
    
}

foreach $file (@ARGV) { 
    if ( $file =~ /.*\.(uunix|punix|tpunx|cpnix|cunix|upnix)$/ ) {
        $rel_path = "sys/src/su/$file";
    } else {
        ($vmake_path, $rel_path) = ICopy::GetFilePath($file, 0, 0, 0);
    }
    print FILE_OUT "\t$ENV{DEPOT}/$rel_path  #edit\n";
}

close (FILE_OUT);
close (PIPE_IN);

system ("$P4Exec submit -i $opt_f < $tempfile");
unlink ($tempfile);
