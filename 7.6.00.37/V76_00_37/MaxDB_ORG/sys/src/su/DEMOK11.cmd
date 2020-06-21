*   *ID* DEMOK11  DBCMD    changed on 1992-09-28-13.21.34 by ADMIN     *
file connect ( travel00 travel00 sqlmode internal !
* April 1992, Folie 11.010 !
create table hotelexample
   (hno       travel10.hno_dom key,
    price     fixed (6,2),
    weekprice fixed (7,2))!
insert hotelexample values (3824, 78, 78 * 7 * 0.95) !
commit release !
* April 1992, Folie 11.020 !
file connect ( travel00 travel00 sqlmode internal !
create table avgroomprice (
   zip      travel00.zip_dom key,
   roomtype travel10.room_dom key,
   cityname char (20),
   avgprice fixed (5,2))!
insert avgroomprice
    select distinct hotel.zip, roomtype, city.name, NULL
      from city, hotel, room
     where city.zip = hotel.zip
       and hotel.hno = room.hno !
select * from avgroomprice !
fetch !
update avgroomprice set avgprice =
     (select avg (price)
        from hotel, room
       where hotel.zip = avgroomprice.zip
         and room.roomtype = avgroomprice.roomtype
         and room.hno = hotel.hno)!
select * from avgroomprice !
fetch !
commit release !
