*   *ID* A13SEL   DBCMD    changed on 1992-09-28-13.22.34 by ADMIN     *
 ***************************
 ***   A 1 3 S E L       ***
 ***************************!
file connect ( kern test !
SELECT maxrecord.*  FROM maxrecord  WHERE
   KEY1  >= 's' !
SELECT KEY1, KEY2, KEY3, KEY4  FROM maxrecord  WHERE
   KEY1 >= 's' !
FETCH !
SELECT KEY1, KEY2, KEY3, KEY4  FROM MAXrec1  WHERE
   KEY1 <= 's' !
FETCH !
SELECT MAXREC1.KEY1, MAXREC1.KEY2, 
       MAXREC1.KEY3, MAXREC1.KEY4  
   FROM maxrecord, MAXREC1   WHERE
       MAXREC1.KEY1 <= 's'  AND  MAXREC1.A110F1 = maxrecord.A110F1  !
FETCH !
SELECT MAXREC1.KEY1, MAXREC1.KEY2, 
       MAXREC1.KEY3, MAXREC1.KEY4  
   FROM maxrecord, MAXREC1   WHERE
  maxrecord. KEY10 >= 's'  AND
  maxrec1. KEY1 <= 's'  AND  maxrec1.A110F1 = maxrecord.A110F1  !
FETCH !
select key1, key2, key3, b210f4  from maxrec1  where
   key1 < b210f4  !
FETCH !
select key1, key2, key3, b210f1, b210f4  from maxrec1  where
   b210f4  >=   b210f1  !
FETCH !
SELECT maxrecord.*  FROM maxrecord  WHERE
   KEY1  >= 's' !
SELECT MAXREC1.*               FROM maxrecord, MAXREC1   WHERE
  maxrecord. KEY10 >= 's'  AND
  MAXREC1. KEY1 <= 's'  AND  MAXREC1.A110F1 = maxrecord.A110F1  !
ERRORMSG LONG !
SELECT MAXREC1.* FROM MAXREC1, maxrecord  WHERE
MAXREC1.KEY1 = 'a1' AND maxrecord.KEY1 = MAXREC1.KEY1 !
FETCH !
commit work !
alter table maxrec1 add col255 char (1) !
select * from maxrec1 !
fetch !
rollback work release !
