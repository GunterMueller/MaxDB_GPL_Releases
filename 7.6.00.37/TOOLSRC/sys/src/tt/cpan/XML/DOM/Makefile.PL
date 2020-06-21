use ExtUtils::MakeMaker;

@extras = ();

my ($InstBin, $InstLib, $InstMan1, $InstMan3) = get_inst_dirs (get_tool_dir ());

push (@extras,
    ABSTRACT_FROM => 'DOM.pm',
    AUTHOR        => 'Enno Derksen (enno@att.com) and Clark Cooper (coopercl@sch.ge.com)'
)
        if ($ExtUtils::MakeMaker::Version >= 5.4301);

WriteMakefile (
    NAME         => 'XML::DOM',
    dist         => {COMPRESS => 'gzip', SUFFIX => '.gz'},
    VERSION_FROM => 'DOM.pm',
    PREREQ_PM    => { XML::Parser => '2.23' },
    PM           => { 'DOM.pm' => '$(INST_LIBDIR)/DOM.pm'},
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
    @extras
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
