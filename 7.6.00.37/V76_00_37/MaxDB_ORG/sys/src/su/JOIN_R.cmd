senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* RECURSIVE.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( w char (3),
x char (8) ,
y fixed (12))!
insert t values ('aaa', 'bbb', 12)!
insert t values ('aaa', 'ccc', 13)!
insert t values ('b  ', 'ccc', 83)!
insert t values ('bbb', 'ddd', 124)!
insert t values ('bbb', 'eee', 125)!
insert t values ('ccc', 'eee', 135)!
insert t values ('eee', 'fff', 12561356)!
create table tt ( a char (1))!
insert tt set a = 'b'!

declare c cursor for 
  with recursive px (minor) as
  (select x from t where w = 'aaa'
   union all
   select x from t, px
       where minor = t.w
  )
select minor
from px !
fetch c!
pars_then_ex !
declare c cursor for 
  with recursive px (minor) as
  (select x from t where w = 'aaa'
   union all
   select x from t, px
       where minor = t.w
  )
select minor
from px !
fetch c into :a!
parsing !
declare c cursor for 
  with recursive px (minor) as
  (select x from t where w = :a
   union all
   select x from t, px
       where minor = t.w
  )
select minor
from px !
pars_execute!
pdescribe!
1
executing !
data !
declare c cursor for !
1
c 3 'aaa'

nodata !
pars_then_ex !
fetch c into :a!
pars_execute !
declare c cursor for 
  with recursive px (major, minor, zahl, mainmajor, mylevel) as
  (select w,x,y,w , 1 from t where w = 'aaa'
   union all
   select w, x, y, mainmajor, mylevel + 1 from t, px
       where minor = t.w
  )
select mainmajor,minor,zahl, fixed (mylevel,5)
from px
order by zahl !
fetch c!
declare c cursor for 
  with recursive px (major,minor,zahl, mainmajor,mainminor,mainzahl) as
  (select w,x,y, t.* from t where w = 'aaa'
   union all
   select w, x, y, mainmajor,mainminor,mainzahl from t, px
       where minor = t.w
  )
select *
from px, tt
with lock isolation level 2 !
fetch c!
*-------------------------------------------------------------!
pars_then_ex !
declare c cursor for 
  with recursive px (major, minor, zahl, mainmajor) as
  (select w,x,y,w from t where w = 'aaa'
   union all
   select w, x, y, mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,zahl
from px
order by zahl !
fetch c!
close c !
*-------------------------------------------------------------!
pars_then_ex !
data !
declare c cursor for 
  with recursive px (major, minor, zahl, mainmajor) as
  (select w,x,y,w from t where w = :a
   union all
   select w, x, y, mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,zahl
from px
order by zahl !
c 3 'aaa'

fetch c!
close c !
*----------------------------------------------------------- !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
  )
select mainmajor,minor,folge
from px
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
   except
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
union
select * from t
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor, zu_viele_namen) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w, x, folge || substr(y, length(y),1), mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
*----------------------------------------------------------- !
declare c cursor for 
  with recursive px (major, minor, folge, mainmajor) as
  (select w,x,chr(y),w from t where w = 'aaa'
   union all
   select w, x, folge || substr(chr(y), length(chr(y)),1), 
                              mainmajor from t, px
       where minor = t.w
  )
select mainmajor,minor,folge
from px
order by folge !
fetch c!
*----------------------------------------------------------- !
declare c cursor for 
  with recursive px (major, minor, zahl) as
  (select w,x,y from t where w in (select * from tt)
   union all
   select w,x,y from t, px
       where minor = t.w and x < (select 'f' from tt)
  )
select *
from px
where zahl in (select y from t) !
fetch c !
pars_execute !
commit release !
* PTS 1118098 - 100 !
file connect ( kern test !
create table KMS_MESSAGE_REFERENCE
( MESSAGE_ID  smallint,
  PARENT_TYPE smallint,
  PARENT      smallint)!
create index i_p_t on KMS_MESSAGE_REFERENCE (PARENT_TYPE)!
insert into KMS_MESSAGE_REFERENCE values (1,1,1)!
insert into KMS_MESSAGE_REFERENCE values (1,1,2)!

create table KMS_MESSAGE
(ID     smallint,
 PARENT smallint)!
insert into KMS_MESSAGE values (11, 1)!
insert into KMS_MESSAGE values (12, 1)!
insert into KMS_MESSAGE values (111, 11)!
insert into KMS_MESSAGE values (1111, 111)!
insert into KMS_MESSAGE values (21, 2)!
select * from KMS_MESSAGE !
fetch !

create table KMS_USER_MESSAGE
(MESSAGE_ID smallint,
 USER_ID    smallint,
 READ_TIME  time)!
create index i on KMS_USER_MESSAGE (user_id)!
insert into KMS_USER_MESSAGE values (11, 4711, time)!
insert into KMS_USER_MESSAGE values (12, 9, time)!
insert into KMS_USER_MESSAGE values (111, 4711, time)!
insert into KMS_USER_MESSAGE values (1111, 9, time)!
insert into KMS_USER_MESSAGE values (21, 4711, time)!
select * from KMS_USER_MESSAGE !
fetch !

* parameter in first select !
parsing !
declare c cursor for with recursive result(ID, PARENT) as (
  select MESSAGE_ID, null 
    from KMS_MESSAGE_REFERENCE 
   where PARENT_TYPE=:pt and PARENT=:p
  union all 
  select KMS_MESSAGE.ID, KMS_MESSAGE.PARENT 
    from KMS_MESSAGE, result 
   where KMS_MESSAGE.PARENT=result.ID
) 
select KMS_MESSAGE.*, READ_TIME 
  from KMS_MESSAGE, KMS_USER_MESSAGE, result 
 where KMS_MESSAGE.ID=MESSAGE_ID(+) 
   and USER_ID(+)= 4711
   and KMS_MESSAGE.ID=result.ID !
data !

executing !
declare !
1, C
n 5 1
n 5 1

pars_then_ex!
nodata !
fetch c into :a, :b, :c, :d !

* parameter in recursive (middle) select !
parsing !
declare c cursor for with recursive result(ID, PARENT) as (
  select MESSAGE_ID, null 
    from KMS_MESSAGE_REFERENCE 
   where PARENT_TYPE=:pt and PARENT=:p
  union all 
  select KMS_MESSAGE.ID, KMS_MESSAGE.PARENT 
    from KMS_MESSAGE, result 
   where KMS_MESSAGE.PARENT=result.ID
     AND KMS_MESSAGE.ID   between :ID AND 1000
) 
select KMS_MESSAGE.*, READ_TIME 
  from KMS_MESSAGE, KMS_USER_MESSAGE, result 
 where KMS_MESSAGE.ID=MESSAGE_ID(+) 
   and USER_ID(+)= 4711 
   and KMS_MESSAGE.ID=result.ID !
data !

executing !
declare !
1, C
n 5 1
n 5 1
n 5 7

pars_then_ex!
nodata !
fetch c into :a, :b, :c, :d !

parsing !
declare c cursor for with recursive result(ID, PARENT) as (
  select MESSAGE_ID, null 
    from KMS_MESSAGE_REFERENCE 
   where PARENT_TYPE=:pt and PARENT=:p
  union all 
  select KMS_MESSAGE.ID, KMS_MESSAGE.PARENT 
    from KMS_MESSAGE, result 
   where KMS_MESSAGE.PARENT=result.ID
) 
select KMS_MESSAGE.*, READ_TIME 
  from KMS_MESSAGE, KMS_USER_MESSAGE, result 
 where KMS_MESSAGE.ID=MESSAGE_ID(+) 
   and USER_ID(+)= :uid 
   and KMS_MESSAGE.ID=result.ID !
data !

executing !
declare !
1, C
n 5 1
n 5 1
n 5 4711

pars_then_ex!
nodata !
fetch c into :a, :b, :c, :d !

* no parameter in last select and parameter there, but
* different select_list to last to checks above.
parsing !
declare c cursor for with recursive result(ID, PARENT) as (
  select MESSAGE_ID, null 
    from KMS_MESSAGE_REFERENCE 
   where PARENT_TYPE=:pt and PARENT=:p
  union all 
  select KMS_MESSAGE.ID, KMS_MESSAGE.PARENT 
    from KMS_MESSAGE, result 
   where KMS_MESSAGE.PARENT=result.ID
) 
select KMS_MESSAGE.*, 
       KMS_USER_MESSAGE.message_id mid, KMS_USER_MESSAGE.user_id usid,
       result.id resid, result.parent resparent
  from KMS_MESSAGE, KMS_USER_MESSAGE, result 
 where KMS_MESSAGE.ID=MESSAGE_ID(+) 
   and USER_ID(+)= 4711 
   and KMS_MESSAGE.ID=result.ID !
data !

executing !
declare !
1, C
n 5 1
n 5 1

pars_then_ex!
nodata !
fetch c into :a, :b, :c, :d, :e, :f!
parsing !
declare c cursor for with recursive result(ID, PARENT) as (
  select MESSAGE_ID, null 
    from KMS_MESSAGE_REFERENCE 
   where PARENT_TYPE=:pt and PARENT=:p
  union all 
  select KMS_MESSAGE.ID, KMS_MESSAGE.PARENT 
    from KMS_MESSAGE, result 
   where KMS_MESSAGE.PARENT=result.ID
) 
select KMS_MESSAGE.*, 
       KMS_USER_MESSAGE.message_id mid, KMS_USER_MESSAGE.user_id usid,
       result.id resid, result.parent resparent
  from KMS_MESSAGE, KMS_USER_MESSAGE, result 
 where KMS_MESSAGE.ID=MESSAGE_ID(+) 
   and USER_ID(+)= :uid 
   and KMS_MESSAGE.ID=result.ID !
data !

executing !
declare !
1, C
n 5 1
n 5 1
n 5 4711

pars_then_ex!
nodata !
fetch c into :a, :b, :c, :d, :e, :f, :g !
pars_execute!
rollback release !

*****************************************************************************
* test memory handling for addnl. qual on index
* PTS 1138227
file connect ( kern test !

CREATE TABLE "PVC_VSETELEMENT"
(
"VERSIONSETID"           Char (16) BYTE    NOT NULL,
"OBJECTID"               Char (16) BYTE    NOT NULL,
"VERSIONID"              Char (16) BYTE    NOT NULL,
"ACTIVATIONSEQNO"        Integer    NOT NULL  DEFAULT 0,
"MODOPERATION"           Integer,
"ISIMPORTED"             Smallint,
"DEACTIVATED"               Smallint,
"CREATIONTIME"               Timestamp    NOT NULL,
"LASTMODIFIED"               Timestamp    NOT NULL,
"AUTOVERSION"               Smallint    NOT NULL,
"IDXDECLID"               Integer,
"LOCKINGVSID"               Char (16) BYTE,
PRIMARY KEY ("VERSIONSETID", "OBJECTID")
)!
CREATE INDEX "PVC_VSE_VID_ISN" ON "PVC_VSETELEMENT"(
 "VERSIONSETID" ASC, 
 "ACTIVATIONSEQNO" ASC)!
CREATE INDEX "PVC_VSE_VS_O_V" ON "PVC_VSETELEMENT"(
 "VERSIONSETID" ASC, 
 "OBJECTID" ASC, 
 "VERSIONID" ASC)!

CREATE TABLE "PVC_VERSION"
(
"VERSIONID"               Char (16) BYTE    NOT NULL,
"OBJECTID"               Char (16) BYTE    NOT NULL,
"BRANCHID"               Char (16) BYTE    NOT NULL,
"BRANCHSEQNO"               Integer    NOT NULL,
"OBJSTATE"               Integer    NOT NULL,
"ISDELETED"               Smallint    NOT NULL,
"CREATORNAME"               Varchar (255)   NOT NULL,
"CREATIONTIME"               Timestamp    NOT NULL,
"LASTMODIFIED"               Timestamp    NOT NULL,
"ISARCHIVED"               Smallint    NOT NULL    DEFAULT      0,
"VERSIONTYPE"               Integer    NOT NULL,
"DELETELOCK"               Smallint    NOT NULL,
PRIMARY KEY ("VERSIONID")
)!
CREATE INDEX "PVC_V_VID_STATE" ON "PVC_VERSION"(
 "VERSIONID" ASC, "OBJSTATE" ASC)!
 
explain
SELECT /*+ordered, join(access=indexaccess,l2_strat_no_optim, 
l2_more_strategies) */ DISTINCT *
FROM "PVC_VSETELEMENT" "VSE" INNER JOIN "PVC_VERSION" "V" 
 ON "V"."VERSIONID" = "VSE"."VERSIONID"
WHERE ("VSE"."VERSIONSETID" = x'C45CD580373A11DACEE7003005711AC9'
 OR "VSE"."VERSIONSETID" = x'C45CD56E373A11DABF68003005711AC9') AND 
 "VSE"."DEACTIVATED" =  0 and  "V"."ISDELETED" = 0 !

SELECT /*+ordered, join(access=indexaccess,l2_strat_no_optim, 
l2_more_strategies) */ DISTINCT *
FROM "PVC_VSETELEMENT" "VSE" INNER JOIN "PVC_VERSION" "V" 
 ON "V"."VERSIONID" = "VSE"."VERSIONID"
WHERE ("VSE"."VERSIONSETID" = x'C45CD580373A11DACEE7003005711AC9'
 OR "VSE"."VERSIONSETID" = x'C45CD56E373A11DABF68003005711AC9') AND 
 "VSE"."DEACTIVATED" =  0 and  "V"."ISDELETED" = 0 !

rollback work release!
*****************************************************************************
* PTS 1138333
file connect ( kern test !

CREATE TABLE "INVEST"
(
	"COUNTER"               Integer    NOT NULL,
	"CARTNUM"               Integer    NOT NULL,
	"MEDORDER"               Integer    NOT NULL,
	"FINANSID"               Smallint    NOT NULL,
	"POLSER"               Varchar (15) ,
	"POLNUM"               Varchar (30) ,
	"DOGNUM"               Varchar (50) ,
	"COMPANY"               Smallint,
	"DOCID"               Varchar (10) ,
	"RESID"               Varchar (10) NOT NULL,
	"DATERES"               Date,
	"TIMERES"               Time,
	"CABID"               Varchar (15) ,
	"KUO"               Fixed (3,2)    NOT NULL,
	"IDPRIHOD"               Integer,
	"IDRASHOD"               Integer,
	"DATE_TARIF"               Date,
	"COMMENTS"               Varchar (1000) ,
	"FACTRESID"               Varchar (10) ,
	"REASON"               Varchar (100) ,
	"KMU"               Integer,
	"KOORDINATOR_COMMENTS"               Varchar (1000) ,
	"DOCTORID"               Varchar (10) ,
	PRIMARY KEY ("COUNTER")
)!

CREATE TABLE "T_RESTYPE"
(
	"RESID"               Varchar (10) NOT NULL,
	"RESEARCHTYPE"               Varchar (255) NOT NULL,
	"TARIF"               Fixed (20,2)    NOT NULL,
	"NOT_EXIST"               Boolean    NOT NULL    DEFAULT FALSE,
	"PLAT"               Boolean    NOT NULL    DEFAULT FALSE,
	"OTKL"               Boolean    NOT NULL    DEFAULT FALSE,
	"BDATE"               Date    NOT NULL    DEFAULT DATE,
	"EDATE"               Date,
	"FORMA"               Varchar (20) ,
	"OMS_TARIF"               Fixed (20,2),
	"BUD_TARIF"               Fixed (20,2),
	"FORMULAR"               Integer,
	"UE"               Fixed (4,1),
	"UE_CHILD"               Fixed (4,1),
	"UE_LABOR"               Fixed (4,1),
	"UE_SECOND"               Fixed (4,1),
	"UE_SECOND_LABOR"               Fixed (4,1),
	"UE_CHILD_LABOR"               Fixed (4,1),
	"MULTI"               Smallint,
	"DOSE_G"               Fixed (4,2),
	"DOSE_S"               Fixed (4,2),
	"DOSE_G_CHILD"               Fixed (4,2),
	"LUCH_FILMS"               Smallint,
	"DURATION"               Smallint,
	"COMPLEXITY"               Fixed (5,3),
	"COMPLEXITY_SMR"               Fixed (5,3)
)!
CREATE UNIQUE INDEX "INDEX001" ON "T_RESTYPE"("RESID" ASC, "BDATE" ASC)!


CREATE TABLE "DID_DOC"
(
	"COUNTER"               Integer    NOT NULL,
	"RESID"               Varchar (10) NOT NULL,
	"RES_NUM"               Smallint    NOT NULL,
	"DOCTORID"               Varchar (10) ,
	"SMRID"               Varchar (10) ,
	"FORMA"               Boolean,
	"DID_DATE"               Date    NOT NULL    DEFAULT DATE,
	"DID_TIME"               Time    NOT NULL    DEFAULT TIME,
	"IS_PRINT"               Boolean,
	"ZAV_CONTROL"               Varchar (10) ,
	"NOT_READY"               Boolean
)!

explain
SELECT i.COUNTER,  
I.RESID, r.TARIF, I.KUO, I.KUO*R.TARIF SUMMA, R.RESEARCHTYPE, i.comments 
FROM < invest i, t_restype r, did_doc d >
WHERE 
i.resid=r.resid and 
i.finansid=4 and 
r.bdate<I.date_tarif And 
(r.edate>=I.date_tarif or r.edate is null) and 
i.cartnum=51701 and 
i.idrashod is null and 
i.idprihod is not null and 
i.counter=d.counter(+) and 
d.counter is null!

rollback work release!
*****************************************************************************
* PTS 1138266
file connect ( kern test !

create table tab1(c1 char(5) key, c2 int)!
insert into tab1 values('12345', 2)!
create table tab2(c1 char(10), c2 int)!
insert into tab2 values('absdfg', 2)!

explain
select * from <tab2, tab1> where tab1.c1 = '1234567890' !
select * from <tab2, tab1> where tab1.c1 = '1234567890' !

rollback work release!
*****************************************************************************
!** test join of longer column to shorter column
file connect ( sut sut !
!* longer key part on top of key sequence
create table s1 ( 
c1 char(11) ascii, c2 char(5) ascii, c3 char(5) ascii, c4 char(11) ascii )!
insert into s1 values ('HALLO'      , 'HALLO', 'HALLO', 'HALLO')!
* c1 forms dst.k1 || dst.k2
insert into s1 values ('HALLO HALLO', 'HALLO', 'HALLO', 'HALLO')!
insert into s1 values ('HALLO'      , 'HALLO', 'HALLO', 'HALLO HALLO')!


!* longer key part in middle of key sequence
create table s2 ( 
c1 char(5) ascii, c2 char(11) ascii, c3 char(5) ascii, c4 char(11) ascii )!
insert into s2 values ('HALLO', 'HALLO'      , 'HALLO', 'HALLO')!
* c2 forms dst.k2 || dst.k3
insert into s2 values ('HALLO', 'HALLO HALLO', 'HALLO', 'HALLO')!
insert into s2 values ('HALLO', 'HALLO'      , 'HALLO', 'HALLO HALLO')!

!* longer key part on end of key sequence
create table s3 ( 
c1 char(5) ascii, c2 char(5) ascii, c3 char(11) ascii, c4 char(11) ascii )!
insert into s3 values ('HALLO', 'HALLO', 'HALLO'      , 'HALLO')!
* c3 forms dst.k3 || PartOf(dst.c4)
insert into s3 values ('HALLO', 'HALLO', 'HALLO HALLO', 'HALLO')!
insert into s3 values ('HALLO', 'HALLO', 'HALLO'      , 'HALLO HALLO')!


create table dst ( k1 char(5) ascii, k2 char(5) ascii, k3 char(5) ascii,
c4 char(5) ascii, primary key(k1,k2,k3))!
insert into dst values ('HALLO', 'HALLO', 'HALLO', 'HALLO')!

!* join key start
explain
select /*+ordered,join(;keyaccess)*/ 
s1.c1, dst.k1 
from s1, dst 
where s1.c1=dst.k1!
select /*+ordered,join(;keyaccess)*/ 
s1.c1, dst.k1 
from s1, dst 
where s1.c1=dst.k1!
fetch!
!* join whole key start
explain
select /*+ordered,join(;keyaccess)*/ 
s1.c1,s1.c2,s1.c3,s1.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s1, dst 
where s1.c1=dst.k1 and s1.c2=dst.k2 and s1.c3=dst.k3 and s1.c4=dst.c4!
select /*+ordered,join(;keyaccess)*/ 
s1.c1,s1.c2,s1.c3,s1.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s1, dst 
where s1.c1=dst.k1 and s1.c2=dst.k2 and s1.c3=dst.k3 and s1.c4=dst.c4!
fetch!

!* join key start
explain
select /*+ordered,join(;keyaccess)*/ 
s2.c1,s2.c2, dst.k1,dst.k2 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2!
select /*+ordered,join(;keyaccess)*/ 
s2.c1,s2.c2, dst.k1,dst.k2 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2!
fetch!
!* join whole key start
explain
select /*+ordered,join(;keyaccess)*/
s2.c1,s2.c2,s2.c3,s2.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2 and s2.c3=dst.k3 and s2.c4=dst.c4!
select /*+ordered,join(;keyaccess)*/ 
s2.c1,s2.c2,s2.c3,s2.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2 and s2.c3=dst.k3 and s2.c4=dst.c4!
fetch!

!* join key start (same as whole key)
explain
select /*+ordered,join(;keyaccess)*/ 
s3.c1,s3.c2,s3.c3, dst.k1,dst.k2,dst.k3
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3!
select /*+ordered,join(;keyaccess)*/ 
s3.c1,s3.c2,s3.c3, dst.k1,dst.k2,dst.k3
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3!
fetch!
!* join whole key start
explain
select /*+ordered,join(;keyaccess)*/ 
s3.c1,s3.c2,s3.c3,s3.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3 and s3.c4=dst.c4!
select /*+ordered,join(;keyaccess)*/ 
s3.c1,s3.c2,s3.c3,s3.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3 and s3.c4=dst.c4!
fetch!

!* drop PK and create index
alter table dst drop primary key!
create index PK on dst(k1,k2,k3)!

!* join key start
explain
select /*+ordered,join(;indexaccess)*/ 
s1.c1, dst.k1 
from s1, dst 
where s1.c1=dst.k1!
select /*+ordered,join(;indexaccess)*/ 
s1.c1, dst.k1 
from s1, dst 
where s1.c1=dst.k1!
fetch!
!* join whole key start
explain
select /*+ordered,join(;indexaccess)*/ 
s1.c1,s1.c2,s1.c3,s1.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s1, dst 
where s1.c1=dst.k1 and s1.c2=dst.k2 and s1.c3=dst.k3 and s1.c4=dst.c4!
select /*+ordered,join(;indexaccess)*/ 
s1.c1,s1.c2,s1.c3,s1.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s1, dst 
where s1.c1=dst.k1 and s1.c2=dst.k2 and s1.c3=dst.k3 and s1.c4=dst.c4!
fetch!

!* join key start
explain
select /*+ordered,join(;indexaccess)*/ 
s2.c1,s2.c2, dst.k1,dst.k2 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2!
select /*+ordered,join(;indexaccess)*/ 
s2.c1,s2.c2, dst.k1,dst.k2 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2!
fetch!
!* join whole key start
explain
select /*+ordered,join(;indexaccess)*/
s2.c1,s2.c2,s2.c3,s2.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2 and s2.c3=dst.k3 and s2.c4=dst.c4!
select /*+ordered,join(;indexaccess)*/ 
s2.c1,s2.c2,s2.c3,s2.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s2, dst 
where s2.c1=dst.k1 and s2.c2=dst.k2 and s2.c3=dst.k3 and s2.c4=dst.c4!
fetch!

!* join key start (same as whole key)
explain
select /*+ordered,join(;indexaccess)*/ 
s3.c1,s3.c2,s3.c3, dst.k1,dst.k2,dst.k3
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3!
select /*+ordered,join(;indexaccess)*/ 
s3.c1,s3.c2,s3.c3, dst.k1,dst.k2,dst.k3
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3!
fetch!
!* join whole key start
explain
select /*+ordered,join(;indexaccess)*/ 
s3.c1,s3.c2,s3.c3,s3.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3 and s3.c4=dst.c4!
select /*+ordered,join(;indexaccess)*/ 
s3.c1,s3.c2,s3.c3,s3.c4, dst.k1,dst.k2,dst.k3,dst.c4 
from s3, dst 
where s3.c1=dst.k1 and s3.c2=dst.k2 and s3.c3=dst.k3 and s3.c4=dst.c4!
fetch!



rollback work release!
*****************************************************************************

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
