*   *ID* N99K1    DBCMD    changed on 1992-09-28-13.27.26 by ADMIN     *
file connect ( kern test !
create table tab1 ( s1 fixed(5), s2 fixed(5) ) !
create table tab2 ( v1 fixed(5), v2 fixed(5) ) !
select s1, sum ( s2 )
  from tab1
  group by s1 !
fetch !
insert into tab2
  select s1, sum ( s2 )
  from tab1
  group by s1 !
insert tab1 values ( 5, 5 ) !
select s1, sum ( s2 )
  from tab1
  group by s1 !
fetch !
insert into tab2
  select s1, sum ( s2 )
  from tab1
  group by s1 !
rollback work release !
