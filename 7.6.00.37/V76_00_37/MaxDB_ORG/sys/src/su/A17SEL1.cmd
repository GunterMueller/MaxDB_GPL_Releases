*   *ID* A17SEL1  DBCMD    changed on 1992-09-28-13.22.40 by ADMIN     *
FILE CONNECT ( kern test !
CREATE TABLE T1 ( A CHAR (4) KEY, B CHAR (11) KEY,
     C CHAR (22), D CHAR (5) NOT NULL, E CHAR (7),
                  F FIXED (6))!
INSERT T1 SET B = 'ABCDEFG', A = 'NULL', E = 'MANNN',
                                  D = 'ZYXWV' !
INSERT T1 SET B = 'ABCDEFG', A = 'EINS', E = 'MANNN',
              C = '123456789012', D = 'ZYXWV' !
INSERT T1 VALUES ('ZWEI', 'ABCDEFGHIJK', NULL, 'NOTNU', '1234567', NULL)!
INSERT T1 (B, A, D, C, F) VALUES ('10987654321', 'DREI', '55555','12',3)!
pars_execute !
commit work release !
