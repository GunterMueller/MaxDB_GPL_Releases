*   *ID* CORR8    DBCMD    changed on 1992-10-02-14.23.50 by KERN      *
file connect ( kern test !
* NIST-Test 434 und umliegendes !
create table t ( a decimal (8), b decimal (8))!
insert into t values (4, 1)!
insert into t values (5, 1)!
insert into t values (5, 1)!
declare c cursor for select a, avg(b)
  from t
 group by a
having exists (select a from t x where
x.b = sum(t.b)) !
fetch c into :a, :b!
close c !
declare c cursor for select a, avg(b)
  from t
 group by a
having exists (select a from t x where x.b = avg(t.b) 
                                   and x.b = count(distinct t.b) 
								   and x.b = sum(t.b))
order by 1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select 2*a, avg(b)
  from t
 group by a
having exists (select a from t x where
x.b = sum(t.b)) !
fetch c into :a, :b!
close c !
declare c cursor for select 2*a, avg(b)
  from t
 group by a
having exists (select a from t x where x.b = avg(t.b) 
                                   and x.b = count(distinct t.b) 
								   and x.b = sum(t.b))
order by 1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select a, avg(b)
  from t
 group by a
having exists (select a from t x where
x.b = sum(t.b)*1) !
fetch c into :a, :b!
close c !



declare c cursor for select a, avg(b)
  from t
 group by a
having exists (select a from t x where
x.a = t.a and
x.b = sum(t.b)) !
fetch c into :a, :b!
close c !
declare c cursor for select a, sum(b)
  from t
 group by a
having exists (select a from t x where
x.a = t.a and
x.b = avg(t.b)) !
fetch c into :a, :b!
close c !
declare c cursor for select avg(b)
  from t
having exists (select a from t x where
x.a = t.a and
x.b = sum(t.b)) !
fetch c into :a!
close c !
declare c cursor for select avg(b)
  from t
having exists (select a from t x where x.b = min(t.b)) !
fetch c into :a !
close c !
declare c cursor for select avg(b)
  from t
having exists (select a from t x where x.b = t.b) !
fetch c into :a !
close c !
declare c cursor for select a, avg(b)
  from t
 group by a
having exists (select a from t x where x.b = sum(t.b))
order by 1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select 2*a, avg(b)
  from t
 group by a
having exists (select a from t x where x.b = sum(t.b))
order by 1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select a*a, avg(b)
  from t
 group by a
having exists (select a from t x where x.b = sum(t.b))
order by 1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select a, avg(b)
  from t
where exists (select a from t x where x.b = sum(t.b)) !
fetch c into :a, :b !
close c !
* wird das innere select korrekt geprueft ?
* ich muss noch hinkriegen, dass er unten mit set functionen
* nur solche sachen akzeptiert, die oben aus group-klauseln
* und having stammen.
declare c cursor for select a, avg(b)
  from t
where a = (select min(a) from t x
          having exists (select a from t y where y.a = min(x.a)))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
where a = (select min(a) from t x
          where exists (select a from t y where y.a = x.a))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
where a = (select min(a) from t x
          having exists (select a from t y where y.a = min(x.a)))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
having a = (select min(a) from t x
          having exists (select a from t y where y.a = min(x.a)))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
where a = (select min(a) from t x
          where exists (select a from t y where y.a = min(x.a)))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
having a = (select min(a) from t x
          where exists (select a from t y where y.a = x.a))!
fetch c into :a, :b !
close c !
declare c cursor for select avg(b)
  from t
having a = (select a from t x
          where exists (select a from t y where y.a = x.a))!
fetch c into :a, :b !
close c !
declare c cursor for select a, avg(b)
  from t
where a = any (select min(a) from t x
          group by b
          having exists (select a from t y where y.a = min(t.a))) !
fetch c into :a, :b !
close c !
declare c cursor for select a, avg(b)
  from t
having a = any (select min(a) from t x
          group by b
          having exists (select a from t y where y.a = min(t.a))) !
fetch c into :a, :b !
close c !
declare c cursor for select a
  from t
where    a  = any (select min(a) from t x
          where exists (select a from t y where y.a = t.a)) !
fetch c into :a, :b !
close c !
declare c cursor for select a, avg(b)
  from t
group by a
having max(a) = any (select min(a) from t x
          group by b
          having exists (select a from t y where y.a = min(t.a))) !
fetch c into :a, :b !
close c !
commit work release !
file connect ( kern test !
drop table t !
commit release !
* PTS 1000785 !
file connect ( kern test !
CREATE TABLE TABX
( K FIXED(5),
  I FIXED(6),
  N FIXED(7),
  PRIMARY KEY( K,N ))!
INSERT INTO TABX VALUES( 1, 1, 1)!
COMMIT!
CREATE TABLE TABY
( K FIXED(8),
  I FIXED(9),
  N FIXED(10),
  PRIMARY KEY( K,N ))!
INSERT INTO TABY VALUES( 1, 1, 1)!
COMMIT!
CREATE TABLE TABZ
( K FIXED(11),
  I FIXED(12),
  N FIXED(13),
  PRIMARY KEY( K,N ))!
INSERT INTO TABZ VALUES( 1, 1, 1)!
COMMIT!
CREATE VIEW A1
  AS SELECT K,I,N FROM TABX
  WHERE K IN ( SELECT N FROM TABY) !
CREATE VIEW C2
  AS SELECT K,I,N FROM TABX T0
  WHERE K IN ( SELECT AVG(N) FROM A1
                GROUP BY I
               HAVING T0.K = MIN(N))!
COMMIT!
select k, i, n
  from tabx t0
 where k in ( select avg(n)
                from tabx
               where k in (select n
                             from taby)
              group by i
              having t0.k = min(n)
            ) !
fetch into :a, :b, :c !
select k, i, n
  from tabx t0
 where k in ( select avg(n)
                from tabx
               where k in (select n
                             from taby)
              group by i
              having min(n) in
                  (select n from tabx where n = t0.n)
            ) !
fetch into :a, :b, :c !
select * from c2 !
fetch into :a, :b, :c !
pars_then_ex !
select * from c2 !
fetch into :a, :b, :c !
PARSING!
declare erg cursor for 
SELECT T1.K , T1.N FROM TABX T1
WHERE T1.K
IN (SELECT T2.N FROM C2 T2)!
EXECUTING!
SELECT     X!
1
pars_then_ex !
fetch erg into :a, :b, :c !
pars_execute !
drop parseid !
1
drop table tabx !
drop table taby !
drop table tabz !
commit release !
