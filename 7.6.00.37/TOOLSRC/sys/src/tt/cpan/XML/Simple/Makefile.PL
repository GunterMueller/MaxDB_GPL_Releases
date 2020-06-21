# $Id: Makefile.PL,v 1.6 2003/09/09 09:37:15 grantm Exp $

use ExtUtils::MakeMaker;

my ($InstBin, $InstLib, $InstMan1, $InstMan3) = get_inst_dirs (get_tool_dir ());

my $make_params = {

    'NAME'					=> 'XML::Simple',
    'VERSION_FROM'			=> 'lib/XML/Simple.pm',
    'DISTNAME'				=> 'XML-Simple',
   # 'PREREQ_PM'				=> { Test::Simple => 0.41 },
    'dist'					=> { COMPRESS => 'gzip --best', SUFFIX => 'gz' },
#    'PM'					=> { 'XML/Simple.pm' => '$(INST_LIBDIR)/Simple.pm'},
    'INSTALLDIRS'			=> "site",
    'INSTALLSITELIB'		=> $InstLib,
    'INSTALLSITEARCH'		=> $InstLib,
    'INSTALLBIN'			=> $InstBin,
    'INSTALLSCRIPT'			=> $InstBin,
    'INSTALLARCHLIB'		=> $InstLib,
    'INSTALLMAN1DIR'		=> $InstMan1,
	'INSTALLMAN3DIR'		=> $InstMan3,
	'INSTALLSITEMAN1DIR' =>  "$InstMan1/site",
	'INSTALLSITEMAN3DIR' =>  "$InstMan3/site",
    ($] >= 5.005 ? (
      'AUTHOR'				=> 'Grant McLean <grantm@cpan.org>',
      'ABSTRACT_FROM'		=> 'lib/XML/Simple.pm',
    ) : () )
};

print "Checking installed modules ...\n";

if ( eval { require XML::SAX } && ! $@ ) {
    print "XML::SAX is installed, it will be used by the test suite\n";
    $make_params->{PREREQ_PM}->{'XML::SAX'} = 0;
    $make_params->{PREREQ_PM}->{'XML::NamespaceSupport'} = 1.04;
}
elsif ( eval { require XML::Parser } && ! $@ ) {
    print "XML::Parser is installed, it will be used by the test suite\n";
    $make_params->{PREREQ_PM}->{'XML::Parser'} = 0;
} 
else {
    print "You don't have either XML::SAX or XML::Parser installed!\n";
    $make_params->{PREREQ_PM}->{'XML::SAX'} = 0;
    $make_params->{PREREQ_PM}->{'XML::NamespaceSupport'} = 1.04;
}

eval { require Storable };
if($@) {
  print "Storable is not installed ... caching functions will not be available\n";
}


WriteMakefile(%$make_params);

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
