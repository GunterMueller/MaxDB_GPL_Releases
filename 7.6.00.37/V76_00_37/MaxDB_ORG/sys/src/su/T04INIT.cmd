*   *ID* T04INIT  DBCMD    changed on 1992-09-28-13.28.37 by ADMIN     *
 ***************************
 ***   T 0 4 I N I T     ***
 ***************************!
commit work !
create table p2   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p3   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p4   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p5   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p6   (k1 char(1) key, f1 char(1) ) !
commit work !
create view p31 as
       select * from p3
              where k1 < 'd' !
commit work !
create view p32 as
       select * from p3
              where k1 >= 'b' and k1 < 'r' !
commit work !
create view p33 as
       select * from p3
              where k1 = 'c' or k1 = 'x' !
commit work !
create view p34 as
       select * from p3
              where k1 = 'c' or f1 = 'x' !
commit work !
lock table p3 , p4 , p5 in exclusive mode !
insert into p3 set k1 = 'a', f1 = 'c' !
insert into p3 set k1 = 'c', f1 = 'c' !
insert into p3 set k1 = 'x', f1 = 'x' !
insert into p4 set k1 = 'u', f1 = 'u' !
insert into p4 set k1 = 'x', f1 = 'x' !
insert into p4 set k1 = 'b', f1 = 'c' !
insert into p5 set k1 = 'l', f1 = 'l' !
insert into p5 set k1 = 'm', f1 = 'c' !
insert into p5 set k1 = 'n', f1 = 'u' !
insert into p5 set k1 = 'o', f1 = 'o' !
insert into p6 set k1 = 'a', f1 = 'c' !
insert into p6 set k1 = 'c' !
insert into p6 set k1 = 'x' !
commit work !
