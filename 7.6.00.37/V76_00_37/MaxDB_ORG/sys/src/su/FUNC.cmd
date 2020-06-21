*   *ID* FUNC     DBCMD    changed on 1992-09-28-13.21.38 by ADMIN     *
file connect ( kern test sqlmode internal !
set format &3 !
file func1 ( &1 &2 !
commit release !
file connect ( kern test sqlmode db2 !
set format &3 !
select char        ( &1                 ) into :a from &2 where k1 = 1 !
select char        ( &1, internal       ) into :a from &2 where k1 = 1 !
select char        ( &1, iso            ) into :a from &2 where k1 = 1 !
select char        ( &1, usa            ) into :a from &2 where k1 = 1 !
select char        ( &1, jis            ) into :a from &2 where k1 = 1 !
select char        ( &1, eur            ) into :a from &2 where k1 = 1 !
select date        ( &1                 ) into :a from &2 where k1 = 1 !
select days        ( &1                 ) into :a from &2 where k1 = 1 !
select decimal     ( &1, 18             ) into :a from &2 where k1 = 1 !
select digits      ( &1                 ) into :a from &2 where k1 = 1 !
select float       ( &1                 ) into :a from &2 where k1 = 1 !
select integer     ( &1                 ) into :a from &2 where k1 = 1 !
select microsecond ( &1                 ) into :a from &2 where k1 = 1 !
select time        ( &1                 ) into :a from &2 where k1 = 1 !
select timestamp   ( &1                 ) into :a from &2 where k1 = 1 !
select timestamp   ( &1, '00013000'     ) into :a from &2 where k1 = 1 !
commit release !
file connect ( kern test sqlmode oracle !
select add_months  ( &1, 3              ) into :a from &2 where k1 = 1 !
select ascii       ( &1                 ) into :a from &2 where k1 = 1 !
select chr         ( &1                 ) into :a from &2 where k1 = 1 !
select concat      ( &1, ' Testfall'    ) into :a from &2 where k1 = 1 !
select cos         ( &1                 ) into :a from &2 where k1 = 1 !
select cosh        ( &1                 ) into :a from &2 where k1 = 1 !
select exp         ( &1                 ) into :a from &2 where k1 = 1 !
select hextoraw    ( &1                 ) into :a from &2 where k1 = 1 !
select instr       ( &1, 'c', 2         ) into :a from &2 where k1 = 1 !
select instr       ( &1, ' ', 2         ) into :a from &2 where k1 = 1 !
select last_day    ( &1                 ) into :a from &2 where k1 = 1 !
select length      ( &1                 ) into :a from &2 where k1 = 1 !
select ln          ( &1                 ) into :a from &2 where k1 = 1 !
select log         ( 10, &1             ) into :a from &2 where k1 = 1 !
select lpad        ( &1, 8, 's'         ) into :a from &2 where k1 = 1 !
select lpad        ( &1, 350, 's'       ) into :a from &2 where k1 = 1 !
select ltrim       ( &1                 ) into :a from &2 where k1 = 1 !
select ltrim       ( &1, 'ae0123'       ) into :a from &2 where k1 = 1 !
select mod         ( &1, 3              ) into :a from &2 where k1 = 1 !
select months_between( &1, &1           ) into :a from &2 where k1 = 1 !
select new_time    ( &1, 'AST', 'MST'   ) into :a from &2 where k1 = 1 !
select next_day    ( &1, 1              ) into :a from &2 where k1 = 1 !
select next_day    ( &1, 'monday'       ) into :a from &2 where k1 = 1 !
select nvl         ( &1, &1             ) into :a from &2 where k1 = 1 !
select rawtohex    ( &1                 ) into :a from &2 where k1 = 1 !
select rpad        ( &1, 350, 's'       ) into :a from &2 where k1 = 1 !
select rtrim       ( &1                 ) into :a from &2 where k1 = 1 !
select rtrim       ( &1, 'ae0123'       ) into :a from &2 where k1 = 1 !
select sin         ( &1                 ) into :a from &2 where k1 = 1 !
select sinh        ( &1                 ) into :a from &2 where k1 = 1 !
select tan         ( &1                 ) into :a from &2 where k1 = 1 !
select tanh        ( &1                 ) into :a from &2 where k1 = 1 !
select timestamp   ( &1                 ) into :a from &2 where k1 = 1 !
select timestamp   ( &1, '00013000'     ) into :a from &2 where k1 = 1 !
select to_char     ( &1                 ) into :a from &2 where k1 = 1 !
select to_date     ( &1                 ) into :a from &2 where k1 = 1 !
select to_number   ( &1                 ) into :a from &2 where k1 = 1 !
select vsize       ( &1                 ) into :a from &2 where k1 = 1 !
commit release !
senderid ODB !
file connect ( kern test !
 * ODBC !
select ifnull      ( &1, 'a'            ) into :a from &2 where k1 = 1 !
select lcase       ( &1                 ) into :a from &2 where k1 = 1 !
select left        ( &1, 3              ) into :a from &2 where k1 = 1 !
select right       ( &1, 3              ) into :a from &2 where k1 = 1 !
select truncate    ( &1                 ) into :a from &2 where k1 = 1 !
select ucase       ( &1                 ) into :a from &2 where k1 = 1 !
commit release !
senderid XCI !
