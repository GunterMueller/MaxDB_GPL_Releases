* *ID* D01MODE1.cmd 
* called by D01MODE
file connect ( kern test sqlmode &1 !
declare c cursor for select * from tab1 order by 1 !
fetch c into :a, :b !
close c !
declare c cursor for select * from tab1 order by s2 !
fetch c into :a, :b !
close c !
declare c cursor for select * from tab1 order by s1 + 5 !
fetch c into :a, :b !
close c !
declare c cursor for select s1 + 5 r, s2 from tab1 order by r !
fetch c into :a, :b !
close c !
commit work release !
