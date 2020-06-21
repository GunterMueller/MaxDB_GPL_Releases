* This is a test for the NULL-keyword in all DB2-functions in SQL-DB!

file CONNECT ( kern test SQLMODE DB2!

* arithmetic-functions

select  integer (NULL) into :a from t where k1 = 1!
select  integer (1.2345) into :a from t where k1 = 1!

select  decimal (NULL) into :a from t where k1 = 1!
select  decimal (NULL, NULL) into :a from t where k1 = 1!
select  decimal (1.2345, NULL) into :a from t where k1 = 1!
select  decimal (NULL, 3) into :a from t where k1 = 1!
select  decimal (NULL, NULL, NULL) into :a from t where k1 = 1!
select  decimal (1.2345, NULL, NULL) into :a from t where k1 = 1!
select  decimal (NULL, 3, NULL) into :a from t where k1 = 1!
select  decimal (NULL, NULL, 1) into :a from t where k1 = 1!
select  decimal (1.2345, 3, NULL) into :a from t where k1 = 1!
select  decimal (1.2345, NULL, 1) into :a from t where k1 = 1!
select  decimal (NULL, 3, 1) into :a from t where k1 = 1!

select  float (NULL) into :a from t where k1 = 1!
select  float (12.345) into :a from t where k1 = 1!

select  length (NULL) into :a from t where k1 = 1!


* string-functions!

select  NULL || NULL into :a from t where k1 = 1!
select  'abc' || NULL into :a from t where k1 = 1!
select  NULL || 'abc' into :a from t where k1 = 1!

select  NULL & NULL into :a from t where k1 = 1!
select  'abc' & NULL into :a from t where k1 = 1!
select  NULL & 'abc' into :a from t where k1 = 1!

select  substr (NULL, NULL) into :a from t where k1 = 1!
select  substr ('abcdefghij', NULL) into :a from t where k1 = 1!
select  substr (NULL, 1) into :a from t where k1 = 1!
select  substr (NULL, 10) into :a from t where k1 = 1!
select  substr (NULL, NULL, NULL) into :a from t where k1 = 1!
select  substr ('abcdefghij', NULL, NULL) into :a from t where k1 = 1!
select  substr (NULL, 1, NULL) into :a from t where k1 = 1!
select  substr ('abcdefghij', NULL, 1) into :a from t where k1 = 1!
select  substr (NULL, 1, 1) into :a from t where k1 = 1!

* extraction-functions!

select  year (NULL) into :a from t where k1 = 1!

select  month (NULL) into :a from t where k1 = 1!

select  day (NULL) into :a from t where k1 = 1!

select  hour (NULL) into :a from t where k1 = 1!

select  minute (NULL) into :a from t where k1 = 1!

select  second (NULL) into :a from t where k1 = 1!

select  timestamp (NULL)  into :a from t where k1 = 1!

select  timestamp (NULL, NULL) into :a from t where k1 = 1!

select  timestamp (NULL, '090000') into :a from t where k1 = 1!

select  timestamp ('19940801', NULL) into :a from t where k1 = 1!

select  microsecond (NULL) into :a from t where k1 = 1!

select  date (NULL) into :a from t where k1 = 1!

select  time (NULL) into :a from t where k1 = 1!

select  days (NULL) into :a from t where k1 = 1!


* special-functions !

select  value (NULL, NULL) into :a from t where k1 = 1!
select  value (NULL, fi) into :a from t where k1 = 1!
select  value (fi, NULL) into :a from t where k1 = 1!
select  value (NULL, NULL, NULL) into :a from t where k1 = 1!
select  value (fi, NULL, NULL) into :a from t where k1 = 1!
select  value (NULL, fi, NULL) into :a from t where k1 = 1!
select  value (NULL, NULL, fi) into :a from t where k1 = 1!
select  value (NULL, 1, 2) into :a from t where k1 = 1!
select  value (1, NULL, 2) into :a from t where k1 = 1!
select  value (1, 2, NULL) into :a from t where k1 = 1!

* conversion-functions!

select  digits (fi) into :a from t where k1 = 1!
select  digits (NULL) into :a from t where k1 = 1!

select  char (NULL) into :a from t where k1 = 1!
select  char (NULL, NULL) into :a from t where k1 = 1!
select  char (NULL, 'YYYY-MM-DD') into :a from t where k1 = 1!
select  char (cad, NULL) into :a from t where k1 = 1!


* set-functions!

select  count (NULL) into :a from t where k1 = 1!

select  max (NULL) into :a from t where k1 = 1!

select  min (NULL) into :a from t where k1 = 1!

select  sum (NULL) into :a from t where k1 = 1!

select  avg (NULL) into :a from t where k1 = 1!

commit work release!
