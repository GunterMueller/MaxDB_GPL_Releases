*** STAUAB.cmd ***
* Aufruf in STAUA.cmd
*
* Anlegen der Stapel-VIEWs
CREATE VIEW VE&1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM VE&2
            WHERE (K = 1) OR (N = 1) OR (I = 1) 
               OR (N + K = 2) OR (N + I = 2) OR (I + K = 2)
               OR (N * K = 1) OR (N * I = 1) OR (I * K = 1)
               OR (SQRT(N) = POWER(K,2)) OR (SQRT(N) = POWER(I,2)) 
               OR (SQRT(I) = POWER(K,2))!
CREATE VIEW VW&1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM VW&2
            WHERE (K < 6) AND (I < 6) AND (N < 6)
              AND (N + K < 10) AND (N + I < 10) AND (K + I < 10)
              AND (N * K < 10) AND (N * I < 10) AND (K * I < 10)
              AND (SQRT(N) = POWER(K,2)) AND (SQRT(N) = POWER(I,2)) 
              AND (SQRT(I) = POWER(K,2))!
CREATE VIEW VS&1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 FROM VS&2
            WHERE I IN ( -1.25, 13E+14, K * 120, 1, N * N, I / (I + 1)) 
              AND K IN ( -2.25, 12E-14, N * 20, 1, I * I, N / (N + 1)) 
              AND N IN ( -3.25, 11E+55, I * 21, 1, K * K, K / (K + 1))! 
*
CREATE VIEW VX&1 AS SELECT T1.K,T1.N FROM VE&1 T1,VW&1 T2,VS&1 T3 
WHERE T1.K = T2.K AND T1.N = T2.N 
  AND T3.N = T2.N AND T3.K = T2.K!
*
CREATE VIEW VY&1 AS SELECT N,K FROM VE&1
                    UNION ALL
                    SELECT N,K FROM VW&1
                    UNION ALL
                    SELECT N,K FROM VS&1!
*
CREATE VIEW VZ&1 AS SELECT T1.K,T1.N 
                    FROM VE&1 T1,VW&1 T2,VS&1 T3 
                    WHERE T1.K = T2.K OR T1.N = T2.N 
                    OR T3.N = T2.N OR T3.K = T2.K
                    OR T2.K = T3.N OR T1.N = T2.K!
*
CREATE VIEW VA&1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 
                    FROM VE&1 
                    WHERE K IN 
                          (SELECT N FROM VW&1
                           WHERE K IN 
                                (SELECT N FROM VS&1
                                 WHERE (K = 2 * N) OR (K = 1)))!
*
CREATE VIEW VB&1 AS SELECT K,I,N,F4,FILL1,FILL2,FILL3,FILL4 
                    FROM VE&1 
                    WHERE (K IN 
                             (SELECT N FROM VW&1
                              WHERE K = 1))
                      AND (N IN    
                             (SELECT K FROM VS&1
                              WHERE K = 1))!
*
CREATE VIEW VC&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N)
  AND T0.N IN
              (SELECT K FROM VS&1 T2
               WHERE N = T0.K)!
*
CREATE VIEW VD&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T0.N IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE K = T0.K))!
*
CREATE VIEW VF&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T0.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VG&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T0.K))!
*
CREATE VIEW VH&1 AS SELECT N FROM VE&1 T0
WHERE T0.K IN
              (SELECT N FROM VW&1 T1
               WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K))!
*
CREATE VIEW VI&1 AS SELECT N FROM VE&1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW&1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K AND K IS NULL))!
*
CREATE VIEW VJ&1 AS SELECT N FROM VE&1 T0
WHERE K = N AND NOT EXISTS
               (SELECT N FROM VW&1 T1
                WHERE K = T0.N
                         AND T1.K IN
                                    (SELECT N FROM VS&1 T2
                                     WHERE N = T1.K AND T0.K IS NULL))!
*
CREATE VIEW VK&1 AS SELECT T0.N FROM VE&1 T0
WHERE T0.N IN
              (SELECT T1.N FROM VW&1 T1, VS&1 T2
               WHERE T1.N = T0.N
                 AND T2.N = T0.K)!
FILE STAUAS( &1!
COMMIT!
