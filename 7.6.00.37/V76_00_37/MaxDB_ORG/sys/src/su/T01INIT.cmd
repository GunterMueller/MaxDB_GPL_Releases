*   *ID* T01INIT  DBCMD    changed on 1992-09-28-13.28.30 by ADMIN     *
 ***************************
 ***   T 0 1 I N I T     ***
 ***************************!
CREATE TABLE emp  ( name CHAR (15) ascii KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       currsal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
if &rc <> 0 then return
file sel_cols_1tab ( KERN EMP !
CREATE TABLE fam   ( name CHAR (15) ascii KEY,
                       deptno FIXED (2) NOT NULL RANGE BETWEEN 10 AND 90,
                       status CHAR (8) NOT NULL,
                       depend FIXED (2) NOT NULL RANGE BETWEEN 0 AND 15,
                       address CHAR (30) NOT NULL ) !
file sel_cols_1tab ( KERN FAM !
CREATE TABLE dept  ( deptno FIXED (2) KEY RANGE BETWEEN 10 AND 90 ,
                        deptname CHAR (10) NOT NULL,
                        locname  CHAR (15) NOT NULL )  !
commit work !
file sel_cols_1tab ( KERN DEPT !
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
file sel_table !
file sel_view ( MARRIED !
file sel_view ( DIVORCED !
file sel_view ( SINGLE !
file sel_view ( RICH_EMP !
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
