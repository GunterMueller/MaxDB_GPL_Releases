* *ID* D01MODE2.cmd 
*  
file connect ( kern test sqlmode &1 !
declare c cursor for select * from tab1 order by s1 * -1 !
fetch c into :a, :b !
close c !
 * ... by  2 sortiert nach Spalte 2 !
declare c cursor for select * from tab1 order by 2 !
fetch c into :a, :b !
close c !
 * ... by  '2' (nicht Spalte 2) ist sinnlos; es wird nicht sortiert !
declare c cursor for select * from tab1 order by 1+1 !
fetch c into :a, :b !
close c !
 * ... sortieren nur nach der 2.Spalte !
declare c cursor for select * from tab1 order by 1+1, 2 !
fetch c into :a, :b !
close c !
declare c cursor for select * from tab1 order by substr(s2, 2, 1) desc !
fetch c into :a, :b !
close c !
commit work release !
