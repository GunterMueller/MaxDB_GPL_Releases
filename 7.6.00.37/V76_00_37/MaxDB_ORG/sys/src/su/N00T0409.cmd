*   *ID* N00T0409 DBCMD    changed on 1992-09-28-13.24.50 by ADMIN     *
 ***       <select ordered statement: positioned> !
file connect ( kern test !
create table tab ( spalte1 fixed(5) key,
                   spalte2 fixed(3),
                   spalte3 fixed(4) ) !
create index "tab.spalte2"
	on tab( spalte2 ) !
create index ind1 on tab ( spalte3 desc ) !
create index ind2 on tab ( spalte2 desc, spalte3 ) !
insert tab values ( 3, 3, 3 ) !
insert tab values ( 5, 5, 5 ) !
insert tab values ( 1, 1, 1 ) !
declare erg cursor for
select 3 + spalte1 col, spalte3 from tab for update !
fetch1 erg !
fetch1 erg !
rollback work release !
