#!/usr/bin/perl

BEGIN
{
    unless (defined $ENV{'TOOL'})
    {
    	my $callers_path = (caller())[1];
	    $callers_path  =~ tr/\//\\/ if ($^O=~/.*win32.*/i);
		$ENV{'TOOL'} = $1 if ($callers_path =~ /^(.*)[\\\/]bin[\\\/]create_hotfix.*/);
		if ($^O=~/.*win32.*/i) { $ENV{'PATH'} = "$ENV{'TOOL'};$ENV{'PATH'}";}
		else { $ENV{'PATH'} = "$ENV{'TOOL'}:$ENV{'PATH'}"; }
    }

    unshift @INC, "$ENV{'TOOL'}/lib/perl5";
    unshift @INC, "$ENV{'TOOL'}/bin";
    
    $ENV{'PATH'} = "/devtool/local/bin:$ENV{'PATH'}" if (-e "/devtool/local/bin");
    $ENV{'PATH'} = "/usr/local/bin:$ENV{'PATH'}" if (-e "/usr/local/bin");
}

use strict;
use Getopt::Long;
use File::Path;
use Perforce::P4Admin;
use Perforce::P4Info;
use Perforce::P4Sync;
use Perforce::P4File;
use Tool::RequestInput;
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

my ($rel, $corr, $build) = (undef, undef, undef);
my $customer = undef;
my $platform = undef;
my $cl = undef;
my $index = undef;
my $lcstate = 'HOT';
my $remove = 0;
my $check = 0;
my $root = ($^O =~ /win32/i ? "d:\\depot\\bas\\SAP_DB\\SCRATCH" : "/SAP_DB/SCRATCH");
my $delimiter = ($^O =~ /win32/i ? "\\" : "/");
my $tool = undef;
my $p4user = undef;
my $allow_init = (getlogin() =~ /falcmake|makefact/ ? 1 : 0);
my $init = 0;
my $proj_name = 'hotfix';
my $p4proj_subdir = 'hot';
my $source = undef;
my $p4passwd = undef;

chomp(my $hostname = `hostname`);

$| = 1;

print "\ncreate_hotfix v1.00 (c)2005, SAP AG\n\n";

GetOptions( 
            'help'       => sub { pod2usage(1); },
            'man'        => sub { pod2usage( -exitstatus => 0, -verbose => 2 ); },
            'rel=i'      => sub { if ($_[1] =~ /^(\d{2})(\d{2})(\d{2})$/) {($rel, $corr, $build) = ($1, $2, $3);} else {pod2usage("Option rel has to be numeric and exactly 6 chars long.\n");}},
            'customer=s' => \$customer,
            'platform=s' => \$platform,
            'cl=i'       => \$cl,
            'index=i'    => sub { $index = sprintf("%02d", $_[1]); } ,
            'lcstate=s'  => sub { if ($_[1] =~ /(HOT|ANA)/) {$lcstate = $_[1];} else {pod2usage("Option lcstate has to be either HOT or ANA\n");}},
            'clean'      => \$remove,
            'remove'     => \$remove,
            'check'      => \$check,
            'root=s'     => \$root,
            'tool=s'     => \$tool,
            'user=s'     => \$p4user,
            'passwd=s'   => \$p4passwd,
            'src=s'      => \$source
          ) or pod2usage("\n");

pod2usage("Mandatory argument rel is missing!\n") unless (defined($rel) && defined($corr) && defined($build));
pod2usage("Mandatory argument customer is missing!\n") unless ($customer);
pod2usage("Mandatory argument platform is missing!\n") unless ($platform);
pod2usage("If you use the remove option, you also have to define an index!\n") if ($remove && !$index);

if ($remove) {
    $allow_init = 0;
}

if ($source) {
    unless (-e $source) {
        print "ERROR! Manual source path '$source' does not exist! Aborting!\n";
        exit(1);
    }
}

if ($lcstate ne 'HOT') { $proj_name = 'analysis branch'; $p4proj_subdir = 'ana'; $allow_init = 1; }

$ENV{'P4PORT'} = 'perforce3006.wdf.sap.corp:3006';
$ENV{'P4USER'} = $p4user ? $p4user : ($ENV{'P4USER'} ? $ENV{'P4USER'} : '');
$ENV{'P4PASSWD'} = $p4passwd ? $p4passwd : ($ENV{'P4PASSWD'} ? $ENV{'P4PASSWD'} : '');

my $hotfix_name = $rel . $corr . $build . "_" . $customer . "_" . $platform . "_";
my $p4path_name = "b_" . $build . "_" . $customer . "_" . $platform . "_";
my $p4client_name = $hotfix_name . $hostname . "_";

if ($lcstate eq 'HOT') {
    print "Retrieving existing $proj_name information.\n\n";
    my $dirref = P4GetDir($ENV{'P4PORT'}, $ENV{'P4USER'}, "//sapdb/V$rel/c_$corr/$p4proj_subdir");
    my @olddirs = sort grep { /$p4path_name.*\d+$/ } @{$dirref->{'DIRS'}} if ($dirref->{'DIRS'});
    
    
    if (scalar(@olddirs)) {
        if (defined($index)) {
            if (grep {/$p4path_name$index/} @olddirs) {
                print "Taking existing $proj_name '$hotfix_name$index'.\n";
            } elsif ($allow_init) {
                $init = 1;
                print "Creating new $proj_name '$hotfix_name$index'.\n";
            } else {
                print "Sorry, you are not allowed to create a new $proj_name!\n";
                exit(1);
            }
        } else {
            my $askstr = "Please select one of the following " . $proj_name . "es:\n\n";
            my $i = 0;
            for ($i = 0; $i < scalar(@olddirs); $i ++) {
                    $askstr .= "[" . ($i + 1) . "] = $hotfix_name$1\n" if ($olddirs[$i] =~ /.*_(\d+)$/);
            }
            my $hinum = ($olddirs[-1] =~ /.*_(\d+)$/ ? sprintf("%02d", int($1)+1) : sprintf("%02d", $i+1));
            $askstr .= "\n[" . ($i + 1) . "] = Create new $proj_name '$hotfix_name$hinum'\n" if ($allow_init);
            $askstr .= "\nSelection: ";
            my $erg = 0;
            while(1) {
                $erg = RequestInput($askstr, undef, "[0-9]+", "Input incorrect, please just use numbers!");
                last if (($allow_init && $erg <= (scalar(@olddirs) + 1)) || !$allow_init && $erg <= scalar(@olddirs));
                print "Value out of range! Better luck next time...\n";
            }
            if ($erg <= scalar(@olddirs)) { $index = ($olddirs[$erg - 1] =~ /.*_(\d+)$/ ? $1 : undef); }
            else { $index = $hinum; $init = 1; }
        }
    } elsif (!$allow_init) {
        print "ERROR: There is no suitable $proj_name.\n";
        exit(1);
    } else {
        $init = 1;
        $index = (defined($index) ? $index : "01");
        print "Creating new $proj_name '$hotfix_name$index'.\n";
    }
} else {
    # Quick fix unless Perforce tree .../ana/... exists.
    unless ($index) {
        if (opendir(DIR, "$root")) {
            my @old_idx = reverse sort (grep { /^$hotfix_name/ } readdir(DIR)); close (DIR);
            $index = sprintf("%02d", int($1) + 1) if ($old_idx[0] =~ /$hotfix_name(\d+)/);
        } 
        $index = "01" unless ($index);
    }
    $init = 0;
}
print "\n";

unless (defined($index)) {
    print "ERROR: There has been a heavy error gathering the index number!\n";
    exit(1);
}

$hotfix_name .= $index;
$p4path_name .= $index;
$p4client_name .= $index;
$ENV{'P4CLIENT'} = "$p4client_name";

my $own = $root . $delimiter . $hotfix_name;

if ($remove) {
    unless (-e $own) { print "ERROR: The directory '$own' does not exist! Aborting!\n"; exit(1); }
    print "\nRemoving hotfix '$hotfix_name':\n";
    if (-e "$own/ramp") {
        print "\nRemoving files under 'OWN/ramp':\n\n";
        P4Sync($ENV{'P4PORT'}, $ENV{'P4USER'}, $ENV{'P4CLIENT'}, "//sapdb/V$rel/c_$corr/b_$build/...", 'none',10, 1);
    }
    print "\nRemoving P4 client '$ENV{'P4CLIENT'}'..";
    if (P4RemClient($ENV{'P4PORT'}, $ENV{'P4CLIENT'})) { print "..OK\n"; }
    else { print "..FAILED! Please remove manually!\n"; }
    print "\nRemoving leftover files:\n\n";
    chmod (0777, "$own/hotfix_info.txt");
    rmtree ($own, 1, 1);
    print "\nAll done. Please check for any leftovers under '$own'.\n\n";
    exit(0);
}

createpath($own);
createpath($own . $delimiter . 'sys');
createpath($own . $delimiter . 'sys' . $delimiter . 'src');
createpath($own . $delimiter . 'sys' . $delimiter . 'wrk');
createpath($own . $delimiter . 'sys' . $delimiter . 'desc');
createpath($own . $delimiter . 'tmp');
createpath($own . $delimiter . 'usr');
createpath($own . $delimiter . 'usr' . $delimiter . 'bin');
createpath($own . $delimiter . 'usr' . $delimiter . 'pgm');
createpath($own . $delimiter . 'usr' . $delimiter . 'lib');
createpath($own . $delimiter . 'usr' . $delimiter . 'incl');
createpath($own . $delimiter . 'usr' . $delimiter . 'wrk');

print "\nCreating Perforce client '$ENV{'P4CLIENT'}'..";

my @views = ("//sapdb/V$rel/c_$corr/$p4proj_subdir/$p4path_name/... //$ENV{'P4CLIENT'}/...", "//sapdb/V$rel/c_$corr/b_$build/... //$ENV{'P4CLIENT'}/ramp/...");

if ($check) {
    print "..DUMMY\n";
} else {
    if (P4SetClient($ENV{'P4PORT'}, $ENV{'P4CLIENT'}, $own, $ENV{'P4USER'}, $hostname, undef, undef, \@views)) {
        print "..OK\n";
    } else {
        print "..FAILED! Aborting...\n";
        exit(1);
    }
}

unless ($cl) {
    if ($source) {
        print "\nManual Sources! Skipping chagelist retrieval.\n";
    } else {
        print "\nGetting Perforce change list..";
        $cl = P4GetChangeList($ENV{'P4PORT'}, $ENV{'P4USER'}, "//sapdb/V$rel/c_$corr/b_$build/...");
        if ($cl) {
            print "..$cl\n";
        } else {
            print "..FAILED! Can't continue without changelist!\n\n";
            exit(1);
        }
    }
} elsif ($source) {
    print "\nWARNING! You have defined a manual source AND a changelist! CL will be ignored!\n";
}

print "\nSynchronizing release $rel.$corr.$build:\n\n" unless ($source);
if ($check) {
    print "(nothing to sync in check mode...)\n" unless ($source);
} else {
    if ($source) {
        print "\nManual Sources! Skipping sync of '$rel$corr$build'.\n";
    } else {
        P4Sync($ENV{'P4PORT'}, $ENV{'P4USER'}, $ENV{'P4CLIENT'}, "//sapdb/V$rel/c_$corr/b_$build/...", $cl, 0, 1);
        $source = "$own/ramp";
    }
    unless ($init || $lcstate ne 'HOT') {
        print "\nSynchronizing $proj_name files '$p4path_name':\n\n";
        my $hotcl = P4GetChangeList($ENV{'P4PORT'}, $ENV{'P4USER'}, "//sapdb/V$rel/c_$corr/$p4proj_subdir/$p4path_name/...");
        if ($hotcl) { P4Sync($ENV{'P4PORT'}, $ENV{'P4USER'}, $ENV{'P4CLIENT'}, "//sapdb/V$rel/c_$corr/$p4proj_subdir/$p4path_name/...", $hotcl, 0, 1); }
        else { print "Failed to get $proj_name changelist! Please sync $proj_name mannually!\n"; }
    }
}

unless ($tool) {
    print "\nLooking for correct TOOL release..";
    $tool = P4GetToolVer($rel . $corr . "RAMP");
    if ($tool) {
        print "..'$tool'\n";
    } else {
        print "..FAILED! Setting TOOL to 'dev'!\n";
        $tool = 'dev';
    }
}

if ($ENV{'TOOL'} =~ /^(.*)[\/\\]+[^\/\\]+[\\\/]+tool$/) {
    $tool = $1 . $delimiter . $tool . $delimiter . "tool";
    print "\nINFO: TOOL has been set  to '$tool'.\n";
} else {
    print "\nSuspicious TOOL directory! Setting TOOL '$ENV{'TOOL'}'!\n";
    $tool = $ENV{'TOOL'};
}

print "\nCreating iprofile.\n";
unless ($check) {
    system($tool . $delimiter . "bin" . $delimiter . "createlcprofile -own $own -src $source -tool $tool -lc_state $lcstate -prod");
}

if ($^O =~ /win32/i) {
    print "\nSorry, no 'go' script under Windows. Just change directory to '$own' and execute iprofile.bat manually.\n";
} else {
    print "\nGenerating script 'go$hotfix_name'..";
    if ($check) {
        print "..DUMMY\n";
    } else {
        if (open (CMD_OUT, "> /devtool/local/bin/go$hotfix_name")) {
            print CMD_OUT "#!/bin/sh\n";
            print CMD_OUT "P4PORT=$ENV{'P4PORT'}; export \$P4PORT\n";
            print CMD_OUT "P4USER=$ENV{'P4USER'}; export \$P4USER\n";
            print CMD_OUT "P4CLIENT=$ENV{'P4CLIENT'}; export \$P4CLIENT\n";
            print CMD_OUT "P4PASSWD=$ENV{'P4PASSWD'}; export \$P4PASSWD\n" if ($ENV{'P4PASSWD'});
            print CMD_OUT "cd $own\n";
            print CMD_OUT "source .iprofile\n";
            close CMD_OUT;
            chmod 0775, "/devtool/local/bin/go$hotfix_name";
            print "..OK\n";
        } else {
            print "..FAILED! Please chdir to '$own' and execute '.iprofile' manually.\n";
        }
    }
}

if ($init) {
    print "\nGenerating info file 'hotfix_info.txt'..";
    
    if ($check) {
        print "..DUMMY\n";
    } else {
        if (open(INFO_OUT, "> $own$delimiter" . "hotfix_info.txt")) {
            print INFO_OUT "ORIGINAL_RELEASE=$rel.$corr.$build\n";
            print INFO_OUT "ORIGINAL_CHANGELIST=$cl\n" if ($cl);
            print INFO_OUT "CUSTOMER=$customer\n";
            print INFO_OUT "PLATFORM=$platform\n";
            print INFO_OUT "LC_STATE=$lcstate\n";
            print INFO_OUT "INDEX=$index\n";
            print INFO_OUT "OWN_DIRECTORY=$own\n";
            print INFO_OUT "TOOL_DIRECTORY=$tool\n";
            print INFO_OUT "P4USER=$ENV{'P4USER'}\n";
            print INFO_OUT "P4CLIENT=$ENV{'P4CLIENT'}\n";
            print INFO_OUT "TIME_CREATED=" . scalar(localtime(time)) . "\n";
            close INFO_OUT;
            print "..OK\n";
        } else {
            print "..FAILED! Aborting...\n";
            exit(1);
        }
    }
    
    print "\nSubmitting 'hotfix_info.txt'..";
    
    if ($check) {
        print "..DUMMY\n";
    } else {
        if (P4Add($ENV{'P4PORT'}, $ENV{'P4USER'}, $ENV{'P4CLIENT'}, "//sapdb/V$rel/c_$corr/$p4proj_subdir/$p4path_name/hotfix_info.txt")) {
            print ".";
        } else {
            print "..FAILED to ADD file! Please add manually.\n";
            exit(1);
        }
        
        if (P4Submit($ENV{'P4PORT'}, $ENV{'P4USER'}, $ENV{'P4CLIENT'}, "//sapdb/V$rel/c_$corr/$p4proj_subdir/$p4path_name/hotfix_info.txt", "Initial HOTFIX info file.")) {
            print "..OK\n";
        } else {
            print "..FAILED to ADD file! Please add manually.\n";
            exit(1);
        }
    }
}

print "\n*** ALL DONE! *** " . ($^O =~ /win32/i ? "\n\n" : "Use 'go$hotfix_name' to start.\n\n");

exit(0);

sub createpath {
    print "Creating directory '$_[0]'.\n";
    unless ($check) {
        eval {mkpath("$_[0]", 0, 0755);};
        if($@) { print "ERROR: Could not create directory '$_[0]'! ($@)\n"; exit(1); }
    }
}

__END__

=head1 NAME

create_hotfix - Creates a new hotfix environment for lC/MaxDB

=head1 SYNOPSIS

 create_hotfix -rel <MMCCBB> -customer <NAME> -platform <NAME> 
               [-cl <NUMBER>] [-index <NUMBER>] [-lcstate <NAME>] 
               [-(clean|remove)] [-check] [-root <DIRECTORY>] [-tool <NAME>] 
               [-user <P4USER>] [-passwd <P4PASSWD>] [-help] [-man]
               [-src <DIRECTORY>]
              
Mandatory arguments:
  
  -rel <MMCCBB>         MaxDB release including build number
  -customer <NAME>      customers name to build a fix for
  -platform <NAME>      the platform architecture to work on
  
Options:

  -cl <NUMBER>          a specific RAMP changelist to sync to
  -index <NUMBER>       a specific index rather than default increment
  -lcstate <STATE>      state can either be HOT or ANA, default is HOT
  -clean | -remove      removes the hotfix directory again
  -check                just does a simpulation of what would happen
  -root <DIRECTORY>     a different root directory (def: /SAP_DB/SCRATCH)
  -tool <NAME>          sets a specific TOOL release to be used
  -user <P4USER>        use a specific Perforce user
  -passwd <P4PASSWD>    set a password for the given perforce user
  -src <DIRECTORY>      use a specific pre-synced source directory
  -help                 displays this brief help
  -man                  displays the complete manual

=head1 OPTIONS

=over 8

=item B<-rel <MMCCBB>>

Sets the release to build a hotfix for. Must be in numeric format like '760012'.
[MANDATORY]

=item B<-customer <NAME>>

Defines the customer that requires this hotfix. [MANDATORY]

=item B<-platform <NAME>>

Defines the platform on which this hotfix has to be made. The following platform
names are defined so far (and should be used): [MANDATORY]

 NTintel, NTia64, NTx86_64	
 alphaosf
 hp, hp_64, hpia64
 linux2.6ia32, linux2.6ia64	
 linuxia64, linuxia64sles9	
 linuxintel, linuxintelsles8	
 linuxppc64, linuxs390x, linuxx86_64	
 rs6000, rs6000_51, rs6000_51_64, rs6000_52_64, rs6000_64	
 sun, sun_64

NOTE: You might use any other name you want since its just used for naming purposes.
Using a standard name is strongly recommended anyway.

=item B<-cl <NUMBER>>

Sets a specific changelist number rather than the latest available for the release given.

=item B<-index <NUMBER>>

Enables the user to set a specific index number rather than the first one available.

=item B<-lcstate <STATE>>

Defines a specific LC_STATE. The only valid values so far are HOT and ANA. If
ANA is defined, Perforce path creation will be disabled.

=item B<-clean | -remove>

Removes the installed hotfix again.

=item B<-check>

Starts create_hotfix in a debug mode rather than the normal one. In this mode,
no action is executed but just documented (dummy mode).

=item B<-root <DIRECTORY>>

Sets a different installation root rather than '/SAP_DB/SCRATCH'.

=item B<-tool <NAME>>

Sets a specific TOOL release to be used. If omitted, the default TOOL release
for the given lC release will be used as defined in //sapdb/V74/toolmap.cfg. 

=item B<-user <P4USER>>

Sets a specific Perforce user to be used. If omitted, 'remuser' will be used.

=item B<-passwd <P4UPASSWD>>

Sets a password for the P4 user defined before.

=item B<-src <DIRECTORY>>

Defines a specific source directory to be used rather than syncing all sources
from Perfoce into the OWN/ramp directory. 
NOTE: If this option is given, the option '-cl' will be ignored!

=item B<-help>

Shows a brief description.

=item B<-man>

Shows the complete manual.

=back

=head1 DESCRIPTION

*** THIS PROGRAM IS FOR INTERNAL USE ONLY ***

This program will:

 -create all file structures needed, including OWN directory
 -create a Perforce client in order to be able to sync all files needed
 -create a new path on the Perforce server to hold all hotfix changes
 -sync all RAMP files needed to re-create the old (delivered) release
 -create a specific .iprofile that contains all settings needed.
 -create a 'go' file that enables the user to jump into the dev environment
 
For bugfixes / suggestions, please contact gerald.arnold@sap.com 

=head1 EXAMPLES

 create_hotfix -rel 760012 -customer Braun -platform rs6000_64


=head1 COPYRIGHT

(c)2005, SAP AG

=cut

