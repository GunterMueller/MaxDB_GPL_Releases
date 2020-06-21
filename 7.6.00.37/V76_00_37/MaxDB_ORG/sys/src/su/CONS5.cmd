*   *ID* CONS5    DBCMD    changed on 1992-11-11-17.01.55 by KERN      *
set format eur!
create table b
 (bt  time  constraint bt1 >= bt,
  bt1 time  constraint bt = time) !
file sel_cols_tab ( B !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
select * from b !
fetch!
sleep 3 !
update b set bt1 = time!
drop table b!
commit!
**********************************************************
create table b
 (bt  time  constraint bt = time,
  bt1 time  constraint bt1 >= bt) !
file sel_cols_tab ( B !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
select * from b !
fetch!
sleep 3 !
update b set bt1 = time!
drop table b!
commit!
**********************************************************
create table b
 (bt  time  constraint (bt = time) and (bt1 >= bt),
  bt1 time  ) !
file sel_cols_tab ( B !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
select * from b !
fetch!
sleep 3 !
update b set bt1 = time!
drop table b!
commit!
**********************************************************
create table b
 (bt  time  ,
  bt1 time  constraint (bt = time) and (bt1 >= bt)) !
file sel_cols_tab ( B !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
insert b values (time, time) !
select * from b !
fetch!
sleep 3 !
update b set bt1 = time!
drop table b!
commit!
