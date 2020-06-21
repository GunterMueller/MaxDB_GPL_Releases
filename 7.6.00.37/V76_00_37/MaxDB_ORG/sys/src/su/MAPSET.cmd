*   *ID* MAPSET   DBCMD    changed on 1992-07-07-09.13.58 by DIRK      *
file connect ( kern test sqlmode &1 !
create table t1 (s1 char(10) &2) !
create table t2 (s1 char(10) &2, s2 char(10) &2, s3 date) !
insert into t1 values ('a') !
insert into t1 values ('aba') !
insert into t1 values ('dirk') !
insert into t1 values ('bahnhof') !
insert into t1 values ('aaaaaaaaaa') !
insert into t1 values ('abcdefghij') !
insert into t1 values ('ababababab') !
PARS_THEN_EX !
DATA !
INSERT T1 SET s1 = :A !
b 10 6DFC4C6C657220202020

INSERT T1 SET s1 = :A !
b 10 C4D6DCE4F6FCDF202020

NODATA !
PARS_EXECUTE !
insert into t2 values ('a','@#:%%~$',date) !
PARS_THEN_EX !
DATA !
INSERT T2 SET s1 = :A, s2 = :B, s3 = date  !
b 10 53747261DF6520202020
b 10 6DFC4C6C657220202020

NODATA !
PARS_EXECUTE !
commit work !
declare sel_map cursor for 
 select distinct mapcharsetname, code 
   from domain.mapcharsets !
fetch sel_map into :a,:b !
close sel_map !
select alpha (10,20) from t1 !
select alpha (10,20) from t1 !
select alpha (s1,20,set1) from t1 !
select alpha (s1,20) from t1 !
fetch !
select mapchar(20) from t1 !
select mapchar(20,20) from t1 !
select mapchar(20,20,set1) from t1 !
select mapchar(20,set1) from t1 !
select mapchar(s1,set4) from t1 !
select mapchar(s1,20) from t1 !
fetch !
select mapchar(s1,set1) from t1 !
fetch !
select mapchar(s1,20,set1) from t1 !
fetch !
select mapchar(s1) from t2 !
fetch !
select mapchar(s2) from t2 !
fetch !
select mapchar (s1, set3) from t1!
select mapchar (s1, set5) from t1!
commit work !
drop table t1!
drop table t2!
diagnose share parse drop !
commit release !
