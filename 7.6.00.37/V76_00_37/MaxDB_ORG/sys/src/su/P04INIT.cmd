*   *ID* P04INIT  DBCMD    changed on 1992-09-28-13.27.44 by ADMIN     *
file connect ( kern test !
 *******************
 *                 *
 *  P 0 4 I N I T  *
 *                 *
 ******************!
CREATE TABLE emp ( name CHAR (15) key,
                       deptno FIXED (2) not null
                           RANGE BETWEEN 10 AND 90,
                       currsal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
commit work  !
LOCK TABLE emp IN EXCLUSIVE MODE !
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
INSERT INTO emp SET name = 'Alice Teir',
             deptno = 50,
             currsal = 13500,
             manager = 'Fred Gerard',
             startsal = 8500,
             sex = 'female'!
INSERT INTO emp SET name = 'Alice Te',
             deptno = 50,
             currsal = 12000,
             manager = 'Fred Gerardus',
             startsal = 7000,
             sex = 'female'!
commit work !
CREATE TABLE emp1 ( name CHAR (7) key,
                       deptno FIXED (2) not null
                            range BETWEEN 10 AND 90,
                       currsal FIXED (5) NOT NULL,
                       manager CHAR (15) NOT NULL,
                       startsal FIXED (5) NOT NULL,
                       sex CHAR (6) NOT NULL ) !
commit work  !
LOCK TABLE emp IN EXCLUSIVE MODE !
INSERT INTO emp1 SET name = 'Fred',
             deptno = 50,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 8000,
             sex = 'male'!
INSERT INTO emp1 SET name = 'Norma',
             deptno = 40,
             currsal = 15500,
             manager = 'Jill Crowley',
             startsal = 7500,
             sex = 'female'!
INSERT INTO emp1 SET name = 'Paul',
             deptno = 70,
             currsal = 15000,
             manager = 'George Taylor',
             startsal = 7000,
             sex = 'male'!
INSERT INTO emp1 SET name = 'Alice',
             deptno = 50,
             currsal = 13500,
             manager = 'Fred Gerard',
             startsal = 8500,
             sex = 'female'!
INSERT INTO emp1 SET name = 'Alic',
             deptno = 50,
             currsal = 12000,
             manager = 'Fred Gerardus',
             startsal = 7000,
             sex = 'female'!
commit work release !
