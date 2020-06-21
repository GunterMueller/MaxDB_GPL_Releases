*   *ID* N30NUM1  DBCMD    changed on 1992-09-28-13.26.04 by ADMIN     *
select num ( '723'), NUM('0345'), num ( ' 723 ' ) ,
       NUM ('-6738944' ), num ( ' +  654 ' ), num ('+00745')
       from tab where spalte1 = 0 !
fetch !
select num (' 88.'), num ( ' -88.72000 ' ),
       num ( '  + 000000076543.0000021345678' ), num ( '  .0230' ),
       num ('-.31'  ), num (  '+  .7' )
       from tab  where NUM(kern.tab.spalte2) is null !
fetch !
select num ('70E12') , num ('+ 8.300e12 ' ), num (' -.03E+13' ),
       num ( '- .03E-13'), num ( '760.e6 ' ), num(' +07.8e06' )
       from tab  where   spalte1 <=  0 !
fetch !
select num (spalte4 ), num(spalte5) col2 from tab !
select num (spalte4 ), num(spalte5) col2
   from tab where spalte4 between '000' and '999'
               or spalte5 like '%6.78%' !
fetch !
select num (spalte4 ) from tab where spalte4 is null !
fetch !
select num (spalte1 ) from tab !
fetch !
select num   (spalte2 ) from tab !
fetch !
select  num(8) from tab !
fetch !
select num ( %)   from tab !
select num ( '7 8 ' ) from tab !
select num ( ' ' ) from tab !
select num ( '56' ' 58' ) from tab !
commit work !
create table elke ( a fixed (8))!
insert elke set a = 3 !
* resulted in -3019 invalid exponent up to 6.1.1 cl8 !
select a - num(1) into :a from elke !
select a - num('1') into :a from elke !
select a - fixed(num('1'),1) into :a from elke !
rollback work !
