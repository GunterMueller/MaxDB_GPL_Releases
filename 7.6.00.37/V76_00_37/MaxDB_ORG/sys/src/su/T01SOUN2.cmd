*   *ID* T01SOUN2 DBCMD    changed on 1992-09-28-13.28.34 by ADMIN     *
 ***************************
 ***   T 0 1 S O U N 2   ***
 ***************************!
DECLARE RESULT CURSOR FOR 
SELECT name, currsal, startsal FROM emp
       WHERE name SOUNDS 'Phaul' !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT name, currsal, startsal FROM emp
       WHERE name NOT LIKE 'J__n%'
             AND (currsal BETWEEN 16000 AND 18000
                  OR startsal > 8000)
       ORDER BY currsal, startsal !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT name, status, address FROM fam
       WHERE name LIKE '% T%'  !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT fam.name, fam.address, dept.locname FROM fam, dept
       WHERE fam.address not like '%Stockton%'
                 AND fam.deptno = dept.deptno
                 AND dept.locname = 'Stockton' !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT fam.name, fam.address, emp.currsal, dept.locname
       FROM dept, emp, fam
       WHERE fam.name sounds 'Marie Rown'
                  AND fam.deptno = dept.deptno
                  AND fam.name = emp.name !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT fam.name, fam.address, dept.locname FROM dept, fam
       WHERE fam.deptno = dept.deptno
                  AND dept.deptno BETWEEN 40 AND 60
       ORDER BY dept.locname, fam.name !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT manager.name man_name, manager.currsal man_sal,
       emp.name emp_name, emp.currsal emp_sal, fam.address
       FROM emp, emp  manager, fam
       WHERE manager.sex = 'female'
                 AND manager.name = emp.manager
                 AND emp.name = fam.name
                 AND (fam.address like '%Stockton'
                 OR fam.address like '%San Rafael')
       ORDER BY emp.name, manager.name !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT emp.name, fam.status, dept.locname, fam.address
       FROM emp, fam, dept
       WHERE emp.sex = 'female'
                AND emp.name = fam.name
                AND fam.address like '27_1%'
                AND fam.deptno = dept.deptno !
FETCH result!
CLOSE result!
DECLARE RESULT CURSOR FOR 
SELECT name, currsal, startsal FROM emp
       WHERE name NOT LIKE '____ %'
       ORDER BY currsal, startsal !
FETCH result!
CLOSE result!
SELECT name, currsal, startsal FROM emp
       WHERE name LIKE '____ %'
       ORDER BY currsal, startsal !
FETCH!
CLOSE !
DECLARE RESULT CURSOR FOR 
SELECT fam.name, fam.address, dept.locname FROM dept, fam
       WHERE fam.address not like '%Stockton%'
                AND fam.deptno = dept.deptno
                AND dept.locname = 'Stockton' !
FETCH result!
CLOSE result!
