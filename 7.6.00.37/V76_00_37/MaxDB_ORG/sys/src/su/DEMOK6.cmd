*   *ID* DEMOK6   DBCMD    changed on 1993-03-23-15.00.00 by MAX       *
file connect ( travel10 travel10 !
 * Maerz 1993, Folie 6.010 !
create view HOTELROOM
as select h.hno, h.name, h.zip, h.address, r.roomtype, r.max_free, r.price
from      hotel h, room r
where     h.hno = r.hno
with check option !
grant insert, select, update, delete
      on HOTELROOM
      to travel00, travel20
      with grant option !
file sel_priv_grant_on ( HOTELROOM !
file sel_priv_grant_on ( HOTELROOM !
grant execute 
      on travel00.dbprocs.proc00
      to travel10, travel20 !
grant execute 
      on travel00.proc00
      to travel10, travel20 !
rollback release !
