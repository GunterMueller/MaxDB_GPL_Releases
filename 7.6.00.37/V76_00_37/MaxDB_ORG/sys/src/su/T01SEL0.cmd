*   *ID* T01SEL0  DBCMD    changed on 1992-09-28-13.28.32 by ADMIN     *
 ***************************
 ***   T 0 1 S E L 0     ***
 ***************************!
DECLARE RESULT CURSOR FOR 
SELECT name, deptno, currsal, manager, startsal, sex
  FROM emp!
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT name, deptno, status, depend, address
  FROM fam!
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT deptno, deptname, locname
  FROM dept!
FETCH result!
CLOSE result!
