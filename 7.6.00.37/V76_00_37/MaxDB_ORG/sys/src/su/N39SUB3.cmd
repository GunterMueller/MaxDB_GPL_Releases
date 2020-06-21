*   *ID* N39SUB3  DBCMD    changed on 1992-09-28-13.26.29 by ADMIN     *
file CONNECT ( KERN TEST !
create table tt ( a char (9))!
insert tt set a ='aaaaa'!
create table tt1 ( b char (7))!
insert tt1 set b = 'aaaaaa'!
select * from tt where a = any (select b from tt1)!
delete tt1 !
insert tt1 set b = 'aaaaa'!
select * from tt where
   a = any (select b from tt1 where b = any(select a from tt )) !
drop table tt !
drop table tt1!
create table tt ( a char (7))!
insert tt set a ='aaaaa'!
create table tt1 ( b char (9))!
insert tt1 set b = 'aaaaa'!
select * from tt where
   a = any (select b from tt1 where b = any(select a from tt )) !
commit work release !
