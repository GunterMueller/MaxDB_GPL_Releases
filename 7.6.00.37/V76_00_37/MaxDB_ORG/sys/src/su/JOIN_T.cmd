senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* T01EMP.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE emp  ( name CHAR (15) ascii KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       currsal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
if &rc <> 0 then return
CREATE TABLE fam   ( name CHAR (15) ascii KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       status CHAR (8) NOT NULL,
                       depend FIXED (2) NOT NULL RANGE BETWEEN 0 AND 15,
                       address CHAR (30) NOT NULL ) !
CREATE TABLE dept  ( deptno FIXED (2) KEY RANGE BETWEEN 10 AND 90 ,
                        deptname CHAR (10) NOT NULL,
                        locname  CHAR (15) NOT NULL )  !
commit work !
CREATE VIEW married AS SELECT *
                       FROM fam
                       WHERE ( status = 'married' ) !
CREATE VIEW divorced AS SELECT *
                        FROM fam
                        WHERE ( status = 'divorced' ) !
CREATE VIEW single AS SELECT *
                      FROM fam
                      WHERE (status = 'single' ) !
CREATE VIEW rich_emp AS SELECT *
                        FROM emp
                        WHERE ( currsal > 15000 ) !
commit work !
LOCK TABLE emp, fam, dept IN EXCLUSIVE MODE !
INSERT INTO emp SET name = 'Fred Gerard',
             deptno = 50,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Norma Wilson',
             deptno = 40,
             currsal = 15500,
             manager = 'Jill Crowley',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Paul Brown',
             deptno = 70,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 7000,
             sex = 'male'!
INSERT INTO emp SET name = 'Paul Rogers',
             deptno = 40,
             currsal = 15000,
             manager = 'Jill Crowley',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'John Johnes',
             deptno = 60,
             currsal = 16500,
             manager = 'Dana Smith',
             startsal = 8500,
             sex = 'male'!
INSERT INTO emp SET name = 'Pat Mingo',
             deptno = 50,
             currsal = 13000,
             manager = 'Fred Gerard',
             startsal = 6500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dana Smith',
             deptno = 60,
             currsal = 16000,
             manager = 'George Taylor',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp SET name = 'Mary Rowen',
             deptno = 60,
             currsal = 15500,
             manager = 'Dana Smith',
             startsal = 9000,
             sex = 'female'!
INSERT INTO emp SET name = 'Jill Crowley',
             deptno = 40,
             currsal = 17000,
             manager = 'George Taylor',
             startsal = 8250,
             sex = 'female'!
INSERT INTO emp SET name = 'Jerry Denny',
             deptno = 70,
             currsal = 14000,
             manager = 'Paul Brown',
             startsal = 6000,
             sex = 'male'!
INSERT INTO emp SET name = 'Alice Teir',
             deptno = 50,
             currsal = 13500,
             manager = 'Fred Gerard',
             startsal = 8500,
             sex = 'female'!
INSERT INTO emp SET name = 'Dave Cowel',
             deptno = 60,
             currsal = 14000,
             manager = 'Dana Smith',
             startsal = 7500,
             sex = 'male'!
INSERT INTO emp SET name = 'George Taylor',
             deptno = 10,
             currsal = 20000,
             manager = ' ',
             startsal = 10000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joan Bird',
             deptno = 50,
             currsal = 14500,
             manager = 'Fred Gerard',
             startsal = 8000,
             sex = 'female'!
INSERT INTO emp SET name = 'Ed Nader',
             deptno = 70,
             currsal = 8000,
             manager = 'Paul Brown',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp SET name = 'Joyce Adams',
             deptno = 70,
             currsal = 16000,
             manager = 'Paul Brown',
             startsal = 6500,
             sex = 'female'!
INSERT INTO fam SET name = 'Fred Gerard',
                    deptno = 50,
               status = 'married',
               depend = 3,
               address = '37 Drake St. #2, Carmichael'!
INSERT INTO fam SET name = 'Norma Wilson',
                    deptno = 40,
               status = 'single',
               depend = 1,
               address = '4175 Oak Ct., Daly City'!
INSERT INTO fam SET name = 'Paul Brown',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '271 Oxnard St., Lodi'!
INSERT INTO fam SET name = 'Paul Rogers',
                    deptno = 40,
               status = 'divorced',
               depend = 5,
               address = '27811 Embarcodero, S.F.'!
INSERT INTO fam SET name = 'John Johnes',
                    deptno = 60,
               status = 'single',
               depend = 1,
               address = '2785 Drake St., Stockton'!
INSERT INTO  fam SET name = 'Pat Mingo',
                    deptno = 50,
               status = 'married',
               depend = 2,
               address = '2751 Peer St., #1, Reno'!
INSERT INTO fam SET name = 'Dana Smith',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = 'PO Box 571, Rt 51, Waterloo'!
INSERT INTO fam SET name = 'Mary Rowen',
                    deptno = 60,
               status = 'married',
               depend = 3,
               address = '27451 Mills St., San Rafael'!
INSERT INTO fam SET name = 'Jill Crowley',
                    deptno = 40,
               status = 'divorced',
               depend = 1,
               address = '5671 Powell St., S.F.'!
INSERT INTO fam SET name = 'Jerry Denny',
                    deptno = 70,
               status = 'married',
               depend = 4,
               address = '67511 Market St., Stockton'!
INSERT INTO fam SET name = 'Alice Teir',
                    deptno = 50,
               status = 'single',
               depend = 1,
               address = '2751 Maple Ct., Davis'!
INSERT INTO fam SET name = 'Dave Cowel',
                    deptno = 60,
               status = 'divorced',
               depend = 1,
               address = '1561 Main St., French Camp'!
INSERT INTO fam SET name = 'George Taylor',
                    deptno = 10,
               status = 'married',
               depend = 5,
               address = '56715 - 31st St., Sacramento'!
INSERT INTO fam SET name = 'Joan Bird',
                    deptno = 50,
               status = 'divorced',
               depend = 1,
               address = '2731 Coronado Wy, Pacifica'!
INSERT INTO fam SET name = 'Ed Nader',
                    deptno = 70,
               status = 'divorced',
               depend = 1,
               address = '774 S. St., Victor'!
INSERT INTO fam SET name = 'Joyce Adams',
                    deptno = 70,
               status = 'married',
               depend = 2,
               address = '8741 Takay Bowl Blvd, Lodi'!
INSERT INTO dept SET deptno = 10,
              deptname = 'Management',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 40,
              deptname = 'Sales',
              locname  = 'San Francisco'!
INSERT INTO dept SET deptno = 50,
              deptname = 'Materials',
              locname   = 'Sacramento'!
INSERT INTO dept SET deptno = 60,
              deptname = 'Production',
              locname  = 'Stockton'!
INSERT INTO dept SET deptno = 70,
              deptname = 'Supply',
              locname  = 'Lodi'!
COMMIT WORK!
SELECT emp.name, fam.address, emp.currsal
  FROM emp, fam
  WHERE  fam.status = 'married'
                   AND depend > 2
                   AND fam.name = emp.name   !
FETCH !
CLOSE !

declare res cursor for 
select manager.name man_name, manager.currsal man_sal ,
                      emp.name emp_name , emp.currsal emp_sal
  FROM emp, emp  manager
  WHERE manager.sex = 'female'
            AND manager.name = emp.manager
            AND emp.sex = 'male'  !
FETCH res!
CLOSE res!
declare res cursor for 
select emp.name emp_name, emp.currsal emp_sal,
            manager.name man_name , manager.currsal man_sal 
   FROM emp, emp  manager
   WHERE emp.manager = manager.name
             AND emp.currsal > manager.currsal  !
FETCH res!
CLOSE res!
declare res cursor for 
select emp.name,fam.status,dept.locname,fam.address
  FROM emp, fam, dept
  WHERE emp.sex = 'female'
            AND emp.name = fam.name
            AND fam.status = 'single'
            AND fam.deptno = dept.deptno  
  ORDER BY NAME!
FETCH res!
CLOSE res !

declare result cursor for 
select fam.name, fam.address, dept.locname FROM fam, dept
       WHERE fam.address not like '%Stockton%'
                 AND fam.deptno = dept.deptno
                 AND dept.locname = 'Stockton' !
FETCH result!
CLOSE result!
declare result cursor for 
select fam.name, fam.address, emp.currsal, dept.locname
       FROM dept, emp, fam
       WHERE fam.name sounds 'Marie Rown'
                  AND fam.deptno = dept.deptno
                  AND fam.name = emp.name !
FETCH result!
CLOSE result!
declare result cursor for 
select fam.name, fam.address, dept.locname FROM dept, fam
       WHERE fam.deptno = dept.deptno
                  AND dept.deptno BETWEEN 40 AND 60
       ORDER BY dept.locname, fam.name !
FETCH result!
CLOSE result!
declare result cursor for 
select manager.name man_name , manager.currsal man_sal ,
               emp.name emp_name, emp.currsal emp_sal , fam.address
       FROM emp, emp  manager, fam
       WHERE manager.sex = 'female'
                 AND manager.name = emp.manager
                 AND emp.name = fam.name
                 AND (fam.address like '%Stockton'
                 OR fam.address like '%San Rafael')
       ORDER BY emp.name, manager.name !
FETCH result!
CLOSE result!
declare result cursor for 
select emp.name, fam.status, dept.locname, fam.address
       FROM emp, fam, dept
       WHERE emp.sex = 'female'
                AND emp.name = fam.name
                AND fam.address like '27_1%'
                AND fam.deptno = dept.deptno !
FETCH result!
CLOSE result!
declare result cursor for 
select name, currsal, startsal FROM emp
       WHERE name NOT LIKE '____ %'
       ORDER BY currsal, startsal !
FETCH result!
CLOSE result!
SELECT name, currsal, startsal FROM emp
       WHERE name LIKE '____ %'
       ORDER BY currsal, startsal !
FETCH!
CLOSE !
declare result cursor for 
select fam.name, fam.address, dept.locname FROM dept, fam
       WHERE fam.address not like '%Stockton%'
                AND fam.deptno = dept.deptno
                AND dept.locname = 'Stockton' !
FETCH result!
CLOSE result!
CREATE INDEX "emp.manager" on emp( manager ) !
CREATE INDEX "fam.status" on fam( status ) !
CREATE INDEX "dept.locname" on dept( locname ) !
commit work release!
file connect ( kern test !
drop table dept !
drop table emp !
drop table fam !
commit work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* T04SEL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
CONNECT kern IDENTIFIED BY test isolation level  0 !
create table p3   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p4   (k1 char(1) key, f1 char(1) not null) !
commit work !
create table p5   (k1 char(1) key, f1 char(1) not null) !
commit work !
create view p31 as
       select * from p3
              where k1 < 'd' !
commit work !
create view p32 as
       select * from p3
              where k1 >= 'b' and k1 < 'r' !
commit work !
create view p33 as
       select * from p3
              where k1 = 'c' or k1 = 'x' !
commit work !
create view p34 as
       select * from p3
              where k1 = 'c' or f1 = 'x' !
commit work !
insert into p3 set k1 = 'a', f1 = 'c' !
insert into p3 set k1 = 'c', f1 = 'c' !
insert into p3 set k1 = 'x', f1 = 'x' !
insert into p4 set k1 = 'u', f1 = 'u' !
insert into p4 set k1 = 'x', f1 = 'x' !
insert into p4 set k1 = 'b', f1 = 'c' !
insert into p5 set k1 = 'l', f1 = 'l' !
insert into p5 set k1 = 'm', f1 = 'c' !
insert into p5 set k1 = 'n', f1 = 'u' !
insert into p5 set k1 = 'o', f1 = 'o' !
declare erg4 cursor for 
select p3.k1 f1,p3.f1 f2,p4.k1 f3,p4.f1 f4
     from p3, p4
     where p3.f1 = p4.f1 !
fetch erg4 !
declare erg5 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p4.f1 = p3.f1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.k1 = p4.f1 and
             p3.k1 = p4.k1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 <> p4.k1 !
fetch erg5 !
close erg5 !
declare erg5 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 = 'x' !
fetch erg5 !
close erg5 !
declare erg5 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
       from p3, p4
       where p3.f1 = p4.f1 and
             p3.k1 = 'b' !
fetch erg5 !
close erg5 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 = p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 > p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 < p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 >= p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 <= p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.f1 <> p4.f1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 = p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 > p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 < p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 >= p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 <= p4.k1 !
fetch erg6 !
close erg6 !
declare erg6 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4
     from p3, p4
     where p3.k1 <> p4.k1 !
fetch erg6 !
close erg6 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p3.f1 = p4.f1 and
           p4.f1 = p5.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p5.f1 = p4.f1 and
           p4.f1 = p3.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p5.f1 = p4.f1 and
           p3.f1 = p4.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p3.f1 = p4.f1 and
           p5.f1 = p4.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p4.f1 = p3.f1 and
           p4.f1 = p5.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
     from p3, p4, p5
     where p4.f1 = p5.f1 and
           p4.f1 = p3.f1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
       from p3, p4, p5
       where p3.f1 = p4.f1 and
             p4.k1 = p5.k1 !
fetch erg7 !
close erg7 !
declare erg7 cursor for 
select p3.k1 e1,p3.f1 e2,p4.k1 e3,p4.f1 e4,p5.k1 e5,p5.f1 e6
       from p3, p4, p5
       where p3.f1 = p4.f1 and
             p4.k1 < p5.k1 !
fetch erg7 !
close erg7 !
commit work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* T05TEST.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
CREATE TABLE kern.p6 (k1 char(1) key RANGE in ('a','c','x'),
                    f1 char(1) not null RANGE in ('c','x') ) !
CREATE TABLE kern.p7   (k1 char(1) key RANGE in ('a','x'),
                    f1 char(1) not null RANGE in ('u','x','c')) !
CREATE TABLE kern.p8   (k1 char(1) key,
                    f1 char(1) not null) !
COMMIT WORK !
LOCK TABLE p6 , p7 IN EXCLUSIVE MODE !
INSERT INTO p6 SET k1 = 'a', f1 = 'c' !
INSERT INTO p6 SET k1 = 'c', f1 = 'c' !
INSERT INTO p6 SET k1 = 'x', f1 = 'x' !
INSERT INTO p7 SET k1 = 'x', f1 = 'x' !
INSERT INTO p7 SET k1 = 'a', f1 = 'c' !
INSERT INTO p8 SET k1 = 'x', f1 = 'x' !
INSERT INTO p8 SET k1 = 'a', f1 = 'c' !
COMMIT WORK !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 !
FETCH erg7 !
CLOSE erg7 !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 AND
       p6.k1 = 'x' !
FETCH erg7 !
CLOSE erg7 !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p7.k1 f3,p7.f1 f4
       FROM p6, p7
       WHERE p6.k1 = p7.k1 AND
       p6.k1 = 'c' !
FETCH erg7 !
CLOSE erg7 !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 !
FETCH erg7 !
CLOSE erg7 !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 AND
       p6.k1 = 'x' !
FETCH erg7 !
CLOSE erg7 !
declare erg7 cursor for 
select p6.k1 f1,p6.f1 f2,p8.k1 f3,p8.f1 f4
       FROM p6, p8
       WHERE p6.k1 = p8.k1 AND
       p6.k1 = 'c' !
FETCH erg7 !
CLOSE erg7 !
commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
