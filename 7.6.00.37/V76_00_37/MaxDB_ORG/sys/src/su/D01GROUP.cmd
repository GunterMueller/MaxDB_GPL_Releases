file connect ( kern test !
select * from tab group by s1!
select s2 from tab group by s1!
select count(*),s1*s2 from tab group by s1*s2!
fetch !
select substr(s4,2,2) from tab group by substr(s4,2,2)!
fetch !
select substr(s4,2,2) from tab group by s4!
fetch !
select * from tab group by 1!
select s2 from tab group by 1!
select sum(s1),min(s3),s1 MOD s2 from tab group by s1 MOD s2!
fetch !
select s1*s2 r1 from tab group by r1!
fetch !
select * from tab where s3 = ( select s1 from tab1) !
fetch !
select * from tab where s3 = ( select s1 from tab1) group by s1 !
select s1 from tab where s3 = ( select s1 from tab1) group by s1 !
fetch !
select * from tab where s3 = ( select s1 from tab1) 
group by substr(s4,2,2) !
select substr(tab.s4,2,2) from tab where s3 = ( select s1 from tab1) 
group by substr(s4,2,2) !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 group by 1 !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 
group by tab.s1 !
fetch !
select min(tab.s1),max(tab.s4),tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
group by tab1.s2 !
fetch !
select substr(tab.s4,2,2),min(tab1.s2) from tab,tab1 where tab.s3 = tab1.s1
group by substr(tab.s4,2,2) !
fetch !
select s1,s4 from tab union select * from tab1 group by 1 !
select s1,s4 from tab union select * from tab1 group by s4 !
select s1,s4 from tab union select * from tab1 group by substr(s4,2,2) !
commit work release !
