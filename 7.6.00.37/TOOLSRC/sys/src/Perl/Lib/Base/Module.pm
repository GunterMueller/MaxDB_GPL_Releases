
package Module;

use strict;
use vars qw($Verbose);

$Verbose = 0;

sub Use {
	return _evalfunc("use", shift, "Using");
}

sub Require {
	return _evalfunc("require", shift, "Requiring");
}

sub Import {
	return _evalfunc("import", shift, "Importing");
}

sub No {
	return _evalfunc("no", shift, "Unloading imports for");
}

sub _evalfunc {
	my ($func, $modnam, $title) = @_;
	my $to_eval = "$func $modnam;";
	eval $to_eval;
	print "$title module '$modnam'.." . ($@ ? ($@ =~ /^Can't\slocate/ ? "..not found!\n" : "..ERROR!\n") : "..OK\n") if ($Verbose);
	print STDERR "DBG: $@\n" if ($Verbose == 2);
	return (wantarray ? ($@ ? (0, $@) : (1, undef)) : ($@ ? 0 : 1));
}

1;
