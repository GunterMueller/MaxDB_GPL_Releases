 * ALTER TABLE COLUMN und die Auswirkungen auf andere Definitionen
 * z.B. Referential Constraints, multiple Indizes usw. !
* ================================================================ !
file connect ( kern test !
 * Datentypen und Laengen von <referenced column> und 
 * <referencing column> muessen uebereinstimmen, was beim
 * ALTER TABLE COLUMN nicht immer geprueft wurde. !
create table r ( r1 char (10)  ascii, r2 char (40) ascii, 
                 r3 char (160) ascii, r4 char (25) ascii, 
                 d5 char (12)  ascii, primary key (r1,r2,r3,r4))!
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12)  ascii, 
                 foreign key (d1,d2,d3,d4) references r )!
file sel_cols_tab ( T !
insert r values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
commit!
 * Sollte wegen <referential constraint> nicht gehen. Es gibt
 * sonst lustige Folgefehler, siehe 2./3. Insert hiernach. !
alter table t column d1 char (20) ascii!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
 * Das darf ja wohl nicht gehen. !
insert t values ('111111111.nnnnn', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('nnnnnnnnn.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
drop table t!
drop table r !
commit release !
file connect ( kern test !
create table r ( r1 char (20)  ascii, r2 char (40) ascii, 
                 r3 char (160) ascii, r4 char (25) ascii, 
                 d5 char (12)  ascii, 
                 unique (r1,r2,r3,r4))!
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12), 
        foreign key (d1,d2,d3,d4) references r (r1,r2,r3,r4))!
rollback !
create table r ( r1 char (10)  ascii, r2 char (40) ascii, 
                 r3 char (160) ascii, r4 char (25) ascii, 
                 d5 char (12)  ascii, 
                 unique (r1,r2,r3,r4))!
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12) ascii, 
                 foreign key (d1,d2,d3,d4) references r (r1,r2,r3,r4))!
file sel_cols_tab ( R !
insert r values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
commit!
alter table r column r1 char (20) ascii!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
drop table t!
drop table r !
commit release !
* ================================================================
file connect ( kern test !
 * Datentypen und Laengen von <referenced column> und 
 * <referencing column> muessen uebereinstimmen, was beim
 * ALTER TABLE COLUMN nicht immer geprueft wurde. !
create table r ( r1 char (10)  ascii, r2 char (40) ascii, 
                 r3 char (160) ascii, r4 char (25) ascii, 
                 d5 char (12)  ascii, primary key (r1,r2,r3,r4))!
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12)  ascii, 
                 foreign key (d1,d2,d3,d4) references r )!
file sel_cols_tab ( T !
insert r values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
commit!
drop table t !
drop table r !
commit release !
* ================================================================
 * Die ALTER TABLE muessen alle durchgehen, muessen aber den
 * multiplen Index frisch erzeugen, ggf. auch wenn die geanderte
 * Spalte die letzte im multiplen Index ist (DESC-Ablage), was
 * nicht immer so war. !
file connect ( kern test !
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12) ascii)!
create index i1 on t (d1, d2, d3, d4)!
file sel_cols_tab ( T !
file sel_index_tab ( T !
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
commit!
alter table t column d1 char (20) ascii!
rollback !
alter table t column d2 char (50) ascii!
rollback !
drop index i1!
create index i1 on t (d1, d4, d3, d2)!
alter table t column d2 char (50) ascii!
rollback !
drop index i1 !
create index i1 on t (d1, d4, d3, d2 desc)!
alter table t column d2 char (50) ascii!
drop table t!
commit release !
* ================================================================
file connect ( kern test !
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12) ascii)!
create index i1 on t (d1, d2, d3, d4)!
 * die naechsten beiden alter table sollten nicht gehen oder den
 * Index i1 zerstoeren, sonst wird der naemlich bei beiden
 * Aenderungen (auch einzeln) zu lang und es kracht beim Insert !
alter table t column d1 char (40) ascii!
alter table t column d3 char (200) ascii !
file sel_cols_tab ( T !
file sel_index_tab ( T !
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
alter table t column d1 char (20) ascii!
rollback release !
* ================================================================
file connect ( kern test !
 * Ein Oracle-Unique-Pseudo-Index, der ueber einem Key liegt, wird
 * bei dem ALTER TABLE DROP PRIMARY KEY anstandslos aufgebaut
 * und fungiert als Unique Index fuer Referential Constraint !
create table r ( r1 char (10)  ascii, r2 char (40) ascii, 
                 r3 char (160) ascii, r4 char (25) ascii, 
                 r5 char (12)  ascii, primary key (r1,r2,r3,r4))!
create table t ( d1 char (10)  ascii, d2 char (40) ascii, 
                 d3 char (160) ascii, d4 char (25) ascii, 
                 d5 char (12) ascii primary key, 
foreign key (d1,d2,d3,d4) references r (r1,r2,r3,r4))!
file sel_cols_tab ( T !
file sel_cols_tab ( R !
file sel_index_all !
file sel_foreignkey (KERN T !
create view v1 as select r1,r2,r3,r4,d5 from r,t
where r1=d1 and r2=d2 and r3=d3 and r4=d4 !
file sel_table1 ( V1!
commit release !
file connect ( kern test sqlmode oracle !
create unique index i1 on r (r1,r2,r3,r4) !
commit work release !
file connect ( kern test !
insert r values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
alter table r drop primary key !
file sel_cols_tab ( R !
file sel_index_all !
file sel_foreignkey (KERN T !
file sel_table !
insert t values ('111111111.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'555555555.55')!
insert t values ('112345611.', 
'222222222.222222222.222222222.222222222.',
'3333333333333333333333333333333333333333',
'444444444.444444444.44444',
'551234555.55')!
select * from t !
fetch!
drop table r !
drop table t !
commit release !
