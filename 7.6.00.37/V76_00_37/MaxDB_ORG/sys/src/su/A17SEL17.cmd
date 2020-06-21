*   *ID* A17SEL17 DBCMD    changed on 1992-09-28-13.22.43 by ADMIN     *
file connect ( kern test !
create table aa (
aadstnr    char (5 ) key,
aadat      char (6 ) key,
aalfdnr    char (5 ) key,
aaswa      char (12) ,
aaadat     char (6 ) ,
aabkza     char (7 ) ,
aastatkn   char (1 ) ,
aaanzvv    char (3 ) ,
aagst      char (3 ) ,
aaost      char (3 ) ,
aaostw     char (4 ) ,
aaostm     char (4 ) ,
aaaehz     char (4 ))!
************************!
create table ab (
abswb      char (12) key,
abbkzb     char (7 ) ,
abregsb1   char (3 ) ,
abortknb   char (3 ))!
************************!
insert aa set aadstnr = '12345', aadat = '050687', aalfdnr = '1',
  aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O', aaanzvv = '002',
  aaostm = '1', aaost = '2' !
insert aa set aadstnr = '12346', aadat = '050687', aalfdnr = '1',
  aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O', aaanzvv = '014',
  aaostm = '1', aaost = '2' !
insert ab set abswb = 'abcdef', abbkzb = '52301',
  abregsb1 = 'A05'!
insert ab set abswb = 'fegrhf', abbkzb = '52301',
  abregsb1 = 'B05'!
commit!
select aa.aadstnr, aadat, aastatkn, aaost, aaostm, aaanzvv, ab.abregsb1
  from aa, ab
  where
        (aa.aaost <> ' ' and aa.aaostm < aa.aaost)
    and aa.aaanzvv >= '005'
    and aa.aastatkn = 'O'
    and ab.abregsb1 = 'A05'
    and aa.aaswa = ab.abswb and aa.aabkza = ab.abbkzb
  order by aa.aaanzvv !
fetch!
select aa.aadstnr, aadat, aastatkn, aaost, aaostm, aaanzvv, ab.abregsb1
  from aa, ab
  where
        aa.aaanzvv >= '005'
    and aa.aastatkn = 'O'
    and (aa.aaost <> ' ' and aa.aaostm < aa.aaost)
    and ab.abregsb1 = 'A05'
    and aa.aaswa = ab.abswb and aa.aabkza = ab.abbkzb
  order by aa.aaanzvv !
fetch!
drop table aa !
drop table ab !
commit release !
file connect ( kern test !
create table t ( a char (6))!
create table t1 ( b char (5))!
insert t set a = 'aaa'!
insert t1 set b = 'aaa'!
pars_then_ex !
declare erg cursor for 
select a&b from t1, t!
close erg!
declare erg cursor for 
select a&b from t, t1 !
pars_execute!
create table t2 ( a char (7), b char (7))!
create index i on t2 (a,b)!
declare erg1 cursor for 
select * from t2 where a = 'zzz' for reuse!
drop table t !
drop table t1 !
drop table t2 !
commit release !
