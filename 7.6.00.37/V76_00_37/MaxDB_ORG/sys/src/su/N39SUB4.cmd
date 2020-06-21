*   *ID* N39SUB4  DBCMD    changed on 1992-09-28-13.26.29 by ADMIN     *
file CONNECT ( KERN TEST !
create table tab1 ( a fixed(5) ) !
create table tab2 ( b fixed(5) ) !
insert tab1 values ( 100 ) !
insert tab1 values ( 200 ) !
insert tab2 values ( 300 ) !
insert tab2 values ( 100 ) !
insert tab2 values ( 100 ) !
select * from tab1
   where a = ( select distinct b from tab2 where b = 100 ) !
rollback work release !
