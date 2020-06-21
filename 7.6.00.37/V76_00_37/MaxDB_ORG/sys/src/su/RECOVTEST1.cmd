*
* SAVETEST 
*
* alle putval Aufträge dürfen nur max. 1/2 des Auftragssegmentes füllen, um
* auch im UNICODE Fall zu arbeiten, da dann das Segment vollständig ge-
* füllt ist, bzw. sonst überlaufen würde!
*
dbmcli superdba,colduser!
backup_media_put SAVEDATA1 savetest.data1    FILE DATA 0 8 YES NO ""!
backup_media_put SAVEDATA2 s/avetest.data2   FILE DATA 0 8 YES NO ""!
backup_media_put SAVEDATA3  savetest.data3   FILE DATA 0 8 YES NO ""!
backup_media_put SAVEPAGES1 s/avetest.pages1 FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES2 savetest.pages2  FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES3 savetest.pages3  FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES4 s/avetest.pages4 FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES5 savetest.pages5  FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES6 s/avetest.pages6 FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES7 savetest.pages7  FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES8 s/avetest.pages8 FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGES9 savetest.pages9  FILE PAGES 0 8 YES NO ""!
backup_media_put SAVEPAGESA savetest.pagesA  FILE PAGES 0 8 YES NO ""!
bye!
create table Test1(
a integer,
b varchar(300),
c DATE default DATE,
d long)!
insert into Test1 set a=1, b='this is a test1'!
commit!

dbmcli superdba,colduser!
util_connect sut,sut!
backup_save SAVEDATA1 DATA!
backup_save SAVEDATA2 DATA!
util_release!
bye!

parsing!
insert into Test1 set a=2, b='this is a test2', d=:AAA!
executing!
data!
insert!
1
l 10000,0 'abcdefghij'
  
putval !
1
l 10000,0 'abcdefghij'

putval !
1
l 10000,0 'abcdefghij'

putval !
1
l 10000,2 'abcdefghij'

pars_execute!
nodata !
commit!

dbmcli superdba,colduser!
util_connect sut,sut!
backup_save SAVEPAGES1 PAGES!
backup_save SAVEPAGES2 PAGES!
backup_save SAVEPAGES3 PAGES!
backup_save SAVEPAGES4 PAGES!
util_release!
bye!

insert into Test1 set a=3, b='this is a test3'!
parsing!
insert into Test1 set a=4, b='this is a test4', d=:aaa!
executing!
data!
insert!
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,0 '!"§$%&/()='

putval !
1
l 8000,2 '!"§$%&/()='

pars_execute!
nodata !
commit!

dbmcli superdba,colduser!
util_connect sut,sut!
backup_save SAVEPAGES5 PAGES!
util_release!
bye!

parsing!
insert into Test1 set a=5, b='this is a test5', d=:DDD!
executing!
data!
insert!
1
l 10000,0 '0123456789'
  
putval !
1
l 10000,0 '0123456789'

putval !
1
l 10000,0 '0123456789'

putval !
1
l 10000,0 '0123456789'

putval !
1
l 10000,0 '0123456789'

putval !
1
l 10000,2 '0123456789'

pars_execute!
nodata !
commit!

dbmcli superdba,colduser!
util_connect sut,sut!
db_admin!
util_connect superdba,colduser!
backup_save SAVEPAGES6 PAGES!
backup_save SAVEPAGES7 PAGES!
backup_save SAVEPAGES8 PAGES!
backup_save SAVEPAGES9 PAGES!
backup_save SAVEDATA3  DATA!
db_online!
util_release!
bye!

connect kern identified by test!

*create big restore pages file
parsing!
insert into Test1 set a=6, b='this is a BIGtest', d=:AAA!
executing!
data!
insert!
1
l 14000,0 'BIG****'
  
putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 14000,0 'BIG****'

putval !
1
l 6000,2 '**BIG**'

pars_execute!
nodata !
commit!

dbmcli superdba,colduser!
util_connect sut,sut!
backup_save SAVEPAGESA PAGES!
util_release!
bye!

parsing!
insert into Test1 set a=7, 
b='this was inserted while applying the log', d=:aaa!
executing!
data!
insert!
1
l 13500,0 ',;.:-_#~ß?' 

putval !
1
l 13500,0 ',;.:-_#~ß?'

putval !
1
l 1500,0 ',;.:-_#~ß?'

putval !
1
l 13500,2 ',;.:-_#~ß?'

pars_execute!
nodata !
commit!

select * from Test1!
fetch!

dbmcli superdba,colduser!
db_admin!
util_connect superdba,colduser!
backup_restore SAVEDATA1  DATA!
backup_restore SAVEDATA2  DATA!
backup_restore SAVEPAGES1 PAGES!
backup_restore SAVEPAGES2 PAGES!
backup_restore SAVEPAGES3 PAGES!
backup_restore SAVEPAGES2 PAGES!
backup_restore SAVEPAGES4 PAGES!
db_online!
util_release!
bye!

connect kern identified by test!
select * from Test1!
fetch!

dbmcli superdba,colduser!
db_admin!
util_connect superdba,colduser!
backup_restore SAVEDATA1  DATA!
backup_restore SAVEPAGES5 PAGES!
db_online!
util_release!
bye!

connect kern identified by test!
select * from Test1!
fetch!

dbmcli superdba,colduser!
db_admin!
util_connect superdba,colduser!
backup_restore SAVEDATA1  DATA!
backup_restore SAVEPAGES5 PAGES!
backup_restore SAVEPAGES6 PAGES!
backup_restore SAVEPAGES7 PAGES!
backup_restore SAVEPAGES8 PAGES!
db_online!
util_release!
bye!

connect kern identified by test!
select * from Test1!
fetch!

dbmcli superdba,colduser!
db_admin!
util_connect superdba,colduser!
backup_restore SAVEPAGES9 PAGES!
backup_restore SAVEDATA1  DATA!
backup_restore SAVEPAGESA PAGES!
backup_restore SAVEPAGES9 PAGES!
backup_restore SAVEPAGES9 PAGES!
db_online!
util_release!
bye!

connect kern identified by test!
select * from Test1!
fetch!

dbmcli superdba,colduser!
db_admin!
util_connect superdba,colduser!
backup_restore SAVEDATA3  DATA!
backup_restore SAVEPAGES9 PAGES!
backup_restore SAVEPAGESA PAGES!
db_online!
util_release!
bye!

connect kern identified by test!
select * from Test1!
fetch!

*&os rm savetest.data?   savetest.pages? 
commit work release!

dbmcli superdba,colduser!
backup_media_delete SAVEDATA1!
backup_media_delete SAVEDATA2!
backup_media_delete SAVEPAGES1!
backup_media_delete SAVEPAGES2!
backup_media_delete SAVEPAGES3!
backup_media_delete SAVEPAGES4!
backup_media_delete SAVEPAGES5!
backup_media_delete SAVEPAGES6!
backup_media_delete SAVEPAGES7!
backup_media_delete SAVEPAGES8!
backup_media_delete SAVEPAGES9!
backup_media_delete SAVEPAGESA!
bye!
