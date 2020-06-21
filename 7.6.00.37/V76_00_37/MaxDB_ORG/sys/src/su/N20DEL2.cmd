*   *ID* N20DEL2  DBCMD    changed on 1992-09-28-13.25.30 by ADMIN     *
 *** DELETE wird getestet.
 *** DELETE /FROM/ <table name> WHERE KEY <key spec list>
 *** wird getestet.
 *****************************************************!
file connect ( kern test !
 *****************************************************!
create table t11 ( t11_c1 char(10) key ) !
insert t11 set t11_c1 = 'Hans Meier' !
delete t11 key t11_c1 = 'Hans Meyer' !
delete t11 key t11_c1 = 'Hans Meier' !
insert t11 set t11_c1 = 'Hans Meier' !
commit work !
delete t11 key t11_c1 = 'Hans*' !
delete t11 key t11_c1 = 'Hans Meier' !
insert t11 set t11_c1 = 'Hans Meier' !
insert t11 values ( 'Paul Gans' ) !
insert t11 values ( 'Ute Schaf' ) !
insert t11 values ( 'Otto Huhn' ) !
commit work !
delete t11 key t11_c1 = 'Paul Gans' !
delete t11 key t11_c1 = 'Ute Schaf' !
select * from t11 !
fetch !
commit work !
 ***********************************************************!
create table t12
   ( t12_c1 fixed(5,2) key range between 0 and 999.99,
     t12_c2 fixed (5,2),
     t12_c3 fixed(5) not null ) !
insert t12  values ( 1, 2.3, 5 ) !
commit work !
delete t12 key t12_c1 = 1 !
commit work !
insert t12 values ( 1, 2.3, 5 ) !
insert t12 values ( 2, null, 6 ) !
insert t12 ( t12_c1 , t12_c3 ) values ( 3, 12 ) !
insert t12 values ( 4, +88.8, - 12345 ) !
delete t12 where t12_c1 = 1 !
delete t12 where t12_c1 = 4 !
delete t12 where t12_c1 = 2 !
commit work !
select * from t12 !
fetch !
 ***********************************************************!
create table t13
   ( t13_c1 character (50) key,
     t13_c2 float(10) key range between -9e+30 and +9e+30 ) !
insert t13 values
   ( 'Ein Pferd ! Ein Koenigreich fuer ein Pferd !' ,
     - .88E27 ) !
delete t13
 key t13_c1='Ein Pferd ! Ein Koenigreich fuer ein Pferd !',
     t13_c2=-88E+25 !
insert t13 set
   t13_c1 = 'Sein oder Nichtsein ? Das ist die Frage.' ,
   t13_c2 = + .88 !
insert t13 values ( 'Shakespeare' , 5E+12 ) !
insert t13 values
    ( 'Die Lerche war ''s und nicht die Nachtigall.', -12 ) !
delete t13 key t13_c2 = 5e12 , t13_c1 = 'Shakespeare' !
delete t13 key t13_c1 = 'Shakespeare' ,
                     t13_c2 = 5000000000000 !
delete t13 key
   t13_c1 = 'Sein oder Nichtsein ? Das ist die Frage.' ,
   t13_c2 = +88e-2 !
select * from t13 !
fetch !
commit work !
 ***********************************************************!
create table t14
    ( t14_c1 fixed(5) key,
      t14_c2 fixed(5,5) key range in (.1,.2,.3,.4,.5,.6),
      t14_c3 char(10) not null,
      t14_c4 char(10) ) !
insert into t14 set t14_c1=1 , t14_c2 =+ .1 , t14_c3 = '1' !
delete t14 key t14_c2 = 0.1 , t14_c1 =1 !
insert into t14 set t14_c1 = 1 , t14_c2 = +.1,t14_c3 = '1' !
delete t14 key t14_c1 =1 , t14_c2 = 0.1 !
commit work !
insert into t14 set  t14_c1 = 1, t14_c2 = .1 ,t14_c3 = '1' !
insert t14 values ( 2 , .2 , '1' , '1' ) !
insert t14 values ( 2 , .1 , '1'  , '1' ) !
insert t14 values ( 1 , .2 , '1' , '1' ) !
insert t14 ( t14_c1, t14_c2, t14_c3 ) values (3,+00.3,'3') !
commit work !
delete t14 key t14_c1 = 2 , t14_c2 = .1 !
delete t14 key t14_c1 = 2 , t14_c2 = .2 !
select * from t14 !
fetch !
commit work release !
 ***********************************************************!

* The gimmick is, that a user can write DELETE DB2 PARSEID, and
* what he want is to delete all rows from a table named DB2 with
* the reference name PARSEID (strange, isn't it?). So we can only
* complain about a syntax error, when we saw the first parameter!
file CONNECT ( kern test!
CREATE TABLE db2 (a CHAR (2))!
* First the syntactic correct statements!
DELETE db2!
DELETE kern.db2!
DELETE db2.tabelle!
DELETE "DB2"!
DELETE "KERN"."DB2"!
DELETE "DB2"."TABELLE"!
DELETE DB2 tabelle!
DELETE DB2 PARSEID!
CREATE TABLE explicit (a CHAR(5))!
SELECT * FROM explicit!
SELECT * FROM artikel explicit!
DELETE   FROM artikel explicit!

* Now the syntactic incorrect ones!
DELETE!
DELETE FROM!
DELETE DB2 FROM tabelle!
DELETE DB2 PARSEID :a!
* And finally the one that all is about!
DELETE DB2 PARSEID :a AT :b!

ROLLBACK WORK RELEASE!
