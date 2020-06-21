*   *ID* A17SEL3  DBCMD    changed on 1992-09-28-13.22.46 by ADMIN     *
file connect ( kern test !
CREATE TABLE T3 ( A CHAR (4) KEY, B FIXED (5,2)) !
INSERT T3 VALUES ('AAA', 11)!
INSERT T3 VALUES ('BAA', 11)!
INSERT T3 VALUES ('CAA', 13)!
INSERT T3 VALUES ('DAA', 13)!
INSERT T3 VALUES ('EAA', 15)!
INSERT T3 VALUES ('FAA', 16)!
SELECT * FROM T3 !
FETCH !
FETCH FIRST !
FETCH NEXT !
FETCH POS (3)!
FETCH PREV !
FETCH LAST !
FETCH PREV !
SELECT * FROM T3 !
FETCH1 POS (3) !
FETCH ABSOLUTE 5 !
FETCH1 RELATIVE 2 !
fetch same !
FETCH1 RELATIVE +2 !
FETCH1 RELATIVE -2 !
FETCH1 RELATIVE -2 !
fetch same !
FETCH1 RELATIVE -2 !
FETCH1 RELATIVE -2 !
fetch relative 2 !
fetch absolute 0 !
fetch absolute 2 !
FETCH1 RELATIVE 4 !
FETCH1 RELATIVE 0 !
FETCH1 ABSOLUTE -4 !
FETCH1 ABSOLUTE -2 !
pars_then_ex !
data !
FETCH1 POS (:a) !
n 10 3

FETCH ABSOLUTE :a !
n 10 5

FETCH1 RELATIVE :a !
n 10 2

fetch same !

FETCH1 RELATIVE :a !
n 10 2

FETCH1 RELATIVE :a !
n 10 -2

FETCH1 RELATIVE :a !
n 10 -2

fetch same !

FETCH1 RELATIVE :a !
n 10 -2

FETCH1 RELATIVE :a !
n 10 -2

fetch relative :a !
n 10 2

fetch absolute :a !
n 10 0

fetch absolute :a !
n 10 2

FETCH1 RELATIVE :a !
n 10 4

FETCH1 RELATIVE :a !
n 10 0

FETCH1 ABSOLUTE :a !
n 10 -4

FETCH1 ABSOLUTE :a !
n 10 -2

nodata !
* PTS 1002020 !
fetch1 last !
fetch relative -2 !
fetch relative -3 !
pars_execute !
DECLARE REST CURSOR FOR SELECT DISTINCT B FROM T3 !
DECLARE ERG1 CURSOR FOR SELECT DISTINCT B FROM T3 !
FETCH PREV ERG1 !
FETCH ERG1 !
FETCH FIRST ERG1 !
FETCH NEXT ERG1 !
FETCH POS (3) ERG1 !
FETCH PREV ERG1 !
FETCH LAST ERG1 !
SELECT * FROM ERG1 WHERE B > 12 !
FETCH !
SELECT DISTINCT A,B FROM T3 !
FETCH1 POS (3) !
FETCH ABSOLUTE 5 !
FETCH1 RELATIVE 2 !
fetch same !
FETCH1 RELATIVE +2 !
FETCH1 RELATIVE -2 !
FETCH1 RELATIVE -2 !
fetch same !
FETCH1 RELATIVE -2 !
FETCH1 RELATIVE -2 !
fetch relative 2 !
fetch absolute 0 !
fetch absolute 2 !
FETCH1 RELATIVE 4 !
FETCH1 RELATIVE 0 !
FETCH1 ABSOLUTE -4 !
FETCH1 ABSOLUTE -2 !
pars_then_ex !
data !
FETCH1 POS (:a) !
n 10 3

FETCH ABSOLUTE :a !
n 10 5

FETCH1 RELATIVE :a !
n 10 2

fetch same !

FETCH1 RELATIVE :a !
n 10 2

FETCH1 RELATIVE :a !
n 10 -2

FETCH1 RELATIVE :a !
n 10 -2

fetch same !

FETCH1 RELATIVE :a !
n 10 -2

FETCH1 RELATIVE :a !
n 10 -2

fetch relative :a !
n 10 2

fetch absolute :a !
n 10 0

fetch absolute :a !
n 10 2

FETCH1 RELATIVE :a !
n 10 4

FETCH1 RELATIVE :a !
n 10 0

FETCH1 ABSOLUTE :a !
n 10 -4

FETCH1 ABSOLUTE :a !
n 10 -2

pars_execute !
nodata !
select * from t3 !
fetch relative 3 !
select * from t3 !
fetch1 last !
fetch relative -3 !
fetch1 last into :a, :b !
fetch1 relative -1 into :a, :b !
fetch1 into :a, :b !
fetch1 relative -1 into :a, :b !
fetch1 relative -1 into :a, :b !
fetch1 into :a, :b !
fetch1 relative -1 into :a, :b !
fetch1 relative 2 into :a, :b !
drop table t3 !
COMMIT WORK RELEASE !
