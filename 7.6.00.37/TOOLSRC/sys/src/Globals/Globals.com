&if $OS = UNIX
*.pl dep=+:Perl/scripts/fix_hashbang.pl execscript=fix_hashbang.pl
&endif
