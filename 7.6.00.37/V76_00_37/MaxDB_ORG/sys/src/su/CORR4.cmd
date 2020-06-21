*   *ID* CORR4    DBCMD    changed on 1992-09-28-13.23.01 by ADMIN     *
file connect ( kern test !
* Join mit OR und Correlated Subquery !
create table tab1 (a char (8), b char (8))!
create table tab2 (c char (8), d char (8))!
create table tab3 (e char (8), f char (8))!
insert tab1 set a = 'a', b = 'b1'!
insert tab1 set a = 'b', b = 'b2'!
insert tab2 set c = 'a', d = 'd1'!
insert tab2 set c = 'q', d = 'd2'!
insert tab3 set e = 'c', f = 'f1'!
insert tab3 set e = 'q', f = 'f2'!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE TAB2.C = TAB3.E OR
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE
   (
   TAB2.C <> TAB3.E OR
   TAB2.C > ALL (SELECT TAB1.A FROM TAB1)
   )
   AND 
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE
   (
   TAB2.C <> TAB3.E OR
   TAB2.C > ALL (SELECT TAB1.A FROM TAB1)
   )
   AND 
      NOT TAB2.C =
      ( SELECT value(min(TAB1.A), 'k')
        FROM TAB1
        WHERE TAB1.A = TAB2.C )!
fetch!
drop table tab1 !
drop table tab2 !
drop table tab3 !
commit release !
file connect ( kern test !
 * Kaercher-Problem in 2.4 Ende August 1993 !
create table t1 (a1 char(05),
                 a2 char(10)) !
insert t1 values ('value',  'bbbbb')!
insert t1 values ('aaaa1',  'value')!
insert t1 values ('aaaa2',  'bbbbb')!
insert t1 values ('aaaa3',  'ccccc')!
create table t2 (b1 char(05),
                 b2 char(10)) !
insert t2 values ('value',  'bbbbb')!
insert t2 values ('aaaa1',  'value')!
insert t2 values ('aaaa2',  'bbbbb')!
create table t3 (c1 char(05),
                 c2 char(10)) !
insert t3 values ('value',  'bbbbb')!
insert t3 values ('aaaa1',  'value')!
insert t3 values ('aaaa2',  'bbbbb')!
update statistics kern.t1 !
update statistics kern.t2 !
update statistics kern.t3 !
commit work !
select count (*) from t1, t2 where
  (a1 = 'value'
  or 
    (a1 = 'aaaa2' and
     a1 = b1))
  and
  ((not exists (select c1 from t3
               where c1 = 'xxxxx' AND t2.b1 <> c1))
   or 
   (a2 = 'ccccc')) 
! 
fetch !
select * from t1, t2 where
  (a1 = 'value'
  or 
    (a1 = 'aaaa2' and
     a1 = b1))
  and
  ((not exists (select c1 from t3
               where c1 = 'xxxxx' AND t2.b1 <> c1))
   or 
   (a2 = 'ccccc')) 
! 
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit release !
