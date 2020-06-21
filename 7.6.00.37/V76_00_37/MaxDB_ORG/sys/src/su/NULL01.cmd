* This is a test for the NULL-keyword in all ORACLE-functions in SQL-DB!

file CONNECT ( kern test SQLMODE ORACLE!

* arithmetic-functions

select trunc (NULL) from t!
fetch into :a!
select trunc (1.2345) from t!
fetch into :a!
select trunc (1.2345, NULL) from t!
fetch into :a!
select trunc (NULL, 2) from t!
fetch into :a!
select trunc (NULL, NULL) from t!
fetch into :a!

select round (NULL) from t!
fetch into :a!
select round (1.2345) from t!
fetch into :a!
select round (1.2345, NULL) from t!
fetch into :a!
select round (NULL, NULL) from t!
fetch into :a!

select ceil (NULL) from t!
fetch into :a!

select floor (NULL) from t!
fetch into :a!

select sign (NULL) from t!
fetch into :a!

select abs (NULL) from t!
fetch into :a!

select power (NULL, NULL) from t!
fetch into :a!
select power (NULL, 2) from t!
fetch into :a!
select power (2, NULL) from t!
fetch into :a!

select sqrt (NULL) from t!
fetch into :a!

select mod (NULL, NULL) from t!
fetch into :a!
select mod (NULL, 2) from t!
fetch into :a!
select mod (2, NULL) from t!
fetch into :a!

select length (NULL) from t!
fetch into :a!

select vsize (NULL) from t!
fetch into :a!

select instr (NULL, NULL) from t!
fetch into :a!
select instr ('abcdefghij', NULL) from t!
fetch into :a!
select instr (NULL, 'abcdefghij') from t!
fetch into :a!
select instr (NULL, NULL, NULL) from t!
fetch into :a!
select instr ('abcdefghij', NULL, NULL) from t!
fetch into :a!
select instr (NULL, 'abcdefghij', NULL) from t!
fetch into :a!
select instr (NULL, NULL, 1) from t!
fetch into :a!
select instr (NULL, NULL, 2) from t!
fetch into :a!
select instr ('abcdefghij', 'bc', NULL) from t!
fetch into :a!
select instr (NULL, 'bc', 1) from t!
fetch into :a!
select instr ('abcdefghij', NULL, 1) from t!
fetch into :a!
select instr (NULL, NULL, NULL, NULL) from t!
fetch into :a!
select instr ('abcdefghij', NULL, NULL, NULL) from t!
fetch into :a!
select instr (NULL, 'bc', NULL, NULL) from t!
fetch into :a!
select instr (NULL, NULL, 1, NULL) from t!
fetch into :a!
select instr (NULL, NULL, NULL, 2) from t!
fetch into :a!
select instr ('abcdefghij', 'bc', NULL, NULL) from t!
fetch into :a!
select instr (NULL, 'bc', 1, NULL) from t!
fetch into :a!
select instr ('abcdefghij', NULL, 1, NULL) from t!
fetch into :a!
select instr ('abcdefghij', NULL, NULL, 2) from t!
fetch into :a!
select instr (NULL, 'bc', NULL, 2) from t!
fetch into :a!
select instr (NULL, NULL, 1, 2) from t!
fetch into :a!
select instr ('abcdefghij', 'bc', NULL, 2) from t!
fetch into :a!
select instr (NULL, 'bc', 1, 2) from t!
fetch into :a!
select instr ('abcdefghij', NULL, 1, 2) from t!
fetch into :a!

select sin (NULL) from t!
fetch into :a!

select cos (NULL) from t!
fetch into :a!

select tan (NULL) from t!
fetch into :a!

select sinh (NULL) from t!
fetch into :a!

select cosh (NULL) from t!
fetch into :a!

select tanh (NULL) from t!
fetch into :a!

select log (NULL, NULL) from t!
fetch into :a!
select log (3, NULL) from t!
fetch into :a!
select log (NULL, 3) from t!
fetch into :a!

select ln (NULL) from t!
fetch into :a!

select exp (NULL) from t!
fetch into :a!

* string-functions!

select NULL || NULL from t!
fetch into :a!
select 'abc' || NULL from t!
fetch into :a!
select NULL || 'abc' from t!
fetch into :a!

select substr (NULL, NULL) from t!
fetch into :a!
select substr ('abcdefghij', NULL) from t!
fetch into :a!
select substr (NULL, 1) from t!
fetch into :a!
select substr (NULL, 10) from t!
fetch into :a!
select substr (NULL, NULL, NULL) from t!
fetch into :a!
select substr ('abcdefghij', NULL, NULL) from t!
fetch into :a!
select substr (NULL, 1, NULL) from t!
fetch into :a!
select substr ('abcdefghij', NULL, 1) from t!
fetch into :a!
select substr (NULL, 1, 1) from t!
fetch into :a!

select lpad (NULL, NULL) from t!
fetch into :a!
select lpad ('abcdefghij', NULL) from t!
fetch into :a!
select lpad (NULL, 1) from t!
fetch into :a!
select lpad (NULL, NULL, NULL) from t!
fetch into :a!
select lpad ('abcdefghij', NULL, NULL) from t!
fetch into :a!
select lpad (NULL, 1, NULL) from t!
fetch into :a!
select lpad ('abcdefghij', NULL, 'v') from t!
fetch into :a!
select lpad (NULL, 1, 'v') from t!
fetch into :a!

select rpad (NULL, NULL) from t!
fetch into :a!
select rpad ('abcdefghij', NULL) from t!
fetch into :a!
select rpad (NULL, 1) from t!
fetch into :a!
select rpad (NULL, NULL, NULL) from t!
fetch into :a!
select rpad ('abcdefghij', NULL, NULL) from t!
fetch into :a!
select rpad (NULL, 1, NULL) from t!
fetch into :a!
select rpad ('abcdefghij', NULL, 'v') from t!
fetch into :a!
select rpad (NULL, 1, 'v') from t!
fetch into :a!

select ltrim (NULL) from t!
fetch into :a!
select rtrim (NULL) from t!
fetch into :a!
select ltrim (NULL, NULL) from t!
fetch into :a!
select rtrim (NULL, NULL) from t!
fetch into :a!
select ltrim ('abcdefghi', NULL) from t!
fetch into :a!
select rtrim ('abcdefghi', NULL) from t!
fetch into :a!
select ltrim (NULL, 'ai') from t!
fetch into :a!
select rtrim (NULL, 'ai') from t!
fetch into :a!

select upper (NULL) from t!
fetch into :a!

select lower (NULL) from t!
fetch into :a!

select initcap (NULL) from t!
fetch into :a!

select replace ('abcdefg', 'bc', 'xxxxx') from t!
fetch into :a!
select replace ('abcdefg', 'yy', 'xxxxx') from t!
fetch into :a!
select replace (NULL, NULL, NULL) from t!
fetch into :a!
select replace ('abcdef', NULL, NULL) from t!
fetch into :a!
select replace (NULL, 'yy', NULL) from t!
fetch into :a!
select replace (NULL, 'bc', NULL) from t!
fetch into :a!
select replace (NULL, NULL, 'xxxxx') from t!
fetch into :a!
select replace ('abcdef', NULL, 'xxxxx') from t!
fetch into :a!
select replace ('abcdef', 'yy', NULL) from t!
fetch into :a!
select replace ('abcdef', 'bc', NULL) from t!
fetch into :a!
select replace ('abcdef', 'bc') from t!
fetch into :a!
select replace (NULL, 'yy', 'xxxxx') from t!
fetch into :a!
select replace (NULL, 'bc', 'xxxxx') from t!
fetch into :a!

select translate ('abcdefg', 'bc', 'xy') from t!
fetch into :a!
select translate ('abcdefg', 'yy', 'xx') from t!
fetch into :a!
select translate (NULL, NULL, NULL) from t!
fetch into :a!
select translate ('abcdef', NULL, NULL) from t!
fetch into :a!
select translate (NULL, 'bc', NULL) from t!
fetch into :a!
select translate (NULL, NULL, 'xy') from t!
fetch into :a!
select translate ('abcdef', NULL, 'xy') from t!
fetch into :a!
select translate ('abcdef', 'bc', NULL) from t!
fetch into :a!
select translate (NULL, 'bc', 'xy') from t!
fetch into :a!

select soundex (NULL) from t!
fetch into :a!

* date- and time-functions!

select add_months (NULL, NULL) from t!
fetch into :a!
select add_months ('11-AUG-66', NULL) from t!
fetch into :a!
select add_months (NULL, 12) from t!
fetch into :a!

select last_day (NULL) from t!
fetch into :a!

select next_day (NULL, NULL) from t!
fetch into :a!
select next_day (NULL, 'friday') from t!
fetch into :a!
select next_day ('11-AUG-94', NULL) from t!
fetch into :a!

select months_between (NULL, NULL) from t!
fetch into :a!
select months_between (sysdate, NULL) from t!
fetch into :a!
select months_between (NULL, sysdate) from t!
fetch into :a!

* special-functions !

select nvl (NULL, NULL) from t!
fetch into :a!
select nvl (NULL, fi) from t!
fetch into :a!
select nvl (fi, NULL) from t!
fetch into :a!

select greatest (NULL, NULL) from t!
fetch into :a!
select greatest (fi, NULL) from t!
fetch into :a!
select greatest (NULL, fi) from t!
fetch into :a!
select greatest (NULL, NULL, fi) from t!
fetch into :a!
select greatest (1, 2, 3, 4, NULL, NULL, fi) from t!
fetch into :a!

select least (NULL, NULL) from t!
fetch into :a!
select least (fi, NULL) from t!
fetch into :a!
select least (NULL, fi) from t!
fetch into :a!
select least (NULL, NULL, fi) from t!
fetch into :a!
select least (1, 2, 3, 4, NULL, NULL, fi) from t!
fetch into :a!

select DECODE (NULL, NULL, NULL) from t!
fetch into :a!
select DECODE (1, NULL, NULL) from t!
fetch into :a!
select DECODE (NULL, 2, NULL) from t!
fetch into :a!
select DECODE (NULL, NULL, 3) from t!
fetch into :a!
select DECODE (1, 2, NULL) from t!
fetch into :a!
select DECODE (1, NULL, 3) from t!
fetch into :a!
select DECODE (NULL, 2, 3) from t!
fetch into :a!
select DECODE (NULL, NULL, NULL, NULL) from t!
fetch into :a!
select DECODE (1, NULL, NULL, NULL) from t!
fetch into :a!
select DECODE (NULL, 2, NULL, NULL) from t!
fetch into :a!
select DECODE (NULL, NULL, 3, NULL) from t!
fetch into :a!
select DECODE (NULL, NULL, NULL, 4) from t!
fetch into :a!
select DECODE (1, 2, NULL, NULL) from t!
fetch into :a!
select DECODE (1, NULL, 3, NULL) from t!
fetch into :a!
select DECODE (1, NULL, NULL, 4) from t!
fetch into :a!
select DECODE (NULL, 2, 3, NULL) from t!
fetch into :a!
select DECODE (NULL, 2, NULL, 4) from t!
fetch into :a!
select DECODE (NULL, NULL, 3, 4) from t!
fetch into :a!
select DECODE (NULL, 2, 3, 4) from t!
fetch into :a!
select DECODE (1, NULL, 3, 4) from t!
fetch into :a!
select DECODE (1, 2, NULL, 4) from t!
fetch into :a!
select DECODE (1, 2, 3, NULL) from t!
fetch into :a!

* conversion-functions!

select to_number (NULL) from t!
fetch into :a!

select chr (NULL) from t!
fetch into :a!

select rawtohex (NULL) from t!
fetch into :a!

select hextoraw (NULL) from t!
fetch into :a!

select to_char (NULL, NULL) from t!
fetch into :a!
select to_char (NULL, 'YYYYMMDDHHMISS') from t!
fetch into :a!
select to_char (cad, NULL) from t!
fetch into :a!

* set-functions!

select count (NULL) from t!
fetch into :a!

select max (NULL) from t!
fetch into :a!

select min (NULL) from t!
fetch into :a!

select sum (NULL) from t!
fetch into :a!

select avg (NULL) from t!
fetch into :a!

select stddev (NULL) from t!
fetch into :a!

select variance (NULL) from t!
fetch into :a!

commit work release!
