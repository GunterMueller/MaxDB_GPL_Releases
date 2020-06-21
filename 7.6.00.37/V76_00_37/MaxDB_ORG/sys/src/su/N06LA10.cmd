*   *ID* N06LA10  DBCMD    changed on 1992-09-28-13.25.05 by ADMIN     *
create table tab ( c char(10) ascii ) !
insert tab values ( 'abc' ) !
select * from tab where 'Hans' like '(a)' !
select * from tab where 'Hans' like 'H(a)ns' !
select * from tab where 'Hans' like 'H(o)ns' !
select * from tab where 'Hans' like 'H(%)ns' !
select * from tab where 'Hans' like 'H(_)ns' !
select * from tab where 'Hans' like 'H(%)ns' !
select * from tab where 'Hans' like 'H(_)ns' !
select * from tab where 'Hans' like 'H(()ns' !
select * from tab where 'Hans' like 'H())ns' !
select * from tab where 'Hans' like 'H(-)ns' !
select * from tab where 'Hans' like 'H(~)ns' !
select * from tab where 'Hans' like 'H(~)ns' !
select * from tab where 'Hans' like 'H( )ns' !
*select * from tab where 'Hans' like 'H(')ns' !
*select * from tab where 'Hans' like 'H('')ns' !
*select * from tab where 'Hans' like 'H(' ')ns' !
select * from tab where 'Hans' like 'H(a)s' !
select * from tab where 'Hans' like 'H(~b)ns' !
select * from tab where 'Hans' like 'H(~b)ns' !
select * from tab where 'Hans' like 'H(~b )ns' !
select * from tab where 'Hans' like 'H(~ b)ns' !
select * from tab where 'Hans' like 'H( ~b)ns' !
select * from tab where 'Hans' like 'Ha(k-o)s' !
select * from tab where 'Hans' like 'Ha(k-n)s' !
select * from tab where 'Hans' like 'Ha(n-z)s' !
select * from tab where 'Hans' like 'Ha(o-k)s' !
select * from tab where 'Hans' like 'Ha(a-cn)s' !
select * from tab where 'Hans' like 'Ha(ak-o)s' !
select * from tab where 'Hans' like 'Ha(k-o%)s' !
select * from tab where 'Hans' like 'Ha(a-c%)s' !
select * from tab where 'Hans' like 'Ha(~a-g)s' !
select * from tab where 'Hans' like 'Ha(n~a-g)s' !
select * from tab where 'Hans' like 'Ha(~a-gn)s' !
select * from tab where 'Hans' like 'Ha(~adgn)s' !
select * from tab where 'Hans' like 'Ha(~adg)s' !
select * from tab where 'Hans' like 'Ha(~%)s' !
select * from tab where 'Hans' like 'Ha(~_)s' !
select * from tab where 'Hans' like 'Ha(~()s' !
select * from tab where 'Hans' like 'Ha(~))s' !
select * from tab where 'Hans' like 'H(a)(n)s' !
select * from tab where 'Hans' like 'H(a-c)(k-o)s' !
select * from tab where 'Hans' like 'H(a(k-o))s' !
select * from tab where 'Hans' like '(H)ans' !
select * from tab where 'Hans' like '(H)an(s)' !
select * from tab where 'H(ns' like 'H(()ns' !
select * from tab where 'H)ns' like 'H())ns' !
select * from tab where 'H*ns' like 'H(%)ns' !
select * from tab where 'H*ns' like 'H(~%)ns' !
select * from tab where 'H%ns' like 'H(%)ns' !
select * from tab where 'H%ns' like 'H(~ab)ns' !
select * from tab where 'H%ns' like 'H(~a%)ns' !
select * from tab where 'H~ns' like 'H(~~)ns' !
select * from tab where 'H~ns' like 'H(~)ns' !
select * from tab where 'H~ns' like 'H(~a)ns' !
select * from tab where 'H~ns' like 'H(~a~)ns' !
select * from tab where 'H~ns' like 'H(~a))ns' !
select * from tab where 'H~ns' like 'H(~))ns' !
select * from tab where 'H~ns' like 'H(~()ns' !
select * from tab where 'H~ns' like 'H(~~)ns' !
select * from tab where 'H~ns' like 'H(~~)ns' !
select * from tab where 'Hans' like 'H%(n)s' !
select * from tab where 'Hans' like 'Ha%(n)s' !
select * from tab where 'Hans' like 'H%(n)s' !
select * from tab where 'Hans' like 'H_(n)s' !
select * from tab where 'Hans' like 'H(a)%s' !
select * from tab where 'Hans' like 'H(a)_s' !
select * from tab where 'Hans' like 'H(a)%s' !
select * from tab where 'Hans' like 'H(-)ns' !
select * from tab where 'H-ns' like 'H(~a)ns' !
select * from tab where 'H-ns' like 'H(a-)ns' !
select * from tab where 'H-ns' like 'H(-a)ns' !
select * from tab where 'H-ns' like 'H(a-d)ns' !
select * from tab where c like 'a(a-fh)%' !
fetch !
create index "tab.c"
	on tab( c ) !
select * from tab where c like 'a(a-fh)%' !
fetch !
drop index "tab.c" !
select * from tab where c like 'a(~a-fh)%' !
select * from tab where c like c !
