$| = 1;
system("$ENV{'TOOL'}\\pgm\\robocopy.exe", @ARGV);
my $rc = $? >> 8;
my $sig = $? & 127;
my $core = $? & 128;
print "\nRobocopy => Returncode=$rc, Signal=$sig, Coredump=$core\n\n";
$rc = 0 if ($rc == 1);
exit($rc);
