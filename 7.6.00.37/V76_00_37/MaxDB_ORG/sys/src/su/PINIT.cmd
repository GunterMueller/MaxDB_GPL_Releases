*   *ID* PINIT    DBCMD    changed on 1992-09-28-13.27.40 by ADMIN     *
 *** zweckmaessigen Anfangszustand schaffen !
create table kern.abt
                (name char (12) key RANGE between 'AA' and 'TT',
                      nummer fixed (6),
                      arbeiter fixed (3) RANGE between 10 and 999,
                      num2 fixed(4) not null RANGE in (1000,8000,5300) ,
                      ort char (12) RANGE in ('BERLIN', 'MUENCHEN',
                                             'BONN')) !
create table kern.lnum (l fixed (6) key ,
                        eins char (14) not null,
                        zwei char (250) not null) !
create table kern.lieferant  (lname char(10) key,
                                   ort char (3) RANGE in
                                              ('MUN', 'HAM')) !
commit work !
