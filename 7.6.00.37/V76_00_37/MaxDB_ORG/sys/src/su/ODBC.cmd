*
* spezielle ODBC Syntax
*
*
* ok
*
select { t t }, { d d }, {ts ts } from  odbc !
fetch into :a, :b, :c!
select { t t }, { d d }, {ts ts } into :t, :d, :ts from  odbc 
    where c = 'a' !
select --(* VENDOR ( microsoft), PRODUCT (ODBC) t t *)--, 
       --(* VENDOR ( microsoft), PRODUCT (ODBC) d d *)--,
       --(* VENDOR ( microsoft), PRODUCT (ODBC)ts ts *)-- 
   into :t, :d, :ts from  odbc where c = 'b' !
select { fn ADDTIME( { t t }, { t '10:00:00'  }) } into :f from  odbc 
    where c = 'b' !
select c into :c from odbc where c like 'a%' !
select c into :c from odbc where c like 'a/%' escape '/' !
select c into :c from odbc where c like 'a/%' { escape '/' }!
*
* error
*
select { t t *)--, { d d }, {ts ts } from  odbc !
fetch into :a, :b, :c!
select --(* VENDOR ( microsoft), PRODUCT (ODBC) t t }, { d d }, {ts ts } into :t, :d, :ts from  odbc 
    where c = 'a' !
select --(* VENDOR ( microsoft), PRODUCT  t t *)--, 
       --(* VENDOR ( microsoft), PRODUCT (ODBC) d d *)--,
       --(* VENDOR ( microsoft), PRODUCT (ODBC)ts ts *)-- 
   into :t, :d, :ts from  odbc where c = 'b' !
select { call ADDTIME( { t t }, { t '00010000'  }) } into :f from  odbc 
    where c = 'b' !
*
* outer join syntax
*
create table j1 (j1 int, c1 char)!
create table j2 (j2 int, c2 char)!
insert into j1 values (1, 'a')!
insert into j1 values (2, 'a')!
insert into j1 values (3, 'a')!
insert into j2 values (3, 'b')!
insert into j2 values (4, 'b')!
insert into j2 values (5, 'b')!
select * from j1, j2 where j1    = j2 !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2 !
fetch into :a, :b, :c, :d !
select * from { oj j1 left  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1    = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 right outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
select * from j1, j2 where j1(+) = j2(+) !
fetch into :a, :b, :c, :d !
select * from { oj j1 full  outer join j2 on j1.j1 = j2.j2 } !
fetch into :a, :b, :c, :d !
drop table j1 !
drop table j2 !
*
* ODBC Funktionen
*
 * Numeric Functions !
select { fn abs (f) }                into :a from odbc where c = 'a' !
select { fn acos ( 3.1417 ) }        into :a from odbc where c = 'a' !
select { fn asin ( 3.1417 ) }        into :a from odbc where c = 'a' !
select { fn atan ( 3.1417 ) }        into :a from odbc where c = 'a' !
select { fn atan2 (3.1417, 3.1417/2)}into :a from odbc where c = 'a' !
select { fn ceiling (f) }            into :a from odbc where c = 'a' !
select { fn cos ( 3.1417 ) }         into :a from odbc where c = 'a' !
select { fn cot ( 3.1417 ) }         into :a from odbc where c = 'a' !
select { fn degrees ( 3.1417 ) }     into :a from odbc where c = 'a' !
select { fn exp ( i ) }              into :a from odbc where c = 'a' !
select { fn floor (f) }              into :a from odbc where c = 'a' !
select { fn log ( { fn exp(i) } ) }  into :a from odbc where c = 'a' !
select { fn log10 ( 100) }           into :a from odbc where c = 'a' !
select { fn mod ( f, i ) }           into :a from odbc where c = 'a' !
select { fn pi ( ) }                 into :a from odbc where c = 'a' !
select { fn power ( i, si ) }        into :a from odbc where c = 'a' !
select { fn radians ( 180 ) }        into :a from odbc where c = 'a' !
select { fn rand ( 180 ) }           into :a from odbc where c = 'a' !
select { fn round ( f ) }            into :a from odbc where c = 'a' !
select { fn sign  ( f ) }            into :a from odbc where c = 'a' !
select { fn sin ( 3.1417 ) }         into :a from odbc where c = 'a' !
select { fn sqrt (i) }               into :a from odbc where c = 'a' !
select { fn tan ( 3.1417 ) }         into :a from odbc where c = 'a' !
select { fn truncate ( f ) }         into :a from odbc where c = 'a' !

 * String Functions !
select { fn ascii(c) }               into :a from odbc where c = 'a' !
select { fn char(33) }               into :a from odbc where c = 'a' !
select { fn concat(c,c) }            into :a from odbc where c = 'a' !
select { fn difference('abch','ebcg') } 
                                     into :a from odbc where c = 'a' !
select { fn insert('Hallo',2,3,'xyzwer') } 
                                     into :a from odbc where c = 'a' !

select { fn lcase (c) }              into :a from odbc where c = 'a' !
select { fn left ('hallo', i) }      into :a from odbc where c = 'a' !
select { fn length ('hallo') }       into :a from odbc where c = 'a' !
select { fn locate('ll','Hallo',1) } into :a from odbc where c = 'a' !
select { fn ltrim  ('  hallo') }     into :a from odbc where c = 'a' !
select { fn repeat(c,2) }            into :a from odbc where c = 'a' !
select { fn replace ('hallo','l','f') }
                                     into :a from odbc where c = 'a' !
select { fn right ('hallo', i) }     into :a from odbc where c = 'a' !
select { fn rtrim  ('hallo  ') }     into :a from odbc where c = 'a' !
select { fn soundex ('hallo') }      into :a from odbc where c = 'a' !
select { fn space (7) }              into :a from odbc where c = 'a' !
select { fn substring  ('hallo',i, si)} 
                                     into :a from odbc where c = 'a' !
select { fn ucase (c) }              into :a from odbc where c = 'a' !
 * Time and Date Functions !
select { fn curdate() }              into :a from odbc where c = 'a' !
select { fn curtime() }              into :a from odbc where c = 'a' !
select { fn dayname( { d d }) }      into :a from odbc where c = 'a' !
select { fn dayofmonth( { d d }) }   into :a from odbc where c = 'a' !
select { fn dayofweek( { d d }) }    into :a from odbc where c = 'a' !
select { fn dayofyear( { d d }) }    into :a from odbc where c = 'a' !
select { fn hour( { t t }) }         into :a from odbc where c = 'a' !
select { fn minute( { t t }) }       into :a from odbc where c = 'a' !
select { fn month( { d d }) }        into :a from odbc where c = 'a' !
select { fn monthname( { d d }) }    into :a from odbc where c = 'a' !
select { fn now() }                  into :a from odbc where c = 'a' !
select { fn quarter( { d d }) }      into :a from odbc where c = 'a' !
select { fn second( { t t }) }       into :a from odbc where c = 'a' !
select { fn timestampadd(sql_tsi_hour, 3, 
     '1995-06-24 10:00:00.123456')}  into :a from odbc where c = 'a' !
select { fn timestampdiff(SQL_TSI_WEEK,
     '1995-12-24 10:12:35.123456',
     '1995-06-24 10:00:00.123456')}  into :a from odbc where c = 'a' !
select { fn week('1995-12-24')     } into :a from odbc where c = 'a' !
select { fn year( { d d } )     }    into :a from odbc where c = 'a' !
 * System Functions !
select { fn database() }             into :a from odbc where c = 'a' !
select { fn ifnull(null, i, si) }    into :a from odbc where c = 'a' !
select { fn user() }                 into :a from odbc where c = 'a' !
 * Explicit Data Type Conversion where c = 'a' !
select { fn convert(i,SQL_CHAR)    } into :a from odbc where c = 'a' !
*
* TIMESTAMP 
*
insert into odbc (ts) values ('1900-01-01 12:00:00') !
insert into odbc (ts) values ('1900-01-01 12:00:00.020202')!
pars_then_ex!
data !
insert into odbc (ts) values (:a) !
c 26 '1902-07-07 20:20:00:000000'
 
insert into odbc (ts) values (:a)!
c 26 '1900-01-01-10:10:10:000000'

nodata !
select * from odbc !
fetch into :a, :b, :c, :d, :e, :f, :g!
pars_execute !
parsing !
select * from odbc for update of for reuse !
executing !
select !
1, XYZ
pars_execute!
* just to check the existence !
select * from xyz !
pars_then_ex!
declare erg cursor for
select * from odbc for update of wrong.ts !
declare erg cursor for
select * from odbc for update of odbc.ts !
fetch erg into :a, :b, :c, :d, :e, :f, :g!
pars_execute!
