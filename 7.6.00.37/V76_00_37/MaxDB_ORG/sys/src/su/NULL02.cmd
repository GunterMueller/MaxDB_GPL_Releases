* This is a test for the NULL-keyword in all functions!

file CONNECT ( kern test SQLMODE INTERNAL!

* arithmetic-functions

select trunc (NULL) from t!
fetch!
select trunc (1.2345) from t!
fetch!
select trunc (1.2345, NULL) from t!
fetch!
select trunc (NULL, 2) from t!
fetch!
select trunc (NULL, NULL) from t!
fetch!

select round (NULL) from t!
fetch!
select round (1.2345) from t!
fetch!
select round (1.2345, NULL) from t!
fetch!
select round (NULL, NULL) from t!
fetch!

select noround (NULL) from t!
fetch!

select fixed (NULL) from t!
fetch!
select fixed (NULL, NULL) from t!
fetch!
select fixed (1.2345, NULL) from t!
fetch!
select fixed (NULL, 3) from t!
fetch!
select fixed (NULL, NULL, NULL) from t!
fetch!
select fixed (1.2345, NULL, NULL) from t!
fetch!
select fixed (NULL, 3, NULL) from t!
fetch!
select fixed (NULL, NULL, 1) from t!
fetch!
select fixed (1.2345, 3, NULL) from t!
fetch!
select fixed (1.2345, NULL, 1) from t!
fetch!
select fixed (NULL, 3, 1) from t!
fetch!

select ceil (NULL) from t!
fetch!

select floor (NULL) from t!
fetch!

select sign (NULL) from t!
fetch!

select abs (NULL) from t!
fetch!

select power (NULL, NULL) from t!
fetch!
select power (NULL, 2) from t!
fetch!
select power (2, NULL) from t!
fetch!

select sqrt (NULL) from t!
fetch!

select length (NULL) from t!
fetch!

select index (NULL, NULL) from t!
fetch!
select index ('abcdefghij', NULL) from t!
fetch!
select index (NULL, 'abcdefghij') from t!
fetch!
select index (NULL, NULL, NULL) from t!
fetch!
select index ('abcdefghij', NULL, NULL) from t!
fetch!
select index (NULL, 'abcdefghij', NULL) from t!
fetch!
select index (NULL, NULL, 1) from t!
fetch!
select index (NULL, NULL, 2) from t!
fetch!
select index ('abcdefghij', 'bc', NULL) from t!
fetch!
select index (NULL, 'bc', 1) from t!
fetch!
select index ('abcdefghij', NULL, 1) from t!
fetch!
select index (NULL, NULL, NULL, NULL) from t!
fetch!
select index ('abcdefghij', NULL, NULL, NULL) from t!
fetch!
select index (NULL, 'bc', NULL, NULL) from t!
fetch!
select index (NULL, NULL, 1, NULL) from t!
fetch!
select index (NULL, NULL, NULL, 2) from t!
fetch!
select index ('abcdefghij', 'bc', NULL, NULL) from t!
fetch!
select index (NULL, 'bc', 1, NULL) from t!
fetch!
select index ('abcdefghij', NULL, 1, NULL) from t!
fetch!
select index ('abcdefghij', NULL, NULL, 2) from t!
fetch!
select index (NULL, 'bc', NULL, 2) from t!
fetch!
select index (NULL, NULL, 1, 2) from t!
fetch!
select index ('abcdefghij', 'bc', NULL, 2) from t!
fetch!
select index (NULL, 'bc', 1, 2) from t!
fetch!
select index ('abcdefghij', NULL, 1, 2) from t!
fetch!

select sin (NULL) from t!
fetch!

select cos (NULL) from t!
fetch!

select tan (NULL) from t!
fetch!

select sinh (NULL) from t!
fetch!

select cosh (NULL) from t!
fetch!

select tanh (NULL) from t!
fetch!

select log (NULL, NULL) from t!
fetch!
select log (3, NULL) from t!
fetch!
select log (NULL, 3) from t!
fetch!

select ln (NULL) from t!
fetch!

select exp (NULL) from t!
fetch!

* string-functions!

select NULL || NULL from t!
fetch!
select 'abc' || NULL from t!
fetch!
select NULL || 'abc' from t!
fetch!

select NULL & NULL from t!
fetch!
select 'abc' & NULL from t!
fetch!
select NULL & 'abc' from t!
fetch!

select substr (NULL, NULL) from t!
fetch!
select substr ('abcdefghij', NULL) from t!
fetch!
select substr (NULL, 1) from t!
fetch!
select substr (NULL, 10) from t!
fetch!
select substr (NULL, NULL, NULL) from t!
fetch!
select substr ('abcdefghij', NULL, NULL) from t!
fetch!
select substr (NULL, 1, NULL) from t!
fetch!
select substr ('abcdefghij', NULL, 1) from t!
fetch!
select substr (NULL, 1, 1) from t!
fetch!

select lfill (NULL, NULL) from t!
fetch!
select lfill ('abcdefghij', NULL) from t!
fetch!
select lfill (NULL, 'a') from t!
fetch!
select lfill (NULL, NULL, NULL) from t!
fetch!
select lfill ('abcdefghij', NULL, NULL) from t!
fetch!
select lfill (NULL, 'a', NULL) from t!
fetch!
select lfill ('abcdefghij', NULL, 1) from t!
fetch!
select lfill (NULL, 'a', 1) from t!
fetch!

select rfill (NULL, NULL) from t!
fetch!
select rfill ('abcdefghij', NULL) from t!
fetch!
select rfill (NULL, 'a') from t!
fetch!
select rfill (NULL, NULL, NULL) from t!
fetch!
select rfill ('abcdefghij', NULL, NULL) from t!
fetch!
select rfill (NULL, 'a', NULL) from t!
fetch!
select rfill ('abcdefghij', NULL, 1) from t!
fetch!
select rfill (NULL, 'a', 1) from t!
fetch!

select lpad (NULL, NULL, NULL) from t!
fetch!
select lpad ('abcdefghij', NULL, NULL) from t!
fetch!
select lpad (NULL, 1, NULL) from t!
fetch!
select lpad (NULL, NULL, 'v') from t!
fetch!
select lpad (NULL, 1, 'v') from t!
fetch!
select lpad ('abcdefghij', NULL, 'v') from t!
fetch!
select lpad ('abcdefghij', 1, NULL) from t!
fetch!
select lpad (NULL, NULL, NULL, 10) from t!
fetch!
select lpad ('abcdefghij', NULL, NULL, 10) from t!
fetch!
select lpad (NULL, 1, NULL, 10) from t!
fetch!
select lpad (NULL, NULL, 'v', 10) from t!
fetch!
select lpad (NULL, 1, 'v', 10) from t!
fetch!
select lpad ('abcdefghij', NULL, 'v', 10) from t!
fetch!
select lpad ('abcdefghij', 1, NULL, 10) from t!
fetch!
select lpad (NULL, NULL, NULL, NULL) from t!
fetch!
select lpad ('abcdefghij', NULL, NULL, NULL) from t!
fetch!
select lpad (NULL, 1, NULL, NULL) from t!
fetch!
select lpad (NULL, NULL, 'v', NULL) from t!
fetch!
select lpad (NULL, 1, 'v', NULL) from t!
fetch!
select lpad ('abcdefghij', NULL, 'v', NULL) from t!
fetch!
select lpad ('abcdefghij', 1, NULL, NULL) from t!
fetch!

select rpad (NULL, NULL, NULL) from t!
fetch!
select rpad ('abcdefghij', NULL, NULL) from t!
fetch!
select rpad (NULL, 1, NULL) from t!
fetch!
select rpad (NULL, NULL, 'v') from t!
fetch!
select rpad (NULL, 1, 'v') from t!
fetch!
select rpad ('abcdefghij', NULL, 'v') from t!
fetch!
select rpad ('abcdefghij', 1, NULL) from t!
fetch!
select rpad (NULL, NULL, NULL, 10) from t!
fetch!
select rpad ('abcdefghij', NULL, NULL, 10) from t!
fetch!
select rpad (NULL, 1, NULL, 10) from t!
fetch!
select rpad (NULL, NULL, 'v', 10) from t!
fetch!
select rpad (NULL, 1, 'v', 10) from t!
fetch!
select rpad ('abcdefghij', NULL, 'v', 10) from t!
fetch!
select rpad ('abcdefghij', 1, NULL, 10) from t!
fetch!
select rpad (NULL, NULL, NULL, NULL) from t!
fetch!
select rpad ('abcdefghij', NULL, NULL, NULL) from t!
fetch!
select rpad (NULL, 1, NULL, NULL) from t!
fetch!
select rpad (NULL, NULL, 'v', NULL) from t!
fetch!
select rpad (NULL, 1, 'v', NULL) from t!
fetch!
select rpad ('abcdefghij', NULL, 'v', NULL) from t!
fetch!
select rpad ('abcdefghij', 1, NULL, NULL) from t!
fetch!

select trim (NULL) from t!
fetch!
select ltrim (NULL) from t!
fetch!
select rtrim (NULL) from t!
fetch!
select trim (NULL, NULL) from t!
fetch!
select ltrim (NULL, NULL) from t!
fetch!
select rtrim (NULL, NULL) from t!
fetch!
select trim ('abcdefghi', NULL) from t!
fetch!
select ltrim ('abcdefghi', NULL) from t!
fetch!
select rtrim ('abcdefghi', NULL) from t!
fetch!
select trim (NULL, 'ai') from t!
fetch!
select ltrim (NULL, 'ai') from t!
fetch!
select rtrim (NULL, 'ai') from t!
fetch!

select expand (NULL, NULL) from t!
fetch!
select expand (NULL, 10) from t!
fetch!
select expand ('abcdefghij', NULL) from t!
fetch!

select upper (NULL) from t!
fetch!

select lower (NULL) from t!
fetch!

select initcap (NULL) from t!
fetch!

select replace ('abcdefg', 'bc', 'xxxxx') from t!
fetch!
select replace ('abcdefg', 'yy', 'xxxxx') from t!
fetch!
select replace (NULL, NULL, NULL) from t!
fetch!
select replace ('abcdef', NULL, NULL) from t!
fetch!
select replace (NULL, 'yy', NULL) from t!
fetch!
select replace (NULL, 'bc', NULL) from t!
fetch!
select replace (NULL, NULL, 'xxxxx') from t!
fetch!
select replace ('abcdef', NULL, 'xxxxx') from t!
fetch!
select replace ('abcdef', 'yy', NULL) from t!
fetch!
select replace ('abcdef', 'bc', NULL) from t!
fetch!
select replace ('abcdef', 'bc') from t!
fetch!
select replace (NULL, 'yy', 'xxxxx') from t!
fetch!
select replace (NULL, 'bc', 'xxxxx') from t!
fetch!

select translate ('abcdefg', 'bc', 'xy') from t!
fetch!
select translate ('abcdefg', 'yy', 'xx') from t!
fetch!
select translate (NULL, NULL, NULL) from t!
fetch!
select translate ('abcdef', NULL, NULL) from t!
fetch!
select translate (NULL, 'bc', NULL) from t!
fetch!
select translate (NULL, NULL, 'xy') from t!
fetch!
select translate ('abcdef', NULL, 'xy') from t!
fetch!
select translate ('abcdef', 'bc', NULL) from t!
fetch!
select translate (NULL, 'bc', 'xy') from t!
fetch!

select mapchar (NULL) from t!
fetch!
select mapchar (NULL, NULL) from t!
fetch!
select mapchar (NULL, 1) from t!
fetch!
select mapchar (NULL, 10) from t!
fetch!
select mapchar ('abcdefghij', NULL) from t!
fetch!
select mapchar (NULL, NULL, NULL) from t!
fetch!
select mapchar ('abcdefghij', NULL, NULL) from t!
fetch!
select mapchar ('abcdefghij', 1, NULL) from t!
fetch!
select mapchar (NULL, 1, NULL) from t!
fetch!

select alpha (NULL) from t!
fetch!
select alpha (NULL, NULL) from t!
fetch!
select alpha (NULL, 1) from t!
fetch!
select alpha (NULL, 10) from t!
fetch!
select alpha ('abcdefg', NULL) from t!
fetch!

select ascii (NULL) from t!
fetch!

select soundex (NULL) from t!
fetch!

* datefunctions!

select adddate (NULL, NULL) from t!
fetch!
select adddate (d, NULL) from t!
fetch!
select adddate (NULL, 12) from t!
fetch!

select subdate (NULL, NULL) from t!
fetch!
select subdate (d, NULL) from t!
fetch!
select subdate (NULL, 12) from t!
fetch!

select datediff (NULL, NULL) from t!
fetch!
select datediff (d, NULL) from t!
fetch!
select datediff (NULL, d) from t!
fetch!

select dayofweek (NULL) from t!
fetch!

select weekofyear (NULL) from t!
fetch!

select dayofyear (NULL) from t!
fetch!

select makedate (NULL, NULL) from t!
fetch!
select makedate (NULL, 223) from t!
fetch!
select makedate (1994, NULL) from t!
fetch!

* timefunctions!

select addtime (NULL, NULL) from t!
fetch!
select addtime (t, NULL) from t!
fetch!
select addtime (NULL, t) from t!
fetch!

select subtime (NULL, NULL) from t!
fetch!
select subtime (t, NULL) from t!
fetch!
select subtime (NULL, t) from t!
fetch!

select timediff (NULL, NULL) from t!
fetch!
select timediff (t, NULL) from t!
fetch!
select timediff (NULL, t) from t!
fetch!

select maketime (NULL, NULL, NULL) from t!
fetch!
select maketime (NULL, NULL, 50) from t!
fetch!
select maketime (NULL, 35, NULL) from t!
fetch!
select maketime (12, NULL, NULL) from t!
fetch!
select maketime (12, 35, NULL) from t!
fetch!
select maketime (12, NULL, 50) from t!
fetch!
select maketime (NULL, 35, 50) from t!
fetch!

* extraction-functions!

select year (NULL) from t!
fetch!

select month (NULL) from t!
fetch!

select day (NULL) from t!
fetch!

select hour (NULL) from t!
fetch!

select minute (NULL) from t!
fetch!

select second (NULL) from t!
fetch!

* special-functions !

select value (NULL, NULL) from t!
fetch!
select value (NULL, fi) from t!
fetch!
select value (fi, NULL) from t!
fetch!
select value (NULL, NULL, NULL) from t!
fetch!
select value (fi, NULL, NULL) from t!
fetch!
select value (NULL, fi, NULL) from t!
fetch!
select value (NULL, NULL, fi) from t!
fetch!
select value (NULL, 1, 2) from t!
fetch!
select value (1, NULL, 2) from t!
fetch!
select value (1, 2, NULL) from t!
fetch!

select greatest (NULL, NULL) from t!
fetch!
select greatest (fi, NULL) from t!
fetch!
select greatest (NULL, fi) from t!
fetch!
select greatest (NULL, NULL, fi) from t!
fetch!
select greatest (1, 2, 3, 4, NULL, NULL, fi) from t!
fetch!

select least (NULL, NULL) from t!
fetch!
select least (fi, NULL) from t!
fetch!
select least (NULL, fi) from t!
fetch!
select least (NULL, NULL, fi) from t!
fetch!
select least (1, 2, 3, 4, NULL, NULL, fi) from t!
fetch!

select DECODE (NULL, NULL, NULL) from t!
fetch!
select DECODE (1, NULL, NULL) from t!
fetch!
select DECODE (NULL, 2, NULL) from t!
fetch!
select DECODE (NULL, NULL, 3) from t!
fetch!
select DECODE (1, 2, NULL) from t!
fetch!
select DECODE (1, NULL, 3) from t!
fetch!
select DECODE (NULL, 2, 3) from t!
fetch!
select DECODE (NULL, NULL, NULL, NULL) from t!
fetch!
select DECODE (1, NULL, NULL, NULL) from t!
fetch!
select DECODE (NULL, 2, NULL, NULL) from t!
fetch!
select DECODE (NULL, NULL, 3, NULL) from t!
fetch!
select DECODE (NULL, NULL, NULL, 4) from t!
fetch!
select DECODE (1, 2, NULL, NULL) from t!
fetch!
select DECODE (1, NULL, 3, NULL) from t!
fetch!
select DECODE (1, NULL, NULL, 4) from t!
fetch!
select DECODE (NULL, 2, 3, NULL) from t!
fetch!
select DECODE (NULL, 2, NULL, 4) from t!
fetch!
select DECODE (NULL, NULL, 3, 4) from t!
fetch!
select DECODE (NULL, 2, 3, 4) from t!
fetch!
select DECODE (1, NULL, 3, 4) from t!
fetch!
select DECODE (1, 2, NULL, 4) from t!
fetch!
select DECODE (1, 2, 3, NULL) from t!
fetch!

* conversion-functions!

select num (NULL) from t!
fetch!

select chr (NULL) from t!
fetch!
select chr (NULL, NULL) from t!
fetch!
select chr (fi, NULL) from t!
fetch!
select chr (NULL, fi) from t!
fetch!

select hex (NULL) from t!
fetch!

select char (NULL) from t!
fetch!
select char (NULL, NULL) from t!
fetch!
select char (NULL, 'YYYY-MM-DD') from t!
fetch!
select char (cad, NULL) from t!
fetch!

* set-functions!

select count (NULL) from t!
fetch!

select max (NULL) from t!
fetch!

select min (NULL) from t!
fetch!

select sum (NULL) from t!
fetch!

select avg (NULL) from t!
fetch!

select stddev (NULL) from t!
fetch!

select variance (NULL) from t!
fetch!

commit work release!
