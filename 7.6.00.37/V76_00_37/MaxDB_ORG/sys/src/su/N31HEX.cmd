*   *ID* N31HEX   DBCMD    changed on 1992-09-28-13.26.07 by ADMIN     *
file connect ( kern test !
create table tab ( a char(8) ,
                   b char(8) ascii,
                   c char(8) byte)!
file sel_cols_tab ( TAB !
commit work!
insert tab set a = 'Muenchen', b = 'Muenchen', c = x'4D75656E6368656E'!
insert tab set a = 'abcdefgh', b = 'abcdefgh', c = x'6162636465666768' !
insert tab set a = date, b = date, c = x'f1f9f8f8f0f9f2f9' !
select * from tab !
fetch !
commit work !
select a, hex(a) from tab !
fetch !
select a, hex(substr(a,1,4)) from tab !
fetch !
select b, hex(b) from tab !
fetch !
select a, b, hex(a & b ) from tab !
fetch !
select a, b, hex(b & a ) from tab !
fetch !
select c, hex(c) from tab !
fetch !
select c, length(c), hex(c), length(hex(c)) from tab !
fetch !
select hex(b), hex(ascii(a)) from tab !
fetch !
select hex(fixed(123,3)) from tab !
fetch !
select hex(123) from tab !
fetch !
select hex('123') from tab !
fetch !
select * from tab where hex(a) = c !
select * from tab where hex(b) = hex(a)!
fetch !
select * from tab where hex(b) = hex(ascii(a))!
fetch !
alter table tab add d dbyte (8) !
IF &rc = 0 THEN BEGIN
update tab set d = g 'aabbccdd'!
select d, hex(d) from tab !
fetch !
select d, length(d) from tab !
fetch !
select d, length(d), hex(d), length(hex(d)) from tab !
fetch !
END
