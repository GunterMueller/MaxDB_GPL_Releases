*   *ID* DEMOK5   DBCMD    changed on 1993-03-23-15.00.00 by MAX       *
file connect ( travel10 travel10 !
 * Maerz 1993, Folie 5.010 !
create view customer_res as select * from customer
            where cno in (select cno from booking) !
 * Maerz 1993, Folie 5.040 !
create view hotelroom
as select h.hno, h.name, h.zip, h.address, r.roomtype, r.max_free, r.price
from      hotel h, room r
where     h.hno = r.hno
with check option !
file sel_primkey ( TRAVEL10 HOTELROOM !
 * Maerz 1993, Folie 5.060 !
insert hotelroom values (160, 'Berlin', '10019', '11 Myway', 
                          'single', 20, 75)!
insert hotelroom values (160, 'Berlin', '10019', '11 Myway', 
                          'double', 50, 120)!
select * from hotelroom !
fetch !
select * from hotel !
fetch !
select * from room !
fetch !
 * Maerz 1993, Folie 5.070 !
update hotelroom set name = 'Olympia' where hno = 160 !
update hotelroom set name = 'Olympia' where zip = '10019' 
                                         and address = '11 Myway' !
subtrans begin !
delete hotelroom where zip = '10019' !
select * from hotelroom !
fetch !
select * from hotel !
fetch !
select * from room !
fetch !
subtrans rollback !
 * Maerz 1993, Folie 5.080 !
update hotelroom set price = price * 1.2 where zip = '10019' !
select * from hotelroom !
fetch !
select * from hotel !
fetch !
select * from room !
fetch !
 * Maerz 1993, Folie 5.090 !
delete hotelroom where zip = '10019' !
select * from hotelroom !
fetch !
select * from hotel !
fetch !
select * from room !
fetch !
rollback release!
