*   *ID* C00E212  DBCMD    changed on 1992-09-28-13.23.07 by ADMIN     *
* interne Fehlernummer 212 !
file cleark !
file connect ( kern test !
create table t ( a char (8) key, b char (8))!
insert t set a = 'aaa', b = 'bbb'!
create table t1 (a1 char (8) key, b1 char (8))!
insert t1 values ('aaa', 'bbb')!
create view v as select max(a),b from t group by b !
create view x as select max(a)   from t !
declare erg cursor for 
select * from v for update of b !
fetch1 erg !
update v set b='vvv' where current of erg !
close erg !
declare erg cursor for 
select * from x for update !
fetch1 erg !
update x set b='vvv' where current of erg !
rollback work release!
