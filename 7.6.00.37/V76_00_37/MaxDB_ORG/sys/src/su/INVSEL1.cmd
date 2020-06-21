*   *ID* INVSEL1  DBCMD    changed on 1992-09-28-13.24.02 by ADMIN     *
declare erg cursor for 
select uk1, uk2  from u &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u order by u5 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u order by u5 desc &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u order by u5 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u order by u5 desc &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u order by u4 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u order by u4 desc &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u order by u4 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u order by u4 desc &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u order by u4 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u order by u4 desc &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 = 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 = 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 = 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 > 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 > 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 > 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 < 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 < 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 < 'EEE' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 between 'CCC' and 'KKK' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 between 'CCC' and 'KKK' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 between 'CCC' and 'KKK' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 between 'KKK' and 'CCC' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 between 'KKK' and 'CCC' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 between 'KKK' and 'CCC' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 like 'F%' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u5 like 'F%' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u5 like 'F%' &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u5 in ('LLL', 'CCC', 'GGG') &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u
where u5 in ('LLL', 'CCC', 'GGG') &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u
where u5 in ('LLL', 'CCC', 'GGG') &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 = 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 = 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 = 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 > 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 > 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 > 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 < 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 < 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 < 444 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 between 222 and 555 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 between 222 and 555 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 between 222 and 555 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 between 555 and 222 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 between 555 and 222 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 between 555 and 222 &1!
fetch erg!
declare erg cursor for 
select uk1, uk2  from u where u4 in (666, 222, 444) &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u4  from u where u4 in (666, 222, 444) &1!
fetch erg!
declare erg cursor for 
select uk1, uk2, u5  from u where u4 in (666, 222, 444) &1!
fetch erg!
