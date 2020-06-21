*   *ID* N55VIEW3 DBCMD    changed on 1992-09-28-13.26.54 by ADMIN     *
*
* Dieser VDNTEST-File reproduziert die
* PRISM- Fehler 92198 und PRSIM 92188
* tz, 3.1.90
file connect ( kern test !
CREATE TABLE customer ( cust_no char(7) key,
                        last_name char(15),
                        firstname char(30) not null,
                        address char(15) not null ) !
INSERT customer VALUES ( '2' , 'Mueller', 'Peter', 'Berlin') !
INSERT customer VALUES ( '1' , 'Meier', 'Frank', 'Hamburg') !
CREATE VIEW cust_view as
       SELECT fixed(num(cust_no), 7) custom_no, last_name,
              firstname, address
       FROM customer !
SELECT * FROM cust_view !
fetch !
SELECT * FROM cust_view
order by custom_no !
fetch !
SELECT * FROM cust_view
order by 1 !
fetch !
* PRISM 92188
file sel_cols_tab ( CUST_VIEW !
drop view cust_view !
CREATE VIEW cust_view as
       SELECT fixed(num(cust_no), 7) custom_no, last_name,
              firstname, address, cust_no
       FROM customer !
drop table customer !
COMMIT WORK RELEASE !
