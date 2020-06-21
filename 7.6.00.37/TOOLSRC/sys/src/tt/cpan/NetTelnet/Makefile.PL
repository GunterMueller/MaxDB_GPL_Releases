## -*- Perl -*-

use ExtUtils::MakeMaker qw(WriteMakefile);
my ($InstBin, $InstLib, $InstMan1, $InstMan3) = &get_inst_dirs(&get_tool_dir());

WriteMakefile(NAME	   => "Net::Telnet",
  DISTNAME	   => "Net-Telnet",
			VERSION_FROM => "lib/Net/Telnet.pm",
			INSTALLSITELIB      => $InstLib,
			INSTALLSITEARCH     => $InstLib,
		    INSTALLBIN          => $InstBin,
		    INSTALLSCRIPT       => $InstBin,
		    INSTALLARCHLIB      => $InstLib,   
   		    INSTALLMAN1DIR => $InstMan1,
			INSTALLMAN3DIR => $InstMan3,
			INSTALLSITEMAN1DIR =>  "$InstMan1/site",
			INSTALLSITEMAN3DIR =>  "$InstMan3/site",
		  dist	   => { COMPRESS => "gzip", SUFFIX => "gz" },
	      ($] >= 5.005 ?
	       (ABSTRACT   => "Interact with TELNET port or other TCP ports",
		AUTHOR	   => "Jay Rogers <jay\@rgrs.com>") : ()),
	      );

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