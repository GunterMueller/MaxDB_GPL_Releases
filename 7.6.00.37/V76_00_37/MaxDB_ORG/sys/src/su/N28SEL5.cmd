*   *ID* N28SEL5  DBCMD    changed on 1992-09-28-13.25.55 by ADMIN     *
 *** JOIN ueber key-spalten !
file connect ( kern test !
CREATE TABLE TAB11
   ( col1 CHAR(12) KEY,
     col2 CHAR(12) KEY ) !
CREATE TABLE TAB41
   ( spalte1 CHAR(18) KEY,
     spalte2 CHAR(18) KEY ) !
insert tab11 values ( 'Y', 'M' ) !
insert tab41 values ( 'Y', 'M' ) !
select tab11.col1, tab11.col2, ref.spalte1, ref.spalte2
    from tab41 ref , tab11
    where (tab11.col1, tab11.col2) in ((ref.spalte1, ref.spalte2)) !
fetch !
rollback work release !
file connect ( kern test !
create table eins ( wf char (12), wnr fixed (4))!
create index "eins.wf"
	on eins( wf ) !
insert eins values ('computer', 1)!
insert eins values ('software', 2)!
create table zwei (wnr fixed(4))!
create index "zwei.wnr"
	on zwei( wnr ) !
insert zwei set wnr = 1!
insert zwei set wnr = 2!
select eins.wnr, wf from eins, zwei
  where (eins.wnr, wf) in ((zwei.wnr, 'computer'), (zwei.wnr, 'software')) !
fetch!
rollback release !
