*   *ID* A17SEL6  DBCMD    changed on 1992-11-11-17.01.54 by KERN      *
file connect ( kern test !
CREATE TABLE T6 ( A CHAR (4) KEY, B CHAR (5))!
INSERT T6 VALUES ('AAA', 'SWE')!
INSERT T6 VALUES ('BBB', 'BGT')!
INSERT T6 VALUES ('CCC', 'SWE')!
INSERT T6 VALUES ('DDD', 'LIO')!
INSERT T6 VALUES ('EEE', 'SWE')!
PARS_THEN_EX !
SELECT * FROM T6 !
FETCH INTO :A, :B !
PARS_EXECUTE !
CLOSE !
PARS_THEN_EX !
SELECT * FROM T6 WHERE B > 'KKK'!
FETCH LAST INTO :A, :B !
FETCH PREV !
PARS_THEN_EX !
DATA !
SELECT * FROM T6 WHERE B > :B   !
C 5 'KKK'
 
NODATA !
FETCH INTO :A, :B !
declare a cursor for SELECT * FROM T6 !
FETCH A !
PARS_EXECUTE !
CLOSE A!
drop table t6 !
COMMIT WORK RELEASE !