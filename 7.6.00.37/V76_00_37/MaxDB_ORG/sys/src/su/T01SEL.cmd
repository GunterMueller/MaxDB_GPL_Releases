*   *ID* T01SEL   DBCMD    changed on 1992-09-28-13.28.31 by ADMIN     *
 ***************************
 ***   T 0 1 S E L       ***
 ***************************!
SELECT name, deptno, currsal
  FROM emp
  ORDER BY currsal DESC, name ASC  !
FETCH !
CLOSE !
SELECT locname
  FROM dept !
FETCH !
CLOSE !
SELECT DISTINCT locname
  FROM dept !
FETCH !
CLOSE !
DECLARE RESULT CURSOR FOR 
SELECT name n, address a
  FROM fam
  WHERE status = 'married' AND depend > 2 for reuse !
FETCH result !
SELECT result.n, result.a, emp.currsal
  FROM result, emp
  WHERE  result.n = emp.name  !
FETCH !
CLOSE result !
CLOSE !
SELECT emp.name, fam.address, emp.currsal
  FROM emp, fam
  WHERE  fam.status = 'married'
                   AND depend > 2
                   AND fam.name = emp.name   !
FETCH !
CLOSE !
