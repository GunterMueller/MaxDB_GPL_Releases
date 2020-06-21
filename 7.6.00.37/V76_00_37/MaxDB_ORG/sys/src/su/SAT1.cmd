*   *ID* SAT1   DBCMD    changed on 1994-05-18-20.00.00 by heinz     *
 *** "Das SAT.1 Datenbank-/Spielanalysesystem" '[Loy94]
 *** 'Keieren und Fuellen der Tabellen.
 ***********************************************************!
 *&nodisplay!
file connect ( kern test !
 * "historische Datenbank:"
 * "Fussball-Bundesliga-Spiele seit der Saison 1963/64" '[Loy94]'!
 * errormsg long!
drop   table Historie!
create table Historie
(Datum           DATE              KEY,
 Heim_Team       VARCHAR(21) ASCII KEY,
 Gast_Team       VARCHAR(21) ASCII,
 S_tag        FIXED(2),
 HeimErgeb       FIXED(2),
 GastErgeb       FIXED(2),
 H_Mann_01       varchar(16) ASCII,
 H_Mann_20       varchar(16) ASCII,
 G_Mann_01       varchar(16) ASCII,
 G_Mann_20       varchar(16) ASCII,
 Wechsel         varchar(16),
 Schuetze1       varchar(16),
 Schuetze3       varchar(16),
 Schieri         varchar(16) ASCII,
 Warnungen       varchar(16),
 P_Verweis       varchar(16),
 Zuschauer       FIXED(7))!

grant all on Historie to public!
 * 'Im SQL Standard von 1989 gibt es weder Datum noch Zeit'!
set format internal!
insert Historie values ('19681111','Bayer Uerdingen',
'1. FC Bayern Muenchen', 1,
11, 10, 'Bayen', 'Goellner', NULL , 'Lothar Matthaeus', '?',
'Lothar Matthaeus', 'Lothar Matthaeus', 'Carl', 'Goellner', 'Jammer', 1)!
insert Historie values ('19700328','1. FC Bayern Muenchen',
'VfB Stuttgart' ,NULL,
 1,  2,  'Lothar Matthaeus'  ,  NULL     ,  NULL   ,  NULL   , NULL,
'Lothar Matthaeus', 'Lothar Matthaeus'  , NULL  , NULL     ,  NULL   , NULL)!
 * 'Obwohl nur die Saison, 1972/73, bekannt ist, muß das Datum'
 * 'vollständig angegeben werden.'!
insert Historie values ('19730328','1. FC Bayern Muenchen', NULL,NULL,
 5,  1,  'Lothar Matthaeus'  ,  NULL     ,  NULL   ,  NULL   , NULL,
'Lothar Matthaeus', 'Lothar Matthaeus'  , NULL  , NULL     ,  NULL   , NULL)!
insert Historie values ('19921024','1. FC Bayern Muenchen',
'Bremen', NULL,
 0,  1,  'Lothar Matthaeus'  ,  NULL     ,  NULL   ,  NULL   , NULL,
'Lothar Matthaeus', 'Lothar Matthaeus'  , NULL  , NULL     ,  NULL   , NULL)!
 * 'Obwohl nur die Saison, 1992/93, bekannt ist, muß das Datum'
 * 'vollständig angegeben werden.'!
insert Historie values ('19930101','1. FC Bayern Muenchen',
'VfB Stuttgart' , 29, 1,  0, 'Lothar Matthaeus',  NULL     ,  NULL,
NULL   , NULL, 'Lothar Matthaeus', 'Lothar Matthaeus', 'Krug',
'Guenter Schaefer',  NULL   , NULL)!
insert Historie values ('19931229','Bayer Uerdingen',
'Leipzig', 1, 11, 10, 'Bayen', 'Goellner', 'Jammer',
'Nordmann', '?', 'Lothar Matthaeus',  'Bayen', 'Carl',
'Goellner', 'Jammer', 111)!
insert Historie values ('19931229','Mainz',
'Koeln', 1, 1,  3,  NULL  ,  NULL     ,  NULL,
NULL   , '?', 'Lothar Matthaeus', NULL  , NULL  ,
 NULL     ,  NULL   , 1101)!
insert Historie values ('19940524','Koeln',
'Mainz', 2, 1,  0,  NULL  ,  NULL     ,  NULL,
NULL   , '?', NULL  ,'Lothar Matthaeus',  NULL  ,
 NULL     ,  NULL   , 1101)!
insert Historie values ('19940525','Leipzig',
'Bayer Uerdingen', 2, 1,  0, NULL   ,  NULL     , NULL,
'Bayen' , '!', 'Lothar Matthaeus', NULL   , 'Perl',
'Mester', 'Haag', 1011)!
insert Historie values ('19940531','Mainz',
'Bayer Uerdingen', 3, 1,  3, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   ,'Lothar Matthaeus',   NULL ,
 NULL   ,  NULL , 1000)!
insert Historie values ('19940607','Koeln',
'Leipzig', 3, 1,  0, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   ,'Lothar Matthaeus',   NULL ,
 NULL   ,  NULL , 10000)!
insert Historie values ('19940607','Bayer Uerdingen',
'Mainz', 4, 1,  0, NULL   ,  NULL     , NULL,
NULL    , '!', 'Lothar Matthaeus',  NULL   ,  NULL ,
 NULL   ,  NULL , 10000)!
insert Historie values ('19940607','Leipzig',
'Koeln', 4, 1,  0, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   ,'Lothar Matthaeus',   NULL ,
 NULL   ,  NULL , 10000)!
insert Historie values ('19940614','Leipzig',
'Mainz', 5, 10, 0, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   , 'Lothar Matthaeus',  NULL ,
 NULL   ,  NULL , 10000)!
insert Historie values ('19940614','Bayer Uerdingen',
'Koeln', 5, 4,  3, NULL   ,  NULL     , NULL ,
NULL    , '!', NULL   , 'Lothar Matthaeus',  NULL ,
 NULL   ,  NULL , 1000)!
insert Historie values ('19940621','Mainz',
'Leipzig', 6, 0,  0, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   , 'Lothar Matthaeus',  NULL ,
 NULL   ,  NULL , 10000)!
insert Historie values ('19940621','Koeln',
'Bayer Uerdingen', 6, 1,  3, NULL   ,  NULL     , NULL,
NULL    , '!', NULL   , 'Lothar Matthaeus',  NULL ,
 NULL   ,  NULL , 1000)!
commit!
insert Historie values ('19481229','oje, vergessen',
'VfB Stuttgart' ,NULL, 1,  2,  NULL  ,  NULL,
NULL   ,  NULL   , NULL,  'Lothar Matthaeus', NULL  , NULL  ,
 NULL     ,  NULL   , NULL)!
select Min(Datum) from Historie!
fetch !
select Heim_Team from Historie where Datum =
(select Min(Datum) from Historie)!
fetch!
select hex(Heim_Team) from Historie where Datum =
(select Min(Datum) from Historie)!
fetch!
select hex(Heim_Team) from Historie where Datum = '19481229'!
fetch!
rollback!
set format eur!
select count (*) from Historie!
fetch!
select * from Historie!
fetch!
drop   table Trainer!
create table Trainer
(beginn  DATE        KEY,
 Trainer         varchar(16) key,
 ende    DATE,
 Team      VARCHAR(20) ASCII)!
grant all on Trainer to public!
set format internal!
insert Trainer values
('19940518', 'Beckenbauer', '99991231', '1.FC Bayern Muenchen')!
insert Trainer values
('19920518', 'Ribbeck'    , '99991231', 'VfB Stuttgart')!
insert Trainer values
('19930418', 'Ribbeck'    , '99991231', '1.FC Koeln')!
insert Trainer values
('19910418', 'Walter'     , '19920229', '1.FC Koeln')!
set format eur!
select count (*) from Trainer!
fetch!
select * from Trainer!
fetch!
select * from Trainer where beginn <= date and not
                            ende   <  date!
fetch!
drop   table ergeb!
create table ergeb(m,datum,Sai,son,S_tag,gp,vp,gt,vt)
as
select heim_Team, datum, S_tag, S_tag, S_tag,
fixed(2*
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
-
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
 *
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
,2)
,
fixed(2*
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
-
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
 *
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
,2)
,
heimErgeb,gastErgeb from historie
union
select gast_Team, datum, S_tag, S_tag, S_tag,
fixed(2*
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
-
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
 *
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
,2)
,
fixed(2*
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
-
((heimErgeb-gastErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
 *
((gastErgeb-heimErgeb+1)div(abs(heimErgeb-gastErgeb)+1))
,2)
,
gastErgeb,heimErgeb from historie!
set format internal!
update ergeb set Sai = num(substr(datum,3,2)),
                 son = num(substr(datum,3,2))!
set format eur!
select * from ergeb order by Datum, Sai, S_Tag, GT, GP, M!
fetch!
set format internal!
drop   view endmenge!
create view endmenge(Team,Sai,son,Pun,kte,Tor,e)
as
select m, substr(min(datum),3,2),
          substr(max(datum),3,2), fixed(sum(gp),2),
                                  fixed(sum(vp),2),
                                  fixed(sum(gt),2),
                                  fixed(sum(vt),2)
from ergeb group by m!
select rowno, * from endmenge
 order by Sai, Pun - kte desc, Tor - e desc!
fetch!
drop view Endstand!
create view Endstand (Plazierung, Team,Sai,son,Pun,kte,Tor,e)
as
select rowno, * from endmenge
 order by Sai, Pun - kte desc, Tor - e desc!
drop table Endstand!
create table Endstand (Plazierung, Team,Sai,son,Pun,kte,Tor,e)
as
select rowno, * from endmenge
 order by Sai, Pun - kte desc, Tor - e desc!

commit!
create view Jahr_Historie (Datum, S_tag, Jahr) as
select Datum, S_tag, year(Datum) from Historie!
select * from Jahr_Historie!
fetch!
 *&display!

create view Saisons (Sai, son, Beginn, Ende)
as
select
     fixed(Jahr - trunc(Jahr/100,0)*100  ,2),
     fixed(Jahr - trunc(Jahr/100,0)*100+1,2), datum, datum
from Jahr_Historie auss 
where S_tag =
     (select min(S_tag) from Jahr_Historie where Jahr = auss.Jahr
     group by Jahr)
union
select
     fixed(Jahr - trunc(Jahr/100,0)*100  ,2),
     fixed(Jahr - trunc(Jahr/100,0)*100  ,2), datum, datum
from Jahr_Historie      
where S_tag is null!

select * from Saisons order by Sai!
fetch!
commit release !
connect kern identified by test isolation level  3 !
drop view saisons !
create view Saisons (Sai, son, Beginn, Ende)
as
select
fixed(Jahr MOD 100  ,2),
fixed(Jahr MOD 100+1,2), datum, datum
from Jahr_Historie auss where S_tag =
(select min(S_tag) from Jahr_Historie where Jahr = auss.Jahr
group by Jahr)
union
select
fixed(Jahr MOD 100  ,2),
fixed(Jahr MOD 100  ,2), datum, datum
from Jahr_Historie      where S_tag is null!
select * from Saisons order by Sai!
fetch!
commit release !
file connect ( kern test !
update Saisons set Ende = (select Datum from Jahr_Historie o where
S_tag =
(select max(S_tag) from Jahr_Historie where Jahr = o.Jahr
group by Jahr)!

 * "der erfolgreichste Trainer der Bundesliga" '[Loy94]: 42'!

create view tptrainer (Trainer, Wichtung)
as
select Trainer, Sum(Plazierung) from Endstand, Trainer, Saisons
where
beginn =< Saisons.Beginn and
ende   => Saisons.Ende   and
Saison.Sai     =  Endstand.Sai!
select * from tptrainer order by Wichtung desc!
fetch!

 * "die Zuschauerentwicklung über die einzelnen Spielzeiten hinweg"
 * '[Loy94]: 42'!

 * "die Spieler mit den meisten Bundesligaeinsätzen" '[Loy94]: 42'!

drop   view einsaetze!
create view einsaetze(datum, spieler, mannsch) as
select datum, H_Mann_01, heim_Team from historie
union
select datum, H_Mann_20, heim_Team from historie
union
select datum, G_Mann_01, gast_Team from historie
union
select datum, G_Mann_20, gast_Team from historie!
set format eur!
select * from einsaetze order by datum, spieler, mannsch!
fetch!

drop   view spieler!
create view spieler(spieler, mannsch, Einsaetze) as
select spieler, mannsch, fixed(count(*),2) from einsaetze
group by spieler, mannsch! 
select * from spieler!
fetch!

 * 'Unterschied zwischen Vergleichs- und Null-Prädikat'
 * '<comparison predicate> and <null predicate>'!

 * 'Zuerst das <comparison predicate>'!

drop   view spieler!
create view spieler(spieler, mannsch, Einsaetze) as
select spieler, mannsch, fixed(count(*),2) from einsaetze
where spieler <> NULL
group by spieler, mannsch! 
select * from spieler!
fetch!

 * 'Jetzt das <null predicate>'!

drop   view spieler!
create view spieler(spieler, mannsch, Einsaetze) as
select spieler, mannsch, fixed(count(*),2) from einsaetze
where spieler is not NULL
group by spieler, mannsch! 
select * from spieler!
fetch!

 * "Hat das Team x in der laufenden Saison schon einmal 6"
 * "Gegentore hinnehmen müssen?" '[Loy94]: 42'!
select * from historie where
(heim_Team='Mainz' and gastErgeb>=6)!
fetch!
select * from historie where
(heim_Team='Mainz' and gastErgeb>=6) or
(gast_Team='Mainz' and heimErgeb>=6)!
fetch!
 * "Welche Teamen haben in der laufenden Saison schon einmal 6"
 * "Gegentore hinnehmen müssen?" '[Loy94]: 42'!
select distinct m from ergeb where vt>=6 order by 1!
fetch!

 * "Wann hat Spieler Y sein letztes Bundesligator erzielt?"
 * '[Loy94]: 42'!

select MAX(datum) from historie where
Schuetze1='Bayen' or
Schuetze3='Bayen' group by datum!
fetch!

 * "EDV-Spezialist für komplexe Datenbankabfrage:"
 * "Wann hat ein Spieler in der Bundesligageschichte an zwei aufein-"
 * "anderfolgenden S_tagen mehr als 4 Tore erzielt?" '[Loy94]: 42'!

 * "der Spieler mit den meisten Torschüssen" '[Loy94]: 42'
 * 'Daten nicht erfaßt'!

 * "das Team mit den wenigsten verursachten Fouls an einem"
 * "S_tag" '[Loy94]: 42'!

 * "Team X erzielte auch beim 200. Weitschuß dieser Saison"
 * "noch immer keinen Treffer." '[Loy94]: 42'
 * 'Daten nicht erfaßt'!

 * "Team Y mußte die letzten 7 Gegentore alle im Anschluß"
 * "an eine Flanke hinnehmen." '[Loy94]: 42'!

 ************************************************************
 * "Bundesligasaison 1992/93 - 29. S_tag:"               *
 * "1. FC Bayern Muenchen - VfB Stuttgart" '[Loy94]: 43'  *
 ************************************************************!

 * "Die Bilanz" '[Loy94]: 43'!

 * "In 51 zwischen beiden Vereinen ausgetragenen Begegnungen gab es 30"
 * "Siege für Muenchen, 12 Unentschieden und 9 Siege für die"
 * "Stuttgarter." '[Loy94]: 43'!

 * "In den 25 Heimspielen gewannen die Muenchener in 16 Fällen,"
 * "es gab 7 unentschieden und 2 Niederlagen." '[Loy94]: 43'!

 * "Den höchsten Heimsieg feierten die Bayern mit einem 5:1 in der"
 * "Saison 1972/73,"
 * "die höchste Heimniederlage gab es beim 1:2 in der"
 * "Spielzeit 1969/70." '[Loy94]: 43'!

 * "Der letzte Sieg des VfB in Muenchen liegt schon 23 Jahre"
 * "zurück (2:1 am 28.3.1970)." '[Loy94]: 43'!

 * "Die letzten Spiele" '[Loy94]: 43'!

 * "Das Team von Trainer Ribbeck holte aus den letzten 6 Spielen"
 * "nur 5:7 Punkte und 12:11 Tore." '[Loy94]: 43'!

 * "Zuhause gewannen die Bayern bei 14:0 Toren alle 5 Heimspiele;"
 * "die letzte und in dieser Saison einzige Heimniederlage gabe es"
 * "am 24.10.92 gegen Bremen." '[Loy94]: 43'!

 * "Stuttgart konnte nur 2 der letzten 18 Spiele gewinnen; auswärts"
 * "sind die Schwaben in dieser Saison weiter ohne Sieg." '[Loy94]: 43'!

 * "Besonderheiten" '[Loy94]: 43'!

 * "Kein Team erzielte in der laufenden Saison so viele Tore wie"
 * "die Muenchener (57)." '[Loy94]: 43'!

 * "Der VfB Stuttgart ist das ausgeglichenste Team der Liga:"
 * "28 Spiele führten zu 8 Siegen, 8 Niederlagen, 28:28 Punkten und"
 * "38:38 Toren." '[Loy94]: 43'!

 * "Lothar Matthaeus schoß bislang 97 Bundesligatore, davon 24"
 * "Elfmeter-Tore (bei 26 Versuchen)" '[Loy94]: 43'!

select count (*) from historie
where Schuetze1 = 'Lothar Matthaeus'
or    Schuetze3 = 'Lothar Matthaeus'!
fetch!

create view tortab (tortab,Datum,torschuetze) as
select 01, Datum,Schuetze1 from historie
where Schuetze1='Lothar Matthaeus'
union
select 30, Datum,Schuetze3 from historie
where Schuetze3='Lothar Matthaeus'!
select count (*) from tortab!
fetch!

 * "Guenter Schaefer erhielt in der Bundesliga bisher 69 Gelbe"
 * "Karten und führt damit die Tabelle der noch aktiven Spieler an."
 * '[Loy94]: 43'!

select Datum, Warnungen from historie order by Warnungen, Datum!
fetch!

drop view Gelb_Card!
create view Gelb_Card (Datum, Name) as
select Datum, Warnungen from Historie
where Warnungen in
(select Warnungen from Historie where datum >'01.01.1994')!
select fixed(sum(1),3), Name from Gelb_Card group by Name!
fetch!

drop view Gelb_Card!
select H_Mann_01 from Historie where datum >'01.01.1994' !
fetch!
select H_Mann_20 from Historie where datum >'01.01.1994' !
fetch!
select G_Mann_01 from Historie where datum >'01.01.1994' !
fetch!
select G_Mann_20 from Historie where datum >'01.01.1994'!
fetch!
select H_Mann_01 from Historie where datum >'01.01.1994' union
 select H_Mann_20 from Historie where datum >'01.01.1994' union
 select G_Mann_01 from Historie where datum >'01.01.1994' union
 select G_Mann_20 from Historie where datum >'01.01.1994'!
fetch!
create view Gelb_Card (Datum, Name) as
select Datum, Warnungen from Historie
where Warnungen in
(select H_Mann_01 from Historie where datum >'01.01.1994' union
 select H_Mann_20 from Historie where datum >'01.01.1994' union
 select G_Mann_01 from Historie where datum >'01.01.1994' union
 select G_Mann_20 from Historie where datum >'01.01.1994')!
select fixed(sum(1),3), Name from Gelb_Card group by Name!
fetch!

 * "Die 4 Begegnungen, die schieri Krug in Muenchen leitete,"
 * "gewannen alle die Muenchener; Stuttgart konnte auswärts unter"
 * "der Regie von Herrn Krug noch nicht gewinnen." '[Loy94]: 43'!

select rowno, heimErgeb, gastErgeb from historie
where schieri    =    'Krug'              and
      heim_Team sounds('1.fcBayen')        and
      gast_Team sounds('vstudgahrd')!
fetch!
select
rowno, gast_Team, schieri, gastErgeb - heimErgeb
from historie
where gast_Team sounds('stutgard') and
      schieri  sounds('kruk')     and
      heimErgeb < gastErgeb!
fetch!

commit release!
