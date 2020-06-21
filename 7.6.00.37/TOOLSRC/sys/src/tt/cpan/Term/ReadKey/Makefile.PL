# Term::ReadKey Makefile.PL Version 2.14

use ExtUtils::MakeMaker;
use Carp;

my $mm_version = $ExtUtils::MakeMaker::VERSION || $ExtUtils::MakeMaker::Version;
if( $mm_version < 3.5 ) {
        croak("Sorry, but MakeMaker 3.5 or better is needed to build this package.");
}

#$Verbose = 0;
my ($InstBin, $InstLib, $InstMan1, $InstMan3) = get_inst_dirs (get_tool_dir());

&WriteMakefile(
	NAME => "Term::ReadKey",
	DISTNAME => "TermReadKey",
	VERSION_FROM => "ReadKey.pm",
	INSTALLDIRS => "site",
	INSTALLSITELIB => $InstLib,
	INSTALLSITEARCH => $InstLib,
	INSTALLBIN => $InstBin,
	INSTALLSCRIPT => $InstBin,
	INSTALLARCHLIB => $InstLib,
    INSTALLMAN1DIR => $InstMan1,
	INSTALLMAN3DIR => $InstMan3,
	INSTALLSITEMAN1DIR =>  "$InstMan1/site",
	INSTALLSITEMAN3DIR =>  "$InstMan3/site",
	PM => { "ReadKey.pm" => '$(INST_LIBDIR)/ReadKey.pm'},
	'dist' => { COMPRESS=>"gzip", SUFFIX=>"gz" },
        
# Uncomment these to allow testing of sgtty under Linux. Not needed normally.
#	INC => "-I/usr/include/bsd",
#	LIBS => "-lbsd"
);

sub get_inst_dirs {
	my $InstDir = shift;
	my $InstBin = $InstDir."/bin";
	my $InstMan1 = $InstDir."/man/man1";
	my $InstMan3 = $InstDir."/man/man3";
	my $InstLib = $InstDir;
	if ($^O =~ /win32/) {
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

sub MY::realclean {
	my $self = shift;
	$_ = $self->MM::realclean();
	s/\t/\trm -f cchars.h\n\t/;
	$_;
}

sub MY::top_targets {
	my $self = shift;
	$_ = $self->MM::top_targets();
	$_ .= "

sgtty cchars.h: genchars.pl
	\$(PERL) -I\$(PERL_LIB) genchars.pl

distcc: genchars.pl
	\$(PERL) -I\$(PERL_LIB) genchars.pl dist

ReadKey.c: cchars.h

";
	$_;

}

sub MY::test {
	my $self = shift;
	$_ = $self->MM::test();
	s/test.pl/-w test.pl/;
	$_;
}
