connect kern test !

create table t179 ( mandt varchar (3), prodh varchar (18), stufe varchar (1),
primary key (mandt, prodh))!

create index i on t179 (mandt, stufe, prodh)!

insert into t179 values ('100', '0111010', '2')!

explain select /*+indexaccess*/ * 
from t179 
where 
mandt = '100' and 
prodh = '0111010' and 
stufe = '2'!

select * from t179 where 
mandt = '100' and 
prodh = '0111010' and 
stufe = '2'!

select /*+indexaccess*/ * 
from t179 
where 
mandt = '100' and 
prodh = '0111010' and 
stufe = '2'!

select index_used from indexes where indexname = 'I' !
fetch!
alter index i on t178 init usage!
alter index i on t179 init usage!
select index_used from indexes where indexname = 'I' !
fetch!

commit work release !
