# -*- perl -*-
#
# $Id: Makefile.PL,v 1.2 1999/08/12 14:28:53 joe Exp $
#

use ExtUtils::MakeMaker;


my ($InstBin, $InstLib, $InstMan1, $InstMan3) = &get_inst_dirs(&get_tool_dir());

my %opts = (
    INSTALLSITELIB      => $InstLib,
    INSTALLSITEARCH     => $InstLib,
    INSTALLBIN          => $InstBin,
    INSTALLSCRIPT       => $InstBin,
    INSTALLARCHLIB      => $InstLib,
    INSTALLMAN1DIR => $InstMan1,
	INSTALLMAN3DIR => $InstMan3,
	INSTALLSITEMAN1DIR =>  "$InstMan1/site",
	INSTALLSITEMAN3DIR =>  "$InstMan3/site",	
    'NAME'	=> 'Net::Daemon',
    'VERSION_FROM' => 'lib/Net/Daemon.pm', # finds $VERSION
    'dist' => { 'DIST_DEFAULT' => q[all tardist],
	        'COMPRESS' => q[gzip -9vf],
	        'SUFFIX' => q[.gz]
	    },
    'realclean' => { 'FILES' => 't/config ndtest.prt' }
);

if ($ExtUtils::MakeMaker::VERSION >= 5.43) {
    $opts{ABSTRACT_FROM} = 'lib/Net/Daemon.pm';
    $opts{AUTHOR} = 'Jochen Wiedmann (joe@ispsoft.de)';
}

WriteMakefile(%opts);


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




package MY;

sub libscan {
    my ($self, $path) = @_;
    ($path =~ /\~$/) ? undef : $path;
}

sub postamble {
    qq{

pm_to_blib: README

README: lib/Net/Daemon.pm
\tpod2text lib/Net/Daemon.pm >README

}
}
