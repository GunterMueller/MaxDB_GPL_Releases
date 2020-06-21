*   *ID* N28SEL3  DBCMD    changed on 1992-09-28-13.25.54 by ADMIN     *
 *** Es wird getestet, ob 17 benannte Ergebnistabellen erzeugt
 *** werden koennen. !
file connect ( kern test !
create table tab ( spalte fixed(5) ) !
insert tab values ( 1  ) !
insert tab values ( 18 ) !
insert tab values ( 11 ) !
insert tab values ( 9  ) !
insert tab values ( 7  ) !
insert tab values ( 12 ) !
insert tab values ( 3  ) !
insert tab values ( 7  ) !
insert tab values ( 17 ) !
insert tab values ( 16 ) !
insert tab values ( 15 ) !
insert tab values ( 2  ) !
insert tab values ( 5  ) !
insert tab values ( 13 ) !
insert tab values ( 9  ) !
insert tab values ( 18 ) !
insert tab values ( 16 ) !
insert tab values ( 7  ) !
insert tab values ( 4  ) !
insert tab values ( 6  ) !
insert tab values ( 12 ) !
declare erg1 cursor for select * from tab where spalte < 5 !
fetch erg1 !
declare erg2 cursor for select spalte from tab where spalte > 17 !
fetch erg2 !
declare erg3 cursor for select * from tab !
declare erg4 cursor for select * from tab where spalte > 10 and  spalte < 12 !
declare erg5 cursor for select spalte col from tab 
where spalte between 5 and 6 !
declare erg6 cursor for select * from tab !
declare erg7 cursor for select spalte col from tab where spalte = 7 !
declare erg8 cursor for select * from tab where spalte > 12 and 14 > spalte !
declare erg9 cursor for select spalte +5 col from tab where spalte = 5+7 !
declare erg10 cursor for select * from tab where spalte is not null !
declare erg11 cursor for select * from tab where spalte is null !
declare erg12 cursor for select spalte from tab where spalte = 8 !
declare erg13 cursor for select sum ( spalte) col1 from tab group by spalte !
declare erg14 cursor for select spalte + 3 col1, 3*spalte col2 from tab !
declare erg15 cursor for select * from tab where spalte = 1 or spalte = 4 !
declare erg16 cursor for select avg ( spalte ) col1 from tab !
declare erg17 cursor for select * from tab !
select * from tab where spalte = 12 !
fetch erg11 !
fetch !
fetch erg9 !
commit work release !
