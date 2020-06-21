*   *ID* E02LIKE6 DBCMD    changed on 1992-09-28-13.23.39 by ADMIN     *
file connect ( kern test !
* like fuer letztes Keyfeld !
create table t ( k1 char (8) key,
                 k2 char (3) key,
                 a char (8))!
insert t values ('schrott', '1', 'aaa')!
insert t values ('schrott', '2', 'aaa')!
insert t values ('schrott', '3', 'aaa')!
* column in like-expression verboten
select * from t where k2 like a !
fetch !
drop table t !
* like fuer Keyfeld + mult. Index mit '%','_' !
* PTS --> 2447
create table t (a char (2) key, b char (22) key, c char (6) key)!
pars_then_ex!
data!
insert t values ('CP', :a, 'xxxx')!
b 22 2D2D544F54414C202020313939363035323300B6D530

pars_execute!
nodata!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
fetch!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
fetch!
drop table t !
diagnose share parse drop !
create table t (a char (2), b char (22), c char (6))!
create index ind1 on t (a,b,c)!
pars_then_ex!
data!
insert t values ('CP', :a, 'xxxx')!
b 22 2D2D544F54414C202020313939363035323300B6D530

pars_execute!
nodata!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
fetch!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
fetch!
drop index ind1 on t!
create index ind1 on t (a,b)!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523%'!
fetch!
explain select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
select a,c from t where 
a = 'CP' and b like '--TOTAL   19960523____'!
fetch!
drop table t !
* like fuer Index mit Value-Laenge >= Spaltenlaenge !
* PTS --> 2212
create table t (a char (2), b char (10), c char (6))!
create index "t.b"
	on t( b ) !
insert t values ('CP', '123456789', 'xxxxxx')!
insert t values ('CP', '1234567890', 'xxxxxx')!
insert t values ('CP', '12345678900', 'xxxxxx')!
explain select * from t where b like '123456789%'!
select * from t where b like '123456789%'!
fetch!
explain select * from t where b like '1234567890%'!
select * from t where b like '1234567890%'!
fetch!
explain select * from t where b like '12345678900%'!
select * from t where b like '12345678900%'!
fetch!
explain select * from t where b like '123456789_'!
select * from t where b like '123456789_'!
fetch!
explain select * from t where b like '1234567890_'!
select * from t where b like '1234567890_'!
fetch!
explain select * from t where b like '12345678900_'!
select * from t where b like '12345678900_'!
fetch!
drop table t !
commit release !
