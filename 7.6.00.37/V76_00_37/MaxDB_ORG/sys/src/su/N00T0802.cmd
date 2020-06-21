*   *ID* N00T0802 DBCMD    changed on 1992-09-28-13.24.54 by ADMIN     *
 *** 8.3  <alter table statement> !
file connect ( kern test !
create table tabalter ( c1 fixed(5) key ) !
alter table tabalter add ( c2 char(15) ,
                           c3 char(7 ) byte ) !
alter table tabalter add
   c4 float(7) range between - 3000 and + 3000 !
alter table tabalter drop (c2) !
alter table tabalter drop c3,c4 !
alter table tabalter add
   c2 char(8) ascii range in ( 'BERLIN', 'HAMBURG' ) !
create domain dom1 char(10) !
create domain dom2 char(10) range between 'a' and 'k'  !
alter table tabalter add ( c5 dom1 ) !
alter table tabalter add   c6  dom2   !
alter table tabalter drop c6 !
alter table tabalter add ( c6 char(10), c7 dom2 ) !
commit work !
drop domain dom1 !
drop domain dom2 !
drop table tabalter !
commit work !
create table tab ( c1 char(5),
                   c2 char(5) range between 'A' and 'K',
                   c3 char(5) ascii,
                   c4 fixed(5,1),
                   c5 fixed(3,2),
                   c6 float(3) range not between -1E10 and +1E10,
                   c7 float(5) range between 0 and 10E5 ) !
insert tab values
     ( 'Otto', 'Anna', 'Maria', 3333.3, 1.11, -4.44E15, 100 ) !
alter table tab add constraint c1 check c1 in ( 'Otto', 'Paul' ) !
alter table tab column c2 char(7) ascii !
alter table tab column c3 char(6) !
alter table tab add constraint c3 check c3 between 'M' and 'P' !
alter table tab column c4 fixed(7,2) !
alter table tab add constraint c4 check c4 not in (0) !
alter table tab column c5 float(4) !
alter table tab column c6 float(8) !
ALTER TABLE tab ALTER CONSTRAINT c6 CHECK c6 BETWEEN -1E20 AND 0 !
alter table tab drop constraint c7 !
file sel_cols_tab ( TAB !
rollback work release !
