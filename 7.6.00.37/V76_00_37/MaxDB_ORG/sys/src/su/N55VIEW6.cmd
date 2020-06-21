*   *ID* N55VIEW6 DBCMD    changed on 1992-09-28-13.26.55 by ADMIN     *
file connect ( kern test !
* p121433 vom 25.7.90
create table buchung (bnr fixed(4) range between 1 and 9999,
knr fixed (4) range between 1 and 9999,
hnr fixed (4) range between 1 and 9999,
adatum date ,
edatum date ,
preis fixed (6,2) range between -1000 and 1000)!
insert buchung values (6000, 1, 1, date, date, 23.5) !
create view x as select * from buchung
   where bnr > 5000 or bnr < 0 with check option !
insert x values (4000, 1, 1, date, date, 23.5) !
insert x values (7000, 1, 1, date, date, 23.5) !
select * from buchung !
fetch!
update x set bnr = bnr - 2000 where bnr = 6000 !
select * from buchung !
fetch!
delete buchung !
* Fehlerprotokoll 133 !
create table buchung1 (bnr fixed(4),
knr fixed (4) range between 1 and 9999,
hnr fixed (4) range between 1 and 9999,
adatum date ,
edatum date ,
preis fixed (6,2) range between -1000 and 1000)!
insert buchung1 values (-1000, 1, 1, date, date, 0) !
insert buchung1 values (4000, 1, 1, date, date, 10) !
insert buchung1 values (6000, 1, 1, date, date, 20) !
insert x select * from buchung1 where preis = 0 !
insert x select * from buchung1 where preis = 10!
insert x select * from buchung1 where preis = 20!
select * from buchung !
fetch !
drop table buchung !
drop table buchung1!
commit release !
