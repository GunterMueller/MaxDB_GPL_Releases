# call test_lcversion.pl -no_src_dir -own %OWN% -lcversion 76 -debugX
# call test_lcversion.pl -no_src_dir -own %OWN% -lcversion 7.6.00 -debugX
# call test_lcversion.pl -no_src_dir -own %OWN% -lcversion 7600 -debugX
$release=$ENV{BRANCH_INFO};
$release=~ s/\.//;
push @ARGV, "-release", "$release";

do "vmakeEnv.pl";
my $lcversion = vmakeEnv::getLCVERSION();
print ("lcversion:$lcversion\n");
