*   *ID* N06ESCAP DBCMD    changed on 1992-11-11-17.02.06 by KERN      *
file connect ( kern test !
create table t ( a char (8) &1 ) !
insert t set a = 'abcdefg' !
insert t set a = 'abc%g' !
insert t set a = 'abcdfg' !
* direkt gegebenes escape_char !
* * direkt gegebener like-Wert !
* * * einfaches Feld !
select count(*) into :out from t where a like 'abc%g'!
select count(*) into :out from t where a like 'abc%g' escape '!'!
select count(*) into :out from t where a like 'abc!%g' escape '!'!
select count(*) into :out from t where a like 'abc!!%g' escape '!'!
* * * try_like_optimization (Indexfeld) !
create index "t.a"
	on t( a ) !
select count(*) into :out from t where a like 'abcdefg' !
select count(*) into :out from t where a like 'abcdefg' escape 'e' !
select count(*) into :out from t where a like 'abcde%' !
select count(*) into :out from t where a like 'abcde%' escape 'e' !
drop index "t.a" !
* * * value_expression als like-Wert !
select count(*) into :out from t where a like 'abcd' || 'efg' !
select count(*) into :out from t where a like 'abcd' || 'efg' escape 'e'!
select count(*) into :out from t where a like 'abc' || 'de%' !
select count(*) into :out from t where a like 'abc' || 'de%' escape 'e'!
* * ueber parameter gegebener like-Wert !
pars_then_ex !
data !
* * * einfaches Feld !
select count(*) into :out from t where a like :a!
c 8 'abc%g'
 
select count(*) into :out from t where a like :a escape '!'!
c 8 'abc%g'
 
select count(*) into :out from t where a like :a escape '!'!
c 8 'abc!%g'
 
select count(*) into :out from t where a like :a escape '!'!
c 8 'abc!!%g'
 
* * * try_like_optimization (Indexfeld) !
pars_execute !
nodata !
create index "t.a"
	on t( a ) !
diagnose share parse drop !
pars_then_ex !
data !
select count(*) into :out from t where a like :a !
c 8 'abcdefg'
 
select count(*) into :out from t where a like :a escape 'e' !
c 8 'abcdefg'
 
select count(*) into :out from t where a like :a !
c 8 'abcde%'
 
select count(*) into :out from t where a like :a escape 'e' !
c 8 'abcde%'
 
pars_execute !
nodata !
drop index "t.a" !
diagnose share parse drop !
* * * value_expression als like-Wert !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5) !
c 254 'efg'
 
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5)
       escape 'e'!
c 254 'efg'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5) !
c 254 'de%'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5)
       escape 'e'!
c 254 'de%'
 
pars_execute !
nodata !
* ueber parameter gegebenes escape_char !
* * direkt gegebener like-Wert !
* * * einfaches Feld !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abc%g'!
 
select count(*) into :out from t where a like 'abc%g' escape :e!
c 1 '!'
 
select count(*) into :out from t where a like 'abc!%g' escape :e !
c 1 '!'
 
select count(*) into :out from t where a like 'abc!!%g' escape :e !
c 1 '!'
 
* * * try_like_optimization (Indexfeld) !
pars_execute !
nodata !
create index "t.a"
	on t( a ) !
diagnose share parse drop !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abcdefg' !
 
select count(*) into :out from t where a like 'abcdefg' escape :e !
c 1 'e'
 
select count(*) into :out from t where a like 'abcde%' !
 
select count(*) into :out from t where a like 'abcde%' escape :e !
c 1 'e'
 
pars_execute !
nodata !
drop index "t.a" !
diagnose share parse drop !
pars_then_ex !
data !
* * * value_expression als like-Wert !
select count(*) into :out from t where a like 'abcd' || 'efg' !
 
select count(*) into :out from t where a like 'abcd' || 'efg' escape :e !
c 1 'e'
 
select count(*) into :out from t where a like 'abc' || 'de%' !
 
select count(*) into :out from t where a like 'abc' || 'de%' escape :e !
c 1 'e'
 
* * ueber parameter gegebener like-Wert !
* * * einfaches Feld !
select count(*) into :out from t where a like :a!
c 8 'abc%g'
 
select count(*) into :out from t where a like :a escape :e!
c 8 'abc%g'
c 1 '!'
 
select count(*) into :out from t where a like :a escape :e!
c 8 'abc!%g'
c 1 '!'
 
select count(*) into :out from t where a like :a escape :e!
c 8 'abc!!%g'
c 1 '!'
 
* * * try_like_optimization (Indexfeld) !
pars_execute !
nodata !
create index "t.a"
	on t( a ) !
diagnose share parse drop !
pars_then_ex !
data !
select count(*) into :out from t where a like :a !
c 8 'abcdefg'
 
select count(*) into :out from t where a like :a escape :e !
c 8 'abcdefg'
c 1 'e'
 
select count(*) into :out from t where a like :a !
c 8 'abcde%'
 
select count(*) into :out from t where a like :a escape :e !
c 8 'abcde%'
c 1 'e'
 
pars_execute !
nodata !
drop index "t.a" !
* * * value_expression als like-Wert !
diagnose share parse drop !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5) !
c 254 'efg'
 
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5)
       escape :e!
c 254 'efg'
c 1 'e'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5) !
c 254 'de%'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5)
       escape :e!
c 254 'de%'
c 1 'e'
 
pars_execute !
nodata !
* ueber value_expression gegebenes escape_char !
* * direkt gegebener like-Wert !
* * * einfaches Feld !
select count(*) into :out from t where a like 'abc%g'!
select count(*) into :out from t where a like 'abc%g'
       escape substr('p!o', 2, 1) !
select count(*) into :out from t where a like 'abc!%g'
       escape substr('p!o', 2, 1) !
select count(*) into :out from t where a like 'abc!!%g'
       escape substr('p!o', 2, 1) !
* * * try_like_optimization (Indexfeld) !
create index "t.a"
	on t( a ) !
select count(*) into :out from t where a like 'abcdefg' !
select count(*) into :out from t where a like 'abcdefg'
       escape substr('zye', 3) !
select count(*) into :out from t where a like 'abcde%' !
select count(*) into :out from t where a like 'abcde%'
       escape substr('zye', 3) !
drop index "t.a" !
diagnose share parse drop !
* * * value_expression als like-Wert !
select count(*) into :out from t where a like 'abcd' || 'efg' !
select count(*) into :out from t where a like 'abcd' || 'efg'
       escape substr('zye', 3)!
select count(*) into :out from t where a like 'abc' || 'de%' !
select count(*) into :out from t where a like 'abc' || 'de%'
       escape substr('zye', 3)!
* * ueber parameter gegebener like-Wert !
pars_then_ex !
data !
* * * einfaches Feld !
select count(*) into :out from t where a like :a!
c 8 'abc%g'
 
select count(*) into :out from t where a like :a
       escape substr('p!o', 2, 1)!
c 8 'abc%g'
 
select count(*) into :out from t where a like :a
       escape substr('p!o', 2, 1)!
c 8 'abc!%g'
 
select count(*) into :out from t where a like :a
       escape substr('p!o', 2, 1)!
c 8 'abc!!%g'
 
* * * try_like_optimization (Indexfeld) !
pars_execute !
nodata !
create index "t.a"
	on t( a ) !
diagnose share parse drop !
pars_then_ex !
data !
select count(*) into :out from t where a like :a !
c 8 'abcdefg'
 
select count(*) into :out from t where a like :a
       escape substr('zye', 3) !
c 8 'abcdefg'
 
select count(*) into :out from t where a like :a !
c 8 'abcde%'
 
select count(*) into :out from t where a like :a
       escape substr('zye', 3) !
c 8 'abcde%'
 
pars_execute !
nodata !
drop index "t.a" !
diagnose share parse drop !
* * * value_expression als like-Wert !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5) !
c 254 'efg'
 
select count(*) into :out from t where a like 'abcd' || substr(:a,1,5)
       escape substr('zye', 3)!
c 254 'efg'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5) !
c 254 'de%'
 
select count(*) into :out from t where a like 'abc' || substr(:a,1,5)
       escape substr('zye', 3)!
c 254 'de%'
 
pars_execute !
nodata !
select count(*) into :out from t where a like 'abcdefg' escape 'ef' !
select count(*) into :out from t where a like 'abcdefg'
       escape substr('def', 2) !
pars_then_ex !
data !
select count(*) into :out from t where a like 'abcdefg'
       escape substr(:a, 2, 2) !
c 254 'def'
 
dummy command !
pars_execute !
nodata !
drop table t !
commit release !
