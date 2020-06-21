file connect ( kern test sqlmode ansi!
create table t ( a char (8) ) !
insert into t values ( 'abcdefg') !
insert into t values ( 'abc%g') !
insert into t values ( 'abcdfg') !
select count(*) into :out from t where a like 'abc%g'!
select count(*) into :out from t where a like 'abc!%g' escape '!'!
select count(*) into :out from t where a like 'ab!c!%g' escape '!'!
select count(*) into :out from t where a like 'abc!%g!' escape '!'!
commit work release !
file connect ( kern test !
drop table t !
commit release !
