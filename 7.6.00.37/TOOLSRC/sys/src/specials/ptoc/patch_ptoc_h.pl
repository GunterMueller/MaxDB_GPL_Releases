#
#!/usr/bin/perl5
# patch_ptoc_h.pl
#
# patch ptoc.h on the fly 
#
# patches:
# - change first cast of s35add_addr1 from (char*) to (tsp00_Addr)

use strict;

my $file = shift;

my $original_ptoc_h = "$ENV{'TOOL'}/incl/ptoc.h";


open(NEW_PTOC_H, ">$file") || die "Couldn't open $file for writing";
open(ORG_PTOC_H, "< $original_ptoc_h") || die "Couldn't open $original_ptoc_h for reading";
while (<ORG_PTOC_H>)
{
	print NEW_PTOC_H "$_";
	if (/#define s35add_addr1\(/)
	{
		my $next_line = <ORG_PTOC_H>;
		$next_line =~ s/^\s*\(\(\s*char\s*\*\s*\)/    ((tsp00_Addr)/;
		print NEW_PTOC_H "$next_line";
	}
}
close ORG_PTOC_H;
close NEW_PTOC_H;

