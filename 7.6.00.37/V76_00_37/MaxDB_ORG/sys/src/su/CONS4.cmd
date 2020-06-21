*   *ID* CONS4    DBCMD    changed on 1992-09-28-13.22.59 by ADMIN     *
set format eur !
create table b
 (bd date  constraint (bd >= '1.1.0001') and (bd <= '31.12.2010'),
  bt time  constraint (bt = time) and (bt >= bd)) !
create table b
 (bd date  constraint (bd >= '1.1.0001') and (bd <= '31.12.2010'),
  bt time  constraint (bt >= '0.0.0') and (bt <= '24.0.0')) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (date, time) !
select * from b !
fetch!
set format internal !
insert b values (date, time) !
select * from b !
fetch!
set format usa !
insert b values (date, time) !
select * from b !
fetch!
drop table b!
commit!
set format internal !
create table b
 (bd date  constraint (bd >= '00010101') and (bd <= '20101231'),
  bt time  constraint (bt >= '00000') and (bt <= '240000')) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (date, time) !
select * from b !
fetch!
set format eur !
insert b values (date, time) !
select * from b !
fetch!
set format usa !
insert b values (date, time) !
select * from b !
fetch!
drop table b!
commit!
set format usa !
create table b
 (bd date  constraint (bd >= '1/1/0001') and (bd <= '12/31/2010'),
  bt time  constraint (bt >= '0 am') and (bt <= '12 am')) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (date, time) !
select * from b !
fetch!
set format eur !
insert b values (date, time) !
select * from b !
fetch!
set format usa !
insert b values (date, time) !
select * from b !
fetch!
drop table b!
commit!
set format eur !
create table b
 (bd date  default '1.6.1990'
           constraint (bd >= '1.1.0001') and (bd <= '31.12.2010'),
  bt time  default '59.59.59'
           constraint (bt >= '0.0.0') and (bt <= '60.0.0')) !
file sel_cols_tab ( B !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (default, default)!
select * from b !
fetch!
set format usa !
file sel_checkdef ( USER B BD !
file sel_checkdef ( USER B BT !
insert b values (default, default)!
select * from b !
fetch!
drop table b!
commit!
