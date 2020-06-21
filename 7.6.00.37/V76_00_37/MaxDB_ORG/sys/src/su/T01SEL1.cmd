*   *ID* T01SEL1  DBCMD    changed on 1992-09-28-13.28.32 by ADMIN     *
 ***************************
 ***   T 0 1 S E L 1     ***
 ***************************!
SELECT name, currsal, sex
  FROM emp!
FETCH !
CLOSE !
SELECT name, currsal, sex
  FROM emp
  ORDER BY currsal DESC  !
FETCH !
CLOSE !
SELECT name, currsal, sex
  FROM emp
  WHERE sex = 'female'
  ORDER BY currsal DESC  !
FETCH !
CLOSE !
SELECT name, currsal, sex
  FROM emp
  WHERE sex = 'female'
              AND currsal > 15000
  ORDER BY currsal DESC  !
FETCH!
CLOSE !
SELECT name, currsal, startsal, sex
  FROM emp
  WHERE sex = 'female'
            OR  startsal BETWEEN 10000 AND 12000 !
FETCH !
CLOSE !
