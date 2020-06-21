connect sun identified by sun sqlmode ansi !
pars_then_ex !
      DELETE FROM c_transac !
      DELETE FROM COMMODITY !
      DELETE FROM curr_tab !
      DELETE FROM measure_t !
      INSERT INTO COMMODITY
              VALUES (17, 'Wheat') !
      INSERT INTO COMMODITY
              VALUES (14, 'Saffron') !
      INSERT INTO COMMODITY
              VALUES (23, 'Alfalfa') !
      INSERT INTO curr_tab
              VALUES ('DOLLAR', 1.00) !
      INSERT INTO curr_tab
              VALUES ('POUND', 1.91) !
      INSERT INTO curr_tab
              VALUES ('DM', .45) !
      INSERT INTO measure_t
              VALUES ('POUND', 1.00) !
      INSERT INTO measure_t
              VALUES ('OUNCE', .06) !
      INSERT INTO measure_t
              VALUES ('KILO', 2.20) !
      INSERT INTO measure_t
              VALUES ('TON', 2000.00) !
      INSERT INTO c_transac
              VALUES (17, 1411.5, 'DM', 4000, 'KILO', 871212) !
      INSERT INTO c_transac
              VALUES (17, 7000.0, 'POUND', 100, 'TON', 871012) !
      INSERT INTO c_transac
              VALUES (23, 20000.0, 'DOLLAR', 40000, 'POUND', 880707) !
      INSERT INTO c_transac
              VALUES (14, 10000.0, 'DM', 900, 'OUNCE', 880606) !
      INSERT INTO c_transac
              VALUES (14, 10000.0, 'DM', 900, 'OUNCE', 880707) !
      COMMIT WORK !
* Test0402 !
      SELECT COUNT(*) INTO :count1
              FROM c_transac WHERE COMMOD_NO = 17 !
      DECLARE CCC CURSOR FOR
           SELECT u_price, FROM_DATE, TO_DATE, COMMODITY 
              FROM dol_per_p
              ORDER BY COMMODITY DESC !
      FETCH CCC
              INTO  :PRICE1, :FROMDT, :TODATE, :COMMOD !
      CLOSE CCC !
      COMMIT WORK !
* Test0403 !
      SELECT COUNT(*) INTO :count1
              FROM cost_p_u !
      DECLARE DDD CURSOR FOR 
          SELECT CURRENCY, MEASURE, u_price, COMMODITY
              FROM cost_p_u !
            FETCH DDD
                    INTO :CURREN, :MEASUR, :PRICE2, :COMMOD !
      COMMIT WORK !
* Test0413 !
data !
      DECLARE EEE CURSOR FOR
           SELECT (100 + :PERCNT) * u_price * :UNITZ / 100, COMMODITY
              FROM dol_per_p
              ORDER BY COMMODITY !
n 38 7 n 38 700

nodata !
      FETCH EEE INTO :MYBID1, :COMMOD !
      CLOSE EEE !
pars_execute!
      COMMIT WORK RELEASE !
