*   *ID* CONS2    DBCMD    changed on 1992-09-28-13.22.58 by ADMIN     *
create table b
 (b1 int key ,
  b2 int key ,
  b3 int key ,
  b31 varchar  (254) default 'a',
  b4 varchar  (100) not null with default constraint b4 <= b31,
  b5 varchar  (200) not null with default constraint b5 <= b4,
  b6 varchar  (254) not null with default constraint b6 <= b5)!
commit!
insert b values (1, 1, 1, default, 'a', 'a', 'a')!
insert b values (1, 1, 2, default, default, default, default)!
insert b (b1, b2, b3) values (1, 1, 3)!
select * from b !
fetch!
rollback!
file sel_checkdef ( USER B B6 !
alter table b drop b6 !
file sel_checkdef ( USER B B6 !
alter table b add (b6 varchar (254) constraint b6 <= b5) !
file sel_checkdef ( USER B B6 !
insert b values (1, 1, 1, default, 'a', 'a', 'a')!
insert b values (1, 1, 2, default, default, default, default)!
insert b (b1, b2, b3) values (1, 1, 3)!
select * from b !
fetch!
drop table b !
commit!
***********************************************************
***********************************************************
***********************************************************
create table b
 (
  b31 varchar   (50) default 'a',
  B4  varchar  (100) not null with default constraint B4 <> 'a',
  b5  varchar  (200) constraint b5 = 'e' or b5 <> 'e',
  b6  varchar  (254) default 'g' constraint b6 > b5 or b5 is null,
  b1 int,
  b2 int,
  b3 int,
  primary key (b1,b2,b3))!
insert b (b1, b2, b3) values (1, 1, 1)!
alter table b column b5 add default 'c'!
insert b (b1, b2, b3) values (1, 1, 1)!
select * from b !
fetch!
update b set b4 = 'h', b5 = 'd',b6 = 'f'!
select * from b !
fetch!
update b set b4 = 'a'!
update b set b6 = 'd'!
drop table b !
commit!
***********************************************************
***********************************************************
***********************************************************
commit work release!
file connect ( kern test !
create user aa password aa resource !
commit work release!
file connect ( bubu baba !
create table b
 (b1  int key ,
  b2  int key ,
  b3  int key ,
  b31 varchar   (50) ascii default 'm',
  b4  varchar  (100) default 'o' constraint b4 > b31,
  b5  varchar  (200) ascii default 'p' constraint b5 > b4,
  b6  varchar  (254) default 'q' constraint b6 > b4)!
insert b (b1, b2, b3) values (1, 1, 1)!
grant select (b1, b2, b3, b5, b6) on b to aa!
grant update (b5, b6) on b to aa!
update b set b5 = 'o', b6 = 'm'!
commit work release!
file connect ( aa aa!
select * from bubu.b !
fetch!
file sel_cols_1tab ( BUBU B !
file sel_checkdef ( BUBU B B5 !
file sel_checkdef ( BUBU B B6 !
update bubu.b set b4 = 'p'!
update bubu.b set b5 = 'p', b6 = 'r'!
select * from bubu.b !
fetch!
commit work release!
file connect ( bubu baba!
alter table b drop (b4)!
commit work release!
file connect ( aa aa!
update bubu.b set b5 = 'o', b6 = 'm'!
select * from bubu.b !
fetch!
file sel_cols_1tab ( BUBU B !
file sel_checkdef ( BUBU B B5 !
file sel_checkdef ( BUBU B B6 !
commit work release!
file connect ( kern test !
drop user aa !
commit work release!
file connect ( bubu baba !
drop table b !
commit!
