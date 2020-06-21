senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* FOKUS.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test sqlmode oracle !
create table mitarbeiter (
   ma_id        number (3) not null primary key,
   name         char (30)  not null,
   soll_stunden number (3) not null)!
insert into mitarbeiter values (1, 'Ladewig', 40)!
insert into mitarbeiter values (2, 'Schweiberger', 40)!
insert into mitarbeiter values (3, 'Crossley', 20)!
insert into mitarbeiter values (4, 'Ellis', 20)!
create table projekt (
   projektid   number(3) not null primary key,
   its_name  char(30) not null)!
insert into projekt values (1, 'Beratung IT Verlag')!
insert into projekt values (2, 'Loesung SQL Reatsel')!
insert into projekt values (3, 'Uebersetzung/Dokumentation')!
insert into projekt values (4, 'Vernetzung IT Verlag')!
create table projekt_zuordnung (
   ma_id                number (3) not null references mitarbeiter,
   projektid           number (3) not null references projekt,
   zeit_anteil_prozent  number (3),
        primary key (ma_id, projektid))!
insert into projekt_zuordnung values (1, 1, 50)!
insert into projekt_zuordnung values (1, 2, 50)!
insert into projekt_zuordnung values (2, 1, 30)!
insert into projekt_zuordnung values (2, 2, 60)!
insert into projekt_zuordnung values (2, 3, 10)!
insert into projekt_zuordnung values (3, 2, 10)!
insert into projekt_zuordnung values (3, 3, 90)!
create table projzeit (
   ma_id       number (3) not null,
   projektid  number(3) not null,
   datum       date not null,
   woche       number (4) not null,
   stunden     number (2) not null,
        primary key (ma_id, projektid, datum),
        foreign key (ma_id, projektid) references projekt_zuordnung)!
insert into projzeit values (1, 1, '21-JUN-93', 9325, 8)!
insert into projzeit values (1, 1, '22-JUN-93', 9325, 8)!
insert into projzeit values (1, 2, '23-JUN-93', 9325, 8)!
insert into projzeit values (1, 2, '24-JUN-93', 9325, 8)!
insert into projzeit values (1, 2, '25-JUN-93', 9325, 8)!
insert into projzeit values (2, 1, '21-JUN-93', 9325, 8)!
insert into projzeit values (2, 1, '22-JUN-93', 9325, 8)!
insert into projzeit values (2, 2, '23-JUN-93', 9325, 8)!
insert into projzeit values (2, 2, '24-JUN-93', 9325, 8)!
insert into projzeit values (2, 2, '25-JUN-93', 9325, 8)!
insert into projzeit values (3, 2, '21-JUN-93', 9325, 4)!
insert into projzeit values (3, 3, '21-JUN-93', 9325, 4)!
insert into projzeit values (3, 3, '22-JUN-93', 9325, 8)!
insert into projzeit values (3, 3, '23-JUN-93', 9325, 8)!
insert into projzeit values (3, 3, '24-JUN-93', 9325, 8)!
insert into projzeit values (3, 3, '25-JUN-93', 9325, 8)!
commit work !
select mitarbeiter.name,
       projekt.its_name
  from mitarbeiter, projekt_zuordnung, projekt
 where mitarbeiter.ma_id = projekt_zuordnung.ma_id
   and projekt_zuordnung.projektid = projekt.projektid
 and (mitarbeiter.soll_stunden * projekt_zuordnung.zeit_anteil_prozent)
        / 100
     > (select nvl(sum(projzeit.stunden), 0)
          from projzeit
         where projzeit.ma_id = mitarbeiter.ma_id
           and projzeit.projektid = projekt.projektid
           and woche = 9325)!
fetch into :ma, :pr !
commit release !
file connect ( kern test  !
select soll.ma_id, soll.name, its_name, soll, geleistet
    FROM
    (select fixed (sum (stunden), 5,2) geleistet, ma_id, projektid
       from projzeit
       group by ma_id, projektid
     union
     select 0, ma_id, projektid from projekt_zuordnung where
        chr (ma_id) || chr (projektid) NOT IN
          (select chr(ma_id) || chr (projektid) from projzeit)
    ) geleistet,
    (SELECT A.MA_ID, A.NAME, B.projektid, its_name,
       FIXED (SOLL_STUNDEN / 100 * ZEIT_ANTEIL_PROZENT, 5, 2) soll
                    FROM MITARBEITER A,
                    PROJEKT_ZUORDNUNG B,
                    PROJEKT WHERE A.MA_ID = B.MA_ID AND
                    B.projektid = PROJEKT.projektid) soll
where 
soll.ma_id      = geleistet.ma_id      and
soll.projektid = geleistet.projektid and
soll > geleistet !
fetch !

select soll.ma_id, soll.name, its_name, soll, geleistet
    FROM
    (SELECT A.MA_ID, A.NAME, B.projektid, its_name,
       FIXED (SOLL_STUNDEN / 100 * ZEIT_ANTEIL_PROZENT, 5, 2) soll
                    FROM MITARBEITER A,
                    PROJEKT_ZUORDNUNG B,
                    PROJEKT WHERE A.MA_ID = B.MA_ID AND
                    B.projektid = PROJEKT.projektid) soll,
    (select fixed (sum (stunden), 5,2) geleistet, ma_id, projektid
       from projzeit
       group by ma_id, projektid
     union
     select 0, ma_id, projektid from projekt_zuordnung where
        chr (ma_id) || chr (projektid) NOT IN
          (select chr(ma_id) || chr (projektid) from projzeit)
    ) geleistet
where 
soll.ma_id      = geleistet.ma_id      and
soll.projektid = geleistet.projektid and
soll > geleistet !
fetch!

select soll.ma_id, soll.name, its_name, soll, geleistet
    FROM
    (SELECT A.MA_ID, A.NAME, B.projektid, its_name,
       FIXED (SOLL_STUNDEN / 100 * ZEIT_ANTEIL_PROZENT, 5, 2) soll
                    FROM MITARBEITER A,
                    PROJEKT_ZUORDNUNG B,
                    PROJEKT WHERE A.MA_ID = B.MA_ID AND
                    B.projektid = PROJEKT.projektid) soll,
    (select fixed (sum (stunden), 5,2) geleistet, ma_id, projektid
       from projzeit
       group by ma_id, projektid
     union
     select 0, ma_id, projektid from projekt_zuordnung where
        chr (ma_id) || chr (projektid) NOT IN
          (select chr(ma_id) || chr (projektid) from projzeit)
    ) geleistet
where 
soll.ma_id      = geleistet.ma_id      and
soll.projektid = geleistet.projektid and
soll > geleistet !
fetch !

drop table mitarbeiter !
drop table projekt !
drop table projekt_zuordnung!
drop table projzeit!
COMMIT RELEASE !
file connect ( kern test !
create table tahef 
           (ejj char(2), 
            aus fixed(5), 
            obj fixed(5),
            emm char(2), 
            jjj fixed(5), 
            hhh fixed(5)) !
create table taagn 
           (agn fixed(10), 
            vbe fixed(5), 
            tx1 char(30))!
create table tawer 
           (kdn fixed(10), 
            prn fixed(10), 
            ant fixed(11), 
            vol fixed(5), 
            aus fixed(5), 
            obj fixed(5), 
            agn fixed(10), 
            jjj fixed(5),  
            hhh fixed(5))!
create table takp1 
           (brh fixed(5), 
            bru fixed(5), 
            prn fixed(10),
            kdn fixed(10))!

commit work!
SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , k.vbe , p.brh ,
       p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , taagn  a , takp1  p  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
  k.vbe , p.brh , p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , takp1  p  
 WHERE  NOT  EXISTS  
   (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , 
 k.vbe ,   9999 ,     99 , s.ant , s.vol  FROM tahef  h , tawer  s , 
 taagn  k , taagn  a  
 WHERE  NOT  EXISTS  
  (SELECT  *  FROM takp1  p  
  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
   UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
   k.vbe ,   9999 ,     99 , s.ant , s.vol  
   FROM tahef  h , tawer  s , taagn  k  WHERE NOT  EXISTS  
  (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  AND  NOT  EXISTS 
  (SELECT  *  FROM takp1  p  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
  ORDER BY      4 ,      5 !

SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , k.vbe , p.brh ,
       p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , taagn  a , takp1  p  
 WHERE  (h.obj  =      4 )  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )
  AND  (k.vbe  =      2 )  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )
  AND  (s.aus  = h.aus )  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh ) 
  AND  (k.agn  = s.kdn )  AND  (a.agn  = s.agn )  AND  (p.prn  = s.prn ) 
  AND  (p.kdn  = s.kdn )  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
  k.vbe , p.brh , p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , takp1  p  
 WHERE  (h.obj  =    4 )
  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )  
  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus ) 
  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh )  AND  (p.prn  = s.prn ) 
  AND  (p.kdn  = s.kdn )  AND  NOT  EXISTS  
   (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , 
 k.vbe ,   9999 ,     99 , s.ant , s.vol  FROM tahef  h , tawer  s , 
 taagn  k , taagn  a  
 WHERE  (h.obj  =      4 )  AND  (h.ejj  = '92' )
  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )  AND  (k.agn  = s.kdn ) 
  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus )  AND  (s.jjj  = h.jjj ) 
  AND  (s.hhh  = h.hhh )  AND  (a.agn  = s.agn )  AND  NOT  EXISTS  
  (SELECT  *  FROM takp1  p  
  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
   UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
   k.vbe ,   9999 ,     99 , s.ant , s.vol  
   FROM tahef  h , tawer  s , taagn  k  WHERE  (h.obj  =      4 )  
  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )
  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus )
  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh )  AND  NOT  EXISTS  
  (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  AND  NOT  EXISTS 
  (SELECT  *  FROM takp1  p  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
  ORDER BY      4 ,      5 !



SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , k.vbe , p.brh ,
       p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , taagn  a , takp1  p  
 WHERE  (h.obj  =      4 )  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )
  AND  (k.vbe  =      2 )  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )
  AND  (s.aus  = h.aus )  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh ) 
  AND  (k.agn  = s.kdn )  AND  (a.agn  = s.agn )  AND  (p.prn  = s.prn ) 
  AND  (p.kdn  = s.kdn )  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
  k.vbe , p.brh , p.bru , s.ant , s.vol  
 FROM tahef  h , tawer  s , taagn  k , takp1  p  
 WHERE  (h.obj  =    4 )
  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )  
  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus ) 
  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh )  AND  (p.prn  = s.prn ) 
  AND  (p.kdn  = s.kdn )  AND  NOT  EXISTS  
   (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  
UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn , a.vbe , 
 k.vbe ,   9999 ,     99 , s.ant , s.vol  FROM tahef  h , tawer  s , 
 taagn  k , taagn  a  
 WHERE  (h.obj  =      4 )  AND  (h.ejj  = '92' )
  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )  AND  (k.agn  = s.kdn ) 
  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus )  AND  (s.jjj  = h.jjj ) 
  AND  (s.hhh  = h.hhh )  AND  (a.agn  = s.agn )  AND  NOT  EXISTS  
  (SELECT  *  FROM takp1  p  
  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
   UNION ALL SELECT h.ejj , h.aus , h.obj , s.kdn , s.prn ,     99 , 
   k.vbe ,   9999 ,     99 , s.ant , s.vol  
   FROM tahef  h , tawer  s , taagn  k  WHERE  (h.obj  =      4 )  
  AND  (h.ejj  = '92' )  AND  (h.emm  = '01' )  AND  (k.vbe  =      2 )
  AND  (k.agn  = s.kdn )  AND  (s.obj  = h.obj )  AND  (s.aus  = h.aus )
  AND  (s.jjj  = h.jjj )  AND  (s.hhh  = h.hhh )  AND  NOT  EXISTS  
  (SELECT  *  FROM taagn  a  WHERE a.agn  = s.agn )  AND  NOT  EXISTS 
  (SELECT  *  FROM takp1  p  WHERE p.kdn  = s.kdn  AND p.prn  = s.prn )
  ORDER BY      4 ,      5 !
drop table tahef  !
drop table taagn  !
drop table tawer  !
drop table takp1  !
COMMIT RELEASE !

file connect ( kern test !
create table fluege
 (flug_nr       fixed(3) key,
  ges_plaetze   fixed(3) not null,
  verf_plaetze  fixed(3) not null)!
insert into fluege values (1, 200, 5)!  
insert into fluege values (2, 300, 3)!  
insert into fluege values (3, 450, 4)!  
create table buchungen
 (id      fixed (5) key,
  flug_nr fixed(3) not null references fluege,
  name    char (20) not null)!
insert into buchungen values ( 3, 3, 'Albrecht')!
insert into buchungen values (10, 1, 'Baader')!
insert into buchungen values (11, 1, 'Chall')!
insert into buchungen values ( 4, 1, 'Hansen')!
insert into buchungen values ( 1, 1, 'Jonas')!
insert into buchungen values ( 6, 3, 'Kauffmann')!
insert into buchungen values ( 7, 2, 'Lindemann')!
insert into buchungen values ( 8, 1, 'Linke')!
insert into buchungen values ( 2, 2, 'Medinger')!
insert into buchungen values ( 5, 2, 'Weiss')!
insert into buchungen values ( 9, 2, 'Weyhausen')!
select a.flug_nr, a.name
  from buchungen a, fluege
 where a.flug_nr = fluege.flug_nr 
   and verf_plaetze >=
           (select count(*) 
              from buchungen
             where buchungen.id <= a.id
               and buchungen.flug_nr = a.flug_nr) 
 order by a.flug_nr, id !
fetch !
drop table fluege !
drop table buchungen !
commit work release !

file connect ( kern test !
create table rechnungen (renr fixed (4) key,
                         kdnr fixed (4) not null,
                       betrag fixed(9,2) not null)!
insert into rechnungen values (1, 1, 1000)!
insert into rechnungen values (2, 2, 1000)!
insert into rechnungen values (3, 3, 1000)!
insert into rechnungen values (4, 4, 1000)!
insert into rechnungen values (5, 2, 2000)!
insert into rechnungen values (6, 1, 3000)!
insert into rechnungen values (7, 2, 1000)!
select a.kdnr, a.renr, a.betrag betrag,
       a.betrag / sum (b.betrag) * 100 prozent
from rechnungen a, rechnungen b
group by a.kdnr, a.renr, a.betrag !
fetch !
select a.kdnr,
       float(sum (a.betrag) / count (distinct b.renr),18) betrag,
       float(sum (a.betrag) / count (distinct b.renr) /
      (sum (b.betrag) / count (distinct a.renr)) * 100,18) prozent
from rechnungen a, rechnungen b
group by a.kdnr !
fetch!
select a.kdnr,
       float(sum (a.betrag), 18),
       float(sum (a.betrag) / sum(b.sumbetrag) * 100, 18) prozent
from rechnungen a,
     (select sum (betrag) sumbetrag
      from rechnungen) b
group by a.kdnr !
fetch!
select a.kdnr,
       float(sum (a.betrag), 18),
       float(sum (a.betrag) / b.sumbetrag * 100, 18) prozent
from rechnungen a,
     (select sum (betrag) sumbetrag
      from rechnungen) b
group by a.kdnr, b.sumbetrag !
fetch!
select a.kdnr,
       fixed(sum (a.betrag), 10) betrag, 
       fixed(sum (a.betrag) / b.sumbetrag * 100, 3) prozent
from rechnungen a,
     (select sum (betrag) sumbetrag
      from rechnungen) b
group by a.kdnr, b.sumbetrag !
fetch!
drop table rechnungen!
commit work release !

file connect ( kern test sqlmode oracle !
create table lager_frei (
regal     number (3) not null,
frei_von  number (4) not null,
frei_bis  number (4) not null,
check (frei_von < frei_bis),
primary key (regal, frei_von)) !
insert into lager_frei values (1,  25,3767)!
insert into lager_frei values (1,4234,4965)!
insert into lager_frei values (1,5001,5010)!
insert into lager_frei values (2, 702, 711)!
insert into lager_frei values (1,8001,8029)!
insert into lager_frei values (2,   1,  23)!
insert into lager_frei values (3,  25, 234)!
insert into lager_frei values (2,  55, 600)!
insert into lager_frei values (2, 800,7000)!
insert into lager_frei values (3, 255, 900)!
insert into lager_frei values (1,   1,  23)!
insert into lager_frei values (3,1002,7000)!
insert into lager_frei values (3, 921, 943)!
commit work release!

file connect ( kern test sqlmode oracle !
create table tables
  (name         varchar (30) primary key,
   storage_size number) !
insert into tables values ('KONTO'  , 1000)!
insert into tables values ('ARTIKEL', 1000)!
commit !
create table indexes
  (name        varchar2 (30) primary key,
   table_name  varchar2 (30) not null references tables,
   unique_flag varchar2 ( 4) not null
                      check (unique_flag in ('YES', 'NO'))) !
insert into indexes values ('KT01', 'KONTO'  , 'YES')! 
insert into indexes values ('KT02', 'KONTO'  , 'YES')! 
insert into indexes values ('KT03', 'KONTO'  , 'YES')! 
insert into indexes values ('KT04', 'KONTO'  , 'YES')! 
insert into indexes values ('ART1', 'ARTIKEL', 'YES')! 
insert into indexes values ('ART2', 'ARTIKEL', 'YES')! 
insert into indexes values ('ART3', 'ARTIKEL', 'YES')! 
insert into indexes values ('ART4', 'ARTIKEL', 'YES')! 
commit !
create table index_columns
  (index_name      varchar2 (30) references indexes,
   column_name     varchar2 (30) not null,
   column_position number not null
                          check (column_position > 0),
   primary key (index_name, column_name),
   constraint c1_index_name unique (index_name, column_position)) !
insert into index_columns values ('KT01', 'COL1', 1)!
insert into index_columns values ('KT02', 'COL1', 1)!
insert into index_columns values ('KT02', 'COL2', 2)!
insert into index_columns values ('KT03', 'COL1', 1)!
insert into index_columns values ('KT03', 'COL2', 2)!
insert into index_columns values ('KT03', 'COL3', 3)!
insert into index_columns values ('KT04', 'COL3', 1)!
insert into index_columns values ('KT04', 'COL4', 2)!
insert into index_columns values ('ART1', 'COLA', 1)!
insert into index_columns values ('ART1', 'COLB', 2)!
insert into index_columns values ('ART2', 'COLB', 1)!
insert into index_columns values ('ART2', 'COLC', 2)!
insert into index_columns values ('ART3', 'COLC', 1)!
insert into index_columns values ('ART3', 'COLD', 2)!
insert into index_columns values ('ART4', 'COLC', 1)!
insert into index_columns values ('ART4', 'COLD', 2)!
insert into index_columns values ('ART4', 'COLE', 3)!
insert into index_columns values ('ART4', 'COLF', 4)!
commit !
select substr(i1.table_name, 1, 10), 
       substr(i1.name, 1, 10), 
       substr(i2.name, 1, 10)
  from indexes i1, indexes i2
 where i1.table_name = i2.table_name
   and i1.name <> i2.name
   and not exists
           (select column_name, column_position
              from index_columns c1
             where i2.name = c1.index_name
           minus
           select column_name, column_position
             from index_columns c2
            where i1.name = c2.index_name) !
fetch into :a, :b, :c!
select substr(i1.table_name, 1, 10), 
       substr(i1.name, 1, 10), 
       substr(i2.name, 1, 10)
  from indexes i1, indexes i2
 where i1.table_name = i2.table_name
   and i1.name <> i2.name
   and not exists
           (select *
              from index_columns ic1
             where ic1.index_name = i2.name
               and not exists
                       (select *
                          from index_columns ic2
                         where ic2.index_name = i1.name
                           and ic1.column_name = ic2.column_name)) !
fetch into :a, :b, :c!
select substr(i1.table_name, 1, 10), 
       substr(i2.name, 1, 10), 
       substr(i1.name, 1, 10)
  from indexes i1,
       indexes i2,
       index_columns ic1
 where i1.table_name = i2.table_name
   and i1.name = ic1.index_name
   and i1.name <> i2.name
   and ic1.column_position =
                (select count(*)
                   from index_columns
                  where index_name = i1.name)
   and ic1.column_position =
                (select count(*)
                   from index_columns ic2,
                        index_columns ic3
                  where ic2.index_name = i1.name
                    and ic3.index_name = i2.name
                    and ic2.column_name = ic3.column_name
                    and ic2.column_position = ic3.column_position) !
fetch into :a, :b, :c!
drop table tables !
drop table indexes !
drop table index_columns !
commit release !

file connect ( kern test sqlmode oracle !
create table buecher
   (buch_nr number primary key,
    titel   varchar (30) not null,
    autor   varchar (30) not null)!
insert into buecher values (1, 'ORACLE7'         , 'G. Stuerner')!
insert into buecher values (2, 'INFORMIX 6.0/7.1', 'D. Petkovic')!
insert into buecher values (3, 'SYBASE'          , 'D. McGoveran')!
commit !
create table leser
   (leser_nr number primary key,
    name     varchar (30) not null) !
insert into leser values (1, 'H. Kohl')!
insert into leser values (2, 'B. Clinton')!
insert into leser values (3, 'B. Jelzin')!
commit !
create table ausleihe
   (buch_nr  number references buecher,
    leser_nr number references leser,
    ein_aus  char (3) not null 
                 check (ein_aus in ('EIN', 'AUS')),
    datum    date not null,
    primary key (buch_nr, datum))!
commit release !
file connect ( kern test !
insert into ausleihe values (3, 3, 'AUS', '19950609000000')!
insert into ausleihe values (2, 2, 'AUS', '19950611000000')!
insert into ausleihe values (1, 1, 'AUS', '19950612000000')!
insert into ausleihe values (3, 3, 'EIN', '19950613000000')!
insert into ausleihe values (1, 1, 'EIN', '19950615000000')!
insert into ausleihe values (1, 2, 'AUS', '19950617000000')!
insert into ausleihe values (2, 2, 'EIN', '19950618000000')!
insert into ausleihe values (2, 3, 'AUS', '19950619000000')!
insert into ausleihe values (3, 1, 'AUS', '19950619000000')!
insert into ausleihe values (1, 2, 'EIN', '19950622000000')!
insert into ausleihe values (2, 3, 'EIN', '19950627000000')!
insert into ausleihe values (3, 1, 'EIN', '19950630000000')!
commit !
select titel, name, aus.ein_aus, ein.ein_aus, 
      datediff(ein.datum, aus.datum) dauer
  from buecher, leser, ausleihe aus, ausleihe ein
 where buecher.buch_nr = aus.buch_nr
   and leser.leser_nr  = aus.leser_nr
   and aus.buch_nr     = ein.buch_nr
   and aus.leser_nr    = ein.leser_nr
   and aus.ein_aus     = 'AUS'
   and ein.ein_aus     = 'EIN'
   and ein.datum = (select min (datum) 
                      from ausleihe
                     where ausleihe.ein_aus  = 'EIN'
                       and ausleihe.buch_nr  = aus.buch_nr
                       and ausleihe.leser_nr = aus.leser_nr
                       and ausleihe.datum    > aus.datum) !
fetch!
drop table buecher !
drop table leser !
drop table ausleihe !
commit release!

file connect ( kern test sqlmode internal !
create table artikel (
nummer      integer primary key,
bezeichnung varchar(30))!
insert into artikel values (1, 'Bier')!
insert into artikel values (2, 'Wein')!
create table preis (
nummer      integer,
start_datum date,
preis       fixed(10,2) not null,
primary key (nummer, start_datum),
foreign key (nummer) references artikel)!
insert into preis values (1, '19950101', 1.20)!
insert into preis values (1, '19950201', 1.30)!
insert into preis values (1, '19950301', 1.50)!
insert into preis values (1, '19950601', 1.90)!
insert into preis values (2, '19950101', 1.90)!
insert into preis values (2, '19950401', 1.50)!
insert into preis values (2, '19951101', 2.10)!
create table verkauf (
nummer      integer,
datum       date,
anzahl      integer not null,
primary key (nummer, datum),
foreign key (nummer) references artikel)!
insert into verkauf values (1, '19950120',  20)!
insert into verkauf values (1, '19950311',  20)!
insert into verkauf values (2, '19950514', 345)!
insert into verkauf values (1, '19950811',  55)!
insert into verkauf values (2, '19950214',  55)!
insert into verkauf values (2, '19951014', 345)!
select a.bezeichnung,
       fixed(sum(p.preis * v.anzahl), 8,2) summe
  from artikel a, verkauf v, preis p
 where a.nummer = v.nummer
   and v.nummer = p.nummer
   and p.start_datum =
                     (select max (pr.start_datum)
                        from preis pr
                       where pr.nummer = v.nummer
                         and pr.start_datum <= v.datum)
group by a.bezeichnung !
fetch into :a, :b !
select a.bezeichnung,
       fixed(sum(p.preis * v.anzahl), 8,2) summe
  from artikel a, verkauf v, preis p
 where a.nummer = v.nummer
   and v.nummer = p.nummer
   and p.start_datum <= v.datum
   and not exists (select 1
                     from preis pr
                    where pr.nummer = v.nummer
                      and pr.start_datum > p.start_datum
                      and pr.start_datum <= v.datum)
group by a.bezeichnung !
fetch into :a, :b !
drop table artikel !
drop table preis !
drop table verkauf !
commit work release !

file connect ( kern test sqlmode internal !
create table personal (
id   integer primary key,
name varchar (10))!
insert into personal values (1, 'Ofer')!
insert into personal values (2, 'Hansen')!
insert into personal values (3, 'Meyer')!
insert into personal values (4, 'Albrecht')!
insert into personal values (5, 'Linke')!
create table rente (
id     integer references personal,
jahr   integer,
monate integer not null check (monate between 1 and 12),
lohn   integer not null,
primary key (id, jahr))!
insert into rente values (1, 1989,  2, 1500)!
insert into rente values (2, 1989, 12, 2200)!
insert into rente values (4, 1989,  3, 1200)!
insert into rente values (3, 1989,  2, 1700)!
insert into rente values (2, 1990, 12, 2300)!
insert into rente values (3, 1990,  2, 1900)!
insert into rente values (4, 1990,  3, 1400)!
insert into rente values (2, 1991, 12, 2400)!
insert into rente values (1, 1992,  2, 2100)!
insert into rente values (4, 1992,  3, 1900)!
insert into rente values (3, 1992,  2, 2100)!
insert into rente values (2, 1992, 12, 2500)!
insert into rente values (1, 1993,  2, 2200)!
insert into rente values (4, 1993,  3, 2100)!
insert into rente values (5, 1993,  6, 3000)!
insert into rente values (2, 1993, 12, 2600)!
insert into rente values (1, 1994,  2, 2300)!
insert into rente values (2, 1994, 12, 2700)!
insert into rente values (3, 1994,  2, 2300)!
select p.name, fixed(min(r1.jahr),4,0) beg,
       fixed(max(r1.jahr),4,0) ende,
       fixed(sum(r1.monate),4,0) sum_monate,
       fixed(sum(r1.lohn * r1.monate) / sum(r1.monate), 6,0) avg_lohn
  from rente r1, personal p
 where r1.id = p.id
   and r1.jahr >= (select min (r2.jahr)
                     from rente r2
                    where r1.id = r2.id
                      and exists
                               (select count(*)
                                  from rente r3
                                 where r1.id = r3.id
                                   and r2.jahr <= r3.jahr
                                group by r3.id
                                having count(r3.jahr) = 
                                     max(r3.jahr) - r2.jahr + 1
                                   and sum(r3.monate) >= 6)
                 ) 
group by p.name !
fetch into :a, :b, :c, :d, :e !
select p.name, fixed(min(r1.jahr),4,0) beg,
       fixed(max(r1.jahr),4,0) ende,
       fixed(sum(r1.monate),4,0) sum_monate,
       fixed(sum(r1.lohn * r1.monate) / sum(r1.monate), 6,0) avg_lohn
  from rente r1, personal p
 where r1.id = p.id
   and (  r1.jahr in (select r2.jahr + 1
                        from rente r2
                       where r1.id = r2.id)
        or 
          r1.jahr in (select r3.jahr - 1
                        from rente r3
                       where r1.id = r3.id)
        or
          r1.monate >= 6)
group by p.name, r1.id
having sum(monate) >= 6 !
fetch into :a, :b, :c, :d, :e !
drop table rente !
drop table personal !
commit work release !

file connect ( kern test sqlmode internal !
create table artikel (
artikel_nr  integer primary key,
bezeichnung varchar(12) not null)!
insert into artikel values (1, 'Brot')!
insert into artikel values (2, 'Fisch')!
insert into artikel values (3, 'Nudeln')!
insert into artikel values (4, 'Milch')!
insert into artikel values (5, 'Tomatensosse')!
create table scanner_header (
bon_id  integer primary key,
datum   date not null) !
insert into scanner_header values (1, '19951220')!
insert into scanner_header values (2, '19951220')!
insert into scanner_header values (3, '19951220')!
insert into scanner_header values (4, '19951220')!
insert into scanner_header values (5, '19951220')!
insert into scanner_header values (6, '19951220')!
insert into scanner_header values (7, '19951220')!
insert into scanner_header values (8, '19951220')!
create table scanner_line (
bon_id     integer
           constraint scanner_header_line_ref references scanner_header,
zeile      integer,
artikel_nr integer not null
           constraint scanner_artikel_ref references artikel,
menge      integer not null
           constraint mengen_check check (menge > 0),
constraint scanner_key primary key (bon_id, zeile))!
insert into scanner_line values (1,1,1,1)!
insert into scanner_line values (1,2,2,1)!
insert into scanner_line values (1,3,3,1)!
insert into scanner_line values (1,4,4,1)!
insert into scanner_line values (1,5,5,1)!
insert into scanner_line values (2,1,2,1)!
insert into scanner_line values (2,2,4,2)!
insert into scanner_line values (3,1,1,4)!
insert into scanner_line values (3,2,3,3)!
insert into scanner_line values (3,3,5,1)!
insert into scanner_line values (4,1,1,1)!
insert into scanner_line values (4,2,3,2)!
insert into scanner_line values (4,3,4,2)!
insert into scanner_line values (5,1,2,1)!
insert into scanner_line values (5,2,4,2)!
insert into scanner_line values (5,3,5,2)!
insert into scanner_line values (6,1,3,2)!
insert into scanner_line values (6,2,5,1)!
insert into scanner_line values (7,1,1,1)!
insert into scanner_line values (8,1,3,2)!
insert into scanner_line values (8,2,5,3)!
SELECT EINS.BEZEICHNUNG, ZWEI.BEZEICHNUNG 
  FROM ARTIKEL EINS, ARTIKEL ZWEI, 
       SCANNER_LINE SC1,  SCANNER_LINE SC2 
 WHERE SC1.BON_ID = SC2.BON_ID 
   AND SC1.ARTIKEL_NR  = EINS.ARTIKEL_NR 
   AND SC2.ARTIKEL_NR  = ZWEI.ARTIKEL_NR 
GROUP BY EINS.BEZEICHNUNG, EINS.ARTIKEL_NR, 
         ZWEI.BEZEICHNUNG, ZWEI.ARTIKEL_NR 
HAVING COUNT(EINS.ARTIKEL_NR) >= 
           (SELECT 0.5 * COUNT(*) 
              FROM SCANNER_LINE SC 
             WHERE SC.ARTIKEL_NR = EINS.ARTIKEL_NR) 
    OR COUNT(ZWEI.ARTIKEL_NR) >= 
           (SELECT 0.5 * COUNT(*) 
              FROM SCANNER_LINE SC 
             WHERE SC.ARTIKEL_NR = ZWEI.ARTIKEL_NR)
!
fetch into :a, :b !
SELECT EINS.BEZEICHNUNG, ZWEI.BEZEICHNUNG 
  FROM ARTIKEL EINS, ARTIKEL ZWEI, 
       SCANNER_LINE SC1,  SCANNER_LINE SC2 
 WHERE SC1.BON_ID = SC2.BON_ID 
   AND SC1.ARTIKEL_NR  = EINS.ARTIKEL_NR 
   AND SC2.ARTIKEL_NR  = ZWEI.ARTIKEL_NR 
   AND EINS.ARTIKEL_NR < ZWEI.ARTIKEL_NR 
GROUP BY EINS.BEZEICHNUNG, EINS.ARTIKEL_NR, 
         ZWEI.BEZEICHNUNG, ZWEI.ARTIKEL_NR 
HAVING COUNT(*) > 
           (SELECT 0.5 * COUNT(distinct sc.bon_id) 
              FROM SCANNER_LINE SC 
             WHERE SC.ARTIKEL_NR in (EINS.ARTIKEL_NR, ZWEI.ARTIKEL_NR))
!
fetch into :a, :b !
SELECT EINS.BEZEICHNUNG, ZWEI.BEZEICHNUNG 
  FROM ARTIKEL EINS, ARTIKEL ZWEI, 
       SCANNER_LINE SC1,  SCANNER_LINE SC2 
 WHERE SC1.BON_ID = SC2.BON_ID 
   AND SC1.ARTIKEL_NR  = EINS.ARTIKEL_NR 
   AND SC2.ARTIKEL_NR  = ZWEI.ARTIKEL_NR 
   AND EINS.ARTIKEL_NR < ZWEI.ARTIKEL_NR 
GROUP BY EINS.BEZEICHNUNG, EINS.ARTIKEL_NR, 
         ZWEI.BEZEICHNUNG, ZWEI.ARTIKEL_NR 
HAVING COUNT(*) * 3 > 
           (SELECT COUNT(sc.bon_id) 
              FROM SCANNER_LINE SC 
             WHERE SC.ARTIKEL_NR in (EINS.ARTIKEL_NR, ZWEI.ARTIKEL_NR))
!
fetch into :a, :b !
drop table artikel !
drop table scanner_header !
drop table scanner_line !
commit work release !

file connect ( kern test sqlmode internal !
create table zaehlungen (
zaehlungs_id   integer,
tier           varchar (30),
anzahl         integer not null,
primary key (zaehlungs_id, tier))!
insert into zaehlungen values (1, 'Frosch    ', 12)!
insert into zaehlungen values (1, 'Kaulquappe', 16)!
insert into zaehlungen values (1, 'Lurch     ', 10)!
insert into zaehlungen values (2, 'Frosch    ',  2)!
insert into zaehlungen values (2, 'Lurch     ',  1)!
insert into zaehlungen values (2, 'Kroete    ',  3)!
insert into zaehlungen values (3, 'Frosch    ',  3)!
create table zaehlungsgruppen (
gruppen_id    integer,
zaehlungs_id  integer,
primary key (gruppen_id, zaehlungs_id))!
insert into zaehlungsgruppen values (1,1)!
insert into zaehlungsgruppen values (1,2)!
insert into zaehlungsgruppen values (2,1)!
insert into zaehlungsgruppen values (2,3)!
insert into zaehlungsgruppen values (3,3)!
select tier, fixed(sum(anzahl) / x.nr, 4,1)
  from (select count(gruppen_id) nr 
          from zaehlungsgruppen
         where gruppen_id = 1) x,
       zaehlungen z,
       zaehlungsgruppen zg
 where zg.gruppen_id = 1
   and zg.zaehlungs_id = z.zaehlungs_id
group by tier, x.nr !
fetch into :a, :b!
drop table zaehlungen !
drop table zaehlungsgruppen !
commit work release !

file connect ( kern test sqlmode internal !
create table patienten (
id      integer primary key,
name    varchar (30))!
insert into patienten values (1, 'Albrecht')!
insert into patienten values (2, 'Ladewig')!
insert into patienten values (3, 'Weiss')!
insert into patienten values (4, 'Suhr')!
insert into patienten values (5, 'Medinger')!
insert into patienten values (6, 'Linke')!
create table messungen (
id    integer references patienten,
datum date,
wert  fixed(3,1) not null,
primary key (id, datum))!
insert into messungen values (1, '19960208', 38.4)!
insert into messungen values (2, '19960208', 36.7)!
insert into messungen values (3, '19960208', 37.5)!
insert into messungen values (6, '19960208', 40.7)!
insert into messungen values (5, '19960208', 39.5)!
insert into messungen values (4, '19960208', 40.8)!
insert into messungen values (1, '19960209', 40.6)!
insert into messungen values (6, '19960209', 37.3)!
insert into messungen values (5, '19960209', 41.8)!
insert into messungen values (4, '19960209', 38.7)!
insert into messungen values (3, '19960209', 42.2)!
insert into messungen values (2, '19960209', 38.1)!
insert into messungen values (1, '19960210', 40.5)!
insert into messungen values (6, '19960210', 38.1)!
insert into messungen values (5, '19960210', 37.5)!
insert into messungen values (4, '19960210', 41.7)!
insert into messungen values (3, '19960210', 36.5)!
insert into messungen values (2, '19960210', 36.1)!
insert into messungen values (1, '19960211', 38.1)!
insert into messungen values (4, '19960211', 42.3)!
insert into messungen values (3, '19960211', 41.7)!
insert into messungen values (6, '19960211', 36.0)!
insert into messungen values (5, '19960211', 40.8)!
insert into messungen values (2, '19960211', 37.8)!
select name, fixed(avg(m.wert), 5, 3) avg_wert, mw.wert
  from (select wert, id, datum from messungen me1
         where datum = (select max(datum) 
                          from messungen me2
                         where me2.id = me1.id)
       ) mw,
                     patienten p, messungen m
 where p.id  = m.id
   and mw.id = m.id
group by name, mw.wert
having mw.wert > avg(m.wert) !
fetch into :a, :b, :c !
select p.name, avg (m1.wert) avg_wert, m2.wert
  from patienten p, messungen m1, messungen m2
 where p.id = m1.id
   and p.id = m2.id
   and m2.datum = (select max(mi.datum)
                     from messungen mi
                    where mi.id = m2.id)
group by p.name, m2.wert
having m2.wert > avg (m1.wert)!
fetch into :a, :b, :c !
select p.name, mess.avg_wert, m.wert
  from patienten p, messungen m,
       (select mav.id, avg(mav.wert) avg_wert,
                       max(mav.datum) max_datum
          from messungen mav
        group by mav.id) mess
 where p.id = m.id
   and p.id = mess.id
   and m.datum = mess.max_datum
   and m.wert > mess.avg_wert !
fetch into :a, :b, :c !
drop table patienten !
drop table messungen!
commit work release !

file connect ( kern test sqlmode oracle !
create table flugplan
 (flug_nr varchar(10) constraint flugplan_pk primary key,
  ab_ort  varchar(30) not null,
  an_ort  varchar(30) not null)!
insert into flugplan values ('IT001', 'Hamburg  ', 'Berlin   ')!
insert into flugplan values ('IT002', 'Berlin   ', 'Hamburg  ')!
insert into flugplan values ('IT003', 'Hamburg  ', 'Berlin   ')!
insert into flugplan values ('IT004', 'Berlin   ', 'Hamburg  ')!
insert into flugplan values ('IT005', 'Hamburg  ', 'Bremen   ')!
insert into flugplan values ('IT006', 'Bremen   ', 'Hamburg  ')!
insert into flugplan values ('IT007', 'Bremen   ', 'Berlin   ')!
insert into flugplan values ('IT008', 'Berlin   ', 'Bremen   ')!
insert into flugplan values ('IT009', 'Bremen   ', 'Frankfurt')!
insert into flugplan values ('IT010', 'Frankfurt', 'Bremen   ')!
create table flug
 (flug_nr  varchar(10) constraint flugplan_fk references flugplan,
  datum    date,
  personen number not null,
      constraint flug_pk primary key (flug_nr, datum)) !
insert into flug values ('IT001', '20-APR-96',  60)!
insert into flug values ('IT002', '20-APR-96',  72)!
insert into flug values ('IT003', '20-APR-96',  37)!
insert into flug values ('IT004', '20-APR-96',  61)!
insert into flug values ('IT005', '20-APR-96',  38)!
insert into flug values ('IT006', '20-APR-96',  21)!
insert into flug values ('IT007', '20-APR-96',  63)!
insert into flug values ('IT008', '20-APR-96',  58)!
insert into flug values ('IT009', '20-APR-96',  59)!
insert into flug values ('IT010', '20-APR-96',  66)!
insert into flug values ('IT001', '21-APR-96',  55)!
insert into flug values ('IT002', '21-APR-96',  26)!
insert into flug values ('IT003', '21-APR-96',  68)!
insert into flug values ('IT004', '21-APR-96',  67)!
insert into flug values ('IT005', '21-APR-96',  23)!
insert into flug values ('IT006', '21-APR-96',  50)!
insert into flug values ('IT007', '21-APR-96',  34)!
insert into flug values ('IT008', '21-APR-96',  32)!
insert into flug values ('IT009', '21-APR-96',  44)!
insert into flug values ('IT010', '21-APR-96',  70)!
insert into flug values ('IT001', '22-APR-96',  47)!
insert into flug values ('IT002', '22-APR-96',  91)!
insert into flug values ('IT003', '22-APR-96',  34)!
insert into flug values ('IT004', '22-APR-96', 122)!
insert into flug values ('IT005', '22-APR-96', 109)!
insert into flug values ('IT006', '22-APR-96', 118)!
insert into flug values ('IT007', '22-APR-96',  26)!
insert into flug values ('IT008', '22-APR-96',  79)!
insert into flug values ('IT009', '22-APR-96',  31)!
insert into flug values ('IT010', '22-APR-96', 128)!
insert into flug values ('IT001', '23-APR-96',  82)!
insert into flug values ('IT002', '23-APR-96',  63)!
insert into flug values ('IT003', '23-APR-96',  70)!
insert into flug values ('IT004', '23-APR-96',  32)!
insert into flug values ('IT005', '23-APR-96',  91)!
insert into flug values ('IT006', '23-APR-96',  34)!
insert into flug values ('IT007', '23-APR-96',  74)!
insert into flug values ('IT008', '23-APR-96',  69)!
insert into flug values ('IT009', '23-APR-96',  90)!
insert into flug values ('IT010', '23-APR-96',  66)!
select substr(ab_ort, 1, 10), datum, sum(personen) summe_s
  from flugplan fp, flug f
 where fp.flug_nr = f.flug_nr
group by ab_ort, datum
having (ab_ort, sum(personen)) = any
   (select ab_ort, min(sum_p) 
      from 
          (select ab_ort, sum(personen) sum_p
             from flugplan fp1, flug f1
            where fp1.flug_nr = f1.flug_nr
           group by ab_ort, datum) s
      group by ab_ort
   )!
fetch into :a, :b, :c !
select substr(ab_ort, 1, 10), datum, sum(personen) summe_s
  from flugplan fp, flug f
 where fp.flug_nr = f.flug_nr
group by ab_ort, datum
having (ab_ort, sum(personen)) in
   (select ab_ort, min(sum_p) 
      from 
          (select ab_ort, sum(personen) sum_p
             from flugplan fp1, flug f1
            where fp1.flug_nr = f1.flug_nr
           group by ab_ort, datum) s
      group by ab_ort
   )!
fetch into :a, :b, :c !
commit work release !

file connect ( kern test sqlmode ansi !
declare c cursor for
select ab_ort, datum, sum(personen) summe_s
  from flugplan fp, flug f
 where fp.flug_nr = f.flug_nr
group by ab_ort, datum
having sum(personen) <= all
   (select sum(fi.personen) 
      from flugplan fpi, flug fi
     where fpi.flug_nr = fi.flug_nr
       and fpi.ab_ort = min(fp.ab_ort)
    group by datum )
   !
fetch c into :a, :b, :c !
commit work release !
file connect ( kern test sqlmode oracle !
declare c cursor for
select ab_ort, datum, sum(personen) summe_s
  from flugplan fp, flug f
 where fp.flug_nr = f.flug_nr
group by ab_ort, datum
having sum(personen) <= all
   (select sum(fi.personen) 
      from flugplan fpi, flug fi
     where fpi.flug_nr = fi.flug_nr
       and fpi.ab_ort = fp.ab_ort
    group by datum )
   !
fetch c into :a, :b, :c !
declare c cursor for
select substr(ab_ort, 1, 10), datum, sum(personen) summe_s
  from flugplan fp, flug f
 where fp.flug_nr = f.flug_nr
group by ab_ort, datum
having sum(personen) <= all
   (select sum(fi.personen) 
      from flugplan fpi, flug fi
     where fpi.flug_nr = fi.flug_nr
       and fpi.ab_ort = fp.ab_ort
    group by datum )
   !
fetch c into :a, :b, :c !
drop table flugplan!
drop table flug !
commit work release !

file connect ( kern test sqlmode oracle !
create table abteilungen (
abt_nr      number  ( 3) constraint abteilungen_pk primary key,
bezeichnung varchar (20) not null) !
insert into abteilungen values (10, 'EDV      ')!
insert into abteilungen values (20, 'Vertrieb ')!
insert into abteilungen values (30, 'Marketing')!
insert into abteilungen values (40, 'Personal ')!
create table angestellte (
ma_nr       number   (3) constraint angestellte_pk primary key,
name        varchar (30) not null,
position    varchar (30),
abt_nr      number  ( 3) constraint angestellte_fk_abt
                               references abteilungen) !
insert into angestellte values ( 1, 'Moeller', 'Systemanalytiker', 10)!
insert into angestellte values ( 2, 'Meyer  ', 'EDV-Spezialist  ', 10)!
insert into angestellte values ( 5, 'Chall  ', 'Verkaeufer      ', 20)!
insert into angestellte values ( 6, 'Kunze  ', null              , 20)!
insert into angestellte values ( 7, 'Peter  ', 'Verkaeuferin    ', 20)!
insert into angestellte values ( 8, 'Hagner ', 'EDV-Spezialist  ',null)!
insert into angestellte values ( 9, 'Hansen ', 'Berater         ', 30)!
insert into angestellte values (10, 'Klasen ', 'EDV-Spezialist  ', 30)!
insert into angestellte values (12, 'Bauer  ', 'Sachbearbeiter  ', 40)!
create table kurse (
kurs_nr     number  ( 3) constraint kurs_pk primary key,
bezeichnung varchar (20) not null) !
insert into kurse values (10, 'Rhetorik')!
insert into kurse values (20, 'SQL     ')!
create table kursteilnehmer (
kurs_nr     number  ( 3) constraint ktn_fk_kurs
                                 references kurse,
ma_nr       number  ( 3) constraint ktn_fk_ma
                                 references angestellte,
datum       date,
constraint kursteilnehmer_pk primary key (kurs_nr, ma_nr, datum)) !
insert into kursteilnehmer values (10,  1, '20-NOV-95')!
insert into kursteilnehmer values (10,  5, '20-OCT-95')!
insert into kursteilnehmer values (10,  6, '20-DEC-95')!
insert into kursteilnehmer values (10, 10, '20-SEP-95')!
insert into kursteilnehmer values (10,  9, '20-NOV-95')!
insert into kursteilnehmer values (20,  2, '10-MAY-95')!
insert into kursteilnehmer values (20,  8, '11-JUL-95')!
insert into kursteilnehmer values (20,  6, '02-FEB-96')!
insert into kursteilnehmer values (20,  7, '20-APR-96')!
select bezeichnung
  from abteilungen 
 where abt_nr not in
          (select abt_nr
             from kursteilnehmer kt,
                  kurse k,
                  angestellte a
            where k.bezeichnung = 'SQL'
              and k.kurs_nr     = kt.kurs_nr
              and kt.ma_nr      = a.ma_nr
              and a.abt_nr is not null
          ) !
fetch into :a !
select bezeichnung
  from abteilungen 
 where not exists 
          (select 'a'
             from kursteilnehmer kt,
                  kurse k,
                  angestellte a
            where k.bezeichnung = 'SQL'
              and k.kurs_nr     = kt.kurs_nr
              and kt.ma_nr      = a.ma_nr 
              and a.abt_nr      = abteilungen.abt_nr )!
fetch into :a !
drop table abteilungen !
drop table angestellte !
drop table kurse !
drop table kursteilnehmer !
commit work release !

file connect ( kern test !
create table personal (
pers_no fixed (3) primary key,
name varchar (10) not null)!
insert into personal values (1, 'Gehner')!
insert into personal values (2, 'Jonas')!
insert into personal values (3, 'Hansen')!
insert into personal values (4, 'Clausen')!
create table praemie (
pr_id        fixed (3) primary key,
beschreibung varchar (10) not null,
wert         fixed (10) not null)!
insert into praemie values (2, 'Restaurant',   200)!
insert into praemie values (3, '1100 DM   ',  1100)!
insert into praemie values (4, '3100 DM   ',  3100)!
insert into praemie values (5, '1000 DM   ',  1000)!
insert into praemie values (6, '3000 DM   ',  3000)!
insert into praemie values (1, 'Reise     ', 10000)!
create table ziel (
ziel_id      fixed (3) primary key,
beschreibung varchar(10) not null)!
insert into ziel values (1, '80% Umsatz')!
insert into ziel values (2, '90% Umsatz')!
insert into ziel values (3, '100% Ums. ')!
insert into ziel values (4, '120% Ums. ')!
insert into ziel values (5, '5 neue Kd.')!
insert into ziel values (6, '10 neu Kd.')!
insert into ziel values (7, 'Kurs 1    ')!
insert into ziel values (8, 'Kurs 2    ')!
insert into ziel values (9, 'Gr. Auftr.')!
commit !
create table erreichtes_ziel (
pers_no    fixed (3) references personal,
ziel_id    fixed (3) references ziel,
primary key (pers_no, ziel_id))!
insert into erreichtes_ziel values (4, 1)!
insert into erreichtes_ziel values (3, 1)!
insert into erreichtes_ziel values (2, 1)!
insert into erreichtes_ziel values (1, 1)!
insert into erreichtes_ziel values (4, 2)!
insert into erreichtes_ziel values (3, 2)!
insert into erreichtes_ziel values (2, 2)!
insert into erreichtes_ziel values (1, 2)!
insert into erreichtes_ziel values (4, 3)!
insert into erreichtes_ziel values (3, 3)!
insert into erreichtes_ziel values (1, 3)!
insert into erreichtes_ziel values (1, 4)!
insert into erreichtes_ziel values (4, 5)!
insert into erreichtes_ziel values (3, 5)!
insert into erreichtes_ziel values (2, 5)!
insert into erreichtes_ziel values (1, 5)!
insert into erreichtes_ziel values (2, 6)!
insert into erreichtes_ziel values (1, 6)!
insert into erreichtes_ziel values (4, 7)!
insert into erreichtes_ziel values (2, 7)!
insert into erreichtes_ziel values (1, 7)!
insert into erreichtes_ziel values (3, 8)!
insert into erreichtes_ziel values (1, 8)!
insert into erreichtes_ziel values (4, 9)!
insert into erreichtes_ziel values (1, 9)!
create table notwendiges_ziel (
pr_id     fixed (3) references praemie,
ziel_id   fixed (3) references ziel,
primary key (pr_id, ziel_id))!
insert notwendiges_ziel values (2, 1)!
insert notwendiges_ziel values (2, 5)!
insert notwendiges_ziel values (2, 7)!
insert notwendiges_ziel values (3, 2)!
insert notwendiges_ziel values (3, 5)!
insert notwendiges_ziel values (3, 7)!
insert notwendiges_ziel values (5, 1)!
insert notwendiges_ziel values (5, 6)!
insert notwendiges_ziel values (5, 8)!
insert notwendiges_ziel values (4, 3)!
insert notwendiges_ziel values (4, 5)!
insert notwendiges_ziel values (4, 7)!
insert notwendiges_ziel values (6, 2)!
insert notwendiges_ziel values (6, 6)!
insert notwendiges_ziel values (6, 8)!
insert notwendiges_ziel values (6, 9)!
insert notwendiges_ziel values (1, 4)!
insert notwendiges_ziel values (1, 6)!
insert notwendiges_ziel values (1, 8)!
insert notwendiges_ziel values (1, 9)!
commit !
select name, beschreibung, wert 
from personal, praemie
where wert = 
       (select max(wert)
         from praemie
        where pr_id in 
                  (select pr_id 
                     from notwendiges_ziel
                    group by pr_id
                   having (pr_id, count(*)) in
                               (select pr_id, count(*)
                                  from notwendiges_ziel nz,
                                       erreichtes_ziel ez
                                 where nz.ziel_id = ez.ziel_id
                                   and ez.pers_no = personal.pers_no
                                 group by pr_id
                               )
                  )
       ) 
order by wert !
fetch into :a, :b, :c !
select name, beschreibung, wert 
from personal, praemie
where wert = 
       (select max(wert)
         from praemie
        where pr_id in 
                  (select pr_id 
                     from notwendiges_ziel
                    group by pr_id
                   having (pr_id, count(*)) in
                               (select pr_id, count(*)
                                  from notwendiges_ziel nz,
                                       erreichtes_ziel ez
                                 where nz.ziel_id = ez.ziel_id
                                   and ez.pers_no = personal.pers_no
                                 group by pr_id
                               )
                  )
       ) 
order by 3 !
fetch into :a, :b, :c !
select pe1.name,
       pr1.beschreibung,
       pr1.wert
  from personal pe1,
       praemie pr1
 where pr1.wert in
             (select max (pr2.wert)
                from praemie pr2
               where pr2.pr_id not in
                      (select nz1.pr_id
                         from notwendiges_ziel nz1
                        where nz1.ziel_id not in
                              (select ez1.ziel_id
                                 from erreichtes_ziel ez1
                                where ez1.pers_no = pe1.pers_no
                              )
                       )
             )!
fetch into :a, :b, :c !
drop table personal !
drop table praemie !
drop table ziel !
drop table erreichtes_ziel !
drop table notwendiges_ziel !
commit release !

file connect ( kern test sqlmode oracle!
create table kunden (
kdnr number (6,0) constraint kunden_pk primary key,
name varchar (30))!
insert into kunden values (1, 'Meyer    ')!
insert into kunden values (2, 'Schmidt  ')!
insert into kunden values (3, 'Clausen  ')!
insert into kunden values (4, 'Fischer  ')!
insert into kunden values (5, 'Feddersen')!
create table ausgaben (
titel varchar (30) constraint ausgaben_pk primary key,
datum date not null)!
insert into ausgaben values ('7/96', '05-JUL-96')!
insert into ausgaben values ('8/96', '05-AUG-96')!
insert into ausgaben values ('9/96', '05-SEP-96')!
create table anzeigen (
kdnr    number (6,0) constraint anz_kd_fk references kunden,
titel   varchar (30) constraint anz_ausg_fk references ausgaben,
seite   number (3,0),
groesse number (3,2) constraint groesse_check
        check (groesse > 0 and groesse <= 1),
color   char (1) constraint color_check check (color in ('J','N')),
constraint anzeigen_pk primary key (kdnr, titel, seite))!
insert into anzeigen values (1, '7/96', 15, 0.50, 'J')!
insert into anzeigen values (2, '7/96', 22, 0.50, 'J')!
insert into anzeigen values (5, '7/96', 33, 1.00, 'N')!
insert into anzeigen values (3, '7/96', 41, 1.00, 'N')!
insert into anzeigen values (4, '7/96', 45, 1.00, 'N')!
insert into anzeigen values (4, '7/96', 46, 1.00, 'N')!
insert into anzeigen values (1, '8/96', 15, 0.50, 'J')!
insert into anzeigen values (2, '8/96', 22, 0.50, 'J')!
insert into anzeigen values (5, '8/96', 33, 1.00, 'N')!
insert into anzeigen values (1, '9/96', 15, 0.50, 'J')!
insert into anzeigen values (2, '9/96', 22, 1.00, 'N')!
insert into anzeigen values (5, '9/96', 33, 1.00, 'N')!
insert into anzeigen values (3, '9/96', 41, 1.00, 'J')!
select kd.name
  from kunden kd, anzeigen az
 where az.kdnr = kd.kdnr
group by kd.name
having sum (az.groesse * decode (az.color, 'J', 2, 1)) >= 3 !
fetch into :a !
select kd.name
  from kunden kd
 where exists
      (select sum (az_all.groesse),
              sum (az_col.groesse)
         from anzeigen az_all, anzeigen az_col
        where kd.kdnr        = az_all.kdnr
          and az_all.kdnr    = az_col.kdnr(+)
          and az_all.titel   = az_col.titel(+)
          and az_all.seite   = az_col.seite(+)
          and az_col.color   = 'J'
        having sum (az_all.groesse) + 
               sum (az_col.groesse) >= 3.0
            or sum (az_all.groesse) >= 3.0) !
fetch into :a !
drop table kunden !
drop table ausgaben !
drop table anzeigen!
commit work release !

file connect ( kern test !
create table bahnhof (
name              varchar (10) primary key,
min_umsteige_zeit fixed (5) not null)!
insert into bahnhof values ('Muenchen' , 10)!
insert into bahnhof values ('Hamburg ' , 10)!
insert into bahnhof values ('Frankfurt', 15)!
create table zug (
nummer       varchar (6) primary key,
von_bahnhof  varchar (10) references bahnhof,
bis_bahnhof  varchar (10) references bahnhof,
start_zeit   time not null,
ankunft_zeit time not null,
preis        fixed (6,2) not null)!
insert zug values('ICE111','Muenchen ','Frankfurt','00090000','00130000',120)!
insert zug values('IC121 ','Muenchen ','Frankfurt','00091000','00134000', 90)!
insert zug values('ICE112','Muenchen ','Frankfurt','00102000','00141500',120)!
insert zug values('IC122 ','Muenchen ','Frankfurt','00111100','00154000', 90)!
insert zug values('ICE211','Frankfurt','Hamburg  ','00131000','00160000',100)!
insert zug values('ICE113','Muenchen ','Frankfurt','00131200','00172000',120)!
insert zug values('IC123 ','Muenchen ','Frankfurt','00132300','00180000', 90)!
insert zug values('ICE124','Muenchen ','Frankfurt','00135000','00175000',120)!
insert zug values('IC221 ','Frankfurt','Hamburg  ','00135000','00172000', 80)!
insert zug values('ICE212','Frankfurt','Hamburg  ','00141000','00170000',100)!
insert zug values('IC222 ','Frankfurt','Hamburg  ','00145000','00183000', 80)!
insert zug values('IC223 ','Frankfurt','Hamburg  ','00171000','00204000', 80)!
insert zug values('ICE213','Frankfurt','Hamburg  ','00173000','00204000',100)!
insert zug values('IC224 ','Frankfurt','Hamburg  ','00182300','00220000', 80)!
insert zug values('ICE214','Frankfurt','Hamburg  ','00190000','00220000',100)!
select * from
(select zug1.nummer num1,
       zug1.start_zeit start_m,
       zug1.ankunft_zeit end_f,
       zug2.nummer num2,
       zug2.start_zeit start_f,
       zug2.ankunft_zeit end_h,
       fixed(hour  (zug2.ankunft_zeit) * 60 + 
       minute(zug2.ankunft_zeit)          -
       (hour (zug1.start_zeit) * 60 +
       minute(zug1.start_zeit)), 3) dauer,
       fixed(zug1.preis + zug2.preis, 3) preis
  from zug zug1, zug zug2, bahnhof
 where zug1.von_bahnhof = 'Muenchen'
   and (hour (zug1.ankunft_zeit) * 60 +
       minute(zug1.ankunft_zeit)) + bahnhof.min_umsteige_zeit 
         <= (hour(zug2.start_zeit) * 60 + minute(zug2.start_zeit))
   and zug2.von_bahnhof = 'Frankfurt'
   and zug1.bis_bahnhof = bahnhof.name) xx
order by (dauer+preis)
!
fetch into :a, :b, :c, :d, :e, :f, :g, :h!
commit work release!

file connect ( kern test sqlmode oracle !
create table tabellen (
tabelle      varchar (30),
beschreibung varchar(100),
constraint tabellen_pk primary key ( tabelle))!
insert into tabellen values ('Tabellen', 'Beschreibung aller Tabellen')!
insert into tabellen values ('Spalten', 
            'Beschreibung aller Tabellenspalten')!
insert into tabellen values ('Indices', 'Beschreibung aller Indices')!
insert into tabellen values ('Index_Spalten', 
            'Beschreibung der Index Spalten')!
create table spalten (
tabelle    varchar (30) constraint sptabfk
                              references tabellen,
spalte     varchar (30),
is_key     varchar(1) not null constraint is_key_check
                              check (is_key in ('Y', 'N')),
fremdschl  varchar (30) constraint sptabfk2
                              references tabellen,
constraint spalten_pk primary key (tabelle, spalte)) !
insert into spalten values ('Tabellen     ', 'Beschreibung', 'N', NULL)!
insert into spalten values ('Spalten      ', 'Fremdschl   ', 'N', 'Tabellen')!
insert into spalten values ('Indices      ', 'Index_Name  ', 'Y', NULL)!
insert into spalten values ('Index_Spalten', 'Index_Name  ', 'Y', 'Indices')!
insert into spalten values ('Spalten      ', 'Is_Key      ', 'N', NULL)!
insert into spalten values ('Index_Spalten', 'Sortierung  ', 'N', NULL)!
insert into spalten values ('Spalten      ', 'Spalte      ', 'Y', NULL)!
insert into spalten values ('Index_Spalten', 'Spalte      ', 'Y', NULL)!
insert into spalten values ('Index_Spalten', 'Tabelle     ', 'Y', 'Indices')!
insert into spalten values ('Tabellen     ', 'Tabelle     ', 'Y', NULL)!
insert into spalten values ('Spalten      ', 'Tabelle     ', 'Y', 'Tabellen')!
insert into spalten values ('Indices      ', 'Tabelle     ', 'Y', 'Tabellen')!
insert into spalten values ('Indices      ', 'Unique      ', 'N', NULL)!
pars_then_ex !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
union
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s2.tabelle = :a
  and s1.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )!
c 30 'Tabellen' c 30 'Spalten'
c 30 'Tabellen' c 30 'Spalten'

nodata !
fetch into :a !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
union
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s2.tabelle = :a
  and s1.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )!
c 30 'Tabellen' c 30 'Indices'
c 30 'Tabellen' c 30 'Indices'

nodata !
fetch into :a !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
union
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s2.tabelle = :a
  and s1.tabelle = :b
  and s1.fremdschl = s2.tabelle
  and s1.spalte = s2.spalte
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )!
c 30 'Index_Spalten' c 30 'Indices'
c 30 'Index_Spalten' c 30 'Indices'

nodata !
fetch into :a !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.spalte = s2.spalte
  and (s1.fremdschl = s2.tabelle
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
or 
    s2.fremdschl = s1.tabelle
and not exists
   (select * 
      from spalten s3
     where s2.tabelle = s3.tabelle
       and s2.fremdschl = s3.fremdschl
       and s3.spalte not in
           (select s4.spalte
              from spalten s4
             where s1.tabelle = s4.tabelle
           )
   )
)!
c 30 'Tabellen' c 30 'Spalten'

nodata !
fetch into :a !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.spalte = s2.spalte
  and (s1.fremdschl = s2.tabelle
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
or 
    s2.fremdschl = s1.tabelle
and not exists
   (select * 
      from spalten s3
     where s2.tabelle = s3.tabelle
       and s2.fremdschl = s3.fremdschl
       and s3.spalte not in
           (select s4.spalte
              from spalten s4
             where s1.tabelle = s4.tabelle
           )
   )
)!
c 30 'Tabellen' c 30 'Indices'

nodata !
fetch into :a !
data !
select s1.tabelle || '.' || s1.spalte || ' = ' ||
       s2.tabelle || '.' || s1.spalte bedingung
from spalten s1, spalten s2
where s1.tabelle = :a
  and s2.tabelle = :b
  and s1.spalte = s2.spalte
  and (s1.fremdschl = s2.tabelle
  and not exists
    (select * 
       from spalten s3
      where s1.tabelle = s3.tabelle
        and s1.fremdschl = s3.fremdschl
        and s3.spalte not in
                (select s4.spalte
                   from spalten s4
                  where s2.tabelle = s4.tabelle
                )
    )
or 
    s2.fremdschl = s1.tabelle
and not exists
   (select * 
      from spalten s3
     where s2.tabelle = s3.tabelle
       and s2.fremdschl = s3.fremdschl
       and s3.spalte not in
           (select s4.spalte
              from spalten s4
             where s1.tabelle = s4.tabelle
           )
   )
)!
c 30 'Index_Spalten' c 30 'Indices'

nodata !
fetch into :a !
pars_execute!
drop table tabellen !
drop table spalten !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* FOKUS97.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test sqlmode oracle !
create table autoren (
aut_id number constraint autoren_pk primary key,
name   varchar(20))!
insert into autoren values (1, 'Bartels   ')!
insert into autoren values (2, 'Lenz      ')!
insert into autoren values (3, 'Unbescheid')!
insert into autoren values (4, 'Hermann   ')!
insert into autoren values (5, 'Stuerner  ')!
insert into autoren values (6, 'Wiesmeier ')!
create table buecher (
buch_id number constraint buecher_pk primary key,
titel   varchar(20))!
insert into buecher values (1, 'Oracle7    ')!
insert into buecher values (2, 'SQL*Net    ')!
insert into buecher values (3, 'Datenbanken')!
insert into buecher values (4, 'Objekte    ')!
create table autoren_referenz (
buch_id number constraint ref_buch_fk references buecher,
aut_id  number constraint ref_autor_fk references autoren,
lfd_nr  number,
constraint autoren_referenz_pk primary key (buch_id, aut_id, lfd_nr))!
insert into autoren_referenz values (1, 1, 1)!
insert into autoren_referenz values (2, 3, 2)!
insert into autoren_referenz values (4, 3, 1)!
insert into autoren_referenz values (3, 3, 3)!
insert into autoren_referenz values (3, 4, 2)!
insert into autoren_referenz values (3, 5, 1)!
insert into autoren_referenz values (2, 6, 1)!
commit work release !
file connect ( kern test !
select decode (anz_autoren, 1, aut1.name,
                            2, aut1.name || ', ' || aut2.name,
                               aut1.name || ' et al') name,
       titel
from (select buch_id, count(*) anz_autoren
        from autoren_referenz
       group by buch_id) cnttab,
      autoren_referenz ar,
      (select buch_id, name
         from autoren_referenz, autoren
        where autoren_referenz.aut_id = autoren.aut_id
          and lfd_nr = 1) aut1,
      (select buch_id, name
         from autoren_referenz, autoren
        where autoren_referenz.aut_id = autoren.aut_id
          and (buch_id, lfd_nr) = any (select buch_id, max(lfd_nr)
                                         from autoren_referenz
                                        group by buch_id)) aut2,
      buecher
where ar.buch_id = cnttab.buch_id
  and ar.buch_id = aut1.buch_id
  and ar.buch_id = aut2.buch_id
  and ar.buch_id = buecher.buch_id 
  and ar.lfd_nr  = 1 
order by ar.buch_id !
fetch!

select a.name, b.titel
  from buecher b,
       autoren a,
       autoren_referenz r
 where a.aut_id = r.aut_id
  and b.buch_id = r.buch_id
  and r.lfd_nr = 1
  and 1 = (select count (*)
             from autoren_referenz ri
            where b.buch_id = ri.buch_id)
union
select a1.name || ', ' || a2.name name, b.titel
  from buecher b,
       autoren a1,
       autoren a2,
       autoren_referenz r1,
       autoren_referenz r2
 where a1.aut_id = r1.aut_id
   and a2.aut_id = r2.aut_id
   and b.buch_id = r1.buch_id
   and b.buch_id = r2.buch_id
   and r1.lfd_nr = 1
   and r2.lfd_nr = 2
   and r1.buch_id in
          (select ri.buch_id
             from autoren_referenz ri
            group by ri.buch_id
            having count(*) = 2 )
union
select a.name || ' et. al.' name, b.titel
  from buecher b,
       autoren a,
       autoren_referenz r
 where a.aut_id = r.aut_id
   and b.buch_id = r.buch_id
   and r.lfd_nr = 1
   and exists (select * 
                 from autoren_referenz ri
                where b.buch_id = ri.buch_id
                  and ri.lfd_nr = 3)
order by 1 !
fetch into :a, :b !

select decode (aut_anz.anzahl,
               1, a1.name,
               2, a1.name || ', ' || a2.name,
               a1.name || ' et. al.') name,
       b.titel
  from buecher b,
       autoren a1,
       autoren a2,
       autoren_referenz r1,
       autoren_referenz r2,
       (select ri.buch_id, count(*) anzahl
          from autoren_referenz ri
         where ri.lfd_nr <= 3
         group by ri.buch_id) aut_anz
 where a1.aut_id      = r1.aut_id
   and a2.aut_id (+)  = r2.aut_id
   and b.buch_id      = r1.buch_id
   and b.buch_id      = r2.buch_id (+)
   and r1.lfd_nr      = 1
   and r2.lfd_nr (+)  = 2
   and r1.buch_id     = aut_anz.buch_id
order by 1 !
fetch into :a, :b !
drop table autoren !
drop table buecher !
drop table autoren_referenz !
commit release !

file connect ( kern test sqlmode oracle !
create table firmen (
f_id   number constraint firmen_pk primary key,
f_name varchar (20))!
insert into firmen values (1, 'Autohaus Meyer  ')!
insert into firmen values (8, 'Friseur Schulz  ')!
insert into firmen values (5, 'Klempner Schmidt')!
insert into firmen values (9, 'Werkstatt Grefe ')!
commit !
create table personen (
p_id    number constraint personen_pk primary key,
p_f_id  number constraint personen_firmen_fk references firmen,
p_name  varchar (20))!
insert into personen values (11, 5, 'Albrecht   ')!
insert into personen values (22, 5, 'Clausen    ')!
insert into personen values (55, 1, 'Fritz      ')!
insert into personen values ( 9, 5, 'Graber     ')!
insert into personen values (36, 9, 'Hansen     ')!
insert into personen values (44, 9, 'Herrmann   ')!
insert into personen values (77, 5, 'Jensen     ')!
insert into personen values ( 2, 5, 'Kunz       ')!
insert into personen values (56, 5, 'Linke      ')!
insert into personen values (23, 8, 'Linkwitz   ')!
insert into personen values (14, 1, 'Paulsen    ')!
insert into personen values (12, 5, 'Petersen   ')!
insert into personen values (25, 9, 'Rackwitz   ')!
insert into personen values ( 5, 1, 'Schweizer  ')!
insert into personen values (13, 5, 'Weidert    ')!
insert into personen values (33, 9, 'Zimmer     ')!
commit !
select f_name, p_name, x.nummer
  from firmen, personen,
       (select count(*) nummer, p1.p_id
          from personen p1, personen p2
         where p1.p_id  >= p2.p_id
           and p1.p_f_id = p2.p_f_id
         group by p1.p_id
       ) x
 where firmen.f_id = personen.p_f_id
   and x.p_id = personen.p_id
   and mod (x.nummer, 3) = 1 
order by 1,3 !
fetch into :a, :b, :c !
select f.f_name, p.p_name, count(*) nummer
  from firmen f,
       personen p,
       personen pz
 where f.f_id   = p.p_f_id
   and p.p_f_id = pz.p_f_id
   and p.p_id  >= pz.p_id
group by f.f_name, f.f_id, p.p_name, p.p_id
having mod(count(*), 3) = 1
order by f_name, nummer !
fetch into :a, :b, :c !
drop table firmen !
drop table personen !
commit work release !

file connect ( kern test sqlmode oracle !
create table mitarbeiter (
m_id   number constraint ma_pk primary key,
name   varchar (10) not null)!
insert into mitarbeiter values (2, 'Fitz')!
insert into mitarbeiter values (3, 'Lenz')!
insert into mitarbeiter values (1, 'Ofer')!
commit work !
create table kenntnisse (
m_id           number constraint k_ma_fk references mitarbeiter,
bezeichnung    varchar (10),
constraint k_pk primary key (m_id, bezeichnung)) !
insert into kenntnisse values (1, '4GL       ')!
insert into kenntnisse values (3, 'C         ')!
insert into kenntnisse values (3, 'C++       ')!
insert into kenntnisse values (2, 'CICS      ')!
insert into kenntnisse values (2, 'COBOL     ')!
insert into kenntnisse values (3, 'DB-LIB    ')!
insert into kenntnisse values (2, 'DB2 DBA   ')!
insert into kenntnisse values (2, 'JCL       ')!
insert into kenntnisse values (1, 'ORACLE DBA')!
insert into kenntnisse values (3, 'SQL       ')!
insert into kenntnisse values (1, 'SQL*NET   ')!
insert into kenntnisse values (3, 'SYBASE DBA')!
commit work !
create table offene_positionen (
pos_id       number,
bezeichnung  varchar (10),
constraint op_pk primary key (pos_id))!
insert into offene_positionen values (1, 'Entwickler')!
insert into offene_positionen values (2, 'WEB Spezi ')!
insert into offene_positionen values (3, 'Entwickler')!
insert into offene_positionen values (4, 'Entwickler')!
commit !
create table notwendige_kenntnisse (
pos_id        number,
bezeichnung   varchar (10),
constraint nk_pk primary key (pos_id, bezeichnung),
constraint nk_op_fk foreign key (pos_id) references offene_positionen)!
insert into notwendige_kenntnisse values (1, '4GL       ')!
insert into notwendige_kenntnisse values (2, 'C++       ')!
insert into notwendige_kenntnisse values (3, 'CICS      ')!
insert into notwendige_kenntnisse values (3, 'COBOL     ')!
insert into notwendige_kenntnisse values (2, 'DB2 DBA   ')!
insert into notwendige_kenntnisse values (2, 'JAVA      ')!
insert into notwendige_kenntnisse values (1, 'ORACLE DBA')!
insert into notwendige_kenntnisse values (2, 'SQL       ')!
insert into notwendige_kenntnisse values (4, 'ORACLE DBA')!
insert into notwendige_kenntnisse values (4, 'COBOL     ')!
insert into notwendige_kenntnisse values (4, 'SQL       ')!
commit work !
select op.pos_id, op.bezeichnung
  from offene_positionen op, notwendige_kenntnisse not_k
 where op.pos_id = not_k.pos_id
group by op.pos_id, op.bezeichnung
having count(*) > all (select count(k.bezeichnung)
                         from kenntnisse k, notwendige_kenntnisse nk
                        where k.bezeichnung = nk.bezeichnung
                          and nk.pos_id = op.pos_id
                        group by k.m_id)!
fetch into :a, :b !
select pos_id, bezeichnung
  from offene_positionen o
 where not exists
       (select k.m_id, count(nk.bezeichnung)
          from notwendige_kenntnisse nk,
               kenntnisse k
         where nk.bezeichnung = k.bezeichnung
           and nk.pos_id = o.pos_id
          group by nk.pos_id, k.m_id
          having count (nk.bezeichnung) = 
                              (select count (nk2.bezeichnung)
                                 from notwendige_kenntnisse nk2
                                where nk.pos_id = nk2.pos_id
                              )
       ) !
fetch into :a, :b !
drop table mitarbeiter !
drop table kenntnisse !
drop table offene_positionen !
drop table notwendige_kenntnisse !
commit work release !


file connect ( kern test sqlmode oracle !
create table auftrag (
auf_id  integer constraint auftrag_pk primary key,
kd_name varchar (10) not null)!
insert into auftrag values (1, 'Parthier')!
insert into auftrag values (2, 'Jansen')!
insert into auftrag values (3, 'Hagner')!
create table position (
auf_id       integer constraint pos_auf_fk references auftrag,
position     integer,
menge        integer not null,
bezeichnung  varchar(10) not null,
constraint position_pk primary key (auf_id, position))!
insert into position values (1,1,5,'Blech')!
insert into position values (1,2,3,'Stuetze')!
insert into position values (1,3,2,'Halter')!
insert into position values (2,1,1,'Gehaeuse')!
insert into position values (2,2,4,'Lager')!
insert into position values (2,3,5,'Korpus')!
insert into position values (3,1,3,'Blech')!
insert into position values (3,2,1,'Lager')!
create table maschine (
m_id        integer constraint maschine_pk primary key,
bezeichnung varchar (10) not null,
dm_stunde   decimal (8,2) not null)!
insert into maschine values (1, 'M1', 998.98)!
insert into maschine values (2, 'M2', 899.98)!
insert into maschine values (3, 'M3', 699.98)!
create table werkzeug (
w_id        integer constraint werkzeug_pk primary key,
bezeichnung varchar (10) not null,
dm_stunde   decimal (8,2) not null)!
insert into werkzeug values (1, 'Bohrer  ', 20.22)!
insert into werkzeug values (2, 'Saege   ', 30.33)!
insert into werkzeug values (3, 'Fraese  ', 40.44)!
insert into werkzeug values (4, 'Schleifer',50.55)!
insert into werkzeug values (5, 'Dreher  ', 60.66)!
create table maschine_werkzeug (
m_id        integer constraint masch_wk_masch_fk references maschine,
w_id        integer constraint masch_wk_wk_fk references werkzeug,
max_mass    decimal (8,2) not null,
constraint maschine_werkzeug_pk primary key (m_id, w_id))!
insert into maschine_werkzeug values (1,1,20.5)!
insert into maschine_werkzeug values (1,2,30.5)!
insert into maschine_werkzeug values (1,3,40.5)!
insert into maschine_werkzeug values (1,4,10.5)!
insert into maschine_werkzeug values (1,5,20.5)!
insert into maschine_werkzeug values (2,1,30.5)!
insert into maschine_werkzeug values (2,2,20.5)!
insert into maschine_werkzeug values (2,3,20.5)!
insert into maschine_werkzeug values (2,4,22.5)!
insert into maschine_werkzeug values (2,5,20.5)!
insert into maschine_werkzeug values (3,1,20.5)!
insert into maschine_werkzeug values (3,3,20.5)!
insert into maschine_werkzeug values (3,5,20.5)!
create table bearbeitung (
auf_id     integer,
position   integer,
schritt    integer,
w_id       integer not null constraint bearb_wk_fk references werkzeug,
mass       decimal (8,2) not null,
dauer      decimal (4,2) not null,
constraint bearbeitung_pk primary key (auf_id, position, schritt),
constraint bearb_pos_fk foreign key (auf_id, position) 
                       references position)!
insert into bearbeitung values (1,1,1,1,20.2,0.3)!
insert into bearbeitung values (1,1,2,3,20.2,0.1)!
insert into bearbeitung values (1,2,1,5,20.3,0.4)!
insert into bearbeitung values (1,2,2,3,20.1,0.3)!
insert into bearbeitung values (1,3,1,1,20.2,0.5)!
insert into bearbeitung values (1,3,2,5,20.1,0.6)!
insert into bearbeitung values (2,1,1,1,20.2,0.3)!
insert into bearbeitung values (2,1,2,3,20.2,0.1)!
insert into bearbeitung values (2,2,1,5,20.3,0.4)!
insert into bearbeitung values (2,2,2,3,40.1,0.7)!
insert into bearbeitung values (2,3,1,1,20.2,0.5)!
insert into bearbeitung values (2,3,2,5,20.1,0.6)!
insert into bearbeitung values (3,1,1,1,20.2,0.3)!
insert into bearbeitung values (3,1,2,3,20.2,0.1)!
insert into bearbeitung values (3,2,1,4,20.3,0.4)!
insert into bearbeitung values (3,2,2,3,40.1,0.7)!
commit work release!
file connect ( kern test !
select a.auf_id, m.m_id, m.bezeichnung, 
 fixed (sum(p.menge*b.dauer*(m.dm_stunde+w.dm_stunde)),10,3)
from  auftrag a,
      position p,
      bearbeitung b,
      werkzeug w,
      maschine_werkzeug m_w,
      maschine m
where a.auf_id = p.auf_id
  and p.auf_id = b.auf_id
  and p.position = b.position
  and b.w_id = w.w_id
  and w.w_id = m_w.w_id
  and b.mass <= m_w.max_mass
  and m_w.m_id = m.m_id
group by a.auf_id, m.m_id, m.bezeichnung
having (count(*) , a.auf_id) = 
     any ( select count(*), auf_id from bearbeitung group by auf_id)!
fetch into :a, :b, :c, :d !

SELECT B.AUF_ID, M.M_ID, M.BEZEICHNUNG "Maschine",
       FIXED(SUM(P.MENGE*B.DAUER*(M.DM_STUNDE+W.DM_STUNDE)),7,2) "Preis"
FROM BEARBEITUNG B, WERKZEUG W, MASCHINE M, MASCHINE_WERKZEUG MW,
     POSITION P
WHERE
    B.W_ID = W.W_ID AND
    MW.W_ID = W.W_ID AND
    MW.M_ID = M.M_ID AND
    B.MASS <= MW.MAX_MASS AND
    P.AUF_ID = B.AUF_ID AND
    P.POSITION = B.POSITION
GROUP BY B.AUF_ID, M.M_ID, M.BEZEICHNUNG
HAVING COUNT(B.SCHRITT) = (
  SELECT COUNT(B1.SCHRITT)
  FROM BEARBEITUNG B1
  WHERE B1.AUF_ID = B.AUF_ID
)
ORDER BY B.AUF_ID, M.M_ID!
fetch into :a, :b, :c, :d !
drop table auftrag !
drop table position !
drop table bearbeitung !
drop table werkzeug !
drop table maschine_werkzeug !
drop table maschine!
commit work release !

file connect ( kern test sqlmode oracle !
create table mitarbeiter (
m_id   number constraint ma_pk primary key,
name   varchar (10) not null)!
insert into mitarbeiter values (2, 'Fitz')!
insert into mitarbeiter values (3, 'Lenz')!
insert into mitarbeiter values (1, 'Ofer')!
commit work !
create table kenntnisse (
k_id           number constraint k_pk primary key,
bezeichnung    varchar (10))!
insert into kenntnisse values (1, '4GL       ')!
insert into kenntnisse values (2, 'C         ')!
insert into kenntnisse values (3, 'C++       ')!
insert into kenntnisse values (4, 'CICS      ')!
insert into kenntnisse values (5, 'COBOL     ')!
insert into kenntnisse values (6, 'DB-LIB    ')!
insert into kenntnisse values (7, 'DB2 DBA   ')!
insert into kenntnisse values (8, 'JAVA      ')!
insert into kenntnisse values (9, 'JCL       ')!
insert into kenntnisse values (10, 'ORACLE DBA')!
insert into kenntnisse values (11, 'SQL       ')!
insert into kenntnisse values (12, 'SQL*NET   ')!
insert into kenntnisse values (13, 'SYBASE DBA')!
commit work !
create table vorhandene_kenntnisse (
m_id         number constraint vk_m_fk references mitarbeiter,
k_id         number constraint vk_k_fk references kenntnisse,
constraint nk_pk primary key (m_id, k_id)) !
insert into vorhandene_kenntnisse values (1, 1)!
insert into vorhandene_kenntnisse values (3, 2)!
insert into vorhandene_kenntnisse values (3, 3)!
insert into vorhandene_kenntnisse values (2, 4)!
insert into vorhandene_kenntnisse values (2, 5)!
insert into vorhandene_kenntnisse values (3, 6)!
insert into vorhandene_kenntnisse values (2, 7)!
insert into vorhandene_kenntnisse values (2, 9)!
insert into vorhandene_kenntnisse values (1,10)!
insert into vorhandene_kenntnisse values (3,11)!
insert into vorhandene_kenntnisse values (1,12)!
insert into vorhandene_kenntnisse values (3,13)!
commit work !
create table offene_positionen (
pos_id       number,
bezeichnung  varchar (10),
constraint op_pk primary key (pos_id))!
insert into offene_positionen values (1, 'Entwickler')!
insert into offene_positionen values (2, 'WEB Spezi ')!
insert into offene_positionen values (3, 'Entwickler')!
insert into offene_positionen values (4, 'Entwickler')!
commit !
create table notwendige_kenntnisse (
pos_id        number constraint nk_op_fk references offene_positionen,
k_id          number constraint nk_k__fk references kenntnisse,
constraint nk_pk primary key (pos_id, k_id))!
insert into notwendige_kenntnisse values (1, 1)!
insert into notwendige_kenntnisse values (2, 3)!
insert into notwendige_kenntnisse values (3, 4)!
insert into notwendige_kenntnisse values (3, 5)!
insert into notwendige_kenntnisse values (4, 5)!
insert into notwendige_kenntnisse values (2, 7)!
insert into notwendige_kenntnisse values (2, 8)!
insert into notwendige_kenntnisse values (1,10)!
insert into notwendige_kenntnisse values (4,10)!
insert into notwendige_kenntnisse values (2,11)!
insert into notwendige_kenntnisse values (4,11)!
commit work !
select op.pos_id, op.bezeichnung, name
  from offene_positionen op, 
       mitarbeiter m, 
       vorhandene_kenntnisse vk,
       notwendige_kenntnisse nk
 where op.pos_id = nk.pos_id
   and nk.k_id   = vk.k_id
   and m.m_id    = vk.m_id 
group by op.pos_id, op.bezeichnung, name
having count(*) >= all ( select count(*)
                  from vorhandene_kenntnisse vk,
                       notwendige_kenntnisse nk
                 where nk.pos_id  = op.pos_id
                   and nk.k_id   = vk.k_id
                group by vk.m_id 
                  ) !
fetch into :a, :b, :c !
select op.pos_id, op.bezeichnung, name
  from offene_positionen op, 
       mitarbeiter m, 
       vorhandene_kenntnisse vk,
       notwendige_kenntnisse nk
 where op.pos_id = nk.pos_id
   and nk.k_id   = vk.k_id
   and m.m_id    = vk.m_id 
group by op.pos_id, op.bezeichnung, name
having not exists (select vk1.m_id
                  from vorhandene_kenntnisse vk1,
                       notwendige_kenntnisse nk1
                 where nk1.pos_id = op.pos_id
                   and nk1.k_id   = vk1.k_id
                group by vk1.m_id 
				having count (vk1.k_id) > count (vk.k_id)
                  ) !
pars_execute!
fetch into :a, :b, :c !
drop table mitarbeiter !
drop table kenntnisse !
drop table vorhandene_kenntnisse !
drop table offene_positionen !
drop table notwendige_kenntnisse !
commit work release !

file connect ( kern test !
create table kfz (
kennzeichen   varchar (20) primary key,
avg_verbrauch decimal)!
insert into kfz values ('HH-XY 1224', 10)!
insert into kfz values ('M-OC 5447', 12)!
insert into kfz values ('XQ-BZ 9090', 11)!
insert into kfz values ('QU-AA 0815', 13)!
create table tanken (
kennzeichen varchar (20) constraint t_k_fk references kfz,
dat         date not null,
km          integer constraint t_km_check check (km > 0),
menge       decimal not null constraint t_m_check check (menge > 0),
primary key (kennzeichen, km))!
commit release !
file connect ( kern test !
set format eur !
insert into tanken values ('HH-XY 1224', '10.03.1997', 13100, 40)!
insert into tanken values ('HH-XY 1224', '11.04.1997', 13800, 40)!
insert into tanken values ('XQ-BZ 9090', '13.03.1997', 46720, 55)!
insert into tanken values ('XQ-BZ 9090', '17.03.1997', 47320, 65)!
insert into tanken values ('M-OC 5447' , '20.02.1997', 12345, 55)!
insert into tanken values ('HH-XY 1224', '20.03.1997', 13420, 40)!
insert into tanken values ('M-OC 5447' , '20.04.1997', 13870, 62)!
insert into tanken values ('HH-XY 1224', '22.02.1997', 12345, 55)!
insert into tanken values ('QU-AA 0815', '22.02.1997', 44556, 50)!
insert into tanken values ('XQ-BZ 9090', '22.02.1997', 45678, 70)!
insert into tanken values ('M-OC 5447' , '22.02.1997',128451, 62)!
insert into tanken values ('M-OC 5447' , '22.04.1997', 14350, 58)!
insert into tanken values ('QU-AA 0815', '22.04.1997', 45500, 60)!
insert into tanken values ('QU-AA 0815', '23.04.1997', 46000, 65)!
insert into tanken values ('QU-AA 0815', '24.02.1997',  5000, 65)!
insert into tanken values ('HH-XY 1224', '25.02.1997', 12750, 40)!
insert into tanken values ('XQ-BZ 9090', '25.02.1997', 45865, 60)!
insert into tanken values ('QU-AA 0815', '27.03.1997', 45400, 50)!
insert into tanken values ('QU-AA 0815', '27.04.1997', 46400, 60)!
insert into tanken values ('XQ-BZ 9090', '28.02.1997', 46223, 50)!
insert into tanken values ('M-OC 5447' , '30.03.1997', 13320, 63)!
set format internal!
alter table tanken add datum timestamp!
update tanken set datum = timestamp (dat, time)!
commit release !
file connect ( kern test !
* PTS 1000966 !
* Versuch, sich an die Lösung des Problems heranzurobben,
* von Till Luhmann !
select k.kennzeichen, k.avg_verbrauch,  
t2.dat as Letzt_Datum, t1.dat as Akt_Datum, 
datediff(t1.dat, t2.dat) as Tage,
t2.km as Letzt_km, t1.km as Akt_km
from kfz k, tanken t1, tanken t2
where 
(t1.kennzeichen = k.kennzeichen) and
(t2.kennzeichen = k.kennzeichen) and
(t2.dat < t1.dat) and
(t2.dat = (select max(dat) from tanken 
 except (select max(dat) from tanken)))!
fetch !
commit release !
file connect ( kern test sqlmode oracle !
select k.kennzeichen, k.avg_verbrauch avg,
       t.datum datum_von, tb.datum datum_bis,
	   t.datum - tb.datum tage,
	   tb.km km_von, t.km km_bis,
	   t.km - tb.km distanz, t.menge,
	   round (t.menge / (t.km - tb.km) * 100, 2) verbrauch,
       round ((t.menge / (t.km - tb.km) * 100 - k.avg_verbrauch )
	   / k.avg_verbrauch * 100, 2) "%Abw."
from kfz k, tanken t, 
       (select min (tank1.datum - tank2.datum) min_tage,
	   tank2.kennzeichen, tank2.datum, tank2.km
	   from tanken tank1, tanken tank2
	   where tank1.kennzeichen = tank2.kennzeichen
	     and tank1.datum > tank2.datum
		 group by tank2.kennzeichen, tank2.datum, tank2.km) tb
where k.kennzeichen = t.kennzeichen
and	  k.kennzeichen = tb.kennzeichen
and   t.datum > tb.datum
and   t.datum - tb.datum = tb.min_tage
and   abs(round((t.menge / (t.km - tb.km) * 100 
       / k.avg_verbrauch - 1) * 100, 2)) > 50 !
fetch into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j, :k !
drop table tanken !
drop table kfz !
commit work release !

file connect ( kern test sqlmode oracle !
create table artikel (
art_nr      integer constraint artikel_pk primary key,
bezeichnung varchar (20) not null)!
insert into artikel values (1, 'Apfelsaft')!
insert into artikel values (2, 'Orangensaft')!
commit work !
create table flaschen (
fl_nr        integer constraint flaschen_pk primary key,
bezeichnung  varchar (20) not null,
inhalt       decimal (5,2) not null
             constraint inhalt_check check (inhalt > 0))!
insert into flaschen values (1, 'Pfandflasche', 1.0)!
insert into flaschen values (2, 'Fläschchen'  , 0.2)!
insert into flaschen values (3, 'Halber'      , 0.5)!
commit work !
create table kunden (
kn_nr integer constraint kunden_pk primary key,
name  varchar (20) not null)!
insert into kunden values (1, 'Durstig')!
insert into kunden values (2, 'Saufaus')!
commit work !
create table bestellung (
bestell_nr   integer constraint bestellung_pk primary key,
kd_nr        integer 
              constraint best_kd_fk references kunden not null,
liefer_datum date not null)!
insert into bestellung values (1, 1, '01-JAN-97')!
insert into bestellung values (2, 1, '01-MAY-97')!
insert into bestellung values (3, 1, '11-DEC-97')!
insert into bestellung values (4, 2, '22-JUN-97')!
insert into bestellung values (5, 2, '21-AUG-97')!
insert into bestellung values (6, 1, '20-DEC-97')!
insert into bestellung values (7, 2, '24-DEC-97')!
commit work !
create table bestell_pos (
bestell_nr integer,
pos        integer,
art_nr     integer not null constraint bp_art_fk references artikel,
fl_nr      integer not null constraint bp_fla_fk references flaschen,
menge      integer not null constraint menge_check check (menge > 0),
constraint bestell_pos_pk primary key (bestell_nr, pos),
constraint bes_pos_u1 unique (bestell_nr, art_nr, fl_nr))!
insert into bestell_pos values (1,1,1,1, 100)!
insert into bestell_pos values (1,2,2,3,  20)!
insert into bestell_pos values (1,3,1,2, 200)!
insert into bestell_pos values (2,1,2,2, 200)!
insert into bestell_pos values (2,2,1,3,  15)!
insert into bestell_pos values (2,3,2,1,  22)!
insert into bestell_pos values (3,1,1,1,  11)!
insert into bestell_pos values (3,2,2,1,  33)!
insert into bestell_pos values (3,3,1,2,  44)!
insert into bestell_pos values (3,4,1,3,  25)!
insert into bestell_pos values (3,5,2,2,  37)!
insert into bestell_pos values (3,6,2,3, 234)!
insert into bestell_pos values (4,1,1,1,1111)!
insert into bestell_pos values (4,2,2,2, 222)!
insert into bestell_pos values (5,1,1,3, 234)!
insert into bestell_pos values (6,1,2,3,1000)!
insert into bestell_pos values (6,2,2,2, 100)!
insert into bestell_pos values (7,1,1,1, 300)!
insert into bestell_pos values (7,2,1,2, 300)!
insert into bestell_pos values (7,3,1,3, 200)!
commit work !
create table produktion (
art_nr      integer,
prod_datum  date,
menge       decimal (10,2) not null 
             constraint prod_menge_check check (menge > 0),
constraint produktion_pk primary key (art_nr, prod_datum))!
insert into produktion values (1,'01-OCT-97',1000)!
insert into produktion values (2,'01-OCT-97', 677)!
commit work !
select bp.art_nr, bp.fl_nr,
       to_char(round(sum (bp.menge * f.inhalt) /
             sum.summe * 100, 2), '99.99') prozent,
       to_char(round(sum (bp.menge * f.inhalt) /
             sum.summe * p.menge), '9999') menge,
       to_char(round(sum (bp.menge * f.inhalt) /
             sum.summe * p.menge / f.inhalt), '9999') flaschen             
  from bestell_pos bp,
       flaschen f,
       produktion p,
       (select bp.art_nr,
               sum (bp.menge * f.inhalt) summe
          from bestell_pos bp, flaschen f
         where bp.fl_nr = f.fl_nr
         group by bp.art_nr)                      sum
 where bp.art_nr = sum.art_nr
   and bp.fl_nr  = f.fl_nr
   and bp.art_nr = p.art_nr
   and p.prod_datum = '01-OCT-97'
 group by bp.art_nr, bp.fl_nr, sum.summe,
               f.inhalt, p.menge !
fetch into :a, :b, :c, :d, :e !
drop table artikel !
drop table flaschen !
drop table kunden !
drop table bestellung !
drop table bestell_pos !
drop table produktion !
commit work release !

file connect ( kern test sqlmode oracle !
create table messungen (
messung integer constraint messung_pk primary key,
wert    decimal (4,1) not null)!
insert into messungen values ( 4, 11.3)!
insert into messungen values ( 6, 11.7)!
insert into messungen values ( 2, 12.2)!
insert into messungen values ( 9, 12.8)!
insert into messungen values (11, 12.9)!
insert into messungen values (13, 13.3)!
insert into messungen values ( 5, 13.4)!
insert into messungen values (18, 13.4)!
insert into messungen values ( 8, 13.9)!
insert into messungen values (15, 13.9)!
insert into messungen values (20, 13.9)!
insert into messungen values (17, 14.0)!
insert into messungen values (10, 14.3)!
insert into messungen values (22, 14.5)!
insert into messungen values (25, 15.0)!
commit release !
file connect ( kern test !
select m1.messung, m1.wert, fixed(round (avg(m2.wert),2),4,2), 
       fixed(round((avg(m2.wert) - m1.wert) * 100 / avg(m2.wert), 2),4,2)
from 
           (select rowno nr1, messung, wert
              from messungen
			  group by messung, wert) m1,
           (select rowno nr2, messung, wert
              from messungen
			  group by messung, wert) m2
where m2.nr2 between m1.nr1-3 and m1.nr1+3 
group by m1.messung, m1.wert
having abs(round((avg(m2.wert) - m1.wert) * 100 / avg(m2.wert), 2)) > 5!
fetch into :a, :b, :c, :d !
* Lösung 1 !
select m1.messung, m1.wert,
       fixed(round(avg (m2.wert), 2),4,2) avgr,
       fixed(round((avg(m2.wert) - m1.wert) / 
                    avg (m2.wert) * 100, 2),4,2) abw
  from messungen m1, messungen m2
 where m1.messung <= m2.messung
   and 4 >= (select count (*)
               from messungen mi
              where mi.messung between m1.messung and m2.messung
            )
    or 
       m1.messung >= m2.messung
   and 4 >= (select count (*)
               from messungen mi
              where mi.messung between m2.messung and m1.messung
            )
 group by m1.messung, m1.wert
having round ((avg(m2.wert) - m1.wert) /
               avg(m2.wert) * 100, 2)
       not between -5 and 5 !
fetch into :a, :b, :c, :d !
*Lösung 2 geht nicht, da ACCESS Spezialität !
select c.messung1 as messung,
       round(c.wert1,1) as wert,
       fixed(round(avg(c.wert2),2),4,2) as avgr,
       fixed(round(100-(c.wert1*100 / avg(c.wert2)),2),4,2) as abw
  from (select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = b.messung
        union
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select min(messung) 
                  from messungen
                 where messung > b.messung)
        union          
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select min(messung) 
                  from messungen
                 where messung > 
                       (select min(messung) 
                          from messungen
                         where messung > b.messung))
        union          
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select min(messung) 
                  from messungen
                 where messung > 
                       (select min(messung) 
                          from messungen
                         where messung > 
                               (select min(messung) 
                                  from messungen
                                 where messung > b.messung)))
        union
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select max(messung) 
                  from messungen
                 where messung < b.messung)
        union          
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select max(messung) 
                  from messungen
                 where messung < 
                       (select max(messung) 
                          from messungen
                         where messung < b.messung))
        union          
        select b.messung as messung1, b.wert as wert1,
               a.messung as messung2, a.wert as wert2
          from messungen a, messungen b
         where a.messung = 
               (select max(messung) 
                  from messungen
                 where messung < 
                       (select max(messung) 
                          from messungen
                         where messung < 
                               (select max(messung) 
                                  from messungen
                                 where messung < b.messung)))
         ) c
group by messung1,wert1
having abs(100-(c.wert1*100/avg(c.wert2))) > 5!
fetch into :a, :b, :c, :d !
drop table messungen !
commit release !

file connect ( kern test !
create table personen (
p_id integer primary key,
name char (10) not null)!
insert into personen values (1, 'Golf')!
insert into personen values (2, 'John')!
insert into personen values (3, 'Peter')!
commit !
create table accounting (
login_id integer primary key,
p_id     integer not null references personen,
login    timestamp not null,
logout   timestamp not null,
constraint date_check check (login < logout))!
insert into accounting values 
 ( 1,1,'19971210100000000000','19971210120000000000')!
insert into accounting values 
 ( 2,1,'19971210110000000000','19971210140000000000')!
insert into accounting values 
 ( 3,1,'19971210150000000000','19971210190000000000')!
insert into accounting values
 ( 4,1,'19971210150000000000','19971210160000000000')!
insert into accounting values
 ( 5,1,'19971210170000000000','19971210180000000000')!
insert into accounting values
 ( 6,2,'19971210090000000000','19971210130000000000')!
insert into accounting values
 ( 7,2,'19971210150000000000','19971210160000000000')!
insert into accounting values
 ( 8,2,'19971210150000000000','19971210160000000000')!
insert into accounting values
 ( 9,3,'19971210100000000000','19971210140000000000')!
insert into accounting values
 (10,3,'19971210130000000000','19971210150000000000')!
insert into accounting values
 (11,3,'19971210130000000000','19971210150000000000')!
insert into accounting values
 (12,3,'19971210150000000000','19971210170000000000')!
commit work !
create table preise (
anzahl integer primary key,
betrag decimal (10,2) not null)!
insert into preise values (1, 1.00)!
insert into preise values (2, 0.70)!
insert into preise values (3, 0.55)!
insert into preise values (4, 0.45)!
insert into preise values (5, 0.40)!
commit work !
select p.name, a.login_id, max(pr.betrag)
  from personen p,
       accounting a,
       preise pr
 where p.p_id = a.p_id 
   and pr.anzahl >= ALL 
                (select count (*)
                   from accounting a1,
                        accounting a2
                  where a1.login >= a.login
                    and a1.login <  a.logout
                    and a1.login >= a2.login
                    and a1.login <  a2.logout
                    and a1.p_id  =  p.p_id
                    and a1.p_id  =  a2.p_id
                 group by a1.login_id) 
group by p.name, a.login_id !
fetch into :a, :b, :c !
drop table personen !
drop table accounting !
drop table preise !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* FOKUS98.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !

file connect ( kern test sqlmode oracle !
create table steuerklasse (
klasse       integer constraint steuerklasse_pk primary key,
beschreibung varchar (30))!
insert into steuerklasse values (1, 'Alleinstehend')!
insert into steuerklasse values (2, 'Alleinerziehend')!
insert into steuerklasse values (3, 'Verheiratet')!
commit work!
create table einkommen (
name   varchar (10) constraint einkommen_pk primary key,
klasse integer not null 
         constraint ein_klasse_fk references steuerklasse,
betrag decimal not null)!
insert into einkommen values ('Mayer',   1, 32123)!
insert into einkommen values ('Schmidt', 2, 33333)!
insert into einkommen values ('Franz',   3, 34567)!
insert into einkommen values ('Fredl',   2, 39876)!
commit work !
create table steuer (
klasse        integer not null 
              constraint steuer_klasse_fk references steuerklasse,
ober_grenze   decimal not null
              constraint st_ober_check check (ober_grenze > 0),
steuer_betrag decimal not null
              constraint st_betrag_check check (steuer_betrag >= 0))!
insert into steuer values (1,31000, 7610)!
insert into steuer values (1,32000, 8240)!
insert into steuer values (1,33000, 8890)!
insert into steuer values (1,34000, 9560)!
insert into steuer values (1,35000,10250)!
insert into steuer values (1,36000,10960)!
insert into steuer values (1,37000,11690)!
insert into steuer values (1,38000,12440)!
insert into steuer values (1,39000,13210)!
insert into steuer values (1,40000,14000)!
insert into steuer values (2,31000, 6370)!
insert into steuer values (2,32000, 6960)!
insert into steuer values (2,33000, 7570)!
insert into steuer values (2,34000, 8200)!
insert into steuer values (2,35000, 8850)!
insert into steuer values (2,36000, 9520)!
insert into steuer values (2,37000,10210)!
insert into steuer values (2,38000,10920)!
insert into steuer values (2,39000,11650)!
insert into steuer values (2,40000,12400)!
insert into steuer values (3,31000, 5715)!
insert into steuer values (3,32000, 6210)!
insert into steuer values (3,33000, 6735)!
insert into steuer values (3,34000, 7290)!
insert into steuer values (3,35000, 7875)!
insert into steuer values (3,36000, 8490)!
insert into steuer values (3,37000, 9135)!
insert into steuer values (3,38000, 9810)!
insert into steuer values (3,39000,10515)!
insert into steuer values (3,40000,11250)!
commit work !
select e.name, to_char(e.klasse, '9'), to_char(e.betrag,'99999'),
       to_char(st1.steuer_betrag, '99999') steuer_min,
       to_char(st2.steuer_betrag, '99999') steuer_max,
       to_char(round(
       st1.steuer_betrag +
       ((e.betrag - st1.ober_grenze) * 
        (st2.steuer_betrag - st1.steuer_betrag)) /
       ( st2.ober_grenze - st1.ober_grenze), 2), '99999.99') steuer
  from einkommen e,
       steuer st1,
       steuer st2
 where e.klasse = st1.klasse
   and e.klasse = st2.klasse
   and st1.ober_grenze = (select max(ober_grenze)
                            from steuer sx
                           where e.betrag > sx.ober_grenze
                             and e.klasse = sx.klasse
                         )
   and st2.ober_grenze = (select min(ober_grenze)
                            from steuer sy
                           where e.betrag < sy.ober_grenze
                             and e.klasse = sy.klasse
                         ) !
fetch into :a, :b, :c, :d, :e, :f !    
select e.name, to_char(e.klasse, '9'), to_char(e.betrag,'99999'),
       to_char (max (s2.steuer_betrag), '99999') steuer_min,
       to_char (min (s1.steuer_betrag), '99999') steuer_max,
       to_char (round (
       max (s2.steuer_betrag) + 
       (min (s1.steuer_betrag) - max (s2.steuer_betrag)) /
       (min (s1.ober_grenze) - max (s2.ober_grenze)) *
       (e.betrag - max (s2.ober_grenze)), 2), '99999.99') steuer
  from einkommen e, steuer s1, steuer s2
 where e.klasse = s1.klasse
   and e.betrag <= s1.ober_grenze
   and e.klasse = s2.klasse
   and e.betrag >= s2.ober_grenze
group by e.name, e.klasse, e.betrag !
fetch into :a, :b, :c, :d, :e, :f !    
drop table steuerklasse!
drop table einkommen !
drop table steuer !
commit work release !

file connect ( kern test sqlmode oracle !
create table personen (
p_id    integer constraint personen_pk primary key,
name    char (10) not null,
geb_dat date not null)!
insert into personen values ( 6, 'Albrecht ', '11-APR_67')! 
insert into personen values ( 8, 'Baader   ', '20-JUN_55')! 
insert into personen values (12, 'Hessen   ', '11-JAN_71')! 
insert into personen values ( 1, 'Jansen   ', '01-JAN_65')! 
insert into personen values ( 2, 'Johannsen', '02-FEB_66')! 
insert into personen values ( 3, 'Lenze    ', '05-AUG_60')! 
insert into personen values ( 7, 'Linke    ', '30-JUL_49')! 
insert into personen values ( 9, 'Orterer  ', '18-JAN_70')! 
insert into personen values (11, 'Pape     ', '20-JUN_55')! 
insert into personen values ( 4, 'Raupp    ', '09-SEP_58')! 
insert into personen values (10, 'Smith    ', '19-FEB_66')! 
insert into personen values ( 5, 'Weiss    ', '22-MAR_51')! 
pars_then_ex !
data !
select to_char(p1.p_id, '99') p_id, 
       p1.name,
       p1.geb_dat,
       count(*) nummer,
       to_char (trunc (:anzahl_gruppen * (count(*) - 1) /
                 p_anz.anzahl) + 1, '99') gruppe
  from personen p1, personen p2,
       (select count(*) anzahl from personen) p_anz
 where p2.geb_dat < p1.geb_dat
    or (p2.geb_dat = p1.geb_dat and
        p2.p_id <= p1.p_id)
group by p1.geb_dat, p1.p_id, p1.name, p_anz.anzahl !
n 38 4

fetch into :a, :b, :c, :d, :e !
select to_char(p1.p_id, '99') p_id, 
       p1.name,
       p1.geb_dat,
       count(distinct p2.p_id) nummer,
       to_char (trunc ((count (distinct p2.p_id) - 1) /
                (count (distinct p3.p_id) /
                :anzahl_gruppen)) + 1, '99') gruppe
  from personen p1, personen p2, personen p3
 where p2.geb_dat < p1.geb_dat
    or (p2.geb_dat = p1.geb_dat and
        p2.p_id <= p1.p_id)
group by p1.p_id, p1.name, p1.geb_dat
order by p1.geb_dat !
n 38 5

fetch into :a, :b, :c, :d, :e !
pars_execute!
nodata!
drop table personen !
commit work release !

file connect ( kern test sqlmode oracle !
create table ergebnisse (
name   varchar (10) not null,
ort    varchar (10) not null,
anteil decimal (3,1))!
insert into ergebnisse values ('Sagad    ', 'Verschwand', 34)!
insert into ergebnisse values ('Obergram ', 'Unterach  ', 92)!
insert into ergebnisse values ('Lakitz   ', 'Klamau    ', 67)!
insert into ergebnisse values ('Fitzken  ', 'Borkenbach', 55)!
insert into ergebnisse values ('Aberhoff ', 'Klegil    ', 84)!
insert into ergebnisse values ('Klabutz  ', 'Borstberg ', 23)!
select n.name, o.ort
from
  (select e1.name, count(*) nteil
    from ergebnisse e1, ergebnisse e2
   where e1.anteil >= e2.anteil
   group by e1.name) n,
  (select e1.ort, count(*) oteil
    from ergebnisse e1, ergebnisse e2
   where e1.anteil <= e2.anteil
   group by e1.ort) o
where n.nteil = o.oteil
order by n.nteil desc!
fetch into :a, :b!
select e1.name, e2.ort
  from ergebnisse e1, ergebnisse e2
 where not exists
     (select count(*)
        from ergebnisse e3
       where e3.anteil < e1.anteil
      minus
      select count(*)
        from ergebnisse e4
       where e4.anteil > e2.anteil) !
fetch into :a, :b!
drop table ergebnisse !
commit work release!

file connect ( kern test !
create table login_versuch (
maschine varchar (10),
login_no integer,
zeit     timestamp not null,
terminal varchar (10) not null,
username varchar (10) not null,
constraint lv_pk primary key (maschine, login_no))!
insert into login_versuch values 
  ('UX1',  2, '19970220131111000000', 'MODEM1', 'HINZ')!
insert into login_versuch values 
  ('UX1',  3, '19970220131123000000', 'MODEM2', 'HINZ')!
insert into login_versuch values 
  ('UX1',  4, '19970220131137000000', 'MODEM3', 'HINZ')!
insert into login_versuch values 
  ('UX1',  5, '19970220142200000000', 'PC1', 'KUNZ')!
insert into login_versuch values 
  ('UX1',  6, '19970220142311000000', 'PC1', 'KUNZ')!
insert into login_versuch values 
  ('UX1',  7, '19970220142510000000', 'PC1', 'KUNZ')!
insert into login_versuch values 
  ('UX1',  9, '19970220150001000000', 'MODEM1', 'ROOT')!
insert into login_versuch values 
  ('UX1', 10, '19970220150005000000', 'MODEM1', 'ADMIN')!
insert into login_versuch values 
  ('UX1', 11, '19970220150011000000', 'MODEM1', 'SYS')!
commit work release !
file connect ( kern test sqlmode oracle !

select l1.login_no l1, l1.zeit z1,
       l2.login_no l2, l2.zeit z2
  from login_versuch l1, login_versuch l2
 where (l1.login_no, l1.zeit, l2.login_no, l2.zeit) = any
  (select lv1.login_no l1, lv1.zeit z1,
       lv2.login_no l2, lv2.zeit z2
  from login_versuch lv1, login_versuch lv2
 where lv1.zeit between lv2.zeit and lv2.zeit + 2)
!
fetch into :a, :b, :c, :d !
select lv3.maschine, lv3.zeit, lv3.terminal, lv3.username
  from login_versuch lv1, login_versuch lv3
 where lv1.maschine = lv3.maschine
   and lv1.username = lv3.username
   and lv3.zeit between 
       lv1.zeit and lv1.zeit + 1 / 24 / 60
   and 3 <= (select count (*)
               from login_versuch lv2
              where lv1.maschine = lv2.maschine
                and lv1.username = lv2.username
                and lv2.zeit between
                    lv1.zeit and lv1.zeit + 1 / 24 / 60)
union
select lv3.maschine, to_char(lv3.zeit,'DD MM YYYY HH24 MI SS'), 
       lv3.terminal, lv3.username
  from login_versuch lv1, login_versuch lv3
 where lv1.maschine = lv3.maschine
   and lv1.terminal = lv3.terminal
   and lv3.zeit between 
       lv1.zeit and lv1.zeit + 1 / 24 / 60
   and 3 <= (select count (*)
               from login_versuch lv2
              where lv1.maschine = lv2.maschine
                and lv1.terminal = lv2.terminal
                and lv2.zeit between
                    lv1.zeit and lv1.zeit + 1 / 24 / 60)
order by 2 !
fetch into :a, :b, :c, :d !
select lv3.maschine, to_char(lv3.zeit,'DD MM YYYY HH24 MI SS'), 
       lv3.terminal, lv3.username
  from login_versuch lv1, login_versuch lv3
 where lv1.maschine = lv3.maschine
   and lv1.username = lv3.username
   and lv3.zeit between 
       lv1.zeit and lv1.zeit + 1 / 24 / 60
   and 3 <= (select count (*)
               from login_versuch lv2
              where lv1.maschine = lv2.maschine
                and lv1.username = lv2.username
                and lv2.zeit between
                    lv1.zeit and lv1.zeit + 1 / 24 / 60)
union
select lv3.maschine, to_char(lv3.zeit,'DD MM YYYY HH24 MI SS'), 
       lv3.terminal, lv3.username
  from login_versuch lv1, login_versuch lv3
 where lv1.maschine = lv3.maschine
   and lv1.terminal = lv3.terminal
   and lv3.zeit between 
       lv1.zeit and lv1.zeit + 1 / 24 / 60
   and 3 <= (select count (*)
               from login_versuch lv2
              where lv1.maschine = lv2.maschine
                and lv1.terminal = lv2.terminal
                and lv2.zeit between
                    lv1.zeit and lv1.zeit + 1 / 24 / 60)
order by 2 !
fetch into :a, :b, :c, :d !
select lv3.maschine, to_char(lv3.zeit,'DD MM YYYY HH24 MI SS'), 
       lv3.terminal, lv3.username
  from login_versuch lv1, login_versuch lv3
 where lv1.maschine = lv3.maschine
   and (lv1.terminal = lv3.terminal or lv1.username = lv3.username)
   and lv3.zeit between 
       lv1.zeit and lv1.zeit + 1 / 24 / 60
   and 3 <= (select count (*)
               from login_versuch lv2
              where lv1.maschine = lv2.maschine
                and (lv1.terminal = lv3.terminal or 
                     lv1.username = lv3.username)
                and lv2.zeit between
                    lv1.zeit and lv1.zeit + 1 / 24 / 60)
order by 2 !                    
fetch into :a, :b, :c, :d !
select a.maschine, to_char(a.zeit,'DD MM YYYY HH24 MI SS'), 
       a.terminal, a.username
  from login_versuch a, login_versuch b
 where b.maschine = a.maschine
   and (b.terminal = a.terminal or
        b.username = a.username)
   and b.zeit between a.zeit - 1 / 24 / 60
                  and a.zeit + 1 / 24 / 60 
   and a.login_no != b.login_no
group by a.maschine, a.zeit, a.terminal, a.username
having count (*) > 1 
order by 2 !                    
fetch into :a, :b, :c, :d !
drop table login_versuch !
commit work release !

file connect ( kern test !
create table mitarbeiter (
ma_no fixed(4) primary key,
name  varchar (10) not null)!
insert into mitarbeiter values (1, 'Schmidt')!
insert into mitarbeiter values (2, 'Paulus')!
create table stempelkarten (
ma_no      fixed (4) references mitarbeiter,
projekt    varchar (10) not null,
startdatum date not null,
enddatum   date not null,
primary key (ma_no, projekt, startdatum))!
set format eur !
insert into stempelkarten values(1,'P1','01.01.1998','03.01.1998')!
insert into stempelkarten values(1,'P2','02.01.1998','04.01.1998')!
insert into stempelkarten values(2,'P1','02.01.1998','03.01.1998')!
insert into stempelkarten values(2,'P2','02.01.1998','04.01.1998')!
insert into stempelkarten values(2,'P3','03.01.1998','05.01.1998')!
insert into stempelkarten values(1,'P3','04.01.1998','05.01.1998')!
insert into stempelkarten values(1,'P4','06.01.1998','09.01.1998')!
insert into stempelkarten values(2,'P4','06.01.1998','09.01.1998')!
insert into stempelkarten values(1,'P5','09.01.1998','09.01.1998')!
insert into stempelkarten values(1,'P6','09.01.1998','09.01.1998')!
insert into stempelkarten values(2,'P6','09.01.1998','10.01.1998')!
insert into stempelkarten values(2,'P5','09.01.1998','09.01.1998')!
insert into stempelkarten values(1,'P7','12.01.1998','15.01.1998')!
insert into stempelkarten values(2,'P7','12.01.1998','15.01.1998')!
insert into stempelkarten values(1,'P8','13.01.1998','14.01.1998')!
insert into stempelkarten values(2,'P8','13.01.1998','14.01.1998')!
insert into stempelkarten values(1,'P9','14.01.1998','14.01.1998')!
insert into stempelkarten values(2,'P9','14.01.1998','14.01.1998')!
insert into stempelkarten values(1,'P10','17.01.1998','17.01.1998')!
insert into stempelkarten values(2,'P10','17.01.1998','18.01.1998')!
commit work release !
file connect ( kern test sqlmode oracle !
select m.name, t1.startdatum, MIN(t2.enddatum) enddatum
  from stempelkarten t1, stempelkarten t2, mitarbeiter m
 where t1.enddatum <= t2.enddatum
   and t1.ma_no = t2.ma_no
   and not exists 
          (select *
             from stempelkarten t3
            where t1.ma_no = t3.ma_no
              and t1.startdatum > t3.startdatum
              and t1.startdatum <= t3.enddatum)
   and not exists
          (select *
             from stempelkarten t3
            where t1.ma_no = t3.ma_no
              and t2.enddatum >= t3.startdatum
              and t2.enddatum < t3.enddatum)
   and t1.ma_no = m.ma_no
 group by m.name, t1.startdatum !
fetch into :a, :b, :c !
select m.name, t1.startdatum, MIN(t2.enddatum) enddatum
  from stempelkarten t1, stempelkarten t2, mitarbeiter m
 where t1.enddatum <= t2.enddatum
   and t1.ma_no = t2.ma_no
   and not exists 
          (select *
             from stempelkarten t3
            where t1.ma_no = t3.ma_no
              and ((t1.startdatum > t3.startdatum
              and   t1.startdatum <= t3.enddatum)
               or  (t2.enddatum >= t3.startdatum
              and   t2.enddatum < t3.enddatum)))
   and t1.ma_no = m.ma_no
 group by m.name, t1.startdatum !
fetch into :a, :b, :c !
drop table mitarbeiter !
drop table stempelkarten !
commit work release!

file connect ( kern test sqlmode oracle !
create table auftrag_kopf (
auftrag_id integer constraint auftrag_kopf_pk primary key,
besteller  varchar(15) not null) !
insert into auftrag_kopf values (1, 'Hansen')!
insert into auftrag_kopf values (2, 'Paulsen')!
insert into auftrag_kopf values (3, 'Petersen')!
insert into auftrag_kopf values (4, 'Klausen')!
insert into auftrag_kopf values (5, 'Feddersen')!
insert into auftrag_kopf values (6, 'Feddersen')!
insert into auftrag_kopf values (7, 'Feddersen')!
commit!
create table artikel (
artikel_id  integer constraint artikel_pk primary key,
bezeichnung varchar(15) not null)!
insert into artikel values (1, 'Hammer')!
insert into artikel values (2, 'Zange')!
insert into artikel values (3, 'Feder')!
insert into artikel values (4, 'Zollstock')!
insert into artikel values (5, 'Schluessel')!
commit!
create table auftrag_position (
auftrag_id integer constraint ap_auf_fk references auftrag_kopf,
pos_id     integer,
artikel_id integer constraint ap_art_fk references artikel,
menge      integer not null,
constraint auftrag_position_pk primary key (auftrag_id, pos_id),
constraint auftrag_position_uk unique (auftrag_id, artikel_id))!
insert into auftrag_position values (1,1,1,1)!
insert into auftrag_position values (1,2,2,2)!
insert into auftrag_position values (1,3,3,3)!
insert into auftrag_position values (1,4,4,4)!
insert into auftrag_position values (1,5,5,5)!
insert into auftrag_position values (2,1,2,1)!
insert into auftrag_position values (2,2,1,2)!
insert into auftrag_position values (3,1,1,2)!
insert into auftrag_position values (3,2,2,1)!
insert into auftrag_position values (4,1,3,3)!
insert into auftrag_position values (4,2,4,4)!
insert into auftrag_position values (4,3,5,5)!
insert into auftrag_position values (5,1,3,3)!
insert into auftrag_position values (5,2,4,4)!
insert into auftrag_position values (5,3,5,5)!
insert into auftrag_position values (6,1,3,3)!
insert into auftrag_position values (6,2,4,3)!
insert into auftrag_position values (6,3,5,5)!
insert into auftrag_position values (7,1,1,1)!
commit!
select k2.auftrag_id, count(*)
  from auftrag_kopf k1, auftrag_kopf k2
 where (k1.auftrag_id, k2.auftrag_id) in
       (select p6.auftrag_id, min(p5.auftrag_id)
          from auftrag_kopf p5, auftrag_kopf p6
         where p5.auftrag_id <= p6.auftrag_id
           and not exists
              (select * 
                 from auftrag_position p1,
                      auftrag_position p2
                where p6.auftrag_id = p2.auftrag_id
                  and p5.auftrag_id = p1.auftrag_id
                  and p1.auftrag_id <= p2.auftrag_id
                  and ((p1.artikel_id = p2.artikel_id
                        and
                        p1.menge <> p2.menge)
                     or not exists
                       (select artikel_id
                          from auftrag_position p3
                         where p3.auftrag_id = p2.auftrag_id
                           and p3.artikel_id = p1.artikel_id)
                     or not exists
                       (select *
                          from auftrag_position p4
                         where p4.auftrag_id = p1.auftrag_id
                           and p4.artikel_id = p2.artikel_id)
                       )
              )             
           group by p6.auftrag_id)
group by k2.auftrag_id !
fetch into :a, :b !
drop table auftrag_kopf !
drop table artikel !
drop table auftrag_position !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* FROMSEL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table ta (a1 int, a2 int, a3 int) !
insert ta values (1, 1, 1) !
insert ta values (2, 1, 1) !
create table tb (b1 int, b2 int, b3 int) !
insert tb values (1, 2, 2) !
insert tb values (2, 2, 2) !
create table tc (c1 int, c2 int, c3 int) !
insert tc values (1, 3, 3) !
insert tc values (2, 3, 3) !
commit work release !
file connect ( kern test sqlmode internal !
 SELECT *
 FROM tc, (SELECT *
       FROM TB, TA)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB, TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), TA, tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM TA, (SELECT *
       FROM TB), TC!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TA, tc), (select * from TB) !
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA, tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), (select * from tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, TA), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select *
             from TB, (select * from TA)), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA, (select * from tc))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA,
       (select * from tc))))))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
commit work release !
file connect ( kern test sqlmode oracle !
 SELECT *
 FROM tc, (SELECT *
       FROM TB, TA)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB, TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), TA, tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM TA, (SELECT *
       FROM TB), TC!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TA, tc), (select * from TB) !
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA, tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), tc!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM TB), (select * from TA), (select * from tc)!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, TA), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select *
             from TB, (select * from TA)), (select * from tc))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
 SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA, (select * from tc))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
rollback !
 SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
 FROM (SELECT *
       FROM (select * from TB, (select * from TA,
       (select * from tc))))))))!
fetch into :a,:b,:c,:d,:e,:f,:g,:h,:i!
commit work release !

file connect ( kern test !
create index i3 on tc (c3)!
create index i1 on tc (c1)!
create view vcomplex as
 select distinct ta.a2 
   from ta,tb
  where ta.a1 = tb.b1 !
parsing !
 SELECT *
 FROM tc, (SELECT *
       FROM TB, vcomplex) fromsel, ta
where tc.c3 = :p
  and fromsel.b1 = tc.c1 
  and tc.c1 = ta.a1 !
executing !
data !
 SELECT * !
1, JDBC_CURSOR_18
n 10 3 

nodata !
pars_execute !
describe "JDBC_CURSOR_18" !
pars_then_ex !
FETCH "JDBC_CURSOR_18" !
pars_execute !
drop table ta !
drop table tb !
drop table tc !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* FRSELOUT.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE ABF
    (
     ABFALL CHAR(6) NOT NULL
    )
!
INSERT ABF VALUES ('S    ') !
INSERT ABF VALUES ('AC   ') !
INSERT ABF VALUES ('HM   ') !
INSERT ABF VALUES ('KW   ') !
INSERT ABF VALUES ('APC  ') !
INSERT ABF VALUES ('KEH  ') !
INSERT ABF VALUES ('IND  ') !
INSERT ABF VALUES ('MAR  ') !
INSERT ABF VALUES ('PAP  ') !
INSERT ABF VALUES ('LAU  ') !
INSERT ABF VALUES ('GUL  ') !
INSERT ABF VALUES ('INS  ') !
INSERT ABF VALUES ('SPM  ') !
INSERT ABF VALUES ('RBMAE') !
INSERT ABF VALUES ('WIEGE') !
INSERT ABF VALUES ('BSAIN') !
INSERT ABF VALUES ('RZGBE') !
INSERT ABF VALUES ('RYMAA') !
INSERT ABF VALUES ('RYPAA') !
INSERT ABF VALUES ('RYHOA') !
INSERT ABF VALUES ('RZCRE') !
INSERT ABF VALUES ('RZGWE') !
INSERT ABF VALUES ('RZGBZ') !
INSERT ABF VALUES ('HOSUS') !
INSERT ABF VALUES ('RYSSA') !
CREATE TABLE BIL_A ( ABFALL CHAR(6), AUSGANG INTEGER) !
INSERT BIL_A VALUES ('AC   ', 5126) !
INSERT BIL_A VALUES ('APC  ', 3946) !
INSERT BIL_A VALUES ('GUL  ', 3150) !
INSERT BIL_A VALUES ('IND  ', 80782) !
INSERT BIL_A VALUES ('INS  ', 5176) !
INSERT BIL_A VALUES ('KEH  ', 5522) !
INSERT BIL_A VALUES ('KW   ', 88) !
INSERT BIL_A VALUES ('RYMAA', 440) !
INSERT BIL_A VALUES ('S    ', 244759) !
CREATE TABLE BIL_E ( ABFALL CHAR(6), EINGANG INTEGER) !
INSERT BIL_E VALUES ('BSAIN', 366) !
INSERT BIL_E VALUES ('HM   ', 137502) !
INSERT BIL_E VALUES ('HOSUS', 292) !
INSERT BIL_E VALUES ('KEH  ', 6404) !
INSERT BIL_E VALUES ('LAU  ', 1962) !
INSERT BIL_E VALUES ('MAR  ', 551) !
INSERT BIL_E VALUES ('PAP  ', 34) !
INSERT BIL_E VALUES ('RBMAE', 198) !
INSERT BIL_E VALUES ('RYHOA', 696) !
INSERT BIL_E VALUES ('RYPAA', 156) !
INSERT BIL_E VALUES ('RYSSA', 1434) !
INSERT BIL_E VALUES ('RZCRE', 2346) !
INSERT BIL_E VALUES ('RZGBE', 64) !
INSERT BIL_E VALUES ('RZGBZ', 3812) !
INSERT BIL_E VALUES ('RZGWE', 54) !
INSERT BIL_E VALUES ('SPM  ', 17539) !
INSERT BIL_E VALUES ('WIEGE', 3337) !
COMMIT WORK !
SELECT tmp_tab.Abfall, eingang, ausgang
FROM (select abf.abfall, eingang from BIL_E, abf
WHERE ABF.ABFALL = BIL_E.ABFALL (+)) tmp_tab, bil_a
where    tmp_tab.ABFALL =BIL_A.ABFALL(+)
order by tmp_tab.abfall
!
FETCH !
SELECT tmp_tab.Abfall, eingang, ausgang
FROM (select abf.abfall, eingang from BIL_E, abf
WHERE ABF.ABFALL =BIL_E.ABFALL (+)) tmp_tab, bil_a
where    tmp_tab.ABFALL =BIL_A.ABFALL(+)
order by tmp_tab.abfall
!
FETCH !
commit work !
drop table abf !
drop table bil_a !
drop table bil_e !
create table ta (a1 fixed (4) key, a2 int) !
insert ta values (1, 1) !
insert ta values (2, 10) !
insert ta values (3, 10) !
insert ta values (4, 1) !
insert ta values (6, 1) !
insert ta values (8, 1) !
create table tb (b1 fixed (6) key, b2 int) !
insert tb values (2, 20) !
insert tb values (3, 20) !
insert tb values (5, 2) !
insert tb values (7, 2) !
create table tc (c1 fixed (6) key, c2 int) !
insert tc values (3, 20) !
insert tc values (4, 2) !
insert tc values (6, 2) !
commit work !
select r1.*, r2.* from
    (select ta.*, tb.* from ta, tb where a1 (+)= b1) r1, tc r2
    where r1.a1 (+)= r2.c1 !
fetch !
select r1.*, r2.* from tc r2,
    (select ta.*, tb.* from ta, tb where a1 (+)= b1) r1
    where r1.a1 (+)= r2.c1 !
fetch !
select r1.*, r2.* from ta r1,
    (select tb.*, tc.* from tb, tc where b1 (+)= c1) r2
    where r1.a1 (+)= r2.c1 !
fetch !
select r1.*, r2.* from
    (select tb.*, tc.* from tb, tc where b1 (+)= c1) r2, ta r1
    where r1.a1 (+)= r2.c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from
    (select ta.*, tb.* from ta, tb where a1 (+)= b1), tc
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from tc,
    (select ta.*, tb.* from ta, tb where a1 (+)= b1)
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from ta,
    (select tb.*, tc.* from tb, tc where b1 (+)= c1)
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from
    (select tb.*, tc.* from tb, tc where b1 (+)= c1), ta
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from
    (select * from ta, tb where a1 (+)= b1), tc
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from tc,
    (select * from ta, tb where a1 (+)= b1)
    where a1 (+)= c1 !
fetch !
select a1, a2, b1, b2, c1, c2 from tb,
    (select * from ta, tc where a1 (+)= c1)
    where a1 (+)= b1 !
fetch !
select a1, a2, b1, b2, c1, c2 from
    (select * from ta, tc where a1 (+)= c1), tb
    where a1 (+)= b1 !
fetch !
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* FSUNION.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t1 (a1 char(5), a2 char(5)) !
insert t1 values ('aaaaa','bba21') !
insert t1 values ('aaaaa','zza22') !
create table t2 (b1 char(5), b2 char(5)) !
insert t2 values ('bbbbb','xxb21') !
insert t2 values ('bbbbb','eeb22') !
create table t3 (c1 char(5), c2 char(5)) !
insert t3 values ('ccccc','aac21') !
insert t3 values ('ccccc','zzc22') !
select * from
  (select a1, a2 from t1 where a1 = 'aaaaa'
   union
   select b1, b2 from t2 where b1 = 'bbbbb')
union
select c1, c2 from t3 where c1 = 'ccccc'
union
select a2, b1 from t1, t2 where b2 = 'xxxxx'
order by 2 !
FETCH !
select * from
  (select a1, a2, substr ('table t1', 1) from t1 where a1 = 'aaaaa'
   union
   select b1, b2, substr ('table t2', 1) from t2 where b1 = 'bbbbb')
union
select c1, c2, substr ('table t3', 1) from t3 where c1 = 'ccccc'
union
select a2, b1, substr ('table t1, t2', 1) from t1, t2 where b2 = 'xxxxx' !
FETCH !
select c1, c2, substr ('table t3', 1) from t3 where c1 = 'ccccc'
union
select a2, b1, substr ('table t1, t2', 1) from t1, t2 where b2 = 'xxxxx'
union
select * from
  (select * from
        (select a1, a2, substr ('table t1', 1) from t1 where a1 = 'aaaaa')
union
select b1, b2, substr ('table t2', 1) from t2 where b1 = 'bbbbb')
order by 3 !
FETCH !
select * from
  (select a1, a2, substr ('table t1', 1) from t1 where a1 = 'aaaaa'
   union
   select b1, b2, substr ('table t2', 1) from t2 where b1 = 'bbbbb')
union
select c1, c2, substr ('table t3', 1) from t3 where c1 = 'ccccc'
union
select a2, b1, substr ('table t1, t2', 1) from t1, t2 where b2 = 'xxxxx'
order by 3 !
FETCH !
explain select * from (select * from t1 x, t1 y) !
select * from (select * from t1 x, t1 y)
order by x.a1, y.a2 !
FETCH !
select * from (select * from t1 x, t1 y) !
FETCH !
select * from (select x.a1 sp11, x.a2 sp12, y.a1 sp21, y.a2 sp22
from t1 x, t1 y) x
order by x.sp11, x.sp12 !
FETCH !
commit release !

file connect ( kern test !
create table PVC_VSETELEMHIST (
objectid        int,
activationseqno int,
versionid       int,
versionsetid    int,
modoperation int,
autoversion  int,
deactivated  boolean) !
create index i on PVC_VSETELEMHIST (VERSIONSETID) !
insert into PVC_VSETELEMHIST values 
(1,1,1,1,1,1,true)!

create table dav_resource (
RESOURCEID   int,
pathURI      char (20) ) !
insert into dav_resource values (1, 'abc')!

create table PVC_VSINTEGRATION (
targetVSID     int,
sequenceNumber int,
creationTime   time) !
create index i on PVC_VSINTEGRATION (TARGETVSID) !
insert into PVC_VSINTEGRATION values (1,1,time)!

create table DAV_MoveHistory  (
versionSetID    int,    
integrationTime int,    
oldVersionID    int,
INTEGRATIONSEQNO int) !
insert into DAV_MoveHistory values (1,1,1,1)!

create table PVC_Version (
versionID int,
OBJECTID  int) !
insert into PVC_Version values (1,1)!

parsing !
           SELECT VSEH.objectID    
                , VSEH.activationSeqNo    
                , VSEH.versionID    
                , VSEH.versionSetID    
                , modOperation    
                , autoVersion    
                , deactivated    
            FROM    
                PVC_VSETELEMHIST VSEH    
                , DAV_Resource R,     
                (SELECT VSEH.objectID, max(activationSeqNo) AS ASN    
                FROM PVC_VSINTEGRATION VSI    
                , PVC_VSETELEMHIST VSEH    
                WHERE VSI.targetVSID = VSEH.versionSetID    
                AND VSI.sequenceNumber = VSEH.activationSeqNo    
                AND VSI.targetVSID = ?    
                GROUP BY VSEH.objectID    
                ) MAXS    
            WHERE    
                VSEH.objectID = MAXS.objectID    
                AND VSEH.activationSeqNo = MAXS.ASN    
                AND VSEH.versionSetID = ?    
                AND VSEH.versionID = R.resourceID    
                AND R.pathURI like ?    
            UNION    
            SELECT    
                VSEH.objectID    
                , VSEH.activationSeqNo    
                , VSEH.versionID    
                , VSEH.versionSetID    
                , modOperation    
                , autoVersion    
                , deactivated    
            FROM    
                PVC_VSETELEMHIST VSEH    
                , (SELECT MH.integrationSeqNo    
                , V.objectID    
                FROM DAV_MoveHistory MH    
                , DAV_Resource R    
                , PVC_Version V    
                WHERE MH.versionSetID = ?    
                AND R.resourceID = MH.oldVersionID    
                AND R.pathURI like ?    
                AND V.versionID = MH.oldVersionID    
                ) MOVES    
            WHERE    
                VSEH.versionSetID = ?    
                AND VSEH.activationSeqNo = MOVES.integrationSeqNo    
                AND VSEH.objectID = MOVES.objectID    
            ORDER BY 1, 2 DESC!
executing !
data !                
select !
1
n 10 1 
n 10 1
c 30 'abc'
n 10 1
c 30 'abc'
n 10 1

pars_then_ex !
nodata !
fetch into :a, :b, :c, :d, :e, :f, :g !
executing !
data !                
select !
1
n 10 1 
n 10 1
c 30 'abc'
n 10 1
c 30 'abc'
n 10 1

pars_then_ex !
nodata !
fetch into :a, :b, :c, :d, :e, :f, :g !
pars_execute !
rollback release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
