file connect ( kern test !
select * from tab order by s1!
fetch !
select * from tab order by s1 DESC!
fetch !
select s2 from tab order by s1!
fetch !
select s2 from tab order by s1 DESC!
fetch !
select *,s1*s2 from tab order by s1*s2!
fetch !
select *,s1*s2 from tab order by s1*s2 DESC!
fetch !
select s4 from tab order by substr(s4,2,2)!
fetch !
select s4 from tab order by substr(s4,2,2) DESC!
fetch !
select * from tab order by 1!
fetch !
select * from tab order by 1 DESC!
fetch !
select s2 from tab order by s1 MOD s3!
fetch !
select s2 from tab order by s1 MOD s3 DESC!
fetch !
select s1,s3,s1 MOD s2 from tab order by power(s3,2),s1 MOD s2!
fetch !
select s1,s3,s1 MOD s2 from tab order by power(s3,2),s1 MOD s2 DESC!
fetch !
select s1*s2 r1 from tab order by r1!
fetch !
select s1*s2 r1 from tab order by r1 DESC!
fetch !
select * from tab where s3 = ( select s1 from tab1) !
fetch !
select * from tab where s3 = ( select s1 from tab1) order by s1 !
fetch !
select * from tab where s3 = ( select s1 from tab1) order by s1 DESC!
fetch !
select * from tab where s3 = ( select s1 from tab1) 
order by substr(s4,2,2) !
fetch !
select * from tab where s3 = ( select s1 from tab1) 
order by substr(s4,2,2) DESC!
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 order by 1 !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by 1 DESC!
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 
order by tab.s1 !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1 
order by tab.s1 DESC!
fetch !
select tab.s1,tab.s4,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by tab1.s2 !
fetch !
select tab.s1,tab.s4,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by tab1.s2 DESC!
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by substr(tab.s4,2,2) !
fetch !
select tab.*,tab1.s2 from tab,tab1 where tab.s3 = tab1.s1
order by substr(tab.s4,2,2) DESC!
fetch !
select s1,s4 from tab union select * from tab1 order by 1, 2 !
fetch !
select s1,s4 from tab union select * from tab1 order by 1 DESC, 2 !
fetch !
select s1,s4 from tab union select * from tab1 order by s4 !
select s1,s4 from tab union select * from tab1 order by s4 DESC!
select s1,s4 from tab union select * from tab1 order by substr(s4,2,2) !
select s1,s4 from tab union select * from tab1 
order by substr(s4,2,2) DESC!
commit work release !
