*   *ID* N00T0402 DBCMD    changed on 1992-09-28-13.24.46 by ADMIN     *
 *** 4.6  <select ordered statement> !
file connect ( kern test !
create table tab1 ( spalte1 char(10) key,
                    spalte2 char(10) not null,
                    spalte3 fixed(5),
                    spalte4 char(10) ) !
create synonym syn1 for tab1 !
create index "tab1.spalte2"
	on tab1( spalte2 ) !
create index index1 on tab1 ( spalte3 desc, spalte4 ) !
insert tab1 values ( 'Paul', 'Berlin', 3, null ) !
insert tab1 values ( 'Eva', 'Bonn', 7, 'Ameise' ) !
insert tab1 values ( 'Karin', 'Hamburg', null, 'Hund' ) !
insert tab1 values ( 'Otto', 'Berlin', 1, 'Katze' ) !
select * from tab1 !
fetch !
commit work !
commit work !
create table tab2 ( col1 char(10),
                    col2 char(10) ascii,
                    col3 float(3) not null ) !
create index index2 on tab2 ( col1 desc ) !
create index "tab2.col3"
	on tab2( col3 desc ) !
create index index3 on tab2 ( col3, col1 desc ) !
insert tab2 values ( 'Hans', 'BREMEN', 8E3 ) !
insert tab2 values ( null, 'HAMBURG', 8E12 ) !
insert tab2 values ( 'Paul', 'KIEL', 8E3 ) !
insert tab2 values ( 'Emma', 'HAMBURG', 70 ) !
select syskey, * from tab2 !
fetch !
commit work !
commit work !
drop table tab1 !
drop table tab2 !
commit work release !
* Und das ganze noch mal mit langen Spalten !
file connect ( kern test !
create table tab1 ( spalte1 char(310) key,
                    spalte2 char(410) not null,
                    spalte3 char(305) ascii,
                    spalte4 char(310) ascii) !
create synonym syn1 for tab1 !
create index "tab1.spalte2"
	on tab1( spalte2 ) !
create index index1 on tab1 ( spalte3 desc, spalte4 ) !
insert tab1 values ( 'Paul', 'Berlin', '3', null ) !
insert tab1 values ( 'Eva', 'Bonn', '7', 'Ameise' ) !
insert tab1 values ( 'Karin', 'Hamburg', null, 'Hund' ) !
insert tab1 values ( 'Otto', 'Berlin', '1', 'Katze' ) !
select * from tab1 !
fetch !
commit work !
commit work !
drop table tab1 !
commit work release !
