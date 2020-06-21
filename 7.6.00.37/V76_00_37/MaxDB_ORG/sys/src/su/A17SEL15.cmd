*   *ID* A17SEL15 DBCMD    changed on 1992-09-28-13.22.42 by ADMIN     *
file connect ( kern test !
CREATE TABLE T ( A CHAR (4) KEY, B CHAR (6), C FIXED (6))!
INSERT T VALUES ('AAA', 'FFF', 1)!
INSERT T VALUES ('CCC', 'EEE', 2)!
INSERT T VALUES ('EEE', 'FFF', 3)!
INSERT T VALUES ('GGG', 'GGG', 4)!
CREATE INDEX I1 ON T (B, C)!
SELECT * FROM T WHERE C = 3 OR C = 4 !
SELECT * FROM T WHERE A > 'A' AND  B = 'FFF' AND (C = 3 OR C = 4)!
SELECT * FROM T WHERE (B = 'FFF' OR C = 4) AND C = 5!
SELECT * FROM T WHERE B = 'FFF' AND (C = 3 OR C = 4) AND C = 5!
SELECT * FROM T WHERE
     A > 'A' AND
     ((B = 'FFF' AND (C = 5 OR C = 3)) OR C = 4) AND
     B = 'FFF'!
SELECT * FROM T WHERE A > 'A' AND
                      (C = 4 OR ((C = 5 OR C = 3) AND B = 'FFF' AND
                                   A < 'Z' AND A > 'A' AND
                                               B <> 'GGG')) AND
                      B = 'FFF'!
SELECT * FROM T WHERE A > 'A' AND
                      (C = 5     OR B = 'DDD' OR B <> 'SSS') AND

                      ((C = 3 AND B = 'FFF' AND A < 'Z' AND A > 'A')
                                OR C = 4) AND
                      B = 'FFF'!
SELECT * FROM T WHERE A > 'A' AND
                      (C = 5     OR B = 'DDD' OR B <> 'SSS') AND

              ((C = 3 AND B = 'FFF' AND A < 'Z' AND A > 'A' AND A > 'AA')
                                OR C = 4) AND
                      B = 'FFF'!
SELECT * FROM T WHERE A > 'A' AND
                      (C = 5     OR B = 'DDD' OR B = 'SSS') AND

                      ((C = 3 AND B = 'FFF' AND A < 'Z' AND A > 'A')
                                OR C = 4) AND
                      B = 'FFF'!
DROP INDEX I1 ON T !
CREATE INDEX "T.C"
	on T( C ) !
SELECT * FROM T WHERE (C > 3 OR C < 5)!
CREATE INDEX "T.C"
	on T( C ) !
SELECT * FROM T WHERE C = 3 OR C = 4 !
SELECT * FROM T WHERE C = 3 OR C = 4 OR C = 5 OR C = 6 OR C = 7!
SELECT * FROM T WHERE C <> 3 OR C = 4 OR C = 5 OR C = 6 OR C = 7!
SELECT * FROM T WHERE (C = 3 OR C = 4) AND B = 'FFF'!
SELECT * FROM T WHERE (B = 'FFF' OR C = 4) AND B = 'FFF'!
SELECT * FROM T WHERE (B <> 'FFF' OR C = 4) AND B = 'FFF'!
SELECT * FROM T WHERE ((B = 'FFF' AND C = 3) OR C = 4) AND B = 'FFF'!
SELECT * FROM T WHERE ((B <> 'FFF' AND C = 3) OR C = 4)
 AND B = 'FFF'!
SELECT * FROM T WHERE ((B = 'FFF' AND C = 3) OR C = 4)
 AND B = 'FFF'!
SELECT * FROM T WHERE
                      ((C = 3 AND B = 'FFF' AND A < 'Z' AND A > 'A' AND
                                                        B <> 'GGG')
                                OR C = 4) AND
                      B = 'FFF'!
SELECT * FROM T WHERE
                      (B = 'SSS' OR B = 'DDD' OR B <> 'SSS') AND

                      ((C = 3 AND B = 'FFF' AND A < 'Z' AND A > 'A')
                                OR C = 4) AND
                      B = 'FFF'!
* Fehlerprotokoll 162 !
pars_then_ex !
data !
select * from t where a = :a or a = :b !
c 4 'AAA' c 4 'EEE'

nodata !
fetch into :a, :b, :c !
pars_execute !
drop table t !
COMMIT RELEASE !
