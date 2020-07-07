#!/usr/bin/perl
#
# put the version of the c- and c++-compilers into the environment so that
# vmake can use them in description files
# the names of variables and the format of version is the same like the macros
# used by the preprocessor
#
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


package CCVers;

use Symbol;
use Fcntl;

sub mkstemp {
	my ($template) = @_;

	# last 6 char of template have to be 'XXXXXX'
	unless ($$template =~ /XXXXXX$/) {
		$$template = '';
		return undef;
	}

	# split base
	my $base = $$template;
	$base =~ s/XXXXXX$//;

	my $fh = gensym ();
	for (my $i = 0; $i < 999999; $i++) {
		my $tmpname = sprintf ("%s%06d", $base, $i);

		next unless (sysopen ($fh,  $tmpname, O_CREAT | O_EXCL | O_RDWR, 0664));

		$$template = $tmpname;
		return $fh;
	}

	$$template = '';
	return undef;
}

sub setup_compiler_version {
    unless (defined $ENV{'IGNORE_CCVERS'})
    {
        if ($^O eq "solaris") {
            setup_compiler_version_solaris ();
        } elsif ($^O eq "aix") {
            setup_compiler_version_aix ();
        } elsif ($^O eq "dec_osf") {
            setup_compiler_version_dec ();
        } elsif ($^O eq "linux") {
            setup_compiler_version_linux ();
    		setup_glibc_version_linux ();
        } elsif ($^O eq "MSWin32") {
            setup_compiler_version_windows ();
        }
    }
}

sub setup_compiler_version_linux {
	# gcc defines 2 constants, __GNUC__ and __GNUC_MINOR__
	# where __GNUC__ is the value before the first decimal dot
	# and __GNUC_MINOR__ is the value after the first dot
	# example:
	# gcc version is 2.91.66
	#     __GNUC__     =  2
	#     __GNUC_MINOR__ = 91
	#
	unless (defined $ENV{'__GNUC__'} && defined $ENV{'__GNUC_MINOR__'}) {
		open (INPUT, "cc -v 2>&1 |");
		while (<INPUT>) {
			chomp;
			last if ($_ =~ /gcc version/);
		}
		close (INPUT);
		$_ =~ /(\d+)\.(\d+)\.(\d+)/;
		$ENV{'__GNUC__'} = $1;
		$ENV{'__GNUC_MINOR__'} = $2;
	}
}

sub setup_glibc_version_linux {

	unless (defined $ENV{'__GNULIBC__'})
	{
		my $tmp = $ENV{TMP};
		$tmp = '/tmp' unless (defined $tmp);

		my $tmpname = $tmp.'/glibcversionXXXXXX';

		my $fh = mkstemp (\$tmpname);
		return undef unless (defined $fh);

		print $fh
		"#include <stdio.h>\n".
		"\n".
		"char *gnu_get_libc_version ();\n".
		"\n".
		"int main (int argc, char **argv) {\n".
		"\tprintf (\"%s\\n\", gnu_get_libc_version ());\n".
		"}\n";

		close ($fh);

		rename ($tmpname, $tmpname.'.c');
		my $rc = system ('cc', '-o', $tmpname, $tmpname.'.c');
		unless (defined $rc && $rc == 0) {
			unlink ($tmpname.'.c');
			unlink ($tmpname);
			return undef;
		}

		unlink ($tmpname.'.c');
		open ($fh, $tmpname.' |');
		my $line = <$fh>;
		chomp $line;
		close ($fh);
		unlink ($tmpname);

		my ($d2, $d1, $d0) = ($line =~ /^(\d+)\.(\d+)\.(\d+)/);
		unless (defined $d2 && defined $d1 && defined $d0) {
			$d0 = 0;
			($d2, $d1) = ($line =~ /^(\d+)\.(\d+)/);
		}

		return undef unless (defined $d2 && defined $d1 && defined $d0);

		$ENV{__GNULIBC__} = 1000 * $d2 + 100 * $d1 + $d0;
	}
	return undef;
}

sub setup_NPTL_support_linux {
	my @execmd = @_;
	return if(defined $ENV{'__HAS_NPTL'});
	my $tmp = ($ENV{'TMP'} ? $ENV{'TMP'} : "/tmp");
	my $tmpname = "$tmp/nptlsupportXXXXXX";
	my $fh = mkstemp(\$tmpname) or return;
	print $fh <<NPTL_CODE_END;
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#define PROGRAM_EXIT_WITHOUT_RESULT             (1)
#define PROGRAM_EXIT_LINUXTHREAD_BEHAVIOUR      (42)
#define PROGRAM_EXIT_STANDARD_PTHREAD_BEHAVIOUR (0)
pid_t mainProcessId = 0;
void *testThreadMain(void *arg)
{
	pid_t threadProcessId = getpid();
	if ( threadProcessId == mainProcessId )
  {
		exit(PROGRAM_EXIT_STANDARD_PTHREAD_BEHAVIOUR);
	}
	else
	{
		exit(PROGRAM_EXIT_LINUXTHREAD_BEHAVIOUR);
	}
	return arg;
}

int main(int argc, char *argv[])
{
	int errCode = 0;
	pthread_attr_t attr;
	pthread_t testThread;
	void *joinValue = 0;
	mainProcessId = getpid();
	errCode = pthread_attr_init(&attr);
	if ( errCode != 0 )
	{
		exit(PROGRAM_EXIT_WITHOUT_RESULT);
	}
#ifdef PTHREAD_CREATE_UNDETACHED
	errCode = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#endif
#ifdef PTHREAD_CREATE_JOINABLE
	errCode = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#endif
	if ( errCode != 0 )
	{
		exit(PROGRAM_EXIT_WITHOUT_RESULT);
	}
	errCode = pthread_create(&testThread, &attr, testThreadMain, &mainProcessId);
	if ( errCode != 0 )
	{
		exit(PROGRAM_EXIT_WITHOUT_RESULT);
	}
	errCode = pthread_join(testThread, &joinValue);
	if ( errCode != 0 )
	{
		exit(PROGRAM_EXIT_WITHOUT_RESULT);
	}
	return PROGRAM_EXIT_WITHOUT_RESULT;
}
NPTL_CODE_END
	close ($fh);
	rename ($tmpname, "$tmpname.c");
	push @execmd, ("$tmpname.c", '-o', "$tmpname");
	my $rc = 0;
	if(open(CMD_OUT, "@execmd 2>&1 |")) {
		while(<CMD_OUT>) {next;}
		close(CMD_OUT);
		$rc = $? >> 8;
	}
#	my $rc = system (@execmd);
	unless (defined $rc && $rc == 0) {
		unlink ("$tmpname.c", "$tmpname");
		print "NOTE: NPTL check resulted in compilation error.\n";
		$ENV{'__HAS_NPTL'} = 0;
		return;
	}
	if(open(CMD_OUT, "$tmpname 2>&1 |")) {
		while(<CMD_OUT>) {next;}
		close(CMD_OUT);
		$rc = $? >> 8;
	}
#	$rc = system("$tmpname");
#	$rc = ($rc >> 8);
	unlink ("$tmpname.c", "$tmpname");
	if ($rc == 0) {
		print "NOTE: NPTL support activated.\n";
		$ENV{'__HAS_NPTL'} = 1;
	} elsif ($rc == 1) {
		print "NOTE: NPTL doesn't work correctly --> using old pthreads.\n";
		$ENV{'__HAS_NPTL'} = 0;
	} elsif ($rc == 42) {
		print "NOTE: System doesn't support NPTL --> using old pthreads.\n";
		$ENV{'__HAS_NPTL'} = 0;
	}
	return;
}

sub setup_compiler_version_solaris {
    unless (defined $ENV{'__SUNPRO_C'}) {
	# please keep in mind that we are using a hexadecimal compiler version
	# like the preprocessor does
	# e.g. Workshop Compiler 4.2     => __SUNPRO_C=0x0420
	# or   Workshop Compiler 5.0     => __SUNPRO_C=0x0500
	# or   Workshop 6 update 1 C 5.2 => __SUNPRO_C=0x0520

        # first C compiler version
        open (INPUT, "cc -V 2>&1 |");
        $_ = <INPUT>;
        chomp;
        close (INPUT);
	if ($_ =~ /WorkShop Compilers/) {
        	$_ =~ /WorkShop Compilers ([\d.]+)/g;
        	$_ = 256 * $1;
	} elsif ($_ =~ /WorkShop .* update/) {
        	$_ =~ /C ([\d+])\.([\d+])/g;
		$_ = 256 * $1 + 16 * $2;
	} elsif ($_ =~ /Sun/) {
        	$_ =~ /C ([\d+])\.([\d+])/g;
		$_ = 256 * $1 + 16 * $2;
	} else {
		die "CCVers.pm unknown compiler version\n";
	}
        $ENV{'__SUNPRO_C'} = $_;
	# printf "cc 0x%04x\n", $_;
    }

    unless (defined $ENV{'__SUNPRO_CC'}) {
        # then C++-Compiler-Version
        open (INPUT, "CC -V 2>&1 |");
        $_ = <INPUT>;
        chomp;
        close (INPUT);
	if ($_ =~ /WorkShop Compilers/) {
	        $_ =~ /WorkShop Compilers ([\d.]+)/g;
        	$_ = 256 * $1;
	} elsif ($_ =~ /WorkShop .* update/) {
        	$_ =~ /C\+\+ ([\d+])\.([\d+])/g;
        	$_ = 256 * $1 + 16 * $2;
	} elsif ($_ =~ /Sun/) {
        	$_ =~ /C\+\+ ([\d+])\.([\d+])/g;
        	$_ = 256 * $1 + 16 * $2;
	} else {
		die "CCVers.pm unknown compiler version\n";
	}
        $ENV{'__SUNPRO_CC'} = $_;
	# printf "CC 0x%04x\n", $_;
    }
}

sub setup_compiler_version_aix
{
	unless (defined $ENV{'__AIXCPP'}) {

		my($number)="0";
		open (INPUT, "lslpp -L *.cmp 2>&1 |");
		while ( $line=<INPUT>)
		{
			if ($line=~/.*\.cmp\s*(\d\.\d\.\d\.\d)/g)
			{
				$number=$1;
				last;
			}
		}
		close INPUT;
		if ($number eq "0")
		{
			open (INPUT, "lslpp -L *.cmp.C 2>&1 |");
			while ( $line=<INPUT>)
			{
				if ($line=~/.*\.cmp\.C\s*(\d\.\d\.\d\.\d)/g)
				{
					$number=$1;
					last;
				}
			}
			close INPUT;
		}
		if ($number eq "0")
		{
			open (INPUT, "lslpp -L vac.C 2>&1 |");
			while ( $line=<INPUT>)
			{
				if ($line=~/.*vac\.C\s*(\d\.\d\.\d\.\d)/g)
				{
					$number=$1;
					last;
				}
			}
			close INPUT;
		}
		$number=~s/\.//g;
		$ENV{__AIXCPP}=$number;
		return;
	}
}

sub setup_compiler_version_dec {
	unless (defined $ENV{'__DECC'}) {
        open (INPUT, "cc -V 2>&1 |");
        $_ = <INPUT>;
        chomp;
		if ($_ =~ /DEC C V(\d)\.(\d+)-(\d+)/g) {
		    $ENV{'__DECC'} = (100 * $1 + $2);
		}
		if ($_ =~ /Compaq C V(\d)\.(\d+)-(\d+)/g) {
		    $ENV{'__DECC'} = (100 * $1 + $2);
		}
		close (INPUT);
	}

	unless (defined $ENV{'__DECCXX'}) {
        open (INPUT, "cxx -V 2>&1 |");
        $_ = <INPUT>;
        chomp;
		if ($_ =~ /DIGITAL C\+\+ V(\d)\.(\d+)-(\d+)/g) {
		    $ENV{'__DECCXX'} = (100 * $1 + $2);
		}
		if ($_ =~ /Compaq C\+\+ V(\d)\.(\d+)-(\d+)/g) {
		    $ENV{'__DECCXX'} = (100 * $1 + $2);
		}
		close (INPUT);
	}
}

sub setup_compiler_version_windows {
	unless ( $ENV{'_MSC_VER'} =~ /\S/ ) {
        open (INPUT, "cl 2>&1 |");
        $_ = <INPUT>;
        chomp;
		if ($_ =~ /.*Compiler Version ([0-9]+).([0-9]+).([0-9]+)/g) {
		    $ENV{'_MSC_VER'} = $1;
		    $ENV{'_MSC_VER_MINOR'} = $2;
		}
		close (INPUT);
	}
	unless ( $ENV{'_MFC_VER'} =~ /\S/ )
	{
		$ENV{'_MFC_VER'} = ( int($ENV{'_MSC_VER'}) == 12 ) ? "42" :
		                   (( int($ENV{'_MSC_VER'}) == 13 ) &&  ( $ENV{'_MSC_VER_MINOR'} =~ /^(\d)/ )) ? "7$1" : "unknown";
	}
}
setup_compiler_version ();

1;
