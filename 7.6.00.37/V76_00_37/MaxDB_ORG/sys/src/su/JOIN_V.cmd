senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* VER02280.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

CREATE TABLE VERI_CLNT (
CLIENT VARCHAR(3) ASCII DEFAULT '000',
ARG1 VARCHAR(3) ASCII DEFAULT ' ',
ARG2 VARCHAR(3) ASCII DEFAULT ' ',
FUNCTION VARCHAR(10) ASCII DEFAULT ' ',
FUNCTINT FIXED(10,0) DEFAULT 0,
PRIMARY KEY (CLIENT,ARG1,ARG2))!

INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 0' ,  ' 0' ,  'Text 001' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 0' ,  ' 1' ,  'Text 002' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 0' ,  ' 2' ,  'Text 003' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 0' ,  ' 3' ,  'Text 004' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 1' ,  ' 0' ,  'Text 005' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 1' ,  ' 1' ,  'Text 006' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 1' ,  ' 2' ,  'Text 007' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 1' ,  ' 3' ,  'Text 008' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 2' ,  ' 0' ,  'Text 009' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 2' ,  ' 1' ,  'Text 010' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 2' ,  ' 2' ,  'Text 011' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 2' ,  ' 3' ,  'Text 012' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 3' ,  ' 0' ,  'Text 013' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 3' ,  ' 1' ,  'Text 014' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 3' ,  ' 2' ,  'Text 015' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '000' ,  ' 3' ,  ' 3' ,  'Text 016' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 0' ,  ' 0' ,  'Text 017' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 0' ,  ' 1' ,  'Text 018' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 0' ,  ' 2' ,  'Text 019' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 0' ,  ' 3' ,  'Text 020' ,  0 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 1' ,  ' 0' ,  'Text 021' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 1' ,  ' 1' ,  'Text 022' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 1' ,  ' 2' ,  'Text 023' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 1' ,  ' 3' ,  'Text 024' ,  1 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 2' ,  ' 0' ,  'Text 025' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 2' ,  ' 1' ,  'Text 026' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 2' ,  ' 2' ,  'Text 027' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 2' ,  ' 3' ,  'Text 028' ,  2 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 3' ,  ' 0' ,  'Text 029' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 3' ,  ' 1' ,  'Text 030' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 3' ,  ' 2' ,  'Text 031' ,  3 )!
INSERT INTO VERI_CLNT VALUES ( '001' ,  ' 3' ,  ' 3' ,  'Text 032' ,  3 )!


CREATE TABLE "VERI_JOIN" (
CLIENT VARCHAR(3) ASCII DEFAULT '000',
ARG1 VARCHAR(3) ASCII DEFAULT ' ',
ARG2 VARCHAR(3) ASCII DEFAULT ' ',
FUNCTION VARCHAR(10) ASCII DEFAULT ' ',
FUNCTINT FIXED(10,0) DEFAULT 0,
FUNCTFLT FLOAT(20) DEFAULT 0.0000000000000000000E+00,
FUNCTPKD FIXED(8,2) DEFAULT 0.00,
PRIMARY KEY (CLIENT,ARG1,ARG2))!

INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 0' ,  ' 0' ,  'Text 001' ,  0 ,  0.0e+000 ,  0 )!
INSERT INTO VERI_JOIN VALUES 
 ( '000' ,  ' 0' ,  ' 1' ,  'Text 002' ,  0 ,  0.0e+000 ,  0 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 0' ,  ' 2' ,  'Text 003' ,  0 ,  0.0e+000 ,  0 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 0' ,  ' 3' ,  'Text 004' ,  0 ,  0.0e+000 ,  0 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 1' ,  ' 0' ,  'Text 005' ,  1 ,  1.0e+000 ,  1 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 1' ,  ' 1' ,  'Text 006' ,  1 ,  1.0e+000 ,  1 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 1' ,  ' 2' ,  'Text 007' ,  1 ,  1.0e+000 ,  1 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 1' ,  ' 3' ,  'Text 008' ,  1 ,  1.0e+000 ,  1 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 2' ,  ' 0' ,  'Text 009' ,  2 ,  2.0e+000 ,  2 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 2' ,  ' 1' ,  'Text 010' ,  2 ,  2.0e+000 ,  2 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 2' ,  ' 2' ,  'Text 011' ,  2 ,  2.0e+000 ,  2 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 2' ,  ' 3' ,  'Text 012' ,  2 ,  2.0e+000 ,  2 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 3' ,  ' 0' ,  'Text 013' ,  3 ,  3.0e+000 ,  3 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 3' ,  ' 1' ,  'Text 014' ,  3 ,  3.0e+000 ,  3 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 3' ,  ' 2' ,  'Text 015' ,  3 ,  3.0e+000 ,  3 )! 
INSERT INTO VERI_JOIN VALUES
 ( '000' ,  ' 3' ,  ' 3' ,  'Text 016' ,  3 ,  3.0e+000 ,  3 )! 


SELECT
 T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
 VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
( T_01 . CLIENT = '000' AND
T_00 . ARG1 = T_01 . ARG1 AND
T_00 . ARG2 = T_01 . ARG2 ) AND
T_00 . CLIENT = '000' !

SELECT
 T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
 VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
( T_01 . CLIENT = '000' AND
T_00 . ARG1 = T_01 . ARG1 AND
T_00 . ARG2 = T_01 . ARG2 AND
T_01 . FUNCTINT = 3 ) AND
T_00 . CLIENT = '000' !

SELECT
T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_01 . FUNCTINT = 3 ) AND
  T_00 . CLIENT = '000'!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_01 . FUNCTINT >= 3 ) AND
  T_00 . CLIENT = '000'!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000' AND
  T_00 . ARG2 = T_01 . ARG2!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000' AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . FUNCTINT = 3 !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000' AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_00 . FUNCTINT = 3 !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_00 . CLIENT = T_01 . CLIENT AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_00 . CLIENT = T_01 . CLIENT AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . ARG1 = ' 1' ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . ARG1 = ' 1' )!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) AND
  T_01 . FUNCTINT (+) = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) ) AND
  T_00 . CLIENT = '000' AND
  T_00 . FUNCTINT = 3 !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_00 . CLIENT = T_01 . CLIENT (+) AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_00 . CLIENT = T_01 . CLIENT (+) AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) AND
  T_01 . ARG1 (+) = ' 1' ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 ) AND
  ( T_02 . CLIENT = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 ) AND
  ( T_02 . CLIENT = '000' AND
  T_02 . FUNCTINT = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_01 . FUNCTINT ) AND
  ( T_02 . CLIENT = '000' AND
  T_02 . ARG1 = T_01 . ARG1 AND
  T_02 . ARG2 = T_01 . ARG2 AND
  T_02 . FUNCTINT = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_00 . ARG1 = T_02 . ARG1 AND
  T_00 . ARG2 = T_02 . ARG2 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_02 . FUNCTINT (+) = T_00 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) AND
  T_01 . FUNCTINT (+) = 3 ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_00 . FUNCTINT = T_02 . FUNCTINT (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . FUNCTINT = T_01 . FUNCTINT (+) ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_02 . ARG1 (+) = T_00 . ARG1 AND
  T_02 . ARG2 (+) = T_00 . ARG2 AND
  T_02 . FUNCTINT (+) = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) ) AND
  ( T_02 . CLIENT = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) AND
  T_01 . FUNCTINT (+) = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . ARG1 = T_01 . ARG1 (+) AND
  T_00 . ARG2 = T_01 . ARG2 (+) AND
  T_01 . FUNCTINT (+) = 3 ) AND
  ( T_02 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_02 . FUNCTINT ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT (+) = '000' AND
  T_00 . FUNCTINT = T_01 . FUNCTINT (+) ) AND
  ( T_02 . CLIENT = '000' AND
  T_02 . ARG1 = T_01 . ARG1 AND
  T_02 . ARG2 = T_01 . ARG2 AND
  T_02 . FUNCTINT = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_01 . FUNCTINT = T_02 . FUNCTINT (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 AND
  T_00 . ARG2 = T_01 . ARG2 AND
  T_01 . FUNCTINT = 3 ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_00 . FUNCTINT = T_02 . FUNCTINT (+) ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_01 . FUNCTINT ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_02 . ARG1 (+) = T_01 . ARG1 AND
  T_02 . ARG2 (+) = T_01 . ARG2 AND
  T_02 . FUNCTINT (+) = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD ,
 T_02 . CLIENT ,
 T_02 . ARG1 ,
 T_02 . ARG2 ,
 T_02 . FUNCTION ,
 T_02 . FUNCTINT  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01 ,
 VERI_CLNT T_02  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . FUNCTINT = T_01 . FUNCTINT AND
  T_01 . FUNCTINT = 1 ) AND
  ( T_02 . CLIENT (+) = '000' AND
  T_02 . ARG1 (+) = T_01 . ARG1 AND
  T_02 . ARG2 (+) = T_01 . ARG2 AND
  T_02 . FUNCTINT (+) = 3 ) AND
  T_00 . CLIENT = '000' !

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 MAX( T_00 . ARG2 ) ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000'  
GROUP BY  
 T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ALIAS ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000'  
ORDER BY  
 T_00 . ARG1 ,
 ALIAS ,
 T_00 . FUNCTION ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD!

SELECT
  T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ALIAS ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD  
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000'  
ORDER BY  
 T_00 . CLIENT ,
 T_00 . ARG1 ,
 ALIAS ,
 T_00 . FUNCTION ,
 T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_01 . FUNCTFLT ,
 T_01 . FUNCTPKD!

SELECT
   *   
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000'  
ORDER BY  T_00 . ARG1!

SELECT
   *   
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000' AND
  T_00 . ARG2 = T_01 . ARG2!

SELECT
   *   
FROM
  VERI_CLNT T_00 ,
 VERI_JOIN T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_00 . ARG1 = T_01 . ARG1 ) AND
  T_00 . CLIENT = '000' AND
  T_00 . ARG1 IN ( ' 1' , ' 1' , ' 1' , ' 1' , ' 1' ,
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' ,
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' ,
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' ,
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , 
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' , ' 1' ,
 ' 1' , ' 1' , ' 1' , ' 1' , ' 1' )!

SELECT
  T_01 . CLIENT ,
 T_01 . ARG1 ,
 T_01 . ARG2 ,
 T_01 . FUNCTION ,
 T_01 . FUNCTINT ,
 T_00 . CLIENT ,
 T_00 . ARG1 ,
 T_00 . ARG2 ,
 T_00 . FUNCTION ,
 T_00 . FUNCTINT ,
 T_00 . FUNCTFLT ,
 T_00 . FUNCTPKD  
FROM
  VERI_JOIN T_00 ,
 VERI_CLNT T_01  
WHERE
  ( T_01 . CLIENT = '000' AND
  T_01 . ARG1 = T_00 . ARG1 AND
  T_01 . ARG2 = T_00 . ARG2 ) AND
  T_00 . CLIENT = '000' !

commit work release !

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* VIEW.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !
create table t ( a fixed(4))!
insert t set a = 3!
create table t1 ( b fixed (5))!
insert t1 set b = 8!
insert t1 set b = 4!
create view v1 (a, b, x) as select a, b, a+b from t, t1!
lock table v1 in share mode !
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
insert t1 set b = 5!
drop view v1 !
create view v1 (a, b, x) as select a, b, a+b from t, t1 where b > 4 !
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
drop table t!
drop table t1 !
commit !
*=================================================
* View mit Set-Function
*=================================================
create table t ( a fixed(4))!
insert t set a = 3!
insert t set a = 5!
create table t1 ( b fixed (5))!
insert t1 set b = 8!
insert t1 set b = 4!
create view v2 (x) as select sum(b) from t, t1 group by a!
create view v3 (x) as select sum(b) from t, t1 having avg(b) > 3!
create view v1 (x) as select sum(b) from t, t1!
select * from v1 !
fetch!
select * from v1 where x > 10!
select * from v1 group by a!
select x from v1 group by x !
select avg(x) from v1!
select x from v1 having x > 12!
select x from v1 having x >= 12 order by x!
select x from v1 having x >= 12 order by 1!
drop view v1 !
create view v1 (x) as select sum(b) from t, t1 where a > 3!
select * from v1 !
fetch!
select * from v1 where x > 10!
drop table t!
drop table t1!
commit !
create table t ( a char (7) )!
insert t set a = 'aaa'!
insert t set a = 'aaa'!
create view v1 (sysk, a) as select distinct syskey,* from t!
create table t1 (b char (7) )!
insert t1 set b = 'aaa'!
select a,b from t1, v1 !
fetch !
commit release !
file connect ( kern test !
drop table t !
drop table t1 !
create table t ( a fixed(4))!
insert t set a = 3!
create table t1 ( b fixed (5))!
insert t1 set b = 8!
insert t1 set b = 4!
select rowno, a, b from t, t1!
create view v1 (a, b, x) as select a, b, a+b from t, t1 where a+b > 6!
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
insert t1 set b = 5!
drop view v1 !
create view v1 (a, b, x) as select a, b, a+b from t, t1 where a+b > 7 !
select * from v1 where x > 10!
fetch !
select * from v1 where a < x!
fetch !
select * from v1 where a = 3 and x > 10!
fetch!
select * from v1 where b = 4 or x > 10 !
fetch !
select * from v1 where not (x <= 10)!
fetch !
select * from v1 where a = 3 and (b = 4 or x > 10)!
fetch !
select x from v1 !
fetch !
select avg(x) from v1!
fetch !
select avg(x), b from v1 group by b !
fetch!
select avg(x), b from v1 group by b order by 1!
fetch!
select sum(x), b from v1 where x > 6 group by b having avg(a) > 0!
fetch!
select min (x) from v1 group by b having avg(x) > 8!
fetch!
select min (x) from v1 group by b having avg(a) > 2 order by 1!
fetch !
drop table t!
drop table t1 !
commit release !
file cleark !
file connect ( kern test !
create table probew (oid char(36) key,
i_rel fixed(10) key, i_concat_id char (36), land_nr fixed (10),
lfd_nr fixed (10))!
insert probew values ('aaa', 1, 'a', 116, 3)!
create table probewbewill
(oid char(36) key, i_rel fixed (10) key,
i_tab_key char(8) byte, i_concat_id char(9))!
insert probewbewill values ('aaa', 1, x'99', 'a')!
create table project(oid char(27) key,
i_rel fixed(10) key, i_concat_id char (27), land_nr fixed (10),
lfd_nr fixed (10))!
insert project values ('aaa', 1, 'a', 116, 3)!
create view grundbew as select
 probew.oid, project.i_rel, project.land_nr, project.lfd_nr
 from probew, probewbewill, project
where probew.oid = probewbewill.oid
  and probew.land_nr = project.land_nr
  and probew.lfd_nr = project.lfd_nr !
create index "project.land_nr"
	on project( land_nr ) !
drop   index "probew.land_nr" !
explain select * from grundbew where land_nr = 116 !
select * from grundbew where land_nr = 116 !
fetch !
rollback work release !

* === check updateable join views 
file connect ( kern test !

create table test1
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2))!
create index i1 on test1(k2,k1)!
create table test2
( k1 int, k2 int, c3 int, c4 int, primary key(k1,k2),
foreign key(k1, k2) references test1(k1,k2))!
create index i2 on test2(k2,k1)!


create view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.c4 = t2.c4 and t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 
with check option !

insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!

delete from test2!
delete from test1!

*pars_then_ex!
create or replace view testv( k1, k2, c3, c4, c5 ) as 
select t1.k1, t1.k2, t1.c3, t1.c4, t2.c4 
from test1 t1, test2 t2 
where 
t1.k1=t2.k1 and t1.k2=t2.k2 and t2.k1 = 1 and t1.c4 = t2.c4
with check option !

pars_then_ex!
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,2)!
insert into testv(k1,k2,c3,c4) values (1,2,1,1)!
insert into testv(k1,k2,c3,c4,c5) values (1,2,1,1,1)!
insert into testv(k1,k2,c3,c4) values (1,3,1,2)!
insert into testv(k1,k2,c3,c4,c5) values (1,3,1,2,2)!
update testv set k1=3 where k1=1!
update testv set c5=45 where k1=1!
update testv set c5=2 where k2=2!
update testv set c4=2, c5=2 where k2=2!
pars_execute!
* both tables get new roots
delete from test2!
delete from test1!
* execution of join permanent mesblocks must contain valid roots
insert into testv(k1,k2,c3,c4) values (3,2,1,1)!

pars_execute!
rollback work release!

file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
