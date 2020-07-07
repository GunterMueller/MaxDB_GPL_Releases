
package Error;

use strict;
use File::Basename;
use vars qw($Dump_Env);

$Dump_Env = 0;

sub throw {

    my ($errname, $err_text, $rc, $dump_env) = @_;
	my ($oldpkg, $oldfile, $errstr) = (0, "", "", "");

	for (my $i=0; (caller($i))[0]; $i++)
	{
		if (((caller($i))[0] ne $oldpkg) || ((caller($i))[1] ne $oldfile))
		{
			$errstr .= "\n  Package '" . (caller($i))[0] . "' in file '" . (caller($i))[1] . "':\n\n";
			$oldpkg = (caller($i))[0];
			$oldfile = (caller($i))[1];
		}
		$errstr .= "    Func '" . (caller($i))[3] . "' called in line " . (caller($i))[2] . "\n";
   	}
	my $component = basename($oldfile);
	print "\n**********************************************************\n";
	print "\n$errname: $component reported an error:\n\n";
	print "Info:\n\n$err_text\n\n" if ($err_text);
	print "Function trace:\n";
	print "$errstr\n";
	print "\n**********************************************************\n\n";
	if ($dump_env || $Dump_Env) {
		print "**********************************************************\n\n";
		print "ENVIRONMENT DUMP:\n\n";
		foreach my $key (sort keys %ENV) {
			print "$key = $ENV{$key}\n";
		}
		print "\n**********************************************************\n\n";
	
	}
	if ($rc)
	{
		print "Exiting...\n";
		exit ($rc ? $rc : 10);
	}
}

1;