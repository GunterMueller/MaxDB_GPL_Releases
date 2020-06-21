file connect ( kern test !
create table base ( a char (8), f fixed (10))!
create table t ( a char (12) constraint c check a > 
                          (select a from base) )!

create table t ( a char (12), b fixed (11))!

* insert !

insert t values ((select a from base), (select f from base))!
select * from t !
fetch !

insert base values ('abc', 4711)!

insert t values ((select a from base), (select f from base))!
select * from t !
fetch !

insert t values ((select a from base)||'xyz', 
                  (select f from base) * 4)!
select * from t !
fetch !

insert t values ((select * from base))!

* update !
delete t !
insert t values ('old', 4711)!

update t set a = 'new' || (select a from base) !
select * into :a, :b from t !

update t set a = (select a from base) || 'new' !
select * into :a, :b from t !
update t set a = upper((select a from base)) !
select * into :a, :b from t !
update t set a = lower((select a from base)),
             b = 12 * (select f from base) !

select * into :a, :b from t !

* with VALUES <extended value SPEC> has to be given, no expression !
update t (a,b) values (select * from base)!
select * into :a, :b from t !
update t (a,b) values ((select a from base), (select f from base))!
select * into :a, :b from t !
update t (a,b) values ('UPP' || upper((select a from base)),
                  7 * (select a from base)) !
select * into :a, :b from t !


* select !

* select list !
 
select '"' || (select a from base) || '"' from t !
fetch !
select a || '"' || (select a from base) || '"', b from t !
fetch!
select '"' || substr((select a from base), 1, 2) || '"' from t !
fetch !
select a, upper((select a from base)), b from t !
fetch !

* too many cols !
select '"' || (select * from base) || '"' from t !
fetch !
* where !

insert t values ('fed', -4)!
select * from t 
  where a between 'aaa' and (select a from base) || 'xyz'!
fetch !
select * from t 
  where (select a from base) between a and 'abcxyz'!
fetch !
select * from t 
  where (select a from base) || 'xyz' > a !
fetch !
select * from t 
  where (select a from base) = a !
fetch !
select * from t 
  where a || 'xyz' = (select a from base) || 'xyz' !
fetch!
select * from t 
  where a || 'xyz' = any (select a from base) || 'xyz' !
fetch!
select * from t 
  where (a ,b) = (((select a from base), 56532)) !
fetch !
select * from t 
  where (a || 'xyz',b) = (((select a from base) || 'xyz', 56532)) !
fetch !
select * from t
  where a in ((select a from base), 'fed')!
fetch !
select * from t 
  where a || 'xyz' like (select a from base) || 'xyz'!
fetch !
select * from t 
  where (select a from base) is not null !
fetch !
select * from t 
  where a sounds like (select a from base)!
fetch !

* group by / order by !

select sum(f) from t group by (select a from base)!
select sum(f) from t group by (select a from t)!
select * from t order by (select a from base)!

* Schachtelung !

select '"', upper(
           (select a 
              from base
             where a||'wert' > (select a from base))), '"' from t !
fetch!

* dbproc / trigger !
noscript !

create dbproc a1(in oid int) as 
declare c cursor for select '"', (select a from kern.base), '"'
   from kern.t ; 
   stop((select f from kern.base));
!
create dbproc a2(in oid int) as 
declare c cursor for select '"', (select a from kern.base), '"'
   from kern.t ; 
   if (select a from kern.base) > 'a'
   then 
   stop(3);
!
create dbproc a3(in oid int) as 
declare c cursor for select '"', (select a from kern.base), '"'
   from kern.t ; 
   while (select a from kern.base) > 'a' do
     stop(3);
!
create dbproc a4(in oid int) as 
declare c cursor for select '"', (select a from kern.base), '"'
   from kern.t ; 
   int myint;
   set myint = (select f from kern.base);
!
create dbproc agood(in oid int) as 
var
   myint int;
   set myint = oid + 3;
!
call agood ((select f from base))!
script !

* correlated subqueries !
insert t values ((select a from base where base.a = t.a), 
                 (select f from base))!
select * from t !
fetch !

delete t !
insert t values ('old', 4711)!

update t set a = (select a from base where base.a <> t.a) !
select * into :a, :b from t !
update t set a = 'new' || (select a from base where base.a = t.a) !
select * into :a, :b from t !
update t set a = (select a from base where base.a <> t.a) || 'new' !
select * into :a, :b from t !

select a || '"' || (select a from base where base.a = t.a) || '"', 
                                                           b from t !
fetch!
SELECT A || '"' || (SELECT A FROM BASE WHERE BASE.A <> T.A) || '"',
                                                           B FROM T !
fetch!
SELECT A || '"' || (SELECT substr(t.a, 1, 1) || A 
                                 FROM BASE WHERE BASE.A = T.A) || '"',
                                                           B FROM T !
fetch!
select a || '"' || v.a || '"', b from t,
         (select a from base where base.a = t.a) v  !
fetch!

select * from t !
fetch!
select * from base !
fetch !
select * from t 
  where a between 'aaa' and 
               (select a from base where base.a =  t.a) || 'xyz'!
fetch !
select * from t 
  where a between 'aaa' and 
               (select a from base where base.a <> t.a) || 'xyz'!
fetch!

* too many columns/rows !

delete t !
insert t values ('old', 4711)!

update t set a = 'new' || (select a,b from base) !

insert base values ('def', 2)!

select * from t 
  where a || 'xyz' = (select a from base) || 'xyz' !
fetch !

drop table base!
drop table t!
commit release !
* PTS 1120680/1 !
file connect ( kern test !
create table foo (id fixed(20) primary key, 
x fixed(20), y fixed(20))!
create table bar (id fixed(20) primary key)!
parsing !
select * from foo foo1
 where (? = (select o2.id from bar o2 where foo1.x = o2.id)) or
     (? = (select m3.id from bar m3 where foo1.y = m3.id)) !
select * from foo foo1
 where ((select o2.id from bar o2 where foo1.x = o2.id) = ?) or
     ((select m3.id from bar m3 where foo1.y = m3.id) = ?) !
pars_execute !
rollback release !
*========================!
file connect ( kern test !
CREATE TABLE "OPS_FPO_WORKLOAD"
("MASTER_ID"               Integer    NOT NULL,
"OP_ID"               Integer,
"FPO_ID"               Fixed (14,0))!
CREATE TABLE "OP_MESSAGING"
("SENDER_OP_ID"               Integer,
"RECEIVER_OP_ID"               Integer,
"MSG_QUEUE"               Varchar (4000) ascii,
"CREATED"               Timestamp,
"UPDATED"               Timestamp)!
CREATE TABLE "ACCOUNTS_OPS"
("OP_ID"               Integer,
"MASTER_ID"               Integer    NOT NULL,
"LOGIN"               Varchar (15) ascii    NOT NULL,
"PASSWORD"               Varchar (15) ascii,
"FIRST_NAME"               Varchar (40) ascii,
"LAST_NAME"               Varchar (40) ascii,
"EMAIL"               Varchar (40) ascii,
"CREATED"               Timestamp,
"LAST_LOGIN"               Timestamp,
PRIMARY KEY ("OP_ID"))!
CREATE TABLE "OPS_ONLINE"
("OP_ID"               Integer    NOT NULL,
"MASTER_ID"               Integer    NOT NULL,
"LAST_FPO_RECEIVED"               Timestamp,
"CREATED"               Timestamp,
"UPDATED"               Timestamp)!
CREATE TABLE "DEPARTMENTS"
("DEPT_ID"               Integer,
"MASTER_ID"               Integer    NOT NULL,
"NAME"               Varchar (50) ascii,
"EMAIL"               Varchar (50) ascii,
"CAN_INITIATE"               Boolean,
"CAN_SHARE"               Boolean,
"GREETING"               Varchar (120) ascii,
"DEFAULT_IMAGE_OFFLINE"               Varchar (120) ascii,
"DEFAULT_IMAGE_ONLINE"               Varchar (120) ascii,
PRIMARY KEY ("DEPT_ID"))!
CREATE TABLE "DEPARTMENTS_ROSTER"
("DEPT_ID"               Integer    NOT NULL,
"OP_ID"               Integer    NOT NULL)!

SELECT 
  AO.op_id, 
  AO.first_name, 
  AO.last_name, 
  D.name AS dept_name, 
  D.dept_id, 
( SELECT 
    count(OPFPOW.op_id) 
  FROM 
    OPS_FPO_WORKLOAD OPFPOW 
  WHERE 
    OPFPOW.op_id=AO.op_id) AS fpo_count 
FROM 
  ACCOUNTS_OPS AO, 
  OPS_ONLINE OO, 
  DEPARTMENTS D, 
  DEPARTMENTS_ROSTER DR 
WHERE 
  AO.master_id=1 AND 
  AO.op_id<>1 AND 
  AO.op_id=OO.op_id AND 
  AO.op_id=DR.op_id AND 
  DR.dept_id=D.dept_id 
ORDER BY 
  D.name ASC!
SELECT 
  AO.op_id, 
  AO.first_name, 
  AO.last_name, 
  D.name AS dept_name, 
  D.dept_id, 
( SELECT 
    count(OPFPOW.op_id) 
  FROM 
    OPS_FPO_WORKLOAD OPFPOW 
  WHERE 
    OPFPOW.op_id=AO.op_id) AS fpo_count,
( SELECT 
    OM.msg_queue 
  FROM 
    OP_MESSAGING OM 
  WHERE 
    OM.sender_op_id=AO.op_id AND 
    OM.receiver_op_id=1) AS op_msg_queue  
FROM 
  ACCOUNTS_OPS AO, 
  OPS_ONLINE OO, 
  DEPARTMENTS D, 
  DEPARTMENTS_ROSTER DR 
WHERE 
  AO.master_id=1 AND 
  AO.op_id<>1 AND 
  AO.op_id=OO.op_id AND 
  AO.op_id=DR.op_id AND 
  DR.dept_id=D.dept_id 
ORDER BY 
  D.name ASC!
* PTS 1121191-2 !
SELECT (SELECT MSGNO+MSGNO FROM DUAL) FROM SYSDBA.MESSAGES!
* PTS 1121191-2 !
SELECT (SELECT MSGNO+MSGNO FROM DUAL) FROM SYSDBA.MESSAGES!
* PTS 1122970-2 !
SELECT (SELECT MAX(1) FROM DUAL ) FROM DUAL !
fetch !
* PTS 1126089/1126130-3 !
create view vmax as SELECT (SELECT MAX(1) FROM DUAL ) FROM DUAL !
select * from vmax !
fetch!
rollback work release !
* PTS 1122903-4!
file connect ( kern test sqlmode oracle !
create table tab89p_1 (b char (8), c int)!
insert into tab89p_1 values ('milk', 1)!
insert into tab89p_1 values ('bread', 2)!
insert into tab89p_1 values ('butter', 12)!
create table tab89p_2 (b char (8), c int)!
insert into tab89p_2 values ('milk', 0)!
insert into tab89p_2 values ('bread', 1)!
insert into tab89p_2 values ('butter', 30)!
create table tab89p_3 (a int, b char (8), c int)!
select (select b from tab89p_1), (select c from tab89p_2) from dual!
insert into tab89p_3 values (2, 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c < sc.c
union    select sc.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c > sc.c),  
3)!
insert into tab89p_3 values (20, 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c < sc.c
union all select sc.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c > sc.c),  
30)!
insert into tab89p_3 values (
        (select 3 from dual), 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c > sc.c),  
        (select 4 from dual))!
insert into tab89p_3 (a,b,c) values (12, 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c < sc.c
union    select sc.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c > sc.c),  
13)!
insert into tab89p_3 values (4, 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b
minus   select sc.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c > sc.c),  
5)!
insert into tab89p_3 values (5, 
        (select ft.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b
minus   select sc.b from tab89p_1 ft, tab89p_2 sc 
          where ft.b = sc.b and ft.c < sc.c),  
6)!
select * from tab89p_3!
fetch into :a, :b, :c!
rollback release !
* PTS 1123040/50-52 !
file connect ( kern test !
create table I (
Plan_Belegnummer INTEGER,
Satz_Nr FIXED(3),
Datum DATE default DATE,
Menge FLOAT(8) default 0,
FertigenM1 CHAR(6),
FertigenMengeM1  FLOAT(9) default 0,
FertigenM2 CHAR(6),
FertigenMengeM2  FLOAT(9) default 0,
Material CHAR(6),
MaterialMenge INTEGER default 0,
SAP_Nr CHAR(10),
SAP_ERROR CHAR(255),
storniert FIXED(1) default 0,
Storno_Nr CHAR(10),
StornierDatum DATE,
constraint PK_I primary key (Plan_Belegnummer, Satz_Nr) )
!
create table Istbeleg_Satz (
Plan_Belegnummer INTEGER,
Satz_Nr FIXED(3),
Datum DATE default DATE,
Menge FLOAT(8) default 0,
FertigenM1 CHAR(6),
FertigenMengeM1  FLOAT(9) default 0,
FertigenM2 CHAR(6),
FertigenMengeM2  FLOAT(9) default 0,
Material CHAR(6),
MaterialMenge INTEGER default 0,
SAP_Nr CHAR(10),
SAP_ERROR CHAR(255),
storniert FIXED(1) default 0,
Storno_Nr CHAR(10),
StornierDatum DATE,
constraint PK_I primary key (Plan_Belegnummer, Satz_Nr) )
!
create table r_Info( last_run date, max_Plan_Belegnummer integer)!
insert into r_Info values ('20030415', 17)!
insert into Istbeleg_Satz 
          (Plan_Belegnummer, Satz_Nr, storniert, StornierDatum)
   values (10              , 1      , 0        , null)!
insert into Istbeleg_Satz 
          (Plan_Belegnummer, Satz_Nr, storniert, StornierDatum)
   values (12              , 1      , 1        , date)!
insert into Istbeleg_Satz 
          (Plan_Belegnummer, Satz_Nr, storniert, StornierDatum)
   values ( 8              , 1      , 1        , '20030331')!
select * from Istbeleg_Satz where Plan_Belegnummer <= (select
max_Plan_Belegnummer from r_Info) and storniert=1 and Stornierdatum >
(select last_run from r_Info)!
fetch!
commit release !
file connect ( kern test !
insert into I select * from Istbeleg_Satz where Plan_Belegnummer <= (select
max_Plan_Belegnummer from r_Info) and storniert=1 and Stornierdatum >
(select last_run from r_Info)!
select Plan_Belegnummer, Satz_Nr, storniert, StornierDatum from i!
fetch!
rollback release !
senderid ODB !
file connect ( kern test !
insert into I select * from Istbeleg_Satz where Plan_Belegnummer <= (select
max_Plan_Belegnummer from r_Info) and storniert=1 and 
char(Stornierdatum, ISO) >
char((select last_run from r_Info), ISO)!
select Plan_Belegnummer, Satz_Nr, storniert, StornierDatum from i!
fetch!
rollback!
insert into I select * from Istbeleg_Satz where Plan_Belegnummer <= (select
max_Plan_Belegnummer from r_Info) and storniert=1 and 
char(Stornierdatum, ISO) >
char(adddate ((select last_run from r_Info), 1), ISO)!
select Plan_Belegnummer, Satz_Nr, storniert, StornierDatum from i!
fetch!
rollback!
insert into i (Plan_Belegnummer, Satz_Nr, storniert, StornierDatum)
   select 20, 1, 1,(select last_run from r_Info) from dual!
select Plan_Belegnummer, Satz_Nr, storniert, StornierDatum from i!
fetch!
rollback!
insert into I select * from Istbeleg_Satz where Plan_Belegnummer <= (select
max_Plan_Belegnummer from r_Info) and storniert=1 and Stornierdatum >
(select last_run from r_Info)!
select Plan_Belegnummer, Satz_Nr, storniert, StornierDatum from i!
fetch!
rollback release !
senderid XCI !
file connect ( kern test !
drop table i !
drop table Istbeleg_Satz!
drop table r_Info !
commit release !
* PTS 1133523/4 !
file connect ( kern test !
create table t ( a fixed (18,2), b int)!
insert t values (1.23, 1)!
insert t values (2.34, 2)!
create table t2 ( c char (8))!
insert t2 values ('abc')!
declare curs cursor for 
select c, (select sum(a) from t) from t2 !
fetch curs!
declare curs cursor for 
select c, (select sum(a) from t) mysum from t2 !
fetch curs!
select columnname, datatype, len, dec from columns
  where tablename = 'CURS' !
fetch!
rollback release !
* PTS 1136786/ 1136811/2 !
file connect ( kern test !
create table xy ( k int, a int )!
insert into xy values (1,2)!
select 1+max(k) from xy!
fetch!
select (select k from xy)+1 from xy!
fetch!
select (select k from xy)+max(k) from xy!
fetch!
select fixed ((select k from xy), 10, 0)+max(k) from xy!
fetch!
select (select k from xy)+a from xy!
fetch!
select max(k+(select k from xy )) from xy !
fetch!
rollback release !
 * PTS 1136877, 1137020/1 !
file connect ( kern test !
create table dt ( a int )!
select sum(a+1)+2, 'hallo' from dt !
fetch!
select sum(a+1)+2, (select * from dual) from dt !
fetch!
select sum(a+1)+2, (select * from dual union select * from dual) 
from dt!
fetch!
select * from dual where exists ( select count(*) from dual )!
fetch!
select * from dual where exists ( select (select * from dual) from dual )!
fetch!
pars_then_ex!
data!
select * from dual where exists ( select :A + 1 from dual )!
n 38 1

nodata!
fetch!

pars_execute!
rollback release !
 * PTS 1138682/3 !
file connect ( kern test !
SELECT (SELECT * FROM DUAL) y
FROM dual!
SELECT (SELECT * FROM DUAL UNION SELECT * FROM DUAL) y
FROM dual!
SELECT (SELECT * FROM DUAL UNION SELECT * FROM DUAL) 
FROM dual!
rollback release !
