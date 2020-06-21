*   *ID* A17SEL9  DBCMD    changed on 1992-09-28-13.22.49 by ADMIN     *
file connect ( kern test !
CREATE TABLE T ( A CHAR (4) KEY, B CHAR (4) not null)!
CREATE INDEX "T.B"
	on T( B ) !
INSERT T SET A = 'aaa', B = 'bbb'!
INSERT T SET A = 'abc', B = 'az '!
drop table t !
commit work release !
file connect ( kern test !
CREATE TABLE T ( A CHAR (4) KEY, B CHAR (40) ,
                 C CHAR(4)  )!
CREATE INDEX I1 ON T (B, C)!
INSERT T SET A = 'AAA', B = 'BBB', C = 'DDD'!
INSERT T SET A = 'ABC', B = 'AZ ', C = 'DDD'!
drop table t !
commit work release !
