*   *ID* A17SEL11 DBCMD    changed on 1992-09-28-13.22.41 by ADMIN     *
file connect ( kern test !
create table t ( a char (4) key, b char (5), c fixed (4))!
insert t values ('aaaa', 'fff', 1)!
insert t values ('qqq',  'sss', 4)!
insert t values ('ccc', 'ppp', 12)!
insert t values ('ssss', 'sss', 2)!
insert t values ('mmm', 'ppp',  9)!
insert t values ('rrr', 'ppp',  9)!
select * from t !
mfetch  !
8
create index "t.b"
	on t( b ) !
create index "t.c"
	on t( c ) !
select * from t where b = 'ppp' !
fetch !
select * from t where b in ('sss')!
fetch1 first !
fetch1 same !
fetch !
fetch pos (3) !
select * from t where b = 'ppp' and c = 9 !
mfetch next  !
2
select * from t where b in ('sss', 'ppp')!
mfetch first  !
7
mfetch pos (4) !
1
select * from t where b between 'ppp' and 'sss'!
mfetch  !
5
select * from t where b between 'ppp' and 'sss' and c < 5 !
mfetch !
15789
select * from t order by b !
fetch  !
drop index "t.b" !
create index "t.b"
	on t( b desc ) !
select * from t order by b !
fetch !
select * from t order by b desc!
mfetch last !
2
mfetch prev !
3
mfetch pos (3) !
3
create index i1 on t (c,b)!
select * from t where b = 'ppp' and c = 9 !
fetch  !
select * from t order by c, b !
fetch  !
drop table t !
commit !
*Fehlerprotokoll 214 !
'Unterschiede nach Kompilation mit -d updstat: scann-->intersect' !
create table t (a char (8), b char (8), c char (8))!
insert t values ('aaa', 'bbb', 'ccc')!
create index "t.a"
	on t( a ) !
create index "t.b"
	on t( b ) !
create index "t.c"
	on t( c ) !
explain select * from t where a = 'aaa' and b = 'bbb' and c = 'ccc'!
select * from t where a = 'aaa' and b = 'bbb' and c = 'ccc'!
fetch !
explain declare erg cursor for 
select * from t where a = 'aaa' and b = 'bbb' and c = 'ccc'
        for reuse !
declare erg cursor for 
select * from t where a = 'aaa' and b = 'bbb' and c = 'ccc'
        for reuse !
fetch erg!
explain select * from t where a = 'vvv' and b = 'bbb' and c = 'ccc'!
select * from t where a = 'vvv' and b = 'bbb' and c = 'ccc'!
fetch !
explain declare erg cursor for 
select * from t where a = 'vvv' and b = 'bbb' and c = 'ccc'
        for reuse !
declare erg cursor for 
select * from t where a = 'vvv' and b = 'bbb' and c = 'ccc'
        for reuse !
fetch erg !
explain select * from t where a = 'aaa' and b = 'vvv' and c = 'ccc'!
select * from t where a = 'aaa' and b = 'vvv' and c = 'ccc'!
fetch !
explain declare erg cursor for 
select * from t where a = 'aaa' and b = 'vvv' and c = 'ccc'
        for reuse !
declare erg cursor for 
select * from t where a = 'aaa' and b = 'vvv' and c = 'ccc'
        for reuse !
fetch erg !
explain select * from t where a = 'aaa' and b = 'bbb' and c = 'vvv'!
select * from t where a = 'aaa' and b = 'bbb' and c = 'vvv'!
fetch !
explain declare erg cursor for 
select * from t where a = 'aaa' and b = 'bbb' and c = 'vvv'
        for reuse !
declare erg cursor for 
select * from t where a = 'aaa' and b = 'bbb' and c = 'vvv'
        for reuse !
fetch erg !
explain select * from t where a = 'vvv' and b = 'vvv' and c = 'ccc'!
select * from t where a = 'vvv' and b = 'vvv' and c = 'ccc'!
fetch !
explain declare erg cursor for 
select * from t where a = 'vvv' and b = 'vvv' and c = 'ccc'
        for reuse !
declare erg cursor for 
select * from t where a = 'vvv' and b = 'vvv' and c = 'ccc'
        for reuse !
fetch erg !
explain declare erg cursor for 
select * from t where a between 'aaa' and 'ggg' and
      a = 'aaa' !
declare erg cursor for 
select * from t where a between 'aaa' and 'ggg' and
      a = 'aaa' !
fetch erg !
drop table t !
* Umsetzen von fe_eval auf more_than_one, rs_result sass nicht !
* ==> beim letzten select kamen 3 Ergebnisse raus (brrh) !
create table t ( a char (8) key, b char (8)key )!
insert t set a = 'aaa', b = 'bbb'!
insert t set a = 'bbb', b = 'bbb'!
pars_then_ex !
data !
select * from t where (a = :a or a = :b) and b = :c !
c 8 'aaa' c 8 'bbb' c 8 'bbb'

nodata !
fetch !
pars_execute !
select distinct b from t !
fetch !
drop table t !
commit work release !
