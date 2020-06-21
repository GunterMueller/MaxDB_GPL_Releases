#
#  Copyright (c) 1995-2000, Raphael Manfredi
#  
#  You may redistribute only under the same terms as Perl 5, as specified
#  in the README file that comes with the distribution.
#

use ExtUtils::MakeMaker;
use Config;

my ($InstBin, $InstLib, $InstMan1, $InstMan3) = &get_inst_dirs(&get_tool_dir());

WriteMakefile(
    NAME                => 'Storable',
    DISTNAME            => "Storable",
    INSTALLSITELIB      => $InstLib,
    INSTALLSITEARCH     => $InstLib,
    INSTALLBIN          => $InstBin,
    INSTALLSCRIPT       => $InstBin,
    INSTALLARCHLIB      => $InstLib,   
    INSTALLMAN1DIR => $InstMan1,
	INSTALLMAN3DIR => $InstMan3,
	INSTALLSITEMAN1DIR =>  "$InstMan1/site",
	INSTALLSITEMAN3DIR =>  "$InstMan3/site",		
    MAN3PODS            => {},
    PREREQ_PM           => { 'Test::More' => '0.41' },
    INSTALLDIRS         => 'perl',
    VERSION_FROM        => 'Storable.pm',
    dist                => { SUFFIX => 'gz', COMPRESS => 'gzip -f' },
);

my $ivtype = $Config{ivtype};

# I don't know if the VMS folks ever supported long long on 5.6.x
if ($ivtype and $ivtype eq 'long long' and $^O !~ /^MSWin/) {
  print <<'EOM';

You appear to have a perl configured to use 64 bit integers in its scalar
variables.  If you have existing data written with an earlier version of
Storable which this version of Storable refuses to load with a

   Byte order is not compatible

error, then please read the section "64 bit data in perl 5.6.0 and 5.6.1"
in the Storable documentation for instructions on how to read your data.

(You can find the documentation at the end of Storable.pm in POD format)

EOM
}


sub get_inst_dirs {
	my $InstDir = shift;
	my $InstBin = $InstDir."/bin";
	my $InstMan1 = $InstDir."/man/man1";
	my $InstMan3 = $InstDir."/man/man3";
	my $InstLib = $InstDir;
	if ($^O =~ /win32/i) {
		$InstLib .= "/lib/Perl";
	} else {
		$InstLib .= "/lib/perl5";
	}
	return ($InstBin, $InstLib, $InstMan1, $InstMan3 );
}


sub get_tool_dir {
    my $TOOL = $ENV{"TOOL"};
    unless (defined $TOOL && $TOOL ne "") {
        die "\$TOOL undefined, please set \$TOOL\n";
    }
    return $TOOL;
}

