*   *ID* A17SEL10 DBCMD    changed on 1992-09-28-13.22.40 by ADMIN     *
file connect ( kern test !
CREATE TABLE A ( A FIXED (3) KEY, B CHAR (3) )!
INSERT A SET A = 1, B = 'aaa'!
INSERT A SET A = 2, B = 'bbb'!
INSERT A SET A = 3, B = 'ccc'!
INSERT A SET A = 4, B = 'ddd'!
INSERT A SET A = 5, B = 'EEE'!
INSERT A SET A = 6, B = 'fff'!
SELECT * FROM A ORDER BY A !
FETCH POS (3)!
FETCH NEXT!
FETCH POS (1)!
MFETCH !
3
FETCH POS (2) !
FETCH POS (4) !
MFETCH POS (4) !
1
FETCH PREV !
FETCH POS (3) !
drop table a !
commit work release !
