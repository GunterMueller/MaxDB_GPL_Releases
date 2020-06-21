*   *ID* E02LIKE7 DBCMD    changed on 1992-09-28-13.23.40 by ADMIN     *
file connect ( kern test !
**
*****************************************************
** complete key   and like term                    **
*****************************************************
**
create table t (
k1  char (18) key,
k2  char (18) key,
k3  char (18) key,
k4  char (18) key,
k5  char (18) key,
k6  char (12) key,
k7  char (4) key)!
insert t values ('module', 'unixwb', 'syswb', 'do_sql',
       'dialog', 'source', 'prod')!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
fetch!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1

nodata!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'syswb'
c 18 'do%sql'

nodata!
parsing!
select * from t where (k1 like :a or k1 like :b) and
k2 = 'unixwb' and k3 = 'syswb' and k4 = 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'mo%ule'
c 18 'fo%m'

nodata!
pars_then_ex !
data !
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
c 18 'syswb' c 18 'do_sql'

nodata!
pars_execute!
drop table t !
diagnose share parse drop !
**
*****************************************************
** incomplete key and like term                    **
*****************************************************
**
create table t (
k1  char (18) key,
k2  char (18) key,
k3  char (18) key,
k4  char (18) key,
k5  char (18) key,
k6  char (12) key,
k7  char (4) key)!
insert t values ('module', 'unixwb', 'syswb', 'do_sql',
       'dialog', 'source', 'prod')!
select * from t where (k2 = 'unixwb' or k2 = 'form') and
k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
fetch!
parsing!
select * from t where (k2 = 'unixwb' or k2 = 'form') and
k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1

nodata!
parsing!
select * from t where (k2 = 'unixwb' or k2 = 'form') and
k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'syswb'
c 18 'do%sql'

nodata!
parsing!
select * from t where (k2 like :a or k2 like :b) and
k3 = 'syswb' and k4 = 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'un%xwb'
c 18 'fo%m'

nodata!
pars_then_ex !
data !
select * from t where (k2 = 'unixwb' or k2 = 'form') and
k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
c 18 'syswb' c 18 'do_sql'

nodata!
pars_execute!
drop table t !
diagnose share parse drop !
**
*****************************************************
** Multiple index and like terms                   **
*****************************************************
**
create table t (
k1  char (18) ,
k2  char (18) ,
k3  char (18) ,
k4  char (18) ,
k5  char (18) ,
k6  char (12) ,
k7  char (4) )!
insert t values ('module', 'unixwb', 'syswb', 'do_sql',
       'dialog', 'source', 'prod')!
create index ind on t (k1, k2, k3, k4 desc, k5, k6, k7)!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
fetch!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1

nodata!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'syswb'
c 18 'do%sql'

nodata!
parsing!
select * from t where (k1 like :a or k1 like :b) and
k2 = 'unixwb' and k3 = 'syswb' and k4 = 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'mo%ule'
c 18 'fo%m'

nodata!
pars_then_ex !
data !
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
c 18 'syswb' c 18 'do_sql'

nodata!
pars_execute!
drop table t !
diagnose share parse drop !
**
*****************************************************
** Multiple unique index and like terms            **
*****************************************************
**
create table t (
k1  char (18) ,
k2  char (18) ,
k3  char (18) ,
k4  char (18) ,
k5  char (18) ,
k6  char (12) ,
k7  char (4) )!
insert t values ('module', 'unixwb', 'syswb', 'do_sql',
       'dialog', 'source', 'prod')!
create unique index ind on t (k1, k2, k3, k4, k5, k6, k7)!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
fetch!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1

nodata!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'syswb'
c 18 'do%sql'

nodata!
parsing!
select * from t where (k1 like :a or k1 like :b) and
k2 = 'unixwb' and k3 = 'syswb' and k4 = 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'mo%ule'
c 18 'fo%m'

nodata!
pars_then_ex !
data !
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
c 18 'syswb' c 18 'do_sql'

nodata!
pars_execute!
drop table t !
diagnose share parse drop !
**
*****************************************************
** Sequ - strategy and like terms                  **
*****************************************************
**
create table t (
k1  char (18) ,
k2  char (18) ,
k3  char (18) ,
k4  char (18) ,
k5  char (18) ,
k6  char (12) ,
k7  char (4) )!
insert t values ('module', 'unixwb', 'syswb', 'do_sql',
       'dialog', 'source', 'prod')!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
fetch!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like 'syswb' and k4 like 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1

nodata!
parsing!
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'syswb'
c 18 'do%sql'

nodata!
parsing!
select * from t where (k1 like :a or k1 like :b) and
k2 = 'unixwb' and k3 = 'syswb' and k4 = 'do_sql' and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
data!
executing!
select!
1
c 18 'mo%ule'
c 18 'fo%m'

nodata!
pars_then_ex !
data !
select * from t where (k1 = 'module' or k1 = 'form') and
k2 = 'unixwb' and k3 like :a and k4 like :b and k5 = 'dialog'
and k6 = 'source' and k7 = 'prod' !
c 18 'syswb' c 18 'do_sql'

nodata!
pars_execute!
rollback work release!
file connect ( kern test !
* 15.8.95 the fill-length for the 'FF's in case a like will be
* changed to between was changed. Before nothing was found
* which ends with 'FF' ( R/3 !!! ) !
create table t( a char (8) ascii)!
insert t set a = 'abcdefgh'!
insert t set a = 'ab defgh'!
insert t set a = 'ac'!
pars_then_ex !
data !
insert t set a=:a!
b 16 6162FFFFFFFFFFFF

pars_execute!
nodata !
select * from t where a like 'ab%'!
fetch!
create index "t.a"
	on t( a ) !
select * from t where a like 'ab%'!
fetch!
pars_then_ex !
data !
select * from t where a like :a !
c 8 'ab%'

nodata !
fetch!
pars_execute!
diagnose share parse drop !
rollback work !
create table t( a char (31) ascii)!
insert t set a = 'abcdefgh'!
insert t set a = 'ab defgh'!
insert t set a = 'ac'!
pars_then_ex !
data !
insert t set a=:a!
b 62 6162FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF

pars_execute!
nodata !
select * from t where a like 'ab%'!
fetch!
create index "t.a"
	on t( a ) !
select * from t where a like 'ab%'!
fetch!
pars_then_ex !
data !
select * from t where a like :a !
c 40 'ab%'

nodata !
fetch!
pars_execute!
drop index "t.a" !
diagnose share parse drop !
pars_then_ex !
data !
select * from t where a like :a !
c 40 'ab%'

nodata !
fetch!
pars_execute!
diagnose share parse drop !
rollback work release !
file connect ( kern test !
create table t ( g char (8))!
create index "t.g"
	on t( g ) !
commit !
* it is only tried to optimize LIKE, not NOT LIKE !
parsing!
select * FROM  T  WHERE g = :a 
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)  and not (g LIKE :a)  
and not (g LIKE :a)  and not (g LIKE :a)!
select * FROM  T  WHERE g = :a 
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)  and     (g LIKE :a)  
and     (g LIKE :a)  and     (g LIKE :a)!
pars_execute !
drop table t !
commit release !
