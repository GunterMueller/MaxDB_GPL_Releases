*   *ID* INVSEL2  DBCMD    changed on 1992-09-28-13.24.05 by ADMIN     *
declare erg cursor for 
select uk1, uk2  from u where uk1 >= 'a2' &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2  from u where uk2 <= 2 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2  from u where u1 > 20 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2  from u where u2 < 'zz' &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1  from u where uk1 >= 'a2' &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1  from u where uk2 <= 2 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1  from u where u1 > 20 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1  from u where u2 < 'zz' &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1, u2  from u where uk1 >= 'a2' &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1, u2  from u where uk2 <= 2 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1, u2  from u where u1 > 20 &1 !
fetch erg !
declare erg cursor for 
select uk1, uk2, u1, u2  from u where u2 < 'zz' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 = 'a1' and uk2 = 1 and u1 = 22.222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 = 'a1' and uk2 > 2 and u1 > 22.22 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 = 'a1' and uk2 < 2 and u1 < 22.2222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 > 'a' and uk2 >= 1 and u1 > 22.22 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 < 'a2' and uk2 < 2 and u1 < 22.2222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u1  from u
where uk1 < 'a2' and uk2 > 1 and u1 > 10 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 = 1 and u2 = 'yy' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 > 2 and u2 > 'y' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 < 2 and u2 < 'yyy' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 > 'a' and uk2 >= 1 and u2 > 'y' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 < 'a2' and uk2 < 2 and u2 < 'yyy' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 < 'a2' and uk2 > 1 and u2 > 'ww' &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 = 1 and u2 = 'yy' and u1 = 22.222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 > 2 and u2 > 'y' and u1 is null &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 = 'a1' and uk2 < 2 and u2 < 'yyy' and u1 < 22.2222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 > 'a' and uk2 >= 1 and u2 > 'y' and u1 > 11 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 < 'a2' and uk2 < 2 and u2 < 'yyy' and u1 < 22.2222 &1 !
fetch erg !
declare erg cursor for 
select uk2, uk1, u2  from u
where uk1 < 'a2' and uk2 > 1 and u2 > 'ww' and u1 < 35 &1 !
fetch erg !