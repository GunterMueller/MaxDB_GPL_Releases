*   *ID* H01SAVE1 DBCMD    changed on 1992-09-28-13.23.52 by ADMIN     *
dbmcli superdba,colduser!
backup_media_put HOST1 host1.host FILE DATA 0 8 YES NO ""!
backup_media_put HOST2 host2.host FILE DATA 0 8 YES NO ""!
util_connect sut,sut!
backup_save HOST1 DATA!
backup_save HOST2 DATA!
util_release!
* -----changed by alex  dboffline!
db_restart!
db_admin!
util_connect superdba,colduser!
backup_restore HOST1 DATA!
util_execute clear log!
util_release!
db_online!
bye!
 ***********************************************************!
connect d1 identified by w !
delete r1.r1xt1 where r1xt1xc1 < 0 !
commit work release !
 ***********************************************************!
dbmcli superdba,colduser!
util_connect d1,w!
* -----changed by alex  dboffline!
db_restart!
db_admin!
util_connect superdba,colduser!
backup_restore HOST2 DATA!
util_execute clear log!
util_connect superdba,colduser!
db_online!
util_release!
bye!
 ***********************************************************!
connect r1 identified by w !
select * from r1.r1xt1 !
fetch !
commit work release !
 ***********************************************************!
dbmcli superdba,colduser!
util_connect sut,sut!
* -----changed by alex  dboffline!
db_restart!
db_admin!
util_connect superdba,colduser!
util_execute clear log!
backup_restore HOST2 DATA!
util_execute clear log!
db_online!
util_release!
bye!
 ***********************************************************!
connect sut identified by sut !
select * from r1.r1xt1 !
fetch !
insert r1.r1xt1 values ( 2e-12) !
commit work release  !
 ***********************************************************!
dbmcli superdba,colduser!
util_connect d1,w!
* -----changed by alex  dboffline!
db_restart!
db_admin!
util_connect superdba,colduser!
backup_restore HOST2 DATA!
util_execute clear log!
backup_save HOST2 DATA!
db_online!
util_release!
backup_media_delete HOST1!
backup_media_delete HOST2!
bye!
 ***********************************************************!
connect r1 identified by w !
select * from r1xt1 !
fetch !
commit work release !
