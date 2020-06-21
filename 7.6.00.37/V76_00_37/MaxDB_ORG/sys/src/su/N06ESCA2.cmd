
*   *ID* N06ESCA1 DBCMD    changed on 1992-11-11-17.02.07 by KERN      *
file connect ( kern test !
create table t ( a char (8) byte ) !
insert t set a = x'abcdef' !
insert t set a = x'abcdfe' !
select count(*) into :out from t where a like x'ab25'!
select count(*) into :out from t where a like x'ab5525' escape x'55'!
pars_then_ex !
data !
select count(*) into :out from t where a like :a!
b 8 ab25000000000000
 
select count(*) into :out from t where a like :a escape x'55' !
b 8 ab55250000000000
 
select count(*) into :out from t where a like :a escape :b!
b 8 ab55250000000000
b 1 55
 
pars_execute !
nodata !
drop table t !
commit release !
