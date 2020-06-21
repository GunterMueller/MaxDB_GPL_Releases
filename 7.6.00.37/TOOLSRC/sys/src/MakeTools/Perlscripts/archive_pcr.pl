#! /usr/bin/perl
#
#    archive_pcr.pl - archives the precompiler runtimes
#
#    @(#)archive_pcr.pl     2003-02-24
#
#    U. Jansen, SAP AG
#
#    *WARNING* *WARNING* *WARNING* *WARNING* *WARNING* *WARNING*
#
#    This script can only run, if a valid Makeprofiles.xml is
#    found under $OWN/sys/wrk/config !!!
#
#    *WARNING* *WARNING* *WARNING* *WARNING* *WARNING* *WARNING*
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

BEGIN {
    unshift @INC, "$ENV{TOOL}/bin";
    unshift @INC, "$ENV{TOOL}/lib/Perl";
}


use IO::SimpleCfg;
use Getopt::Long;
use BuildPackage::Tools;

BuildPackage::Tools::setErrMsgHandler(
	sub {
		die (@_);
	}
);

BuildPackage::Tools::setMsgHandler(
	sub {
		#print (@_);
	}
);

eval {
    require Pod::Usage;
    import Pod::Usage;
};
if ($@) {
    sub pod2usage {
        print "Sorry. POD is not supported on this platform. For help, please call this\nscript under Windows or use a perl that can do Pod's.\n";
        exit(1);
    }
}

select (STDERR); $| = 1;
select (STDOUT); $| = 1;

my $ProgName = "archive_pcr v2.0 (c)2005, U. Jansen, SAP AG";
my %param_cfg;
my %cfg;
my $loaded_cfg;
my $cfg_file = "$ENV{'OWN'}/sys/wrk/config/ArchivePCR.cfg";
my $cd_image_path = $^O =~ /mswin/i ? '//production.wdf.sap.corp/SCM/MaxDB/CD-Images' : '/sapmnt/production/makeresults/SCM/MaxDB/CD-Images';
my $use_old_procedure;

print "\n$ProgName\n\n";

if ($ENV{'LC_STATE'} eq "DEV") { print "Sorry, this release is not supported.\n"; exit(0); }

GetOptions (
    'cfgfile=s'     => sub { $cfg_file = $_[1]; },
    'destdir=s'     => sub { $param_cfg{'DESTDIR'} = $_[1]; },
    'filelist=s'    => sub { $param_cfg{'FILELIST'} = $_[1]; },
    'h|help'        => sub { pod2usage(1); },
    'man'           => sub { pod2usage( -exitstatus => 0, -verbose => 2 ); },
    'password=s'    => sub { $param_cfg{'PASSWORD'} = $_[1]; },
    'server=s'      => sub { $param_cfg{'SERVER'} = $_[1]; },
    'user=s'        => sub { $param_cfg{'USER'} = $_[1]; },
    'old'			=> \$use_old_procedure
) or pod2usage("\n");

EnvSetCfg("MACHINE_CONFIG");
EnvSetCfg("OWN");
EnvSetCfg("RELVER");
EnvSetCfg("CORRECTION_LEVEL");
EnvSetCfg("BUILDPRAEFIX");
$cfg{'RELVER'} = $1 if ($cfg{'RELVER'} =~ /^R(\d\d)$/);
$cfg{'CORRECTION_LEVEL'} = ($cfg{'CORRECTION_LEVEL'} =~ /^\d\d$/ ? $cfg{'CORRECTION_LEVEL'} : "0$cfg{'CORRECTION_LEVEL'}");
$cfg{'BUILDPRAEFIX'} = ($cfg{'BUILDPRAEFIX'} =~ /^\d\d$/ ? $cfg{'BUILDPRAEFIX'} : "0$cfg{'BUILDPRAEFIX'}");
$cfg{'DESTROOT'} = "/PCR_REL/$cfg{'RELVER'}$cfg{'CORRECTION_LEVEL'}$cfg{'BUILDPRAEFIX'}";
if (-e $cfg_file) { 
    $loaded_cfg = LoadSimpleCfg($cfg_file);
    foreach my $key (keys %{$loaded_cfg->{'DEFAULT'}}) { $cfg{$key} = $loaded_cfg->{'DEFAULT'}->{$key}; }
    foreach my $key (keys %{$loaded_cfg->{uc($cfg{'MACHINE_CONFIG'})}}) { $cfg{$key} = $loaded_cfg->{uc($cfg{'MACHINE_CONFIG'})}->{$key}; }
} else { 
    print "WARNING: Config file '$cfg_file' does not exist! Setting defaults...\n\n";
    $cfg{'SERVER'} = 'P26325.ber.sap.corp';
    $cfg{'USER'} = 'rempcr';
    $cfg{'PASSWORD'} = ucfirst($cfg{'USER'});
    $cfg{'FILELIST'} = ($^O =~ /win32/i ? 'SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ,pgm/sqluser76.dll,usr/pgm/sqluser76.dll,symbols/sqluser76.pdb,usr/symbols/sqluser76.pdb,support/mapfiles/sqluser76.map,usr/support/mapfiles/sqluser76.map' : 'SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ');
    if    (uc($cfg{'MACHINE_CONFIG'}) eq "NTINTEL")     { $cfg{'DESTDIR'} = "NT/I386"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "NTIA64")      { $cfg{'DESTDIR'} = "NT/IA64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "NTAMD64")     { $cfg{'DESTDIR'} = "NT/AMD64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "RS6000")      { $cfg{'DESTDIR'} = "UNIX/AIX5_32"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "RS6000_64")   { $cfg{'DESTDIR'} = "UNIX/AIX5_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "ALPHAOSF")    { $cfg{'DESTDIR'} = "UNIX/DEC_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "HP_64")       { $cfg{'DESTDIR'} = "UNIX/HP11_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "HPIA64")      { $cfg{'DESTDIR'} = "UNIX/HPIA64_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "LINUXINTEL")  { $cfg{'DESTDIR'} = "UNIX/LINUX_32"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "LINUXIA64")   { $cfg{'DESTDIR'} = "UNIX/LINUX_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "LINUXX86_64") { $cfg{'DESTDIR'} = "UNIX/LINUXX86_64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "LINUXPPC64")  { $cfg{'DESTDIR'} = "UNIX/LINUXPPC64"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "LINUXS390X")  { $cfg{'DESTDIR'} = "UNIX/LINUXS390X"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "SUN")         { $cfg{'DESTDIR'} = "UNIX/SUNOS_32"; }
    elsif (uc($cfg{'MACHINE_CONFIG'}) eq "SUN_64")      { $cfg{'DESTDIR'} = "UNIX/SUNOS_64"; }
    else { print "ERROR: Platform '$cfg{'MACHINE_CONFIG'}' is unknown!!\n Aborting!\n"; exit(1); }
}
foreach my $key (keys %param_cfg) { $cfg{$key} = $param_cfg{$key}; }
my %flist = split(/,/, $cfg{'FILELIST'});
my $count = 0;
if ($use_old_procedure){
	print "SERVER          = $cfg{'SERVER'}\n";
	print "USER            = $cfg{'USER'}\n";

}
print "SOURCE ROOT     = $cfg{'OWN'}\n";
if ($use_old_procedure){	
	print "DEST. ROOT      = $cfg{'DESTROOT'}\n";
}
else{
	print "DEST. ROOT      = $cd_image_path$cfg{'DESTROOT'}\n";
}
print "DEST. DIRECTORY = $cfg{'DESTDIR'}\n";
print "FILE LIST:\n";
foreach my $key (keys %flist) { print "\t$key (Source: $flist{$key})\n"; }
print "\n";
chdir ($cfg{'OWN'});

my $ftp;
my $dest_root = $cd_image_path.'/'.$cfg{'DESTROOT'};

if ($use_old_procedure){
	require Net::FTP;
	print "Connecting to '$cfg{'SERVER'}'..";
	$ftp = Net::FTP->new($cfg{'SERVER'}, Debug => 0);
	if ($ftp) { print "..OK\n"; }
	else { print "..FAILED to create socket!\n"; exit(1); }
	print "Logging in..";
	if ($ftp->login($cfg{'USER'}, $cfg{'PASSWORD'})) { print "..OK\n"; }
	else { print "..FAILED (wrong user/password?)\n"; $ftp->quit(); exit(1); }
	if (!$ftp->cwd("$cfg{'DESTROOT'}/$cfg{'DESTDIR'}")) {
	    print "Creating directory '$cfg{'DESTROOT'}/$cfg{'DESTDIR'}'..\n";
	    if ($ftp->mkdir("$cfg{'DESTROOT'}/$cfg{'DESTDIR'}", 1)) {
	        print "..OK\n";
	     if (!$ftp->cwd("$cfg{'DESTROOT'}/$cfg{'DESTDIR'}")) { print "ERROR: Can't change to '$cfg{'DESTROOT'}/$cfg{'DESTDIR'}'! Aborting...\n"; $ftp->quit(); exit(1); }
	    } else { print "..FAILED!\n"; $ftp->quit(); exit(1); }
	}
	$ftp->binary();
}

foreach my $destfile (keys %flist) {
    print "Copying file '$destfile'..";
	unless (-e $flist{$destfile}) {
		 print "SKIPPED! (No local file)\n";
		 next;
	}

	if ($use_old_procedure){

		if ($destfile =~ /^(.*)[\/\\](([^\/\\])+)/) { if (!$ftp->mkdir($1, 1)) { print "..FAILED to create directory '$1'! Skipping.\n"; next; }}
		if ($ftp->put($flist{$destfile}, $destfile)) { print "..OK\n"; $count ++; }
		    else { print "..FAILED!\n"; }
	}
	else{
		eval{
			copy ($flist{$destfile},$cd_image_path.$cfg{'DESTROOT'}.'/'.$cfg{'DESTDIR'}.'/'.$destfile,{'binmode' => 1,'createdir' => 1,'nochown' => 1});
		};
		if ($@){
			print "..FAILED!\n";
			print STDERR "$@\n";
		}
		else{
			print "..OK\n";
			$count ++;
		}
	}
	
}

$ftp->quit() if ($use_old_procedure);


print "\nFiles total  : " . (scalar (keys %flist)) . "\n";
print "Files copied : $count\n\n";

exit(0);

################################################################################
# SetEnvCfg() - Tranfsers a variable from ENV to cfg or throws an error.
################################################################################
sub EnvSetCfg {
    if (defined $ENV{$_[0]}) { $cfg{$_[0]} = $ENV{$_[0]}; }
    else { pod2usage("Mandatory environment variable '$_[0]' is missing!\n"); }
}

__END__

=head1 NAME

archive_pcr - A program for archiving Precompiler runtimes

=head1 SYNOPSIS

 archive_pcr [-cfgfile <FILENAME>] [-destdir <DIRECTORY>] 
             [-filelist <FILES>] [-(h|help)] [-man] [-password <PWD>] 
             [-server <HOST>] [-user <NAME>]

Options:

  -cfgfile <FILENAME>           Config file to be loaded
  -destdir <DIRECTORY>          Destination directory on FTP server
  -filelist <FILES>             Comma separated list of files to be copied
  -h or -help                   Shows a brief help
  -man                          Shows the complete manual
  -password <PWD>               Password to be used for FTP server
  -server <HOST>                Server to be used (fully qualified)
  -user <NAME>                  User name for the FTP server


=head1 OPTIONS

=over 8

=item B<-cfgfile <FILENAME>>

Loads the config file given in FILENAME instead of the default one. The default
config file is: $OWN/sys/wrk/config/ArchivePCR.cfg.

=item B<-destdir <DIRECTORY>>

Defines the destination directory where all files will be uploaded. NOTE: This
just defines the platform dependent part of the upload directory like that:
/PCR_REL/<OUR RELEASE NOT EDITABLE>/<DIRECTORY>/. If you REALLY have to redefine
the destination ROOT (/PCR_REL/<Release>), you can set the variable DESTROOT in
the config file. But be aware that this feature is neither really supported nor
tested, yet.

=item B<-filelist <FILES>>

File list to be copied to the FTP server. All entries should be comma separated 
and must have the format <DEST>,<SOURCE>,<DEST>,<SOURCE>,...
A typical entry would be: SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ

=item B<-h or -help>

Shows a brief help page

=item B<-man>

Shows these complete manual pages.

=item B<-password <PWD>>

Sets the password for the FTP user.

=item B<-server <HOST>>

Defines the server to be used to upload all files to. The host name should be
fully qualified in order to avoid connection problems.

=item B<-user <NAME>>

Sets the user to be used when loggin into the FTP server.

=back

=head1 DESCRIPTION

*** THIS PROGRAM IS FOR INTERNAL USE ONLY ***

This program will copy any file defined either as command line parameter or 
within the configuration file to an FTP server for backup purposes.

For bugfixes / suggestions, please contact gerald.arnold@sap.com (51346)

=head1 EXAMPLE CONFIG FILE

 #############################################################################
 # Configuration script for archive_pcr.pl
 # (c)2005 SAP AG, Ulrich Jansen
 #############################################################################
 
 
 #############################################################################
 # DEFAULT: This section will always be loaded first. Any given machine
 #          specific section will then 'overload' these default settings.
 #
 # The following keys are valid:
 #
 # SERVER        Defines the FTP server to upload files to
 # USER          Username to be used as FTP user
 # PASSWORD      PW for the FTP user
 # FILELIST      File list to be uploaded (Format: TO,FROM,TO,FROM,...)
 # DESTDIR       Target directory to put files into.
 #
 [DEFAULT]
 
 SERVER=P26325.ber.sap.corp
 USER=rempcr
 PASSWORD=Rempcr
 FILELIST=SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ
 
 #############################################################################
 # Platform specific settings:
 #
 # The following sections will contain platform specific settings that 
 # differ from the default ones. Any settings already set in the default part
 # does not have to be stated here again, these sections act as incremental 
 # overload.
 
 #----------------------------------------------------------------------------
 # NTintel - Windows on i386 architecture.
 #----------------------------------------------------------------------------
 [NTINTEL]
 DESTDIR=NT/I386
 FILELIST=SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ,pgm/sqluser76.dll,usr/pgm/sqluser76.dll,symbols/sqluser76.pdb,usr/symbols/sqluser76.pdb,support/mapfiles/sqluser76.map,usr/support/mapfiles/sqluser76.map
 
 #----------------------------------------------------------------------------
 # NTia64 - Windows on Intel itanium 64 bit architecture
 #----------------------------------------------------------------------------
 [NTIA64]
 DESTDIR=NT/IA64
 FILELIST=SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ,pgm/sqluser76.dll,usr/pgm/sqluser76.dll,symbols/sqluser76.pdb,usr/symbols/sqluser76.pdb,support/mapfiles/sqluser76.map,usr/support/mapfiles/sqluser76.map
 
 #----------------------------------------------------------------------------
 # NTamd64 - Windows on AMD 64 bit architecture
 #----------------------------------------------------------------------------
 [NTAMD64]
 DESTDIR=NT/AMD64
 FILELIST=SDBC7700.TGZ,sys/wrk/fast/install/SDBC7700.TGZ,pgm/sqluser76.dll,usr/pgm/sqluser76.dll,symbols/sqluser76.pdb,usr/symbols/sqluser76.pdb,support/mapfiles/sqluser76.map,usr/support/mapfiles/sqluser76.map
 
 #----------------------------------------------------------------------------
 # rs6000 - AIX5 32 bit mode
 #----------------------------------------------------------------------------
 [RS6000]
 DESTDIR=UNIX/AIX5_32
 
 #----------------------------------------------------------------------------
 # rs6000_64 - AIX5 64 bit mode
 #----------------------------------------------------------------------------
 [RS6000_64]
 DESTDIR=UNIX/AIX5_64
 
 #----------------------------------------------------------------------------
 # alphaosf - DEC Alpha Tru64 architecture
 #----------------------------------------------------------------------------
 [ALPHAOSF]
 DESTDIR=UNIX/DEC_64
 
 #----------------------------------------------------------------------------
 # hp_64 - HP PA11 Risc architecture
 #----------------------------------------------------------------------------
 [HP_64]
 DESTDIR=UNIX/HP11_64
 
 #----------------------------------------------------------------------------
 # hpia64 - HP PA20 Intel itanium architecture
 #----------------------------------------------------------------------------
 [HPIA64]
 DESTDIR=UNIX/HPIA64_64
 
 #----------------------------------------------------------------------------
 # linuxintel - Linux on i386 32 bit architecture
 #----------------------------------------------------------------------------
 [LINUXINTEL]
 DESTDIR=UNIX/LINUX_32
 
 #----------------------------------------------------------------------------
 # linuxia64 - Linux on Intel itanium 64 bit architecture
 #----------------------------------------------------------------------------
 [LINUXIA64]
 DESTDIR=UNIX/LINUX_64
 
 #----------------------------------------------------------------------------
 # linuxx86_64 - Linux on AMD x86 64 bit architecture 
 #----------------------------------------------------------------------------
 [LINUXX86_64]
 DESTDIR=UNIX/LINUXX86_64
 
 #----------------------------------------------------------------------------
 # linuxppc64 - Linux on Motorola PPC 64 bit architecture
 #----------------------------------------------------------------------------
 [LINUXPPC64]
 DESTDIR=UNIX/LINUXPPC64
 
 #----------------------------------------------------------------------------
 # linuxs390x - Linux on IBM S390X architecture
 #----------------------------------------------------------------------------
 [LINUXS390X]
 DESTDIR=UNIX/LINUXS390X
 
 #----------------------------------------------------------------------------
 # sun - SUN on 32 bit
 #----------------------------------------------------------------------------
 [SUN]
 DESTDIR=UNIX/SUNOS_32
 
 #----------------------------------------------------------------------------
 # sun_64 - SUN on 64 bit
 #----------------------------------------------------------------------------
 [SUN_64]
 DESTDIR=UNIX/SUNOS_64
 

=head1 COPYRIGHT

(c)2005, SAP AG


=cut


