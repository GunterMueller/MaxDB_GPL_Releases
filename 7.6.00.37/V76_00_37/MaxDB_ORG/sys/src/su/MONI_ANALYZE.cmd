* suppress abapart, in order to get same commands
lineinfo off!

file connect ( kern test !
pars_then_ex !
diagnose analyze on !
create table analyze( a char(240) ascii, b char(2) ascii )!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
insert into analyze values ('aaaaaaaaaa', '01') !
select * from sysdata_analyze !
fetch!
pars_execute!
select * from syscmd_analyze order by cmdid!
fetch!
diagnose analyze clear command !
commit work release !

file connect ( kern test !
select * from sysdata_analyze !
fetch!
select * from syscmd_analyze order by cmdid!
fetch!
lineinfo on !
diagnose analyze off !
diagnose monitor off !


diagnose analyze select on !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

diagnose analyze delete on !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

diagnose analyze on !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

diagnose analyze insert off !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

diagnose analyze update off !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

diagnose analyze off !
select description, value into :a, :b 
from sysdd.server_db_state 
where upper(description) like 'DIAGNOSE ANALYZE FILTER'!

commit work release !

dbmcli superdba,colduser!
util_connect superdba,colduser!
util_execute diagnose monitor off !
util_release
bye!

