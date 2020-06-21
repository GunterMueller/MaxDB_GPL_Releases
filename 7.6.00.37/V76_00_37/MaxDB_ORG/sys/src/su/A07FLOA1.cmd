*   *ID* A07FLOA1 DBCMD    changed on 1992-09-28-13.22.28 by ADMIN     *
file connect ( kern test !
create table kern.p1  (f1 char (10) ascii range in ('abcde |@#$',
                                              x'4153432042595421',
                                              x'c2e8e3c5f85a'),
  f2 char (12) range in ('abcde |@#$', x'c2e8e3c5f85a',
                                              x'4153432042595421'),
         f3 float (10)  range in ( 1.0, 1.0002, 000, -999e23, -0.0002 ),
          f4 char (7) byte range in (x'010203aa' , x'fFEe3300',
                                     'abc |@' ))!
create table kern.p2  (f1 char (10) ascii range in ('abcde |@#$') ,
  f2 char (12) range in ('abcde |@#$'),
         f3 float (10)  range in ( 1.0, 1.0002, 000, -999e23, -0.0002 ),
          f4 char (4) byte  )!
create table kern.p3 (n1 fixed (10,2)
        range in (-99.99, -99.00, -987, -998, -99912) ,
  f3 fixed (8) range in (-330, -335, -310, -312, -345, -340)) !
file sel_table !
commit work release !
