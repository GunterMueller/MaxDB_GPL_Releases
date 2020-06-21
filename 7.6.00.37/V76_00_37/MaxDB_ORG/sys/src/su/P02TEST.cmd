*   *ID* P02TEST  DBCMD    changed on 1992-09-28-13.27.41 by ADMIN     *
 ***************************
 ***   P 0 2 T E S T     ***
 ***************************!
create view scp as select * from abt
    where num2 = + 1000
    with check option !
create view scs as select * from abt
    where num2 = 1000
    with check option !
create view scv as select * from abt
    where num2 is null
    with check option !
commit work !
 *** unsinnige Feldbedingungen !
create view sda as select * from abt
    where num2 = 1000 and num2 = 8000
    with check option !
create view sdd as select * from abt
    where arbeiter > 394 and
          arbeiter >= 11 and
          arbeiter < 401 and
          arbeiter > 400
    with check option !
commit work !
 *** richtige Views mit zum Teil doppelten Konstanten !
create view ses as select * from abt
    where arbeiter in (10, 11, 12, 12, 15, 17)
    with check option !
create view sfa as select * from abt
    where num2 in (1000, 8000, 1000)
    with check option !
create view sfd as select * from abt
    where num2 in (1000)
    with check option !
create view sfg as select * from abt
    where num2 not in (1000, 8000, 5300)
    with check option !
commit work !
 *** zum Feld passende Konstante mit comp_op '=' !
create view sja as select * from abt
    where name is not null
    with check option !
create view sjd as select * from abt
    where name = 'MEIER'
    with check option !
commit work !
create view sjg as select * from abt
    where nummer = 123456
    with check option !
create view sjj as select * from abt
    where ort = 'MUENCHEN       '
    with check option !
commit work !
 *** zum Feld passende Konstante in Aufzaehlung !
create view sla as select * from abt
    where arbeiter in (865, 73, 354)
    with check option !
create view sldd as select * from abt
    where ort in ('MUENCHEN       ')
    with check option !
commit work !
create view slg as select * from abt
    where ort in ('BONN', 'BERLIN')
    with check option !
commit work !
 *** Definitionen mit 'OR' !
create view sma as select * from abt
    where arbeiter = 10 or
          arbeiter = 999
    with check option !
commit work !
create view smd as select * from abt
    where arbeiter > 45 or
          ort = 'BONN' or
          ort = 'MUENCHEN'
    with check option !
create view smg as select * from abt
    where arbeiter = 45 or
          arbeiter <> 45
    with check option !
commit work !
 *** Definitionen mit 'OR', 'NOT', '( ... )' !
create view sna as select * from abt
    where not (ort not in ('BONN'))
    with check option !
commit work !
create view snd as select * from abt
    where (ort = 'BONN') or (num2 = 1000)
    with check option !
commit work !
create view snh as select * from abt
    where ((ort = 'BONN' or num2 = 1000) and
            not (nummer > 352))
    with check option !
commit work !
create view snk as select * from abt
    where (not (ort = 'BONN') and num2 = 1000)
    with check option !
commit work !
create view snm as select * from abt
    where not (ort = 'BONN' and
               num2 = 1000)
    with check option !
commit work !
create view snp as select * from abt
    where ort = 'BONN' and ort not in ('BONN') or
          arbeiter > 500
    with check option !
commit work !
create view sns as select * from abt
    where ort = 'BONN' or ort = 'MUENCHEN' and nummer > 3150
    with check option !
commit work !
create view snv as select * from abt
    where (ort = 'BONN' or ort = 'MUENCHEN')
    and nummer > 3150
    with check option !
commit work !
 *** Definitionen mit 'AND' !
create view sqa as select * from abt
    where arbeiter > 45 and
          ort = 'BONN  '
    with check option !
commit work !
create view sqd as select * from abt
    where arbeiter > 45 and
          nummer in (27,38,283,450,675)
    with check option !
commit work !
insert into sdd set name = 'Aaa', num2 = 400 !
insert into sdd set name = 'AAb', arbeiter = 401, num2 = 1000!
insert into sfg set name = 'Baa', num2 = 9000 !
insert into sfg set name = 'Bab', num2 = 1000 !
insert into smg set name = 'Caa', num2 = 1000, arbeiter= 48 !
insert into smg set name = 'Cab', num2 = 1000, arbeiter= 45 !
insert into sna set name = 'Daa',
                    num2 = 1000, ort = 'MUENCHEN'  !
insert into sna set name = 'Dab',
         num2 = 1000,
         ort = 'BONN' !
insert into snh set name = 'Eaa',
         num2 = 1000,
         ort = 'BONN',
         nummer = 352 !
insert into snh set name = 'Eab',
         num2 = 5300,
         ort = 'BONN',
         nummer = 351 !
insert into snh set name = 'Eac',
         num2 = 1000,
         ort = 'MUENCHEN',
         nummer = 351 !
insert into snk set name = 'Faa',
         num2 = 1000,
         ort = 'MUENCHEN' !
insert into snm set name = 'Gaa',
         num2 = 1000,
         ort = 'BONN' !
insert into snm set name = 'Gab',
         num2 = 5300,
         ort = 'BONN' !
insert into sns set name = 'Haa',
         num2 = 1000,
         ort = 'BONN',
         nummer = 2480 !
insert into sns set name = 'Hab',
         num2 = 1000,
         ort = 'MUENCHEN',
         nummer = 5240 !
insert into sns set name = 'Hac',
         num2 = 1000,
         ort = 'BONN',
         nummer = 5240 !
insert into snv set name = 'Iaa',
         num2 = 1000,
         ort = 'BONN',
         nummer = 2480 !
insert into snv set name = 'Iab',
         num2 = 1000,
         ort = 'MUENCHEN',
         nummer = 5240 !
insert into snv set name = 'Iac',
         num2 = 1000,
         ort = 'BONN',
         nummer = 5240 !
commit work !
 *** Vermischtes !
create table sld  (y char (14) key) !
create view sld as select * from abt
    where ort = 'BERLIN' !
commit work !
create view ssa as select * from sld
    where arbeiter > 35 !
create view ssd as select * from abt
    where abt.arbeiter > 40 !
commit work !
