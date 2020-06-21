#!perl -w
$ENV{DBI_PUREPERL}=2;
do 't/80proxy.t';
exit 0
