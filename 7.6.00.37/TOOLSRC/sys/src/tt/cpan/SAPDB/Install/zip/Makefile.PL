#!/usr/bin/perl
#
# $SRC/install/perl/zip/Makefile.PL
#

use Config;
use Symbol;

sub main {
	print "Generating Makefile for zip\n";
	
	write_makefile ();
}

sub write_makefile {
	my $fh = gensym ();
	open ($fh, "> Makefile");

	if ($^O =~ /mswin/i) {
		# just copy Makefile.w32 if we are windows
		my $filename = 
		($Config{'archname'} =~ /IA64/) ? "Makefile.w64" :
		                                  "Makefile.w32";
		
		my $fh_in = gensym ();
		open ($fh_in, $filename);
		while (<$fh_in>) {
			chomp;
			print $fh $_."\n";
		}
		close ($fh_in);
	} else {
		# modify CFLAGS on UNIX

		print $fh "CFLAGS = ".get_cflags ()."\n";
		print $fh "CC = ".get_cc ()."\n";
		print $fh "LD = ld\n";

		while (<DATA>) {
			chomp;
			print $fh $_."\n";
		}
	}
	close ($fh);
}

sub get_cc {
	return ($ENV{'MACH'} eq "HP_IA64" ? "cc" : ($ENV{'OSSPEC'} eq "HPUX" ? "cc" : $Config{'cc'}));
}

sub get_cflags {
	my $CFLAGS;

	if ($^O =~ /mswin/i) {
		print "not yet supported\n";
	} elsif ($^O =~ /^aix/i) {
		$CFLAGS = "-O -qmaxmem=8192";
	} elsif ($^O =~ /^dec_osf/i) {
		$CFLAGS = "-O2";
	} elsif ($^O =~ /^hpux/i && $ENV{'MACH'} !~ /HP_IA64/i) {
		$CFLAGS = "-O +Z +DA1.1 +DS2.0";
	} elsif ($^O =~ /^hpux/i) {
		$CFLAGS = "-O +Z +DD64";
	} elsif ($^O =~ /^linux/i) {
		$CFLAGS = "-O -fPIC";
	} elsif ($^O =~ /^solaris/i) {
		$CFLAGS = "-O -KPIC";
	}

	return $CFLAGS;
}

main ();

__DATA__

SHELL=/bin/sh

ZLIB_DIR = ../zlib
OBJS = zip.o unzip.o

ziplib.ro: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

zip.o: zip.c zip.h
	$(CC) $(CFLAGS) -I$(ZLIB_DIR) -c zip.c

unzip.o: unzip.c unzip.h
	$(CC) $(CFLAGS) -I$(ZLIB_DIR) -c unzip.c

clean:
	$(RM) $(OBJS)
	$(RM) ziplib.ro
	$(RM) Makefile

