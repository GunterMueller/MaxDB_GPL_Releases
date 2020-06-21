connect kern test !

create table t ( i int ) !
create index i on t (i) !
insert t values (1)!
insert t values (2)!
insert t values (3)!
insert t values (4)!
insert t values (4)!
insert t values (4)!
insert t values (5)!
insert t values (5)!
commit work !

explain select /*+indexaccess */ count(i) from t !
select /*+indexaccess */ count(i) from t !
fetch !

explain select /*+indexaccess */ count(distinct i) from t !
select /*+indexaccess */ count(distinct i) from t !
fetch !

commit work release !

SENDERID DBM !

FILE CONNECT ( SUT SUT !
SET PARAMETER OPTIMIZE_AGGREGATION = 'NO' !
commit work release !

SENDERID XCI !

file connect ( kern test !

explain select /*+indexaccess */ count(distinct i) from t !
select /*+indexaccess */ count(distinct i) from t !
fetch !


commit work release !

