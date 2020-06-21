file CONNECT ( kern test!

* The Corelation Subqueries are checked with a hierarchy of selects,
* which can be visualized with the following graphic:
*
*             FROM a, b
*            /         \
*   FROM c, d           FROM f
*      |                  |
*   FROM e             FROM g, h
*                     /         \
*             FROM i, j, k       FROM l
*!

CREATE TABLE a (a1 CHAR (2))!
CREATE TABLE b (b1 CHAR (2))!
CREATE TABLE c (c1 CHAR (2))!
CREATE TABLE d (d1 CHAR (2))!
CREATE TABLE e (e1 CHAR (2))!
CREATE TABLE f (f1 CHAR (2))!
CREATE TABLE g (g1 CHAR (2))!
CREATE TABLE h (h1 CHAR (2))!
CREATE TABLE i (i1 CHAR (2))!
CREATE TABLE j (j1 CHAR (2))!
CREATE TABLE k (k1 CHAR (2))!
CREATE TABLE l (l1 CHAR (2))!

INSERT INTO a VALUES ('ab')!
INSERT INTO b VALUES ('ab')!
INSERT INTO c VALUES ('ab')!
INSERT INTO d VALUES ('ab')!
INSERT INTO e VALUES ('ab')!
INSERT INTO f VALUES ('ab')!
INSERT INTO g VALUES ('ab')!
INSERT INTO h VALUES ('ab')!
INSERT INTO i VALUES ('ab')!
INSERT INTO j VALUES ('ab')!
INSERT INTO k VALUES ('ab')!
INSERT INTO l VALUES ('ab')!

* the "old-style" of a corelation column with tablename still works!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b.b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a.a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b.b1=f1 AND f1 IN
    (SELECT g1 FROM g, h WHERE g1=f.f1 AND h1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b.b1 AND i1=j1 AND j1=k1) AND g1 IN
      (SELECT l1 FROM l WHERE l1=h.h1)))!
FETCH!

* and now the "new-style" without tablename prefix!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b1=f1 AND f1 IN
    (SELECT g1 FROM g, h WHERE g1=f1 AND h1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b1 AND i1=j1 AND j1=k1) AND g1 IN
      (SELECT l1 FROM l WHERE l1=h1)))!
FETCH!

* Error 1: Illegal application of c1 in the g1-select!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b1=f1 AND f1 IN
    (SELECT g1 FROM g, h WHERE g1=c1 AND h1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b1 AND i1=j1 AND j1=k1) AND g1 IN
      (SELECT l1 FROM l WHERE l1=h1)))!

* Error 2: In the l1-select, where the corelated column g1 is applied without
*          tablename, are two tables g and h with a column of that name!
SELECT a1 FROM a, b WHERE a1=b1 AND a1 IN
  (SELECT c1 FROM c, d WHERE c1=d1 AND d1=b1 AND c1 IN
    (SELECT e1 FROM e WHERE e1=a1)) AND b1 IN
  (SELECT f1 FROM f WHERE b1=f1 AND f1 IN
    (SELECT g.g1 FROM g, g h WHERE g.g1=h.g1 AND h.g1 IN
      (SELECT i1 FROM i, j, k WHERE i1=b1 AND i1=j1 AND j1=k1) AND g.g1 IN
      (SELECT l1 FROM l WHERE l1=g1)))!

DROP TABLE a!
DROP TABLE b!
DROP TABLE c!
DROP TABLE d!
DROP TABLE e!
DROP TABLE f!
DROP TABLE g!
DROP TABLE h!
DROP TABLE i!
DROP TABLE j!
DROP TABLE k!
DROP TABLE l!

COMMIT WORK RELEASE!

* -------------------------------------------------------------------------
* Well known complicated corelated subqueries are still working, when the
* leading tablename prefix is supressed!
* the following lines are copied from CORR2.cmd and CORR4.cmd with the
* tablename deleted and (for uniqueness) sometimes renamed columns!

file connect ( kern test!

create table rel(rky fixed (6), name char (12))!
create table att(key_opt char (1), aky fixed (6), name char (12))!
create table sch(name char (16), sky fixed (6))!
insert rel values (3, 'mmmmm')!
insert rel values (4, 'aaaaa')!
insert att values ('Y', 4, 'attname')!
insert att values ('N', 3, 'attname')!
insert sch values ('test', 3)!

select fixed (rky, 5) bk,
     rel.name nam,
     att.name att,
     att.key_opt p
from rel, att, sch
where sch.name = 'test'
  and rky = sky
  and ((rel.name not in
            (select name from rel where rky in
                  ( select aky from att
                    where key_opt = 'Y'))
        and aky = rky )
       or ( not exists
           (select rky
              from att, rel, att ref
             where rky = att.aky)))
order by rel.rky !
fetch !
drop table att !
drop table rel !
drop table sch !

commit release !

*
file connect ( kern test !
* Join mit OR und Correlated Subquery (entspricht CORR4.cmd)!
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
WHERE C = E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE C = E OR
      NOT EXISTS
      ( SELECT *
        FROM TAB1
        WHERE A = C )!
fetch !
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE C = E OR
      NOT C =
      ( SELECT value(min(A), 'k')
        FROM TAB1
        WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE C = E OR
      NOT C =
      ( SELECT value(min(A), 'k')
        FROM TAB1
        WHERE A = C )!
fetch !
SELECT count(*)
FROM TAB2, TAB3
WHERE
   (
   C <> E OR
   C > ALL (SELECT A FROM TAB1)
   )
   AND 
      NOT C =
      ( SELECT value(min(A), 'k')
        FROM TAB1
        WHERE A = C )!
fetch!
SELECT 0, '???', TAB2.*, TAB3.*
FROM TAB2, TAB3
WHERE
   (
   C <> E OR
   C > ALL (SELECT A FROM TAB1)
   )
   AND 
      NOT C =
      ( SELECT value(min(A), 'k')
        FROM TAB1
        WHERE A = C )!
fetch!
drop table tab1 !
drop table tab2 !
drop table tab3 !
commit release !
file connect ( kern test !
create table xxx (
patadmno fixed (6),
patname  char (30),
halareano fixed (5),
patacqs char (1),
"days" fixed (4))!
insert xxx values (1,'heike',12345,'T',2)!
insert xxx values (5,'heike',12345,'T',2)!
insert xxx values (3,'heike',12345,'T',0)!
create table pi7prt (
prtadmno fixed (6),
prtarea  fixed (5),
prttxt   fixed (2),
prtstornono fixed (1),
prtnumber fixed (6))!
insert pi7prt values (5, 12345, 0, 0, 0) !
insert pi7prt values (5, 12345, 0, 0, 7) !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs ! 
fetch!
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > 0 ! 
fetch!
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-1 from dual) ! 
fetch!
* PTS 1117747 !
 * no corr !
 *   only where !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs! 
fetch !
 *   only having !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 *   where and having !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 * with corr !
 *   only where !
 *     no having !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs ! 
fetch !
 *     having without subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > 0 ! 
fetch !
 *     having with subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 *   only having !
 *     no where !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *     where without subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where patname = 'heike'
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *     where with subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *   where and having !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !

 * same as above with max(x.patname) in having !
 * with corr !
 *   only having !
 *     no where !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where max(x.patname) = innerx.patname) ! 
fetch !
 *     where without subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where patname = 'heike'
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where max(x.patname) = innerx.patname) ! 
fetch !
 *     where with subquery !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where max(x.patname) = innerx.patname) ! 
fetch !
 *   where and having !
select patadmno, patname, halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where max(x.patname) = innerx.patname) ! 
fetch !

 * same as above, but with AVG in output-list to check
   the invisible column !
 * no corr !
 *   only where !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs! 
fetch !
 *   only having !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 *   where and having !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 * with corr !
 *   only where !
 *     no having !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs ! 
fetch !
 *     having without subquery !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > 0 ! 
fetch !
 *     having with subquery !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where 'heike' = innerx.patname) ! 
fetch !
 *   only having !
 *     no where !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *     where without subquery !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where patname = 'heike'
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *     where with subquery !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prttxt      = 1
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
 *   where and having !
select patadmno, patname, avg("days"), halareano, patacqs, sum("days")
from xxx x
where not exists (select prtadmno from pi7prt
                   where prtadmno    = x.patadmno
                     and prtarea     = x.halareano
                     and prttxt      = 0
                     and prtstornono = 0
                     and prtnumber  <> 0)
group by patadmno, patname, halareano, patacqs
having sum("days") > (select count(*)-3 from xxx innerx
                        where x.patname = innerx.patname) ! 
fetch !
rollback release !
file connect ( kern test !
create table t ( a char (8), b char (5))!
insert t set a = 'aaaaaaaa', b = 'bbbbb'!
select * from t x
where 'aaaaaaaa' in (select a from t
                     where b in ('a','bbbbb','c')
                     and a in 
                                  (select a from t)
                    ) !
fetch!
select * from t x
where 'aaaaaaaa' in (select a from t
                     where x.b in ('a','bbbbb','c')
                     and a in 
                                  (select a from t)
                    ) !
fetch!
select * from t x
where 'aaaaaaaa' in (select a from t
                     where x.b in ('a','bbbbb','c')
                     and a in 
                                  (select a from t)
                     and 'aaaaaaaa' in
                                  (select a from t)
                    ) !
fetch!
select * from t x
where 'aaaaaaaa' in (select a from t
                     where x.b in ('a','bbbbb','c')
                     and x.a in 
                                  (select a from t)
                    ) !
fetch!
select * from t x
where 'aaaaaaaa' in (select a from t
                     where x.b in 
                                  (select b from t)
                    ) !
fetch !
rollback release !
