
print "\nrunsut 1.01 (21-01-2004)\n\n";

my $rc = 0;
if ($^O =~ /Win32/i)
{
    unless ($ENV{'SUTSEQ'})
    {
        print "Environment SUTSEQ is not set. Exiting.\n";
        exit(0);
    }
    $ENV{'MAKEID'} = 0 unless ($ENV{'MAKEID'});
    $ENV{'CHANGELIST'} = 0 unless ($ENV{'CHANGELIST'});
    $ENV{'BITS'} = ($ENV{'BIT64'} ? "64" : "32");
    print "Removing old SUT files...\n\n";
    system("del /S /Q $ENV{'TEST'}\\jtest\\sut");
    system("del /S /Q $ENV{'TEST'}\\jtest\\tests");
    print "Making all test packages...\n";
    system("imf alltestpkg");
    print "Re-installing SUT files again...\n";
    system("imf instsut");
    chdir ("$ENV{'TEST'}/jtest");
#    print "Removing old x_server...\n";
#    system("x_server remove");
#    print "Registering x_server...\n";
#    system("x_server install");
#    print "Starting x_server...\n";
#    system("x_server start");
    $rc = $rc + system ("$ENV{'TOOLSHELL'} $ENV{'TEST'}\\jtest\\jtrun.pl -scheduled -QA $ENV{'LC_STATE'} -bits $ENV{'BITS'} -sr -MAKEKEY $ENV{'MAKEID'} -monitor -CL $ENV{'CHANGELIST'} $ENV{'SUTSEQ'}");
    $rc = ($rc > 0 ? 0 : -$rc);
    $rc = ($rc >> 8) if ($rc > 255);
}
else
{
    print "This tool does only support Windows for now...\n";
}

exit($rc);