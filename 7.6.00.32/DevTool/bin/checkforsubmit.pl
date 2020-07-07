#!/usr/bin/perl
#
# @(#) checkforsubmit     h.b. 31.07.2002

use FileHandle;
use Getopt::Std;


if (!getopts('vh') or $opt_h) {
  print "$0 [-v] [-h]\n";
  exit -1;
}

my ( $fh, $date, @p4out, @desc, @src, @localtime );

@localtime = localtime (time - (5,0,0,0,0,1,4)[(localtime)[6]] * 24 * 3600);

$date = sprintf ("%04d/%02d/%02d", $localtime[5] + 1900, $localtime[4] + 1, $localtime[3]);

$fh = new FileHandle;

if ($fh->open("p4 changes -s submitted $ENV{DEPOT}/.../desc/...\@$date,now |")) {
        @p4out = <$fh>;
        $fh->close;
}
@desc = grep !/implicit/ , @p4out;
print "desc found:\n@desc\n" if ( @desc and $opt_v);

if ($fh->open("p4 changes -s submitted $ENV{DEPOT}/.../src/...\@$date,now |")) {
        @p4out = <$fh>;
        $fh->close;
}
@src = grep !/implicit/ , @p4out;
print "source found:\n@src\n" if ( @src and $opt_v);

exit -1 if ( @desc or @src );

exit 0;


