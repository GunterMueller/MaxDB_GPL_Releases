*   *ID* A04SEL2  DBCMD    changed on 1992-09-28-13.22.25 by ADMIN     *
 ***************************
 ***   A 0 4 S E L 2     ***
 ***************************!
file connect ( kern test !
CREATE TABLE kern.G1 ( F1 FIXED (6), F2 FIXED (6), F3 FIXED (6)) !
INSERT kern.G1 VALUES ( 1, 2, 3 ) !
INSERT kern.G1 VALUES ( 2, 2, 2 ) !
INSERT kern.G1 VALUES ( 4,5,6   ) !
INSERT kern.G1 VALUES ( 7,8,9   ) !
INSERT kern.G1 VALUES ( 4,4,4   ) !
INSERT kern.G1 VALUES ( 8,8,8   ) !
COMMIT WORK !
select * from kern.g1 !
FETCH !
SELECT *  FROM kern.G1 WHERE ( F1 < 5 ) ORDER BY F1 !
FETCH !
SELECT *  FROM kern.G1 WHERE ( ) ( F1 < 5 ) ORDER BY F1 !
SELECT *  FROM kern.G1 WHERE ((  )AND )  ( F1 < 5 ) ORDER BY F1 !
SELECT *  FROM kern.G1 WHERE ((  ) OR )  ( F1 < 5 ) ORDER BY F1 !
SELECT *  FROM kern.G1 WHERE NOT (( )) NOT( ( F1 < 5 ))( )ORDER BY F1 !
SELECT *  FROM kern.G1 WHERE NOT  OR (NOT(F1 < 5 ))(  )ORDER BY F1 !
SELECT *  FROM kern.G1 WHERE( NOT)OR (NOT(F1 < 5 ))(  )ORDER BY F1 !
 **** create view as ***** !
create view kern.vg1  as select *  FROM kern.G1
      where  ( ) ( F1 < 5 )  !
create view kern.vg10 as select *  FROM kern.G1
      where  (( )) NOT(  ( F1 < 5 ))(  ) !
create view kern.vg11 as select *  FROM kern.G1
      where  NOT (( )) NOT(  ( F1 < 5 ))(  ) !
create view kern.vg14 as select *  FROM kern.G1
      where  NOT  OR (NOT(F1 < 5 ))(  ) !
create view kern.vg15 as select *  FROM kern.G1
      where ( NOT)OR (NOT(F1 < 5 ))(  ) !
COMMIT WORK RELEASE !
