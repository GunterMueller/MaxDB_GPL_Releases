
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

my ($VMAKE_COLLECTION) = "$ENV{TMP}/$$";
my ($OUTFILE) = "a.dot";

    @XMLFILE = ();

my %ext2Type = ( "mac", "macro",
                 "lnk", "program",
                 "shm", "program",
                 "lib", "library",
                 "rel", "relocatable",
                 "dld", "relocatable"
                 );

my %include = ( "macro", 1,
                "program", 1,
                "library", 1,
                "relocatable", 1
                );

%displayTags = ( "*", 0,
                    "name", 1,
                    "module", 0,
                    "unknown", 0,
                    "include", 0,
                    "macro", 1,
                    "program", 1,
                    "library", 1,
                    "relocatable", 1,
                    "dependencies", 1 );

%shapeAttr = ( "mac", "[color=skyblue,style=filled]" );

use Pmod;

getopts('i:o:');
    if ($opt_o) {
        $OUTFILE = $opt_o;
    }
    if ($opt_i) {
        $VMAKE_COLLECTION=$opt_i;
    }
    else {
        if (@ARGV != 0) {
            @desc = @ARGV;
        }
        else {
            @desc = "all";
        }
        open(SAVEOUT, ">&STDOUT");
        open(STDOUT, ">$VMAKE_COLLECTION");
        $name = $desc[0];
        foreach $desc (@desc) {
            @ARGV = ("-c", $desc);
            do "$TOOL/bin/imf$TOOLEXT";
            if ( $@ ) { die "Error while executing ims$TOOLEXT: $@" }
        }

        close(STDOUT);
        open(STDOUT, ">&SAVEOUT");
    }

    open(file_in, "<$VMAKE_COLLECTION");
    open(file_out, ">$OUTFILE");
print file_out "graph G {\n";
    while(<file_in>) {
        chop;
        if (/^macro/
            || /^program/
            || /^relocatable/
            || /^library/
            || /^module/
            || /^include/
            || /^file/
            || /^command/
            ) {
            if (defined $caller) {
                undef $caller;
            }
            $caller = "$&" if ($include{$&});
        }
        if (/ +name +/ ) {
            if (defined $caller) {
                if (/ +(\w+) +'(.*)'/) {
                    $name = $2;
                    if ($name =~ /^(\w+|\w+-\w+)\.(\w+)$/) {
                        $name = $1;
                        $ext = "$2";
                        if ($name =~ /^(\w+)-(\w+)/) {
                            $name = $1;
                            $varspec = $2;
                        }
                        $ext2Type{$ext} = $caller;
                    }
                    $caller = "$name.$ext";
                    if (defined $shapeAttr{$ext} ) {
                        print file_out "\"$caller\" $shapeAttr{$ext}\n";
}
                }
            }
        }
        if (/ +dependencies +/ ) {
            if (defined $caller) {
                if (/(\w+)/) {
                  LINE:
                    while(<file_in>) {
                        chop;
                        if ( /'(.*)'\t.*/ ) {
                            $value = "$1";
                            if ($value =~ /^(\w+|\w+-\w+)\.(\w+)$/) {
                                $name = "$1";
                                $ext = "$2";
                                if ($name =~ /^(\w+)-(\w+)/) {
                                    $name = $1;
                                    $varspec = $2;
                                }
                                $type = $ext2Type{$ext};
                                if (defined $type) {
                                    print file_out "\"$caller\" -- \"$name.$ext\";\n";
                                }
                                else {
#                                print file_out "\"$caller\" -- \"$name.$ext\";\n";
                                }
                            }
                            else {
                                if ($value =~ /^(v[a-zA-Z]{2})(\w+|\w+-\w+)$/) {
#                                print file_out "\"$caller\" -- \"$name.$ext\";\n";
                                }
                            }
                        }
                        else {
                            last LINE;
                        }
                    }
                }
            }
        }
    }
print file_out "}\n";
    close(file_out);
    close(file_in);
system("dotty $OUTFILE");
