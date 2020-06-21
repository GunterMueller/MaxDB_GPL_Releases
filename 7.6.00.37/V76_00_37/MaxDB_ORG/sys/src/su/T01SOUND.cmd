*   *ID* T01SOUND DBCMD    changed on 1992-09-28-13.28.33 by ADMIN     *
 ***************************
 ***   T 0 1 S O U N D   ***
 ***************************!
SELECT name, currsal, startsal
  FROM emp !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name SOUNDS 'Phaul' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name SOUNDS 'Paulrogers' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name LIKE 'J__n%' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name NOT LIKE 'J__n%' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name NOT LIKE 'J__n%Jo%'
     AND name SOUNDS 'Jon' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name SOUNDS 'Jon' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name SOUNDS 'Jon'
     AND startsal BETWEEN 6500 AND 7500!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name NOT SOUNDS 'Jon'
     AND startsal BETWEEN 6500 and 7500!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name SOUNDS 'Jon'
     AND NOT (startsal BETWEEN 6500 and 7500)!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM emp
  WHERE name NOT SOUNDS 'John'
     AND NOT (startsal BETWEEN 6500 AND 7500)!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name SOUNDS 'Phaul' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name SOUNDS 'Paulrogers' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name LIKE 'J__n%' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name NOT LIKE 'J__n%' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name NOT LIKE 'J__n%Jo%'
     AND name SOUNDS 'Jon' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name SOUNDS 'Jon' !
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name SOUNDS 'Jon'
     AND startsal BETWEEN 6500 AND 7500!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name NOT SOUNDS 'Jon'
     AND startsal BETWEEN 6500 AND 7500!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name SOUNDS 'Jon'
     AND NOT (startsal BETWEEN 6500 AND 7500)!
FETCH !
CLOSE !
SELECT  name, currsal, startsal
  FROM rich_emp
  WHERE name NOT SOUNDS 'Jon'
     AND NOT (startsal BETWEEN 6500 AND 7500)!
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM married !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM divorced !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM single !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam
  WHERE name SOUNDS 'Gorge Tailer'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM fam
  WHERE name SOUNDS 'George Teller'!
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam
  WHERE name SOUNDS 'Grg Tlr'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM fam
  WHERE name SOUNDS 'Marie Rown'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM fam
  WHERE address LIKE '%St.%' !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam
  WHERE address LIKE '%Drake%'!
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam
  WHERE address LIKE '1%St.%'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM fam
  WHERE name SOUNDS 'Jon'
     AND address LIKE '%St%'!
FETCH !
CLOSE !
SELECT  name, status, address
  FROM fam
  WHERE (address NOT LIKE '%5%'
           OR address NOT LIKE '%2%')
           AND address LIKE '%St%'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM married
  WHERE name NOT SOUNDS 'Jon'
     AND address LIKE '%St%'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM divorced
  WHERE address LIKE '%St%' !
FETCH !
CLOSE !
SELECT  name, status, address
  FROM single
  WHERE address LIKE '%Drake%'!
FETCH !
CLOSE !
SELECT    name, status, address
  FROM married
  WHERE address LIKE '1%%St%'!
FETCH !
CLOSE !
SELECT  name, status, address
  FROM divorced
  WHERE name SOUNDS 'Jon'
     AND address LIKE '%St%'!
FETCH !
CLOSE !
SELECT name, status, address
  FROM single
  WHERE (address NOT LIKE '%5%'
           OR address NOT LIKE '%2%')
           AND address LIKE '%St%'!
FETCH !
CLOSE !
