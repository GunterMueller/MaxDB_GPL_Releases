*   *ID* DEMOK10  DBCMD    changed on 1992-09-28-13.21.33 by ADMIN     *
file connect ( travel00 travel00 sqlmode internal !
* April 1992, Folie 10.010 !
select zip
  from customer
 where zip between '20000' and '21000'
     UNION
select zip
  from travel10.hotel
 where zip between '20000' and '21000' !
fetch !
select zip
  from customer
 where zip between '20000' and '21000'
     UNION ALL
select zip
  from travel10.hotel
 where zip between '20000' and '21000' !
fetch !
* April 1992, Folie 10.020 !
select c_city, c_zip
  from cust_addr
 where c_city > 'W'
    INTERSECT
select h_city, h_zip
  from hoteladdr
 where h_city > 'W' !
fetch !
select c_city, c_zip
  from cust_addr
 where c_city > 'W'
     EXCEPT
select h_city, h_zip
  from hoteladdr
 where h_city > 'W' !
fetch !
* April 1992, Folie 10.030 !
select travel10.hotel.name, 
       travel20.booking.hno, 
       travel20.booking.rno
  from travel10.hotel, travel20.booking
 where travel10.hotel.zip between '32000' and '33999'
   and travel10.hotel.hno    = travel20.booking.hno !
fetch !
select travel10.hotel.name, 
       travel10.hotel.hno, 
       travel20.booking.rno
  from travel10.hotel, travel20.booking
 where travel10.hotel.zip between '32000' and '33999'
   and travel10.hotel.hno    = travel20.booking.hno (+) !
fetch !
* April 1992, Folie 10.040 !
* by the way: test of create temp.table as select 
create table temp.ca as select * from cust_addr!
drop table temp.ca !
declare ca cursor for 
select * from cust_addr for reuse!
declare ha cursor for 
select * from hoteladdr for reuse!
select c_name, c_state, h_state, h_name
  from ca, ha
 where ca.c_state (+) = ha.h_state (+) !
fetch !
* April 1992, Folie 10.050 !
select c_name, c_state, h_state, h_name
  from (select * from cust_addr) ca,
       (select * from hoteladdr) ha
 where ca.c_state (+) = ha.h_state (+) !
fetch !
select customer.name, hr.name hotelname, rno
  from (select travel10.hotel.name, zip, rno
          from travel10.hotel, travel20.booking
         where travel10.hotel.hno = travel20.booking.hno (+))
       hr, customer
 where hr.zip (+) = customer.zip !
fetch !
* Februar 1993, Folie 10.055 !
select hno, max_free, price, max_free * price
  from room
  order by (max_free * price) desc !
fetch !
commit release !
