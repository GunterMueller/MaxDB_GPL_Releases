#!perl -w
$ENV{DBI_PUREPERL}=2;
do 't/03hleak.t';
exit 0
