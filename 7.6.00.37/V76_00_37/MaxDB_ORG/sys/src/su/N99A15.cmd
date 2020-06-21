*   *ID* N99A15   DBCMD    changed on 1992-09-28-13.27.13 by ADMIN     *
file connect ( kern test !
create table tab ( spalte fixed(5) ) !
insert tab values ( 4 ) !
declare erg cursor for
select * from tab !
declare erg cursor for
select * from tab !
create table erg ( spalte fixed(5) ) !
insert erg values ( 8 ) !
select * from erg !
close erg !
insert erg values ( 9 ) !
rollback work release !
