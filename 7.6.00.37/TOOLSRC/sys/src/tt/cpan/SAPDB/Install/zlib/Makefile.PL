#!/usr/bin/perl
#
# $SRC/install/perl/zlib/Makefile.PL
#

use Config;
use Symbol;

sub main {
	print "Generating Makefile for zlib\n";
	
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
	} elsif ($^O =~ /^hpux/i && $ENV{'MACH'} =~ /HP_IA64/i) {
		$CFLAGS = "-O +Z +DD64";
	} elsif ($^O =~ /^hpux/i) {
		$CFLAGS = "-O +Z +DA1.1 +DS2.0";
	} elsif ($^O =~ /^linux/i) {
		$CFLAGS = "-O -fPIC";
	} elsif ($^O =~ /^solaris/i) {
		$CFLAGS = "-O -KPIC";
	}

	return $CFLAGS;
}

main ();

__DATA__
# Makefile for zlib
# Copyright (C) 1995-1998 Jean-loup Gailly.
# For conditions of distribution and use, see copyright notice in zlib.h

# To compile and test, type:
#   ./configure; make test
# The call of configure is optional if you don't have special requirements
# If you wish to build zlib as a shared library, use: ./configure -s

# To install /usr/local/lib/libz.* and /usr/local/include/zlib.h, type:
#    make install
# To install in $HOME instead of /usr/local, use:
#    make install prefix=$HOME

SHELL=/bin/sh

OBJS = adler32.o compress.o crc32.o gzio.o uncompr.o deflate.o trees.o \
       zutil.o inflate.o infblock.o inftrees.o infcodes.o infutil.o inffast.o

zlib.ro: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

tags:	
	etags *.[ch]

depend:
	makedepend -- $(CFLAGS) -- *.[ch]

clean:
	$(RM) $(OBJS)
	$(RM) zlib.ro

# DO NOT DELETE THIS LINE -- make depend depends on it.

adler32.o: zlib.h zconf.h
compress.o: zlib.h zconf.h
crc32.o: zlib.h zconf.h
deflate.o: deflate.h zutil.h zlib.h zconf.h
example.o: zlib.h zconf.h
gzio.o: zutil.h zlib.h zconf.h
infblock.o: infblock.h inftrees.h infcodes.h infutil.h zutil.h zlib.h zconf.h
infcodes.o: zutil.h zlib.h zconf.h
infcodes.o: inftrees.h infblock.h infcodes.h infutil.h inffast.h
inffast.o: zutil.h zlib.h zconf.h inftrees.h
inffast.o: infblock.h infcodes.h infutil.h inffast.h
inflate.o: zutil.h zlib.h zconf.h infblock.h
inftrees.o: zutil.h zlib.h zconf.h inftrees.h
infutil.o: zutil.h zlib.h zconf.h infblock.h inftrees.h infcodes.h infutil.h
minigzip.o:  zlib.h zconf.h
trees.o: deflate.h zutil.h zlib.h zconf.h trees.h
uncompr.o: zlib.h zconf.h
zutil.o: zutil.h zlib.h zconf.h

