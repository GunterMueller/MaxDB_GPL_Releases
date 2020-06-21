file connect ( kern test !
insert tab1 values (-7, 'zaz') !
insert tab1 values (1, 'aza') !
select * from tab1 !
fetch !
commit work release !
file d01mode1 ( ansi !
file d01mode1 ( db2 !
file d01mode1 ( oracle !
file d01mode1 ( internal !
 * nur fuer SQL-DB und ORACLE funktioniert 'ORDER BY <expr>'!
file d01mode2 ( oracle !
file d01mode2 ( internal !
file connect  ( kern test !
delete from tab1 where s1 <> 5 !
select * from tab1 !
fetch !
commit work release !
