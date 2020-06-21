*   *ID* T03SEL   DBCMD    changed on 1992-09-28-13.28.37 by ADMIN     *
 ***************************
 ***   T 0 3 S E L       ***
 ***************************!
 *** SELECT mit AS                                        ***
 *** SELECT mit ORDER BY                                  ***
 *** SELECT mit Zusatzqualifikation                       ***
 *** Zusatzqualifikation mit Konstanten                   *** !
create table kern.personal       (nummer fixed (4,0) key,
            nachname char (12) not null RANGE between 'A' and 'T',
                jahrgang fixed (2) not null RANGE between 17 and 67,
                kinder fixed (2,0) not null RANGE in (0,1,2,3,4,5,6,12),
                mitarbeit char (11) not null RANGE in
                                ('TISCHLEREI', 'SETZEREI',
                                 'SCHNEIDEREI', 'BUCHHALTUNG'),
                    abtleiter char (15) RANGE in
                      ('TISCHLEREI','SETZEREI','BUCHHALTUNG') ) !
create view kern.kinderlos as
       select * from kern.personal
              where kinder = 0 with check option !
create table kern.abt
      (name char (12) key RANGE between 'AA' and 'WW',
                     nummer fixed (6),
                     arbeiter fixed (3) RANGE between 10 and  999,
                     num2 fixed (4) not null RANGE in (1000, 8000, 5300),
          ort char (12) RANGE in ('BERLIN', 'BONN', 'STUTTGART'))!
alter table kern.personal 
    foreign key vorgesetzt ( nachname ) references kern.personal !
create table kern.stadt
    (ortsname char (3) key RANGE in ('BER','BON','STU'))!
commit work!
lock table personal, kinderlos, abt in exclusive mode !
insert into personal set nummer   = 1,
                 nachname = 'KREUTZER',
                 jahrgang = 19,
                 kinder   = 4,
                 mitarbeit= 'SETZEREI' !
insert into personal set nummer   = 24,
                 nachname = 'RICHTER',
                 jahrgang = 63,
                 kinder   = 2 ,
                 mitarbeit= 'TISCHLEREI',
              abtleiter= 'TISCHLEREI' !
insert into personal set nummer   = 3,
                 nachname = 'HASENFUSS',
                 jahrgang = 25,
                 kinder  to null,
                 mitarbeit= 'SETZEREI',
              abtleiter= 'SETZEREI' !
insert into personal set nummer   = 12,
                 nachname = 'KRAKOWSKI',
                 jahrgang = 47,
                 kinder   = 0,
                 mitarbeit= 'SCHNEIDEREI' !
insert into personal set nummer   = 257,
                 nachname = 'RICHTER',
                 jahrgang = 25,
                 kinder   = 1,
                 mitarbeit= 'TISCHLEREI' !
insert into personal set nummer   = 904,
                 nachname = 'HETZER',
                 jahrgang = 40,
                 kinder  to null,
                 mitarbeit= 'SETZEREI' !
insert into personal set nummer   = 356,
                 nachname = 'SUESSLICH',
                 jahrgang = 60,
                 kinder   = 5,
                 mitarbeit= 'TISCHLEREI' !
insert into personal set nummer   = 403,
                 nachname to null,
                 jahrgang = 29,
                 kinder   = 2,
                 mitarbeit= 'BUCHHALTUNG',
              abtleiter= 'BUCHHALTUNG' !
insert into abt set name        = 'BUCHHALTUNG',
            nummer      = 12,
            arbeiter = 12,
            num2        = 8000,
            ort         = 'STUTTGART' !
insert into abt set name        = 'MALEREI',
            nummer      = 121,
            arbeiter = 43,
            num2        = 5300,
            ort         = 'BONN' !
insert into abt set name        = 'SETZEREI',
            nummer      = 38,
            arbeiter = 17,
            num2        = 5300,
            ort         = 'BONN' !
insert into stadt set ortsname = 'BON' !
commit work release !
file connect ( kern test !
 *** Syntaxpruefung
 ***
 *** wegen der Umorganisation der Anfrage in der AP-Schicht
    muessen ') FROM' in der Anfrage sein, auch wenn der
    vordere Teil falsch ist, bevor irgendwelche Fehler ent-
    deckt werden
 *** bis 'SELECT resultname (resultfieldlist) FROM recordset'
                                       richtig ist !
select !
select personal !
select from !
select ) !
select ) personal !
select ) from !
select ) from *personal !
select ) from persnal !
select ) from personal !
select personal_d ) from personal !
select name, nummer, arbeiter, num2, ort
    from abt !
fetch !
close !
declare erg2a cursor for 
select nummer, nachname, jahrgang j,kinder k,
              mitarbeit, abtleiter
    from personal for reuse !
fetch erg2a !
 *** wenn resultname = recordsetname, ist der recordsetname
    bis zum Loeschen der Ergebnismenge nicht ansprechbar !
declare abt cursor for 
select erg2a.nummer, nachname, erg2a.mitarbeit
    from erg2a
    where erg2a.mitarbeit < 'TISCHLEREI'
    order by mitarbeit !
fetch abt !
declare erg2d cursor for 
select name, arbeiter, num2
    from abt !
close abt !
declare erg2d cursor for 
select name, arbeiter, num2
    from abt !
fetch erg2d !
close erg2d !
close erg2a !
declare erg2g cursor for 
select pers.nummer, nachname
    from personal !
declare erg2j cursor for 
select personal.nummer, l
    from personal !
declare erg2m cursor for 
select personal.nummer, nachname, nummer
    from personal !
declare erg2p cursor for 
select personal.nummer, nachname
    from personal, personal !
declare erg2s cursor for 
select personal.nummer, nachname
    from personal, abt !
declare erg2v cursor for 
select personal.nummer, nachname
    from personal, abt
    where personal.nummer > 3 !
declare erg2y cursor for 
select personal.nummer, nachname, abt.name
    from personal, abt
    where personal.mitarbeit = abt.name !
fetch erg2y !
close erg2y !
declare erg2z cursor for 
select personal.nummer, nachname, abt.name
    from personal, abt
    where abt.name = personal.mitarbeit !
fetch erg2z !
close erg2z !
 *** bis '... (resultfieldname = record_field_spec,
                          ggf. mehrmals)' richtig ist !
declare erg3 cursor for 
select personal.nummer, kinder
    from personal, erg3
    where personal.num2 = erg3.num2 !
declare erg3a cursor for 
select personal.nummer, kinder erg3a.k
    from personal !
declare erg3g cursor for 
select personal.nummer n, kinder n
    from personal !
declare erg3j cursor for 
select personal n, kinder k
    from personal !
declare erg3m cursor for 
select personal. n, kinder k
    from personal !
declare erg3p cursor for 
select personal.jahrgang j, kinder k,
              nummer n
    from personal !
fetch erg3p !
close erg3p !
declare erg3s cursor for 
select personal.nummer erg3s, nummer k
    from personal !
close erg3s !
 *** SELECT mit AS !
declare erg4a cursor for 
select personal.nummer, nachname
    from personal abt !
declare erg4d cursor for 
select personal.nummer, nachname
    from personal, personal as !
declare erg4g cursor for 
select personal.nummer, nachname
    from personal, personal as personal !
declare erg4j cursor for 
select personal.nummer, nachname
    from personal, personal as pers !
declare erg4m cursor for 
select personal.nummer, p.nachname
    from personal as pers as p !
declare erg4p cursor for 
select personal.nummer, p.nachname
    from personal, personal  pers !
declare erg5a cursor for 
select pers.nachname,pers.nummer nu,
              kern.personal.nummer,pers.nachname na
    from kern.personal, kern.personal  pers
    where pers.mitarbeit = kern.personal.mitarbeit !
fetch erg5a !
close erg5a !
declare erg5b cursor for 
select pers.nachname,
              pers.nummer nu,
              kern.personal.nummer,
              pers.nachname na
    from kern.personal, kern.personal  pers
    where pers.mitarbeit = kern.personal.mitarbeit and
          pers.nummer <> kern.personal.nummer !
fetch erg5b !
close erg5b !
declare erg5d cursor for 
select name, ort, abt.nummer, nachname
    from personal  abt, abt  a
    where abt.mitarbeit = a.name !
fetch erg5d !
close erg5d !
declare erg5g cursor for 
select a.nummer, name
    from personal  a,
              abt  a !
 *** SELECT mit ORDER BY !
declare erg6a cursor for 
select personal.nummer, nachname
    from personal
    order !
declare erg5d cursor for 
select personal.nummer, nachname
    from personal
    order personal.jahrgang !
declare erg5g cursor for 
select personal.nummer, nachname
    from personal
    order by !
declare erg5j cursor for 
select nachname
    from personal
    order by . !
declare erg5m cursor for 
select personal.nummer, nachname
    from personal
    order by personal.jahrgang, !
declare erg5p cursor for 
select personal.nummer, nachname
    from personal
    order by personal !
declare erg5s cursor for 
select personal.nummer, nachname
    from personal
    order by personal. !
declare erg5v cursor for 
select personal.nummer, nachname
    from personal
    order by personal.l !
declare erg5y cursor for 
select personal.nummer, nachname, jahrgang
    from personal
    order by jahrgang !
fetch erg5y !
close erg5y !
declare erg6a cursor for 
select personal.nummer, nachname
    from personal
    order by asc !
declare erg6d cursor for 
select personal.nummer, nachname
    from personal
    order by personal.jahrgang asc desc !
declare erg6g cursor for 
select personal.nummer, nachname
    from personal
    order by personal.jahrgang asc, desc !
declare erg6j cursor for 
select personal.nummer, nachname
    from personal
    order by personal.jahrgang asc,
             personal.jahrgang desc !
declare erg6l cursor for 
select nummer, nachname, kinder
    from personal  pers
    order by pers.kinder,
             personal.kinder desc !
 *** SELECT mit Zusatzqualifikation !
declare erg7a cursor for 
select personal.nummer, nachname, abt.name
    from personal, abt
    where name < personal.nachname
    order by abt.name asc, personal.nummer!
fetch erg7a !
close erg7a !
declare erg7d cursor for 
select personal.nummer, nachname
    from personal, abt
    where personal.nachname < 'SCHMIDT'
    order by abt.name !
declare erg7g cursor for 
select personal.nummer, nachname
    from personal
    where !
declare erg7j cursor for 
select personal.nummer, nachname
    from personal
    where nummer !
declare erg7m cursor for 
select personal.nummer, nachname
    from personal
    where personal. !
declare erg7p cursor for 
select personal.nummer, nachname
    from personal
    where personal.nummer !
declare erg7s cursor for 
select personal.nummer, nachname
    from personal
    where personal.nummer not = 17 !
 *** Zusatzqualifikation mit Konstanten !
declare erg8a cursor for 
select personal.nummer, nachname
    from personal
    where personal.nachname > undef !
close erg8a !
declare erg8b cursor for 
select nummer, nachname
    from personal
    where personal.nachname <> undef !
fetch erg8b !
close erg8b !
declare erg8d cursor for 
select personal.nummer, nachname
    from personal
    where personal.jahrgang > 12 !
declare erg8g cursor for 
select personal.nummer, nachname
    from personal
    where nummer in (23, 23, 25, 26) !
declare erg8j cursor for 
select personal.nummer, nachname
    from personal
    where personal.nachname in ('IBM', 'TANDEM') !
declare erg8m cursor for 
select personal.nummer, nachname
    from personal
    where personal.mitarbeit in ('SETZEREI',
                               abtleiter) !
declare erg8n cursor for 
select nachname, jahrgang
    from personal
    where jahrgang > 21 and
          jahrgang < 47 !
fetch erg8n !
close erg8n !
declare erg8q cursor for 
select nummer, nachname
    from personal
    where nachname < 'HIRTE' and
          nachname >= 'ADAM' !
fetch erg8q !
 *** SELECTs mit impliziten und expliziten Links !
declare erg8s cursor for 
select vorgesetzter.nummer vn, 
              kern.personal.abtleiter,
              kern.personal.nummer pn, 
              kern.personal.abtleiter pa
    from kern.personal  vorgesetzter, kern.personal
    where vorgesetzter.abtleiter >
              kern.personal.abtleiter !
fetch erg8s !
again erg8s !
declare erg8v cursor for 
select personal.nummer pn, nachname, v.nummer vn
    from personal, personal  v
    where personal vorgesetzt v !
fetch erg8v !
declare erg8w cursor for 
select kern.personal.nachname, 
              pers.mitarbeit, pers.nummer
    from kern.personal, kern.personal  pers
    where kern.personal.mitarbeit = pers.abtleiter !
fetch erg8w !
create support vorgesetzt from personal to personal !
commit work!
declare erg8y cursor for 
select personal.nummer, nachname, v.nummer vn,
         nachname na, abtleiter
    from personal, personal  v
    where personal vorgesetzt v !
fetch erg8y !
 *** SELECTs mit '.fieldname' mit Enumerations-
    typen und ohne !
create table kern.enum   (erster char (2) key RANGE in
    ('H', 'HH', 'B', 'K', 'M', 'HB', 'HL', 'S'),
     zweiter char (3) not null RANGE in
     ('HH', 'K', 'HL', 'S', 'M'),
    dritter char (4) not null,
    vierter char (3) not null) !
commit work!
lock table enum in exclusive mode !
insert into enum set erster = 'HH',
         zweiter = 'HH',
         dritter = 'HH',
         vierter = 'HH' !
insert into enum set erster = 'B',
         zweiter = 'S',
         dritter = 'XY',
         vierter = 'Z' !
insert into enum set erster = 'M ',
         zweiter = 'M',
         dritter = 'A',
         vierter = 'M' !
insert into enum set erster = 'K',
         zweiter = 'HL',
         dritter = 'K   ',
         vierter = 'M' !
insert into enum set erster = 'S',
         zweiter = 'K',
         dritter = 'K',
         vierter = 'S' !
insert into enum set erster = 'HB',
         zweiter = 'HL',
         dritter = 'N',
         vierter = 'N' !
insert into enum set erster = 'HL',
         zweiter = 'HL',
         dritter = 'OPQ',
         vierter = 'HL' !
commit work!
declare einszwei cursor for 
select erster, zweiter, dritter, vierter
    from enum
    where erster = .zweiter !
fetch einszwei !
declare einsdrei cursor for 
select erster, zweiter, dritter, vierter
    from enum
    where erster = .dritter !
fetch einsdrei !
declare einsvier cursor for 
select erster, zweiter, dritter, vierter
    from enum
    where erster = .vierter !
fetch einsvier !
declare vierzwei cursor for 
select erster, zweiter, dritter, vierter
    from enum
    where vierter = .zweiter !
fetch vierzwei !
declare vierdrei cursor for 
select erster, zweiter, dritter, vierter
    from enum
    where vierter = .dritter !
fetch vierdrei !
 *** SELECTs mit Views !
declare erg9a cursor for 
select kinderlos.nummer, kinder
    from personal
    where kinder > 1 !
declare erg9d cursor for 
select nummer, nachname, kinder
    from kinderlos !
fetch erg9d !
close erg9d !
declare erg9g cursor for 
select kinderlos.nummer, kinder
    from kinderlos
    where kinder > 1 !
declare erg9j cursor for 
select kinderlos.nummer, kinder
    from kinderlos
    where kinder < 1 !
fetch erg9j !
close erg9j !
declare erg9m cursor for 
select personal.nummer, nachname,
            abt.name, arbeiter, ort
    from personal, abt
    where personal.mitarbeit = abt.name !
fetch erg9m !
close erg9m !
declare erg9p cursor for 
select personal.nummer, nachname,
            abt.name, arbeiter, ort
    from personal, abt
    where personal.nummer = abt.nummer !
fetch erg9p !
fetch erg9p !
again erg9p !
fetch erg9p !
close erg9p !
 *** unsinniges SELECT !
declare erg9s cursor for 
select personal.nummer, nachname,
            abt.name
    from personal, abt
    where personal.nummer = abt.nummer
      and personal.nummer > abt.nummer !
close erg9s !
 ***  Probleme mit Enumerationsverschluesselung !
declare erg9t cursor for 
select abt.name, ort, stadt.ortsname
    from abt, stadt
    where abt.ort = stadt.ortsname !
 *** SELECT mit DISTINCT !
select distinc erg10a !
select distinct !
declare distinct cursor for 
select distinct personal.nachname, mitarbeit
    from personal !
fetch distinct !
close distinct !
declare erg10d cursor for 
select distinct mitarbeit
    from personal !
fetch erg10d !
 ***  Probleme, wenn Feldnamen gleich Rekordnamen sind !
create table kern.arbeiter     (eins char (2) key,
                              zwei char (7) not null) !
commit work !
declare erg cursor for 
select name, nummer
    from abt
    where ort = 'BONN' and
          arbeiter > 732 !
drop table kern.arbeiter !
 *** einige UNDEFs, weshalb einige Dinge erst definiert
                      werden muessen !
drop view kern.kinderlos !
grant select ,
      update (nachname, kinder) on kern.personal
      to eins !
create index "kern.personal.kinder"
	on kern.personal( kinder ) !
create view kern.alt as
       select * from kern.personal
              where jahrgang < 40  with check option !
grant update (jahrgang, nachname) on kern.alt
      to eins !
commit work!
file sel_cols_1tab ( KERN PERSONAL !
file sel_view1 ( KERN PERSONAL !
file sel_references ( KERN PERSONAL !
file sel_index_tab ( PERSONAL !
file sel_priv_on_tab ( ALT !
file sel_priv_grant_on ( ALT !
file sel_priv_on_tab ( PERSONAL !
file sel_priv_grant_on ( PERSONAL !
commit work!
alter table kern.alt drop nachname !
alter table kern.personal drop jahrgang !
alter table kern.personal drop mitarbeit !
alter table kern.personal drop abtleiter !
alter table kern.personal drop kinder !
alter table kern.personal drop nummer !
alter table kern.personal drop nachname !
commit work!
file sel_cols_tab ( PERSONAL !
file sel_priv_on_tab ( PERSONAL !
file sel_priv_grant_on ( PERSONAL !
drop table kern.personal !
revoke all privileges
       from eins !
drop table kern.personal !
revoke all privileges
       from eins !
drop table kern.personal) !
commit work!
file sel_tab_owner ( KERN !
file sel_view1 ( KERN ABT !
SELECT OWNER,TABLENAME,COLUMNNAME,REFNAME,
       REFOWNER,REFTABLENAME,REFCOLUMNNAME,
       RULE,"DATE","TIME"
  FROM SYSDD.FOREIGN_KEYS
 where refowner = 'SUT' !
fetch !
file sel_index_1tab ( KERN ABT !
file sel_index_1tab ( KERN ENUM !
file sel_index_1tab ( KERN STADT !
file sel_priv_granted ( PUBLIC !
 *** Pruefung, ob undef in range enthalten ist) !
alter table kern.abt column chef char (12) 
   RANGE in 'MEIER' .. 'SCHULZ' !
commit work!
file sel_cols_1tab ( KERN ABT !
lock table kern.abt in exclusive mode !
update of abt set name = 'BUCHHALTUNG',
         chef = 'KREUTZER' !
update of abt set name = 'SETZEREI',
         chef = 'RICHTER') !
alter table kern.abt drop chef !
 * aufraeumen !
file sel_cols_1tab ( KERN ABT !
drop table kern.abt !
drop table kern.stadt !
drop table kern.enum !
commit work release !
