file connect ( kern test &1 &2 !
create table j ( j1 char (1))!
insert j values (' ')!

CREATE TABLE Tdummy ( K1 INT,
                 I1 CHAR (6) ASCII)!
 
PARSING!
INSERT INTO Tdummy VALUES (:K1, :I1)!
 
DATA!
EXECUTING!
 
INSERT!
1
n 10 1
b 6 092020202020
 
INSERT!
1
n 10 2
b 6 202020202020

INSERT!
1
n 10 3
b 6 202020202020

NODATA!
PARS_EXECUTE!

CREATE TABLE T ( K1 INT,
                 I1 CHAR (6) UNICODE)!
   IF &rc = -3002 THEN 
   BEGIN
*
   CREATE TABLE T ( K1 INT,
                 I1 CHAR (6) ASCII)!
*
   END

Insert t select * from tdummy !
drop table tdummy ! 
CREATE INDEX "t.i1" ON T( I1 ) !
COMMIT! 

explain
  SELECT /*+INDEXACCESS */ hex(MIN(I1)) FROM T! 
 * bei SPACE OPTION kommt '' statt x'09' raus !
SELECT /*+INDEXACCESS */ hex(MIN(I1)) FROM T!
fetch!

explain
  SELECT /*+INDEXACCESS */ hex(MAX(I1)) FROM T! 
 * bei SPACE OPTION kommt x'09' statt x'20' raus !
SELECT /*+INDEXACCESS */ hex(MAX(I1)) FROM T!
fetch!

explain
  select count (distinct i1) from t !
select count (distinct i1) from t !
fetch!

insert into t values (0, 'a')!

explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 = ''!
select /*+INDEXACCESS */ hex(i1) from t where i1 = ''!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 >= ' '!
select /*+INDEXACCESS */ hex(i1) from t where i1 >= ' '!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 <= ''!
 * nicht nur bei SPACE OPTION kommt x'09' nicht mit raus
 * g04onevalue aendern, nee !
select /*+INDEXACCESS */ hex(i1) from t where i1 <= ''!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 <> ''!
select /*+INDEXACCESS */ hex(i1) from t where i1 <> ''!
fetch!

explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 between '' and 'z'!
select /*+INDEXACCESS */ hex(i1) from t where i1 between '' and 'z'!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 between '  ' and 'z'!
select /*+INDEXACCESS */ hex(i1) from t where i1 between '  ' and 'z'!
fetch!

explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 like '_'!
select /*+INDEXACCESS */ hex(i1) from t where i1 like '_'!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 like '%_'!
select /*+INDEXACCESS */ hex(i1) from t where i1 like '%_'!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 like '_%'!
select /*+INDEXACCESS */ hex(i1) from t where i1 like '_%'!
fetch!
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 like ' %'!
 * hier kommt die x'09' mit raus. das ist falsch 
 * es kommt durch die Umwandlung in ein 
 * BETWEEN x'20' and x'2020FFFF' !
select /*+INDEXACCESS */ hex(i1) from t where i1 like ' %'!
fetch!

pars_then_ex !
data !
explain
  select /*+INDEXACCESS */ hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

select /*+INDEXACCESS */ hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

fetch!
* more than 16 for kb71op_in_sorted needed !
explain 
 select /*+INDEXACCESS */ hex(i1) from t where 
 i1 in ('a', ' ', 'z','y','x','w','c','t','s','q','e','m','d','p','o','u',:p)!
b 6 092020202020

select /*+INDEXACCESS */ hex(i1) from t where 
 i1 in ('a', ' ', 'z','y','x','w','c','t','s','q','e','m','d','p','o','u',:p)!
b 6 092020202020

fetch!

select hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

fetch!
pars_execute!
nodata !

explain 
  select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;INDEXACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;NOACCESSPATH)*/ hex(i1) from j, t where j1=i1 !
fetch!
explain 
  select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
select /*+ORDERED*/ hex(i1) from t, j where j1=i1 !
fetch!

explain
  select /*+INDEXACCESS */ hex(i1) from t
  where i1 = (select j1 from j) !
select /*+INDEXACCESS */ hex(i1) from t
  where i1 = (select j1 from j) !
fetch!

explain
  select /*+INDEXACCESS */ hex(i1) from t
  where i1 >= ANY (select j1 from j) !
select /*+INDEXACCESS */ hex(i1) from t
  where i1 >= ANY (select j1 from j) !
fetch!

explain 
  select /*+INDEXACCESS */ hex(i1) from t order by i1 !
 * x'20' vor x'09', übel !
select /*+INDEXACCESS */ hex(i1) from t order by i1 !
fetch!
explain
  select /*+KEYACCESS */ hex(i1) from t order by i1 !
 * korrekte, aber andere Reihenfolge als über den Index !
select /*+KEYACCESS */ hex(i1) from t order by i1 !
fetch!

explain
  select /*+INDEXACCESS */ hex(i1) from t order by i1 desc !
select /*+INDEXACCESS */ hex(i1) from t order by i1 desc !
fetch!
explain
  select /*+KEYACCESS */ hex(i1) from t order by i1 desc !
select /*+KEYACCESS */ hex(i1) from t order by i1 desc !
fetch!

delete t where k1 in (0,3)!
drop index "t.i1" ! 
diagnose share parse drop !
 * die Kommentare der Index-Tests gelten auch hier !
alter table t add primary key (i1)!

 * x'20' kleiner als x'09', übel !
explain
  select hex(min(i1)) from t!
select hex(min(i1)) from t!
FETCH!
explain
  select hex(max(i1)) from t!
select hex(max(i1)) from t!
fetch!

explain
  select count (distinct i1) from t !
select count (distinct i1) from t !
fetch!

insert into t values (0, 'a')!

explain
  select hex(i1) from t where i1 = ''!
select hex(i1) from t where i1 = ''!
fetch!
explain
  select hex(i1) from t where i1 >= ''!
 * x'09' > x'20', übel !
select hex(i1) from t where i1 >= ''!
fetch!
explain
  select hex(i1) from t where i1 <= ' '!
select hex(i1) from t where i1 <= ' '!
fetch!
explain
  select hex(i1) from t where i1 <> ''!
select hex(i1) from t where i1 <> ''!
fetch!

explain
  select hex(i1) from t where i1 between '' and 'z'!
select hex(i1) from t where i1 between '' and 'z'!
fetch!
explain
  select hex(i1) from t where i1 between '  ' and 'z'!
select hex(i1) from t where i1 between '  ' and 'z'!
fetch!

explain
  select hex(i1) from t where i1 like '_'!
select hex(i1) from t where i1 like '_'!
fetch!
explain
  select hex(i1) from t where i1 like '%_'!
select hex(i1) from t where i1 like '%_'!
fetch!
explain
  select hex(i1) from t where i1 like '_%'!
select hex(i1) from t where i1 like '_%'!
fetch!
explain
  select hex(i1) from t where i1 like ' %'!
 * es ist inkorrekt, dass 09 mit rauskommt !
select hex(i1) from t where i1 like ' %'!
fetch!

pars_then_ex !
data !
explain
  select hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

select hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

fetch!
* more than 16 for kb71op_in_sorted needed !
explain 
 select hex(i1) from t where 
 i1 in ('a', ' ', 'z','y','x','w','c','t','s','q','e','m','d','p','o','u',:p)!
b 6 092020202020

select hex(i1) from t where 
 i1 in ('a', ' ', 'z','y','x','w','c','t','s','q','e','m','d','p','o','u',:p)!
b 6 092020202020

fetch!

select hex(i1) from t where i1 in ('a', ' ', :p)!
b 6 092020202020

fetch!
pars_execute!
nodata !

explain 
  select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
select /*+ORDERED,JOIN(;KEYACCESS)*/ hex(i1) from j, t where j1=i1 !
fetch!

explain
  select hex(i1) from t where i1 = (select j1 from j) !
select hex(i1) from t where i1 = (select j1 from j) !
fetch!
explain
  select hex(i1) from t where i1 <= ANY (select j1 from j) !
select hex(i1) from t where i1 <= ANY (select j1 from j) !
fetch!


explain
  select hex(i1) from t order by i1 !
select hex(i1) from t order by i1 !
fetch!
explain
  select hex(i1) from t order by i1 desc !
select hex(i1) from t order by i1 desc !
fetch!
 
explain
  select hex(i1), max(k1) from t group by i1 !
select hex(i1), max(k1) from t group by i1 !
fetch!

drop table t !
drop table j !
diagnose share parse drop !
commit work release !

