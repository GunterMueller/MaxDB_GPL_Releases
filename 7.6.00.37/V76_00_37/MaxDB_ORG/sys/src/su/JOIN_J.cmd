senderid DBM !
file connect ( sut sut ! 
set parameter OPTIMIZE_OPERATOR_JOIN = '&1' !
commit release !
senderid XCI !

*****************************************************************************
* JOIN_N71.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file n71join1 !
file n71join2 !
file n71join3 !
file n71join4 !
file n71join5 !
file n71join6 !
file n71join7 !
file n71join8 !
file n71join9 !
file n71join10 !
file n71join11 !
file n71join12 !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN_PAR_EX.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
file connect ( kern test !

* test parse/execute saving of join informations
* with full filled dmli.d_joins

CREATE TABLE TAB1
(K0 CHAR (240) KEY, K1 FIXED (10) KEY,
I1 FIXED (10), I2 FIXED (10) ,
C1 CHAR (20), C2 VARCHAR (20) NOT NULL,
I3 FIXED (10) NOT NULL,
CLLEN  FIXED (10), CF0 CHAR (1500), CLB LONG ,
BI_1 CHAR(1), BI_2 CHAR(1), BI_3 CHAR(1), 
BI_4 CHAR(1), BI_5 CHAR(1),
BI_6 CHAR(1), BI_7 CHAR(1), BI_8 CHAR(1), 
BI_9 CHAR(5), BI_10 CHAR(5),
BI_11 CHAR(5), BI_12 CHAR(5), BI_13 CHAR(5), 
BI_14 CHAR(5), BI_15 CHAR(5),
BI_16 CHAR(5), BI_17 CHAR(5), BI_18 CHAR(5), 
BI_19 CHAR(5), BI_20 CHAR(5),
BI_21 CHAR(5), BI_22 CHAR(5), BI_23 CHAR(5), 
BI_24 CHAR(5), BI_25 CHAR(5),
BI_26 CHAR(5), BI_27 CHAR(5), BI_28 CHAR(5), 
BI_29 CHAR(5), BI_30 CHAR(5),
BI_31 CHAR(5), BI_32 CHAR(5), BI_33 CHAR(5), 
BI_34 CHAR(5), BI_35 CHAR(5),
BI_36 CHAR(5), BI_37 CHAR(5), BI_38 CHAR(5), 
BI_39 CHAR(5), BI_40 CHAR(5),
BI_41 CHAR(5), BI_42 CHAR(5), BI_43 CHAR(5), 
BI_44 CHAR(5), BI_45 CHAR(5),
BI_46 CHAR(5), BI_47 CHAR(5), BI_48 CHAR(5), 
BI_49 CHAR(5), BI_50 CHAR(5),
BI_51 CHAR(5), BI_52 CHAR(5), BI_53 CHAR(5), 
BI_54 CHAR(5), BI_55 CHAR(5),
BI_56 CHAR(5), BI_57 CHAR(5), BI_58 CHAR(5), 
BI_59 CHAR(5), BI_60 CHAR(5),
BI_61 CHAR(5), BI_62 CHAR(5), BI_63 CHAR(5), 
BI_64 CHAR(5), BI_65 CHAR(5),
BI_66 CHAR(5), BI_67 CHAR(5), BI_68 CHAR(5), 
BI_69 CHAR(5), BI_70 CHAR(5),
BI_71 CHAR(5), BI_72 CHAR(5), BI_73 CHAR(5), 
BI_74 CHAR(5), BI_75 CHAR(5),
BI_76 CHAR(5), BI_77 CHAR(5), BI_78 CHAR(5), 
BI_79 CHAR(5), BI_80 CHAR(5),
BI_81 CHAR(5), BI_82 CHAR(5), BI_83 CHAR(5), 
BI_84 CHAR(5), BI_85 CHAR(5),
BI_86 CHAR(5), BI_87 CHAR(5), BI_88 CHAR(5), 
BI_89 CHAR(5), BI_90 CHAR(5),
BI_91 CHAR(5), BI_92 CHAR(5), BI_93 CHAR(5), 
BI_94 CHAR(5), BI_95 CHAR(5),
BI_96 CHAR(5), BI_97 CHAR(5), BI_98 CHAR(5), 
BI_99 CHAR(5), BI_100 CHAR(5),
BI_101 CHAR(1), BI_102 CHAR(1), BI_103 CHAR(1), 
BI_104 CHAR(1), BI_105 CHAR(1),
BI_106 CHAR(1), BI_107 CHAR(1), BI_108 CHAR(1), 
BI_109 CHAR(1), BI_110 CHAR(1),
BI_111 CHAR(1), BI_112 CHAR(1), BI_113 CHAR(1), 
BI_114 CHAR(1), BI_115 CHAR(1),
BI_116 CHAR(1), BI_117 CHAR(1), BI_118 CHAR(1), 
BI_119 CHAR(1), BI_120 CHAR(1),
BI_121 CHAR(1), BI_122 CHAR(1), BI_123 CHAR(1), 
BI_124 CHAR(1), BI_125 CHAR(1),
BI_126 CHAR(1), BI_127 CHAR(1), BI_128 CHAR(1), 
BI_129 CHAR(1), BI_130 CHAR(1),
BI_131 CHAR(1), BI_132 CHAR(1), BI_133 CHAR(1), 
BI_134 CHAR(1), BI_135 CHAR(1),
BI_136 CHAR(1), BI_137 CHAR(1), BI_138 CHAR(1), 
BI_139 CHAR(1), BI_140 CHAR(1),
BI_141 CHAR(1), BI_142 CHAR(1), BI_143 CHAR(1), 
BI_144 CHAR(1), BI_145 CHAR(1),
BI_146 CHAR(1), BI_147 CHAR(1), BI_148 CHAR(1), 
BI_149 CHAR(1), BI_150 CHAR(1),
BI_151 CHAR(1), BI_152 CHAR(1), BI_153 CHAR(1), 
BI_154 CHAR(1), BI_155 CHAR(1),
BI_156 CHAR(1), BI_157 CHAR(1), BI_158 CHAR(1), 
BI_159 CHAR(1), BI_160 CHAR(1),
BI_161 CHAR(1), BI_162 CHAR(1), BI_163 CHAR(1), 
BI_164 CHAR(1), BI_165 CHAR(1),
BI_166 CHAR(1), BI_167 CHAR(1), BI_168 CHAR(1), 
BI_169 CHAR(1), BI_170 CHAR(1),
BI_171 CHAR(1), BI_172 CHAR(1), BI_173 CHAR(1), 
BI_174 CHAR(1), BI_175 CHAR(1),
BI_176 CHAR(1), BI_177 CHAR(1), BI_178 CHAR(1), 
BI_179 CHAR(1), BI_180 CHAR(1),
BI_181 CHAR(1), BI_182 CHAR(1), BI_183 CHAR(1), 
BI_184 CHAR(1), BI_185 CHAR(1),
BI_186 CHAR(1), BI_187 CHAR(1), BI_188 CHAR(1), 
BI_189 CHAR(1), BI_190 CHAR(1),
BI_191 CHAR(1), BI_192 CHAR(1), BI_193 CHAR(1), 
BI_194 CHAR(1), BI_195 CHAR(1),
BI_196 CHAR(1), BI_197 CHAR(1), BI_198 CHAR(1),
BI_199 CHAR(1), BI_200 CHAR(1),
BI_201 CHAR(1), BI_202 CHAR(1), BI_203 CHAR(1), 
BI_204 CHAR(1), BI_205 CHAR(1),
BI_206 CHAR(1), BI_207 CHAR(1), BI_208 CHAR(1), 
BI_209 CHAR(1), BI_210 CHAR(1),
BI_211 CHAR(1), BI_212 CHAR(1), BI_213 CHAR(1), 
BI_214 CHAR(1), BI_215 CHAR(1),
BI_216 CHAR(1), BI_217 CHAR(1), BI_218 CHAR(1), 
BI_219 CHAR(1), BI_220 CHAR(1),
BI_221 CHAR(1), BI_222 CHAR(1), BI_223 CHAR(1), 
BI_224 CHAR(1), BI_225 CHAR(1),
BI_226 CHAR(1), BI_227 CHAR(1), BI_228 CHAR(1), 
BI_229 CHAR(1), BI_230 CHAR(1),
BI_231 CHAR(1), BI_232 CHAR(1), BI_233 CHAR(1), 
BI_234 CHAR(1), BI_235 CHAR(1),
BI_236 CHAR(1), BI_237 CHAR(1), BI_238 CHAR(1), 
BI_239 CHAR(1), BI_240 CHAR(1),
BI_241 CHAR(1), BI_242 CHAR(1), BI_243 CHAR(1), 
BI_244 CHAR(1), BI_245 CHAR(1),
BI_246 CHAR(1), BI_247 CHAR(1), BI_248 CHAR(1), 
BI_249 CHAR(1), BI_250 CHAR(1),
BI_251 CHAR(1), BI_252 CHAR(1), BI_253 CHAR(1), 
BI_254 CHAR(1), BI_255 CHAR(1),
BI_256 CHAR(1), BI_257 CHAR(1), BI_258 CHAR(1), 
BI_259 CHAR(1), BI_260 CHAR(1),
BI_261 CHAR(1), BI_262 CHAR(1), BI_263 CHAR(1), 
BI_264 CHAR(1), BI_265 CHAR(1),
BI_266 CHAR(1), BI_267 CHAR(1), BI_268 CHAR(1), 
BI_269 CHAR(1), BI_270 CHAR(1),
BI_271 CHAR(1), BI_272 CHAR(1), BI_273 CHAR(1), 
BI_274 CHAR(1), BI_275 CHAR(1),
BI_276 CHAR(1), BI_277 CHAR(1), BI_278 CHAR(1), 
BI_279 CHAR(1), BI_280 CHAR(1),
BI_281 CHAR(1), BI_282 CHAR(1), BI_283 CHAR(1), 
BI_284 CHAR(1), BI_285 CHAR(1),
BI_286 CHAR(1), BI_287 CHAR(1), BI_288 CHAR(1), 
BI_289 CHAR(1), BI_290 CHAR(1),
BI_291 CHAR(1), BI_292 CHAR(1), BI_293 CHAR(1), 
BI_294 CHAR(1), BI_295 CHAR(1),
BI_296 CHAR(1), BI_297 CHAR(1), BI_298 CHAR(1), 
BI_299 CHAR(1), BI_300 CHAR(1),
BI_301 CHAR(1), BI_302 CHAR(1), BI_303 CHAR(1), 
BI_304 CHAR(1), BI_305 CHAR(1),
BI_306 CHAR(1), BI_307 CHAR(1), BI_308 CHAR(1), 
BI_309 CHAR(1), BI_310 CHAR(1),
BI_311 CHAR(1), BI_312 CHAR(1), BI_313 CHAR(1), 
BI_314 CHAR(1), BI_315 CHAR(1),
BI_316 CHAR(1), BI_317 CHAR(1), BI_318 CHAR(1),
 BI_319 CHAR(1), BI_320 CHAR(1),
BI_321 FIXED(2) DEFAULT 1, BI_322 FIXED(2) DEFAULT 1, 
BI_323 FIXED(2) DEFAULT 1,
BI_324 FIXED(2) DEFAULT 1, BI_325 FIXED(2) DEFAULT 1, 
BI_326 FIXED(2) DEFAULT 1,
BI_327 FIXED(2) DEFAULT 1, BI_328 FIXED(2) DEFAULT 1, 
BI_329 FIXED(2) DEFAULT 1,
BI_330 FIXED(2) DEFAULT 1, BI_331 FIXED(2) DEFAULT 1, 
BI_332 FIXED(2) DEFAULT 1,
BI_333 FIXED(2) DEFAULT 1, BI_334 FIXED(2) DEFAULT 1, 
BI_335 FIXED(2) DEFAULT 1,
BI_336 FIXED(2) DEFAULT 1, BI_337 FIXED(2) DEFAULT 1, 
BI_338 FIXED(2) DEFAULT 1,
BI_339 FIXED(2) DEFAULT 1, BI_340 FIXED(2) DEFAULT 1, 
BI_341 FIXED(2) DEFAULT 1,
BI_342 FIXED(2) DEFAULT 1, BI_343 FIXED(2) DEFAULT 1, 
BI_344 FIXED(2) DEFAULT 1,
BI_345 FIXED(2) DEFAULT 1, BI_346 FIXED(2) DEFAULT 1, 
BI_347 FIXED(2) DEFAULT 1,
BI_348 FIXED(2) DEFAULT 1, BI_349 FIXED(2) DEFAULT 1, 
BI_350 FIXED(2) DEFAULT 1,
BI_351 FIXED(2), BI_352 FIXED(2), BI_353 FIXED(2), 
BI_354 FIXED(2), BI_355 FIXED(2),
BI_356 FIXED(2), BI_357 FIXED(2), BI_358 FIXED(2), 
BI_359 FIXED(2), BI_360 FIXED(2),
BI_361 FIXED(2), BI_362 FIXED(2), BI_363 FIXED(2), 
BI_364 FIXED(2), BI_365 FIXED(2),
BI_366 FIXED(2), BI_367 FIXED(2), BI_368 FIXED(2), 
BI_369 FIXED(2), BI_370 FIXED(2),
BI_371 FIXED(2), BI_372 FIXED(2), BI_373 FIXED(2), 
BI_374 FIXED(2), BI_375 FIXED(2),
BI_376 FIXED(2), BI_377 FIXED(2), BI_378 FIXED(2), 
BI_379 FIXED(2), BI_380 FIXED(2),
BI_381 FIXED(2), BI_382 FIXED(2), BI_383 FIXED(2), 
BI_384 FIXED(2), BI_385 FIXED(2),
I_386 VARCHAR(5) DEFAULT 'a', I_387 VARCHAR(5) DEFAULT 'a', 
I_388 VARCHAR(5) DEFAULT 'a',
I_389 VARCHAR(5) DEFAULT 'a', I_390 VARCHAR(5) DEFAULT 'a', 
I_391 VARCHAR(5) DEFAULT 'a',
I_392 VARCHAR(5) DEFAULT 'a', I_393 VARCHAR(5) DEFAULT 'a', 
I_394 VARCHAR(5) DEFAULT 'a',
I_395 VARCHAR(5) DEFAULT 'a', I_396 VARCHAR(5) DEFAULT 'a', 
I_397 VARCHAR(5) DEFAULT 'a',
I_398 VARCHAR(5) DEFAULT 'a', I_399 VARCHAR(5) DEFAULT 'a', 
I_400 VARCHAR(5) DEFAULT 'a',
I_401 VARCHAR(5) DEFAULT 'a', I_402 VARCHAR(5) DEFAULT 'a', 
I_403 VARCHAR(5) DEFAULT 'a',
I_404 VARCHAR(5) DEFAULT 'a', I_405 VARCHAR(5) DEFAULT 'a', 
I_406 VARCHAR(5) DEFAULT 'a',
I_407 VARCHAR(5) DEFAULT 'a', I_408 VARCHAR(5) DEFAULT 'a', 
I_409 VARCHAR(5) DEFAULT 'a',
I_410 VARCHAR(5) DEFAULT 'a', I_411 VARCHAR(5) DEFAULT 'a', 
I_412 VARCHAR(5) DEFAULT 'a',
I_413 VARCHAR(5) DEFAULT 'a', I_414 VARCHAR(5) DEFAULT 'a', 
I_415 VARCHAR(5) DEFAULT 'a',
I_416 VARCHAR(5) DEFAULT 'a', I_417 VARCHAR(5) DEFAULT 'a', 
I_418 VARCHAR(5) DEFAULT 'a',
I_419 VARCHAR(5) DEFAULT 'a', I_420 VARCHAR(5) DEFAULT 'a', 
I_421 VARCHAR(5) DEFAULT 'a',
I_422 VARCHAR(5) DEFAULT 'a', I_423 VARCHAR(5) DEFAULT 'a', 
I_424 VARCHAR(5) DEFAULT 'a',
I_425 VARCHAR(5) DEFAULT 'a', I_426 VARCHAR(5) DEFAULT 'a', 
I_427 VARCHAR(5) DEFAULT 'a',
I_428 VARCHAR(5) DEFAULT 'a', I_429 VARCHAR(5) DEFAULT 'a', 
I_430 VARCHAR(5) DEFAULT 'a',
I_431 VARCHAR(5) DEFAULT 'a', I_432 VARCHAR(5) DEFAULT 'a', 
I_433 VARCHAR(5) DEFAULT 'a',
I_434 VARCHAR(5) DEFAULT 'a', I_435 VARCHAR(5) DEFAULT 'a', 
I_436 VARCHAR(5) DEFAULT 'a',
I_437 VARCHAR(5) DEFAULT 'a', I_438 VARCHAR(5) DEFAULT 'a', 
I_439 VARCHAR(5) DEFAULT 'a',
I_440 VARCHAR(5) DEFAULT 'a', I_441 VARCHAR(5) DEFAULT 'a', 
I_442 VARCHAR(5) DEFAULT 'a',
I_443 VARCHAR(5) DEFAULT 'a', I_444 VARCHAR(5) DEFAULT 'a', 
I_445 VARCHAR(5) DEFAULT 'a',
I_446 VARCHAR(5), I_447 VARCHAR(5), I_448 VARCHAR(5),
 I_449 VARCHAR(5), I_450 VARCHAR(5),
I_451 VARCHAR(5), I_452 VARCHAR(5), I_453 VARCHAR(5), 
I_454 VARCHAR(5), I_455 VARCHAR(5),
I_456 VARCHAR(5), I_457 VARCHAR(5), I_458 VARCHAR(5), 
I_459 VARCHAR(5), I_460 VARCHAR(5),
I_461 VARCHAR(5), I_462 VARCHAR(5), I_463 VARCHAR(5), 
I_464 VARCHAR(5), I_465 VARCHAR(5),
I_466 VARCHAR(5), I_467 VARCHAR(5), I_468 VARCHAR(5), 
I_469 VARCHAR(5), I_470 VARCHAR(5),
I_471 VARCHAR(5), I_472 VARCHAR(5), I_473 VARCHAR(5), 
I_474 VARCHAR(5), I_475 VARCHAR(5),
I_476 VARCHAR(5), I_477 VARCHAR(5), I_478 VARCHAR(5), 
I_479 VARCHAR(5), I_480 VARCHAR(5),
I_481 VARCHAR(5), I_482 VARCHAR(5), I_483 VARCHAR(5), 
I_484 VARCHAR(5), I_485 VARCHAR(5),
I_486 VARCHAR(5), I_487 VARCHAR(5), I_488 VARCHAR(5), 
I_489 VARCHAR(5), I_490 VARCHAR(5),
I_491 VARCHAR(5), I_492 VARCHAR(5), I_493 VARCHAR(5), 
I_494 VARCHAR(5), I_495 VARCHAR(5),
I_496 VARCHAR(5), I_497 VARCHAR(5), I_498 VARCHAR(5), 
I_499 VARCHAR(5), I_500 VARCHAR(5)) !

INSERT TAB1 SET
K0 = 'primkey', K1 = 1, BI_1 = 'a', BI_2 = 'a' ,BI_3 = 'a' , BI_4 = 'a',
BI_5 = 'a' , BI_6 = 'a', BI_7 = 'a', BI_8 = 'a', BI_9 = 'a', 
I3 = 11, C2 = 'blabla' !

COMMIT WORK !

!* provoke overflow of dmli.d_joins
!* !!canceled because of dynamic dmli.d_joins!!


!* ok, let's full fill dmli.d_joins
parsing !
SELECT T1.K0, T1.K1, T1.I1, T1.I2, T1.C1,
       T1.C2, T1.I3, T1.CLLEN, T1.CF0, T1.CLB
FROM
TAB1  T1, TAB1  T2, TAB1  T3, TAB1  T4, TAB1  T5, TAB1  T6, TAB1  T7, 
TAB1  T8, TAB1  T9, TAB1 T10, TAB1 T11, TAB1 T12, TAB1 T13, TAB1 T14, 
TAB1 T15, TAB1 T16, TAB1 T17, TAB1 T18, TAB1 T19, TAB1 T20, TAB1 T21,
TAB1 T22, TAB1 T23, TAB1 T24, TAB1 T25, TAB1 T26, TAB1 T27, TAB1 T28, 
TAB1 T29, TAB1 T30, TAB1 T31, TAB1 T32, TAB1 T33, TAB1 T34, TAB1 T35,
TAB1 T36, TAB1 T37, TAB1 T38, TAB1 T39, TAB1 T40, TAB1 T41, TAB1 T42,
TAB1 T43, TAB1 T44, TAB1 T45, TAB1 T46, TAB1 T47, TAB1 T48, TAB1 T49,
TAB1 T50, TAB1 T51, TAB1 T52, TAB1 T53, TAB1 T54, TAB1 T55, TAB1 T56,
TAB1 T57, TAB1 T58, TAB1 T59, TAB1 T60, TAB1 T61, TAB1 T62, TAB1 T63,
TAB1 T64
WHERE
T2.BI_1  = T1.BI_1  AND T3.BI_1  = T2.BI_1  AND T4.BI_1  = T3.BI_1  AND 
T5.BI_1  = T4.BI_1  AND T6.BI_1  = T5.BI_1  AND T7.BI_1  = T6.BI_1  AND 
T8.BI_1  = T7.BI_1  AND T9.BI_1  = T8.BI_1  AND T10.BI_1 = T9.BI_1  AND 
T11.BI_1 = T10.BI_1 AND T12.BI_1 = T11.BI_1 AND T13.BI_1 = T12.BI_1 AND 
T14.BI_1 = T13.BI_1 AND T15.BI_1 = T14.BI_1 AND T16.BI_1 = T15.BI_1 AND 
T17.BI_1 = T16.BI_1 AND T18.BI_1 = T17.BI_1 AND T19.BI_1 = T18.BI_1 AND 
T20.BI_1 = T19.BI_1 AND T21.BI_1 = T20.BI_1 AND T22.BI_1 = T21.BI_1 AND 
T23.BI_1 = T22.BI_1 AND T24.BI_1 = T23.BI_1 AND T25.BI_1 = T24.BI_1 AND 
T26.BI_1 = T25.BI_1 AND T27.BI_1 = T26.BI_1 AND T28.BI_1 = T27.BI_1 AND 
T29.BI_1 = T28.BI_1 AND T30.BI_1 = T29.BI_1 AND T31.BI_1 = T30.BI_1 AND
T32.BI_1 = T31.BI_1 AND T33.BI_1 = T32.BI_1 AND T34.BI_1 = T33.BI_1 AND 
T35.BI_1 = T34.BI_1 AND T36.BI_1 = T35.BI_1 AND T37.BI_1 = T36.BI_1 AND 
T38.BI_1 = T37.BI_1 AND T39.BI_1 = T38.BI_1 AND T40.BI_1 = T39.BI_1 AND 
T41.BI_1 = T40.BI_1 AND T42.BI_1 = T41.BI_1 AND T43.BI_1 = T42.BI_1 AND 
T44.BI_1 = T43.BI_1 AND T45.BI_1 = T44.BI_1 AND T46.BI_1 = T45.BI_1 AND 
T47.BI_1 = T46.BI_1 AND T48.BI_1 = T47.BI_1 AND T49.BI_1 = T48.BI_1 AND 
T50.BI_1 = T49.BI_1 AND T51.BI_1 = T50.BI_1 AND T52.BI_1 = T51.BI_1 AND 
T53.BI_1 = T52.BI_1 AND T54.BI_1 = T53.BI_1 AND T55.BI_1 = T54.BI_1 AND 
T56.BI_1 = T55.BI_1 AND T57.BI_1 = T56.BI_1 AND T58.BI_1 = T57.BI_1 AND 
T59.BI_1 = T58.BI_1 AND T60.BI_1 = T59.BI_1 AND T61.BI_1 = T60.BI_1 AND
T60.BI_1 = T59.BI_1 AND T61.BI_1 = T60.BI_1 AND T62.BI_1 = T61.BI_1 AND
T61.BI_1 = T60.BI_1 AND T62.BI_1 = T61.BI_1 AND T63.BI_1 = T62.BI_1 AND
T62.BI_1 = T61.BI_1 AND T63.BI_1 = T62.BI_1 AND T64.BI_1 = T63.BI_1 AND
*
T2.BI_2  = T1.BI_2  AND T3.BI_2  = T2.BI_2  AND T4.BI_2  = T3.BI_2  AND 
T5.BI_2  = T4.BI_2  AND T6.BI_2  = T5.BI_2  AND T7.BI_2  = T6.BI_2  AND 
T8.BI_2  = T7.BI_2  AND T9.BI_2  = T8.BI_2  AND T10.BI_2 = T9.BI_2  AND 
T11.BI_2 = T10.BI_2 AND T12.BI_2 = T11.BI_2 AND T13.BI_2 = T12.BI_2 AND 
T14.BI_2 = T13.BI_2 AND T15.BI_2 = T14.BI_2 AND T16.BI_2 = T15.BI_2 AND 
T17.BI_2 = T16.BI_2 AND T18.BI_2 = T17.BI_2 AND T19.BI_2 = T18.BI_2 AND 
T20.BI_2 = T19.BI_2 AND T21.BI_2 = T20.BI_2 AND T22.BI_2 = T21.BI_2 AND 
T23.BI_2 = T22.BI_2 AND T24.BI_2 = T23.BI_2 AND T25.BI_2 = T24.BI_2 AND 
T26.BI_2 = T25.BI_2 AND T27.BI_2 = T26.BI_2 AND T28.BI_2 = T27.BI_2 AND 
T29.BI_2 = T28.BI_2 AND T30.BI_2 = T29.BI_2 AND T31.BI_2 = T30.BI_2 AND
T32.BI_2 = T31.BI_2 AND T33.BI_2 = T32.BI_2 AND T34.BI_2 = T33.BI_2 AND 
T35.BI_2 = T34.BI_2 AND T36.BI_2 = T35.BI_2 AND T37.BI_2 = T36.BI_2 AND 
T38.BI_2 = T37.BI_2 AND T39.BI_2 = T38.BI_2 AND T40.BI_2 = T39.BI_2 AND 
T41.BI_2 = T40.BI_2 AND T42.BI_2 = T41.BI_2 AND T43.BI_2 = T42.BI_2 AND 
T44.BI_2 = T43.BI_2 AND T45.BI_2 = T44.BI_2 AND T46.BI_2 = T45.BI_2 AND 
T47.BI_2 = T46.BI_2 AND T48.BI_2 = T47.BI_2 AND T49.BI_2 = T48.BI_2 AND 
T50.BI_2 = T49.BI_2 AND T51.BI_2 = T50.BI_2 AND T52.BI_2 = T51.BI_2 AND 
T53.BI_2 = T52.BI_2 AND T54.BI_2 = T53.BI_2 AND T55.BI_2 = T54.BI_2 AND 
T56.BI_2 = T55.BI_2 AND T57.BI_2 = T56.BI_2 AND T58.BI_2 = T57.BI_2 AND 
T59.BI_2 = T58.BI_2 AND T60.BI_2 = T59.BI_2 AND T61.BI_2 = T60.BI_2 AND
T60.BI_2 = T59.BI_2 AND T61.BI_2 = T60.BI_2 AND T62.BI_2 = T61.BI_2 AND
T61.BI_2 = T60.BI_2 AND T62.BI_2 = T61.BI_2 AND T63.BI_2 = T62.BI_2 AND
T62.BI_2 = T61.BI_2 AND T63.BI_2 = T62.BI_2 AND T64.BI_2 = T63.BI_2 AND
*
T2.BI_3  = T1.BI_3  AND T3.BI_3  = T2.BI_3  AND T4.BI_3  = T3.BI_3  AND 
T5.BI_3  = T4.BI_3  AND T6.BI_3  = T5.BI_3  AND T7.BI_3  = T6.BI_3  AND 
T8.BI_3  = T7.BI_3  AND T9.BI_3  = T8.BI_3  AND T10.BI_3 = T9.BI_3  AND 
T11.BI_3 = T10.BI_3 AND T12.BI_3 = T11.BI_3 AND T13.BI_3 = T12.BI_3 AND 
T14.BI_3 = T13.BI_3 AND T15.BI_3 = T14.BI_3 AND T16.BI_3 = T15.BI_3 AND 
T17.BI_3 = T16.BI_3 AND T18.BI_3 = T17.BI_3 AND T19.BI_3 = T18.BI_3 AND 
T20.BI_3 = T19.BI_3 AND T21.BI_3 = T20.BI_3 AND T22.BI_3 = T21.BI_3 AND 
T23.BI_3 = T22.BI_3 AND T24.BI_3 = T23.BI_3 AND T25.BI_3 = T24.BI_3 AND 
T26.BI_3 = T25.BI_3 AND T27.BI_3 = T26.BI_3 AND T28.BI_3 = T27.BI_3 AND 
T29.BI_3 = T28.BI_3 AND T30.BI_3 = T29.BI_3 AND T31.BI_3 = T30.BI_3 AND
T32.BI_3 = T31.BI_3 AND T33.BI_3 = T32.BI_3 AND T34.BI_3 = T33.BI_3 AND 
T35.BI_3 = T34.BI_3 AND T36.BI_3 = T35.BI_3 AND T37.BI_3 = T36.BI_3 AND 
T38.BI_3 = T37.BI_3 AND T39.BI_3 = T38.BI_3 AND T40.BI_3 = T39.BI_3 AND 
T41.BI_3 = T40.BI_3 AND T42.BI_3 = T41.BI_3 AND T43.BI_3 = T42.BI_3 AND 
T44.BI_3 = T43.BI_3 AND T45.BI_3 = T44.BI_3 AND T46.BI_3 = T45.BI_3 AND 
T47.BI_3 = T46.BI_3 AND T48.BI_3 = T47.BI_3 AND T49.BI_3 = T48.BI_3 AND 
T50.BI_3 = T49.BI_3 AND T51.BI_3 = T50.BI_3 AND T52.BI_3 = T51.BI_3 AND 
T53.BI_3 = T52.BI_3 AND T54.BI_3 = T53.BI_3 AND T55.BI_3 = T54.BI_3 AND 
T56.BI_3 = T55.BI_3 AND T57.BI_3 = T56.BI_3 AND T58.BI_3 = T57.BI_3 AND 
T59.BI_3 = T58.BI_3 AND T60.BI_3 = T59.BI_3 AND T61.BI_3 = T60.BI_3 AND
T60.BI_3 = T59.BI_3 AND T61.BI_3 = T60.BI_3 AND T62.BI_3 = T61.BI_3 AND
T61.BI_3 = T60.BI_3 AND T62.BI_3 = T61.BI_3 AND T63.BI_3 = T62.BI_3 AND
T62.BI_3 = T61.BI_3 AND T63.BI_3 = T62.BI_3 AND T64.BI_3 = T63.BI_3 AND
*
T2.BI_4  = T1.BI_4  AND T3.BI_4  = T2.BI_4  AND T4.BI_4  = T3.BI_4  AND 
T5.BI_4  = T4.BI_4  AND T6.BI_4  = T5.BI_4  AND T7.BI_4  = T6.BI_4  AND 
T8.BI_4  = T7.BI_4  AND T9.BI_4  = T8.BI_4  AND T10.BI_4 = T9.BI_4  AND 
T11.BI_4 = T10.BI_4 AND T12.BI_4 = T11.BI_4 AND T13.BI_4 = T12.BI_4 AND 
T14.BI_4 = T13.BI_4 AND T15.BI_4 = T14.BI_4 AND T16.BI_4 = T15.BI_4 AND 
T17.BI_4 = T16.BI_4 AND T18.BI_4 = T17.BI_4 AND T19.BI_4 = T18.BI_4 AND 
T20.BI_4 = T19.BI_4 AND T21.BI_4 = T20.BI_4 AND T22.BI_4 = T21.BI_4 AND 
T23.BI_4 = T22.BI_4 AND T24.BI_4 = T23.BI_4 AND T25.BI_4 = T24.BI_4 AND 
T26.BI_4 = T25.BI_4 AND T27.BI_4 = T26.BI_4 AND T28.BI_4 = T27.BI_4 AND 
T29.BI_4 = T28.BI_4 AND T30.BI_4 = T29.BI_4 AND T31.BI_4 = T30.BI_4 AND
T32.BI_4 = T31.BI_4 AND T33.BI_4 = T32.BI_4 AND T34.BI_4 = T33.BI_4 AND 
T35.BI_4 = T34.BI_4 AND T36.BI_4 = T35.BI_4 AND T37.BI_4 = T36.BI_4 AND 
T38.BI_4 = T37.BI_4 AND T39.BI_4 = T38.BI_4 AND T40.BI_4 = T39.BI_4 AND 
T41.BI_4 = T40.BI_4 AND T42.BI_4 = T41.BI_4 AND T43.BI_4 = T42.BI_4 AND 
T44.BI_4 = T43.BI_4 AND T45.BI_4 = T44.BI_4 AND T46.BI_4 = T45.BI_4 AND 
T47.BI_4 = T46.BI_4 AND T48.BI_4 = T47.BI_4 AND T49.BI_4 = T48.BI_4 AND 
T50.BI_4 = T49.BI_4 
!

executing !
select !
1

parsing !
CREATE VIEW VIEW1 AS
SELECT T1.K0, T1.K1, T1.I1, T1.I2, T1.C1,
       T1.C2, T1.I3, T1.CLLEN, T1.CF0, T1.CLB
FROM
TAB1  T1, TAB1  T2, TAB1  T3, TAB1  T4, TAB1  T5, TAB1  T6, TAB1  T7, 
TAB1  T8, TAB1  T9, TAB1 T10, TAB1 T11, TAB1 T12, TAB1 T13, TAB1 T14, 
TAB1 T15, TAB1 T16, TAB1 T17, TAB1 T18, TAB1 T19, TAB1 T20, TAB1 T21,
TAB1 T22, TAB1 T23, TAB1 T24, TAB1 T25, TAB1 T26, TAB1 T27, TAB1 T28, 
TAB1 T29, TAB1 T30, TAB1 T31, TAB1 T32, TAB1 T33, TAB1 T34, TAB1 T35,
TAB1 T36, TAB1 T37, TAB1 T38, TAB1 T39, TAB1 T40, TAB1 T41, TAB1 T42,
TAB1 T43, TAB1 T44, TAB1 T45, TAB1 T46, TAB1 T47, TAB1 T48, TAB1 T49,
TAB1 T50, TAB1 T51, TAB1 T52, TAB1 T53, TAB1 T54, TAB1 T55, TAB1 T56,
TAB1 T57, TAB1 T58, TAB1 T59, TAB1 T60, TAB1 T61, TAB1 T62, TAB1 T63,
TAB1 T64
WHERE
T2.BI_1  = T1.BI_1  AND T3.BI_1  = T2.BI_1  AND T4.BI_1  = T3.BI_1  AND 
T5.BI_1  = T4.BI_1  AND T6.BI_1  = T5.BI_1  AND T7.BI_1  = T6.BI_1  AND 
T8.BI_1  = T7.BI_1  AND T9.BI_1  = T8.BI_1  AND T10.BI_1 = T9.BI_1  AND 
T11.BI_1 = T10.BI_1 AND T12.BI_1 = T11.BI_1 AND T13.BI_1 = T12.BI_1 AND 
T14.BI_1 = T13.BI_1 AND T15.BI_1 = T14.BI_1 AND T16.BI_1 = T15.BI_1 AND 
T17.BI_1 = T16.BI_1 AND T18.BI_1 = T17.BI_1 AND T19.BI_1 = T18.BI_1 AND 
T20.BI_1 = T19.BI_1 AND T21.BI_1 = T20.BI_1 AND T22.BI_1 = T21.BI_1 AND 
T23.BI_1 = T22.BI_1 AND T24.BI_1 = T23.BI_1 AND T25.BI_1 = T24.BI_1 AND 
T26.BI_1 = T25.BI_1 AND T27.BI_1 = T26.BI_1 AND T28.BI_1 = T27.BI_1 AND 
T29.BI_1 = T28.BI_1 AND T30.BI_1 = T29.BI_1 AND T31.BI_1 = T30.BI_1 AND
T32.BI_1 = T31.BI_1 AND T33.BI_1 = T32.BI_1 AND T34.BI_1 = T33.BI_1 AND 
T35.BI_1 = T34.BI_1 AND T36.BI_1 = T35.BI_1 AND T37.BI_1 = T36.BI_1 AND 
T38.BI_1 = T37.BI_1 AND T39.BI_1 = T38.BI_1 AND T40.BI_1 = T39.BI_1 AND 
T41.BI_1 = T40.BI_1 AND T42.BI_1 = T41.BI_1 AND T43.BI_1 = T42.BI_1 AND 
T44.BI_1 = T43.BI_1 AND T45.BI_1 = T44.BI_1 AND T46.BI_1 = T45.BI_1 AND 
T47.BI_1 = T46.BI_1 AND T48.BI_1 = T47.BI_1 AND T49.BI_1 = T48.BI_1 AND 
T50.BI_1 = T49.BI_1 AND T51.BI_1 = T50.BI_1 AND T52.BI_1 = T51.BI_1 AND 
T53.BI_1 = T52.BI_1 AND T54.BI_1 = T53.BI_1 AND T55.BI_1 = T54.BI_1 AND 
T56.BI_1 = T55.BI_1 AND T57.BI_1 = T56.BI_1 AND T58.BI_1 = T57.BI_1 AND 
T59.BI_1 = T58.BI_1 AND T60.BI_1 = T59.BI_1 AND T61.BI_1 = T60.BI_1 AND
T60.BI_1 = T59.BI_1 AND T61.BI_1 = T60.BI_1 AND T62.BI_1 = T61.BI_1 AND
T61.BI_1 = T60.BI_1 AND T62.BI_1 = T61.BI_1 AND T63.BI_1 = T62.BI_1 AND
T62.BI_1 = T61.BI_1 AND T63.BI_1 = T62.BI_1 AND T64.BI_1 = T63.BI_1 AND
*
T2.BI_2  = T1.BI_2  AND T3.BI_2  = T2.BI_2  AND T4.BI_2  = T3.BI_2  AND 
T5.BI_2  = T4.BI_2  AND T6.BI_2  = T5.BI_2  AND T7.BI_2  = T6.BI_2  AND 
T8.BI_2  = T7.BI_2  AND T9.BI_2  = T8.BI_2  AND T10.BI_2 = T9.BI_2  AND 
T11.BI_2 = T10.BI_2 AND T12.BI_2 = T11.BI_2 AND T13.BI_2 = T12.BI_2 AND 
T14.BI_2 = T13.BI_2 AND T15.BI_2 = T14.BI_2 AND T16.BI_2 = T15.BI_2 AND 
T17.BI_2 = T16.BI_2 AND T18.BI_2 = T17.BI_2 AND T19.BI_2 = T18.BI_2 AND 
T20.BI_2 = T19.BI_2 AND T21.BI_2 = T20.BI_2 AND T22.BI_2 = T21.BI_2 AND 
T23.BI_2 = T22.BI_2 AND T24.BI_2 = T23.BI_2 AND T25.BI_2 = T24.BI_2 AND 
T26.BI_2 = T25.BI_2 AND T27.BI_2 = T26.BI_2 AND T28.BI_2 = T27.BI_2 AND 
T29.BI_2 = T28.BI_2 AND T30.BI_2 = T29.BI_2 AND T31.BI_2 = T30.BI_2 AND
T32.BI_2 = T31.BI_2 AND T33.BI_2 = T32.BI_2 AND T34.BI_2 = T33.BI_2 AND 
T35.BI_2 = T34.BI_2 AND T36.BI_2 = T35.BI_2 AND T37.BI_2 = T36.BI_2 AND 
T38.BI_2 = T37.BI_2 AND T39.BI_2 = T38.BI_2 AND T40.BI_2 = T39.BI_2 AND 
T41.BI_2 = T40.BI_2 AND T42.BI_2 = T41.BI_2 AND T43.BI_2 = T42.BI_2 AND 
T44.BI_2 = T43.BI_2 AND T45.BI_2 = T44.BI_2 AND T46.BI_2 = T45.BI_2 AND 
T47.BI_2 = T46.BI_2 AND T48.BI_2 = T47.BI_2 AND T49.BI_2 = T48.BI_2 AND 
T50.BI_2 = T49.BI_2 AND T51.BI_2 = T50.BI_2 AND T52.BI_2 = T51.BI_2 AND 
T53.BI_2 = T52.BI_2 AND T54.BI_2 = T53.BI_2 AND T55.BI_2 = T54.BI_2 AND 
T56.BI_2 = T55.BI_2 AND T57.BI_2 = T56.BI_2 AND T58.BI_2 = T57.BI_2 AND 
T59.BI_2 = T58.BI_2 AND T60.BI_2 = T59.BI_2 AND T61.BI_2 = T60.BI_2 AND
T60.BI_2 = T59.BI_2 AND T61.BI_2 = T60.BI_2 AND T62.BI_2 = T61.BI_2 AND
T61.BI_2 = T60.BI_2 AND T62.BI_2 = T61.BI_2 AND T63.BI_2 = T62.BI_2 AND
T62.BI_2 = T61.BI_2 AND T63.BI_2 = T62.BI_2 AND T64.BI_2 = T63.BI_2 AND
*
T2.BI_3  = T1.BI_3  AND T3.BI_3  = T2.BI_3  AND T4.BI_3  = T3.BI_3  AND 
T5.BI_3  = T4.BI_3  AND T6.BI_3  = T5.BI_3  AND T7.BI_3  = T6.BI_3  AND 
T8.BI_3  = T7.BI_3  AND T9.BI_3  = T8.BI_3  AND T10.BI_3 = T9.BI_3  AND 
T11.BI_3 = T10.BI_3 AND T12.BI_3 = T11.BI_3 AND T13.BI_3 = T12.BI_3 AND 
T14.BI_3 = T13.BI_3 AND T15.BI_3 = T14.BI_3 AND T16.BI_3 = T15.BI_3 AND 
T17.BI_3 = T16.BI_3 AND T18.BI_3 = T17.BI_3 AND T19.BI_3 = T18.BI_3 AND 
T20.BI_3 = T19.BI_3 AND T21.BI_3 = T20.BI_3 AND T22.BI_3 = T21.BI_3 AND 
T23.BI_3 = T22.BI_3 AND T24.BI_3 = T23.BI_3 AND T25.BI_3 = T24.BI_3 AND 
T26.BI_3 = T25.BI_3 AND T27.BI_3 = T26.BI_3 AND T28.BI_3 = T27.BI_3 AND 
T29.BI_3 = T28.BI_3 AND T30.BI_3 = T29.BI_3 AND T31.BI_3 = T30.BI_3 AND
T32.BI_3 = T31.BI_3 AND T33.BI_3 = T32.BI_3 AND T34.BI_3 = T33.BI_3 AND 
T35.BI_3 = T34.BI_3 AND T36.BI_3 = T35.BI_3 AND T37.BI_3 = T36.BI_3 AND 
T38.BI_3 = T37.BI_3 AND T39.BI_3 = T38.BI_3 AND T40.BI_3 = T39.BI_3 AND 
T41.BI_3 = T40.BI_3 AND T42.BI_3 = T41.BI_3 AND T43.BI_3 = T42.BI_3 AND 
T44.BI_3 = T43.BI_3 AND T45.BI_3 = T44.BI_3 AND T46.BI_3 = T45.BI_3 AND 
T47.BI_3 = T46.BI_3 AND T48.BI_3 = T47.BI_3 AND T49.BI_3 = T48.BI_3 AND 
T50.BI_3 = T49.BI_3 AND T51.BI_3 = T50.BI_3 AND T52.BI_3 = T51.BI_3 AND 
T53.BI_3 = T52.BI_3 AND T54.BI_3 = T53.BI_3 AND T55.BI_3 = T54.BI_3 AND 
T56.BI_3 = T55.BI_3 AND T57.BI_3 = T56.BI_3 AND T58.BI_3 = T57.BI_3 AND 
T59.BI_3 = T58.BI_3 AND T60.BI_3 = T59.BI_3 AND T61.BI_3 = T60.BI_3 AND
T60.BI_3 = T59.BI_3 AND T61.BI_3 = T60.BI_3 AND T62.BI_3 = T61.BI_3 AND
T61.BI_3 = T60.BI_3 AND T62.BI_3 = T61.BI_3 AND T63.BI_3 = T62.BI_3 AND
T62.BI_3 = T61.BI_3 AND T63.BI_3 = T62.BI_3 AND T64.BI_3 = T63.BI_3 AND
*
T2.BI_4  = T1.BI_4  AND T3.BI_4  = T2.BI_4  AND T4.BI_4  = T3.BI_4  AND 
T5.BI_4  = T4.BI_4  AND T6.BI_4  = T5.BI_4  AND T7.BI_4  = T6.BI_4  AND 
T8.BI_4  = T7.BI_4  AND T9.BI_4  = T8.BI_4  AND T10.BI_4 = T9.BI_4  AND 
T11.BI_4 = T10.BI_4 AND T12.BI_4 = T11.BI_4 AND T13.BI_4 = T12.BI_4 AND 
T14.BI_4 = T13.BI_4 AND T15.BI_4 = T14.BI_4 AND T16.BI_4 = T15.BI_4 AND 
T17.BI_4 = T16.BI_4 AND T18.BI_4 = T17.BI_4 AND T19.BI_4 = T18.BI_4 AND 
T20.BI_4 = T19.BI_4 AND T21.BI_4 = T20.BI_4 AND T22.BI_4 = T21.BI_4 AND 
T23.BI_4 = T22.BI_4 AND T24.BI_4 = T23.BI_4 AND T25.BI_4 = T24.BI_4 AND 
T26.BI_4 = T25.BI_4 AND T27.BI_4 = T26.BI_4 AND T28.BI_4 = T27.BI_4 AND 
T29.BI_4 = T28.BI_4 AND T30.BI_4 = T29.BI_4 AND T31.BI_4 = T30.BI_4 AND
T32.BI_4 = T31.BI_4 AND T33.BI_4 = T32.BI_4 AND T34.BI_4 = T33.BI_4 AND 
T35.BI_4 = T34.BI_4 AND T36.BI_4 = T35.BI_4 AND T37.BI_4 = T36.BI_4 AND 
T38.BI_4 = T37.BI_4 AND T39.BI_4 = T38.BI_4 AND T40.BI_4 = T39.BI_4 AND 
T41.BI_4 = T40.BI_4 AND T42.BI_4 = T41.BI_4 AND T43.BI_4 = T42.BI_4 AND 
T44.BI_4 = T43.BI_4 AND T45.BI_4 = T44.BI_4 AND T46.BI_4 = T45.BI_4 AND 
T47.BI_4 = T46.BI_4 AND T48.BI_4 = T47.BI_4 AND T49.BI_4 = T48.BI_4 AND 
T50.BI_4 = T49.BI_4 
!

executing !
select !
1

parsing !
select * from view1 !
executing !
select !
1

pars_execute !
commit release!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN2TAB.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOIN2TAB VDNTEST  changed on 1992-09-28-13.21.39 by ADMIN     *
*
*     Die groessere DB ist noetig, weil bei ISOLATION LEVEL 3 sonst
*     kein Platz mehr fuer die RESULT TALBLE ist.
*
file connect ( kern test !
create table t1 (
       s1         char(219),
       s2         char(229),
       s3         char(239),
       s4         char(149),
       s5         char(159),
       s6         char(69),
       s7         fixed(10),
       s8         fixed(10),
       s9         fixed(10),
       s10        char(235),
       sec        char(06),
       sen        fixed(04)
       ) !
insert t1 set
                  s1 = 't1___s1 nr 1',
                  s2 = 't1___s2 nr 1',
                  s3 = 't1___s3 nr 1',
                  s4 = 't1___s4 nr 1',
                  s5 = 't1___s5 nr 1',
                  s6 = 't1___s6 nr 1',
                  s7 = 1,
                  s8 = 100,
                  sen = 4711,
                  sec = 'ABCDEF',
                  s9 = 999
                  !
insert t1 set
                  s1 = 't1___s1 nr 2',
                  s2 = 't1___s2 nr 2',
                  s3 = 't1___s3 nr 2',
                  s4 = 't1___s4 nr 2',
                  s5 = 't1___s5 nr 2',
                  s6 = 't1___s6 nr 2',
                  s7 = 2,
                  s8 = 10,
                  sen = 4711,
                  sec = 'ABCDEF',
                  s9 = 999
                  !
create table t2 (
       s1         char(19),
       s2         char(29),
       s3         char(39),
       s4         char(49),
       s5         char(59),
       s6         char(69),
       s7         fixed(10),
       s10        char(235),
       sec        char(21),
       sen        fixed(08)
       ) !
insert t2 set
                  s1 = 't2___s1 nr 1',
                  s2 = 't2___s2 nr 1',
                  s3 = 't2___s3 nr 1',
                  s4 = 't2___s4 nr 1',
                  s5 = 't2___s5 nr 1',
                  s6 = 't2___s6 nr 1',
                  sen = 4711,
                  sec = 'ABCDEF               ',
                  s7 = 11111
                  !
insert t2 set
                  s1 = 't2___s1 nr 2',
                  s2 = 't2___s2 nr 2',
                  s3 = 't2___s3 nr 2',
                  s4 = 't2___s4 nr 2',
                  s5 = 't2___s5 nr 2',
                  s6 = 't2___s6 nr 2',
                  sen = 47110,
                  sec = 'ABCDEF              x',
                  s7 = 44444
                  !
commit work !
 select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
 , t2.s7, t1.s7
 , t1.s8
 from <t1, t2>
 order by t1.s7
!
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s7 <>  t2.s7
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s7 <>  t2.s7
  order by t1.s7
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s9 <>  t2.s7
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s9 <>  t2.s7
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s7 <>  t2.s7
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
 !
fetch !
  select t1.*, t2.*
  from <t1, t2>
  where t1.s7 <>  t2.s7
 !
fetch !
  select t1.*, t2.*
  from <t1, t2>
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where rowno <= 3
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where rowno <= 3
  and   t1.s7 <>  t2.s7 * 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7 * 3
  , t1.s8
  from <t1, t2>
  where rowno <= 3
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s7 <>  t2.s7 * 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7 * 3
  , t1.s8
  from <t1, t2>
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s1 <> t2.s1
  and   t1.s2 <> t2.s2
  and   t1.s3 <> t2.s3
  and   t1.s4 <> t2.s4
  and   t1.s5 <> t2.s5
  and   t1.s6 <> t2.s6
  and   t1.s7 <> t2.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t1.s7, t1.s7
  , t2.s6, t1.s7
  from <t1, t2>
  where rowno <= 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t1.s7, t1.s7
  , t2.s6, t1.s7
  from <t1, t2>
 !
fetch !
  select t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s1 <> t2.s1
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s7
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
 !
fetch !
  select t1.*, t2.*
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
 !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sen + 3 = 3 + t2.sen
  order by t1.s7
  desc
 !
fetch !
  select rowno, t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t1, t2>
  where t1.sen = t2.sen
  order by t1.s7
  desc
 !
fetch !
********* eine Tabelle enthaelt keine Ausgabefelder **********
  select t1.s2, t1.s3, t1.s4, t1.s5, t1.s6
  , t1.s7
  , t1.s8
  from <t1, t2>
  where t1.s1 <> t2.s1
 !
fetch !
**  andere Tabellenreihenfolge *****************************************
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s7 <>  t2.s7
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s7 <>  t2.s7
  order by t1.s7
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s9 <>  t2.s7
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s9 <>  t2.s7
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s7 <>  t2.s7
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
 !
fetch !
  select t1.*, t2.*
  from <t2, t1>
  where t1.s7 <>  t2.s7
 !
fetch !
  select t1.*, t2.*
  from <t2, t1>
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where rowno <= 3
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where rowno <= 3
  and   t1.s7 <>  t2.s7
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where rowno <= 3
  and   t1.s7 <>  t2.s7 * 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7 * 3
  , t1.s8
  from <t2, t1>
  where rowno <= 3
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s7 <>  t2.s7 * 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7 * 3
  , t1.s8
  from <t2, t1>
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s1 <> t2.s1
  and   t1.s2 <> t2.s2
  and   t1.s3 <> t2.s3
  and   t1.s4 <> t2.s4
  and   t1.s5 <> t2.s5
  and   t1.s6 <> t2.s6
  and   t1.s7 <> t2.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t1.s7, t1.s7
  , t2.s6, t1.s7
  from <t2, t1>
  where rowno <= 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t1.s7, t1.s7
  , t2.s6, t1.s7
  from <t2, t1>
 !
fetch !
  select t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s1 <> t2.s1
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s7
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s9
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
  order by t1.s9
  desc
 !
fetch !
  select distinct t1.s9, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
 !
fetch !
  select t1.*, t2.*
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
 !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
  order by t1.s7
  desc
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sec = t2.sec
    and t2.sen = t1.sen
    and rowno <= 3
  order by t1.s7
 !
fetch !
  select t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sen + 3 = 3 + t2.sen
  order by t1.s7
  desc
 !
fetch !
  select rowno, t1.s1, t2.s2, t2.s3, t1.s4, t1.s5, t2.s6
  , t2.s7, t1.s7
  , t1.s8
  from <t2, t1>
  where t1.sen = t2.sen
  order by t1.s7
  desc
 !
fetch !
********* eine Tabelle enthaelt keine Ausgabefelder **********
  select t1.s2, t1.s3, t1.s4, t1.s5, t1.s6
  , t1.s7
  , t1.s8
  from <t2, t1>
  where t1.s1 <> t2.s1
 !
fetch !
rollback work release !
file connect ( kern test !
CREATE TABLE TAR (OBJEKT_ID CHAR (24) ASCII,
"F2" DATE      NOT NULL,
"F1" FIXED     (2,0), PRIMARY KEY (OBJEKT_ID))!
insert tar values ('1. Objekt', '19941224', 1)!
insert tar values ('2. Objekt', '20010909', 2)!
insert tar values ('3. Objekt', '19600413', null)!
CREATE TABLE ANG (OBJEKT_ID CHAR(24) ASCII,
"F2" DATE      NOT NULL,
"F1" FIXED     (2,0), PRIMARY KEY (OBJEKT_ID))!
insert ang values ('1. Objekt', '19941224', 1)!
insert ang values ('2. Objekt', '19921206', 2)!
insert ang values ('3. Objekt', '19600413', null)!
insert ang values ('4. Objekt', '19540626', 4)!
CREATE VIEW V13 AS SELECT ANG.F1 TARIF ,  
                          TAR.F2  GUELT_VON
                   FROM TAR, ANG 
                   WHERE (TAR.F2 < DATE) AND 
                         ((TAR.F1 = ANG.F1) OR ((TAR.F1 IS NULL))) !
select * from v13
where rowno < 2 !
fetch !
select * from v13
where rowno < 7 !
fetch !
rollback release !
file connect ( kern test !

*** PTS 1118054 1118055 ***
create table tab1(col1 int, col2 int, col3 varchar(10))!
create table tab2(col1 int, col2 int, col3 varchar(10))!

insert into tab1 values (1, 1, 'sdaxg')!
insert into tab1 values (1, 1, 'xbg')!
insert into tab1 values (1, 1, '_g')!
insert into tab2 values (1, 1, '%a%')!
insert into tab2 values (1, 1, 'x_g')!


select col3 from tab1 where col3 like '%a%'!
fetch!
select col3 from tab1 where col3 like 'x_g'!
fetch!
select col3 from tab1 where col3 like 'x_g' escape 'x'!
fetch!
select tab1.col3, tab2.col3 as pattern from <tab1, tab2> where
tab1.col3 like tab2.col3 !
fetch!
select tab1.col3, tab2.col3 as pattern from <tab2, tab1> where
tab1.col3 like tab2.col3 !
fetch!
** these join query will be processed as 
** one phase join (cross product and while writing we do qualification)
** normal joining not possible because we don't know about 
** "escape 'x'" while joining
select tab1.col3, tab2.col3 as pattern from <tab1, tab2> where
tab1.col3 like tab2.col3 escape 'x'!
fetch!
select tab1.col3, tab2.col3 as pattern from <tab2, tab1> where
tab1.col3 like tab2.col3 escape 'x'!
fetch!

* insert wrong LIKE pattern
insert into tab2 values (1, 1, 'ab(-')!
select tab1.col3, tab2.col3 from <tab2, tab1> where
tab1.col3 like tab2.col3 !

rollback !
***************************
create table tab1(col1 int, col2 int, col3 int)!
create table tab2(col1 int, col2 int, col3 int)!

insert into tab1 values (1, 1, 1)!
insert into tab1 values (1, 1, 2)!
insert into tab1 values (1, 1, 3)!
insert into tab2 values (1, 1, 1)!
insert into tab2 values (1, 1, 2)!

select tab1.col3 as T1C3, tab2.col3 as T2C3 from <tab1, tab2> where
tab1.col3 < tab2.col3 !
fetch!
select tab1.col3 as T1C3, tab2.col3 as T2C3 from <tab2, tab1> where
tab1.col3 < tab2.col3 !
fetch!
select tab1.col3 as T1C3, tab2.col3 as T2C3 from <tab1, tab2> where
tab1.col3 <= tab2.col3 !
fetch!
select tab1.col3 as T1C3, tab2.col3 as T2C3 from <tab2, tab1> where
tab1.col3 <= tab2.col3 !
fetch!
rollback release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN3_C00.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* C00JOIN3 VDNTEST  changed on 1992-09-28-13.23.09 by ADMIN     *
 * VON BATU, INFO-CENTER # 306 !
file connect ( kern test !
*
CREATE TABLE t01 (s01 CHAR(2) KEY, s02 CHAR(4) KEY, s03 FIXED(5) KEY)!
CREATE TABLE t02
   (s01 CHAR(30) KEY, s02 CHAR(2) KEY, s03 CHAR(4) KEY, s04 FIXED(5))!
CREATE TABLE t03 (s01 FIXED(5) KEY, s02 CHAR(2) KEY)!
COMMIT!
*
INSERT t01 VALUES('30','2000',20)!
INSERT t02 VALUES('RUDI','30','2000',20)!
INSERT t03 VALUES(20,'XX')!
COMMIT!
update statistics kern.* !
*
EXPLAIN SELECT * FROM t01, t02, t03
WHERE  t02.s02 = t01.s01
AND    t02.s03 = t01.s02
AND    t02.s04 = t01.s03
AND    t01.s03 = t03.s01!
*
SELECT * FROM t01, t02, t03
WHERE  t02.s02 = t01.s01
AND    t02.s03 = t01.s02
AND    t02.s04 = t01.s03
AND    t01.s03 = t03.s01!
fetch !
*
EXPLAIN SELECT * FROM t01, t02, t03
WHERE  t02.s02 = t01.s01
AND    t02.s03 = t01.s02
AND    t02.s04 = t01.s03
AND    t02.s04 = t03.s01!
*
SELECT * FROM t01, t02, t03
WHERE  t02.s02 = t01.s01
AND    t02.s03 = t01.s02
AND    t02.s04 = t01.s03
AND    t02.s04 = t03.s01!
*
FETCH!
*
drop table t01 !
drop table t02 !
drop table t03 !

commit work release!
file connect ( kern test !
create table t1 ( a fixed(5), 
                  a1 char(4) default 'a1', 
                  a2 char(4) default 'a2', 
                  d date default date, 
                  y char(2) default '95' )!
insert t1 set a = 1 !
insert t1 set a = 2 !
insert t1 set a = 3 !
insert t1 set a = 4 !
create table t2 ( b fixed(5), 
                  b1 char(4) default 'b1', 
                  d date default date, 
                  y char(4) default '2006' )!
insert t2 set b = 3 !
insert t2 set b = 4 !
create table t3 ( c fixed(5), 
                  c1 char(4) default 'c1', 
                  d date default date, 
                  y char(4) default '2006' )!
insert t3 set c = 3 !
insert t3 set c = 4 !
commit work !
select a, b, 1 from <t1, t2, t3> 
where a = b 
      and substr(t1.d, 1, 4) = t2.y 
      and substr(t1.d, 1, 4) = t3.y 
      and t1.d between '19990101' and '20061231'
      and t2.b1 = 'b1'
      and t1.a1 = 'a1' and t1.a2 = 'a2' 
      !
fetch !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN3OUTER.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* OJTEST   VDNTEST  changed on 1992-09-28-13.27.35 by ADMIN     *
file connect ( kern test !
create table tab1 ( a fixed(6) key, b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3sy !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6) key, d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6) key, b char(3) key )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6) key, b char(3) )!
create table tab2 ( c fixed(6) key, d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6) key, b char(3) key )!
create table tab2 ( c fixed(6) key, d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6) key, b char(3) key )!
create table tab2 ( c fixed(6) key, d char(3) key )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index "tab1.a"
	on tab1( a ) !
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index "tab2.c"
	on tab2( c ) !
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index "tab1.a"
	on tab1( a ) !
create index "tab2.c"
	on tab2( c ) !
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index i1 on tab1 (a, b) !
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index i2 on tab2 (c, d) !
commit work release !
file outerj3is !
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
create index i1 on tab1 (a, b) !
create index i2 on tab2 (c, d) !
commit work release !
file outerj3is !
file outerj3dw !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN3TB1.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOIN3TB1 VDNTEST  changed on 1992-09-28-13.24.15 by ADMIN     *
file connect ( kern test !
file jn3init !
********************** <T1, T2, T3> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T2, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T1, T2, T3>
where rowno <= 7
!
fetch !
********************** <T1, T3, T2> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T1, T3, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T1, T3, T2>
where rowno <= 7
!
fetch !
rollback work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN3TB2.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOIN3TB2 VDNTEST  changed on 1992-09-28-13.24.15 by ADMIN     *
file connect ( kern test !
file jn3init !
********************** <T2, T1, T3> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T1, T3>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T2, T1, T3>
where rowno <= 7
!
fetch !
********************** <T2, T3, T1> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T2, T3, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T2, T3, T1>
where rowno <= 7
!
fetch !
rollback work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOIN3TB3.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOIN3TB3 VDNTEST  changed on 1992-09-28-13.24.16 by ADMIN     *
file connect ( kern test !
file jn3init !
********************** <T3, T1, T2> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T1, T2>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T3, T1, T2>
where rowno <= 7
!
fetch !
********************** <T3, T2, T1> *************************************
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
ORDER BY T1.S7 desc
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7
!
fetch !
SELECT rowno,
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.s7 <> t1.s7
and  t3.s7 <> t2.s7
and  t1.s7 <> t3.s7
and  t1.s7 <> t2.s7
and  t2.s7 <> t3.s7
and  t3.s7 <> t1.s7
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT distinct
T1.S1, T2.S2, T2.S3
, T3.S1, T3.S5
, T1.S4, T1.S5, T2.S6
, T2.S7
, T3.S2
, T1.S7
 * 3
, T1.S8
, T1.S7
, T3.S8
FROM <T3, T2, T1>
where t2.sen = t1.sen
and  t3.sen = t2.sen
and  t1.sen = t3.sen
and  t1.sen = t2.sen
and  t2.sen = t3.sen
and  t3.sen = t1.sen
and  t2.sec = t1.sec
and  t3.sec = t2.sec
and  t1.sec = t3.sec
and  t1.sec = t2.sec
and  t2.sec = t3.sec
and  t3.sec = t1.sec
ORDER BY T1.S7, T3.S8, T2.S7
!
fetch !
SELECT rowno, *
FROM <T3, T2, T1>
where rowno <= 7
!
fetch !
rollback work release !
file connect ( kern test !

* PTS 1117901
Create Table tab1 (tab1id integer)!
Create Table tab2 (tab1id integer,tab2id integer ,tab3id integer )!
Create Table tab3 (tab1id integer, text varchar(20))!

insert into tab1 values (1)!
insert into tab1 values (2)!

insert into tab2 values (1,1,1)!
insert into tab2 values (1,2,1)!
insert into tab2 values (1,3,1)!
insert into tab2 values (1,4,2)!
insert into tab2 values (1,5,2)!

insert into tab3 values (1,'text1')!
insert into tab3 values (2,'text2')!

SELECT 
tab2.tab2id AS tab2_tabid2, 
tab2.tab2id AS tab2_tabid2, 
tab3.text   AS tab3_text
FROM <tab1, tab2, tab3>
WHERE tab1.tab1id= tab2.tab1id
AND tab2.tab3id = tab3.tab1id
AND tab1.tab1id=1
ORDER BY tab2.tab3id asc!
fetch !
SELECT 
tab2.tab2id AS tab2_tabid2, 
tab2.tab2id AS tab2_tabid2, 
tab3.text   AS tab3_text
FROM <tab1, tab2, tab3>
WHERE tab1.tab1id= tab2.tab1id
AND tab2.tab3id = tab3.tab1id
AND tab1.tab1id=1
ORDER BY tab2.tab3id desc!
fetch !

rollback work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINDRCT.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOINDRCT VDNTEST  changed on 1992-09-28-13.24.11 by ADMIN     *
file connect ( kern test !
create table t1 (a1 char(125) key, a2 char(5) , a3 char(5)) !
create table t2 (b1 char(5) key, b2 char(5), b3 char(5),
b4 char(254), b5 char(254), b6 char(254), b7 char(254),
b8 char(254), b9 char(254), b10 char(254))!
insert t1 values ('aaaaa', 'ccccc', 'bbbbb') !
insert t1 values ('aaaab', 'ccccc', 'bbbbb') !
insert t1 values ('aaaac', 'ccccc', 'bbbbb') !
insert t1 values ('aaaad', 'ccccc', 'bbbbb') !
insert t1 values ('aaaae', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaf', 'ccccc', 'bbbbb') !
insert t1 values ('aaaag', 'ccccc', 'bbbbb') !
insert t1 values ('aaaah', 'ccccc', 'bbbbb') !
insert t1 values ('aaaai', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaj', 'ccccc', 'bbbbb') !
insert t1 values ('aaaak', 'ccccc', 'bbbbb') !
insert t1 values ('aaaal', 'ccccc', 'bbbbb') !
insert t1 values ('aaaam', 'ccccc', 'bbbbb') !
insert t1 values ('aaaan', 'ccccc', 'bbbbb') !
insert t1 values ('aaaao', 'ccccc', 'bbbbb') !
insert t1 values ('aaaap', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaq', 'ccccc', 'bbbbb') !
insert t1 values ('aaaar', 'ccccc', 'bbbbb') !
insert t1 values ('aaaas', 'ccccc', 'bbbbb') !
insert t1 values ('aaaat', 'ccccc', 'bbbbb') !
commit work !
explain select a1, t2.* from <t2, t1> where a1 = b1 !
select a1, t2.* from <t2, t1> where a1 = b1 !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaj', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaak', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
************************************************************************
drop table t2 !
create table t2 (b1 char(5) key, b2 char(5), b3 char(5),
b4 char(254), b5 char(254), b6 char(254), b7 char(254)) !
commit work !
select a1, t2.* from <t2, t1> where a1 = b1 !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaj', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaak', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
COMMIT WORK RELEASE !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINEXISTS.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JEXISTS.VDNTS 
file connect ( kern test !
create table t1 (a1 fixed (10),
                 a2 fixed (10),
                 a3 fixed (10))!
create table t2 (b1 fixed (10),
                 b2 fixed (10),
                 b3 fixed (10))!
insert t1 set a1 = 1,
              a2 = 2,
              a3 = 3 !
insert t2 set b1 = 11,
              b2 = 22,
              b3 = 33 !
insert t1 set a1 = 10,
              a2 = 20,
              a3 = 30 !
insert t2 set b1 = 10,
              b2 = 20,
              b3 = 30 !
create view v as select a1 v1, a2 v2, a3 v3 from t1 !
commit work !
select * from t2 where not exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t1, t2> where not exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t1, t2> where a1 (+) = b1 (+) AND not exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t1, t2> where a1 (+) = b1 (+) AND exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t1, t2> where a1 (+) = b1 (+) AND not exists 
         (select 1111 from v where 3 = 3) !
fetch !
select * from <t1, t2> where a1 (+) = b1 (+) AND exists 
         (select 1111 from v where 3 = 3) !
fetch !
select * from <t2, t1> where not exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND not exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND exists 
         (select 1111 from v where 3 = 4) !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND not exists 
         (select 1111 from v where 3 = 3) !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND exists 
         (select 1111 from v where 3 = 3) !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND 3 = 3 !
fetch !
select * from <t2, t1> where a1 (+) = b1 (+) AND 3 = 4 ! 
fetch !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOININVD.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOININVD VDNTEST  changed on 1992-09-28-13.24.12 by ADMIN     *
file connect ( kern test !
create table t1 (a1 char(125), a2 char(5) , a3 char(5)) !
create index "t1.a1"
	on t1( a1 ) !
create table t2 (b1 char(5), b2 char(5), b3 char(5),
b4 char(254), b5 char(254), b6 char(254), b7 char(254),
b8 char(254), b9 char(254), b10 char(254))!
insert t1 values ('aaaaa', 'ccccc', 'bbbbb') !
insert t1 values ('aaaab', 'ccccc', 'bbbbb') !
insert t1 values ('aaaac', 'ccccc', 'bbbbb') !
insert t1 values ('aaaad', 'ccccc', 'bbbbb') !
insert t1 values ('aaaae', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaf', 'ccccc', 'bbbbb') !
insert t1 values ('aaaag', 'ccccc', 'bbbbb') !
insert t1 values ('aaaah', 'ccccc', 'bbbbb') !
insert t1 values ('aaaai', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaj', 'ccccc', 'bbbbb') !
insert t1 values ('aaaak', 'ccccc', 'bbbbb') !
insert t1 values ('aaaal', 'ccccc', 'bbbbb') !
insert t1 values ('aaaam', 'ccccc', 'bbbbb') !
insert t1 values ('aaaan', 'ccccc', 'bbbbb') !
insert t1 values ('aaaao', 'ccccc', 'bbbbb') !
insert t1 values ('aaaap', 'ccccc', 'bbbbb') !
insert t1 values ('aaaaq', 'ccccc', 'bbbbb') !
insert t1 values ('aaaar', 'ccccc', 'bbbbb') !
insert t1 values ('aaaas', 'ccccc', 'bbbbb') !
insert t1 values ('aaaat', 'ccccc', 'bbbbb') !
commit work !
explain select a1, t2.* from <t2, t1> where a1 = b1 !
select a1, t2.* from <t2, t1> where a1 = b1 !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaj', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaak', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb',
                  'a', 'b', 'c', 'd', 'e', 'f', 'g') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
************************************************************************
drop table t2 !
create table t2 (b1 char(5) key, b2 char(5), b3 char(5),
b4 char(254), b5 char(254), b6 char(254), b7 char(254)) !
commit work !
select a1, t2.* from <t2, t1> where a1 = b1 !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaao', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaa', 'aaaaa', 'bbbbb',  'a', 'b', 'c', 'd') !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work !
insert t2 values ('aaaaa', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaai', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaj', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaak', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaat', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
insert t2 values ('aaaaz', 'aaaaa', 'bbbbb', 'a', 'b', 'c', 'd') !
select a1, t2.* from <t2, t1> where a1 = b1 !
fetch !
rollback work release !
file connect ( kern test !
drop table t1 !
drop table t2 !
commit work release !

* Problem bei Join via Multiple Index Columns (PTS 1104824|26|28|42)
file connect ( kern test !
create table j1test ( 
k1 int key,
c1 int,
c2 int) !

insert j1test values (1,  2, 1)!
insert j1test values (2,  2, 2)!
insert j1test values (3,  2, 3)!
insert j1test values (4,  2, 4)!
insert j1test values (5,  2, 5)!
insert j1test values (6,  2, 6)!
insert j1test values (7,  2, 7)!
insert j1test values (8,  2, 8)!
insert j1test values (9,  2, 9)!

update statistics j1test !

file sel_optimstat ( J1TEST !

create table j2test ( 
k1 int key,
i1 int,
i2 int,
c1 int)!

create index mijtest on j2test (i1,i2) !

insert j2test values (1,2,2,9)!
insert j2test values (2,2,2,8)!
insert j2test values (3,2,2,7)!
insert j2test values (4,2,2,6)!
insert j2test values (5,2,2,5)!
insert j2test values (6,2,2,4)!
insert j2test values (7,2,2,3)!
insert j2test values (8,2,2,2)!
insert j2test values (9,2,2,1)!

update statistics j2test !

file sel_optimstat ( J2TEST !


explain select * from <j1test t1, j2test t2>
where t2.i1 = t1.c1
and   t2.i2 = 2
and   t2.c1 = t1.c2!

select * from <j1test t1, j2test t2>
where t2.i1 = t1.c1
and t2.i2 = 2
and t2.c1 = t1.c2!

explain select * from j1test t1, j2test t2
where t2.i1 = t1.c1
and   t2.i2 = 2
and   t2.c1 = t1.c2!

select * from j1test t1, j2test t2
where t2.i1 = t1.c1
and t2.i2 = 2
and t2.c1 = t1.c2!
commit work release !

file joininv !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINMULFLDS.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* MULFLDS  VDNTEST  changed on 1992-09-28-13.24.38 by ADMIN     *
file connect ( kern test !
create table t1 (s1 char(10),
                 s2 char(15),
                 sl char(15)) !
file mulflds_data ( t1  !
create table ti (i1 char(10),
                 i2 char(15),
                 il char(15)) !
file mulflds_data ( ti  !
create table tk (k1 char(10) key,
                 k2 char(15) key,
                 kl char(15) key) !
file mulflds_data ( tk  !
create table tai (ai1 char(10) ascii,
                 ai2 char(15) ascii,
                 ail char(15) ascii) !
file mulflds_data ( tai !
create table tak (ak1 char(10) ascii key,
                 ak2 char(15) ascii key,
                 akl char(15) ascii key) !
file mulflds_data ( tak !
update statistics kern.* !
create index dummyind01 on ti (syskey, i2, il) !
create index mindx on ti (i1, i2, il) !
drop index dummyind01 !
commit work !
!****** select via all multiple inv-colums *****
explain select * from <t1, ti> where s1 = i1 and s2 = i2 and sl = il !
select * from <t1, ti> where s1 = i1 and s2 = i2 and sl = il !
FETCH !
!****** select via start multiple inv-colums *****
explain select * from t1, ti where s1 = i1 and s2 = i2 !
select * from t1, ti where s1 = i1 and s2 = i2 !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il, i2, i1) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 asc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 desc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il asc,  i2 desc, i1 asc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
!****** select via all multiple key colums *****
explain select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
*FETCH !
!****** select via all multiple key colums (other sequence) *****
explain select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
*FETCH !
!****** select via first multiple key colums *****
explain select * from t1, tk where s1 = k1 and s2 = k2 !
select * from t1, tk where s1 = k1 and s2 = k2 !
*FETCH !
create index mindx on tai (ail desc,  ai2 asc, ai1 desc) !
!****** select via all multiple inv-colums (ascii columns **************
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail asc,  ai2 desc, ai1 asc) !
!****** select via all multiple inv-colums (ascii colums) ***************
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail desc,  ai2 desc, ai1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
*FETCH !
!************************************************************************
!**************** fieldlengths not equal *******************************
!***  multiple fields are longer than sequential table  *****************
!************************************************************************
drop table ti !
drop table tk !
drop table tai !
drop table tak !
create table ti (i1 char(12),
                 i2 char(17),
                 il char(20)) !
file mulflds_data ( ti !
create table tk (k1 char(12) key,
                 k2 char(17) key,
                 kl char(20) key) !
file mulflds_data ( tk !
create table tai (ai1 char(12) ascii,
                 ai2 char(17) ascii,
                 ail char(20) ascii) !
file mulflds_data ( tai !
create table tak (ak1 char(12) ascii key,
                 ak2 char(17) ascii key,
                 akl char(20) ascii key) !
file mulflds_data ( tak !
update statistics kern.* !
create index mindx on ti (i1, i2, il) !
commit work !
!****** select via all multiple inv-colums *****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
!****** select via start multiple inv-colums => table scan *****
explain select * from t1, ti where s1 = i1 and s2 = i2 !
select * from t1, ti where s1 = i1 and s2 = i2 !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il, i2, i1) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 asc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 desc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il asc,  i2 desc, i1 asc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
!****** select via all multiple key colums *****
explain select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
*FETCH !
!****** select via all multiple key colums (other sequence) *****
explain select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
*FETCH !
!****** select via first multiple key colums *****
explain select * from t1, tk where s1 = k1 and s2 = k2 !
select * from t1, tk where s1 = k1 and s2 = k2 !
*FETCH !
create index mindx on tai (ail desc,  ai2 asc, ai1 desc) !
!****** select via all multiple inv-colums (ascii columns **************
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail asc,  ai2 desc, ai1 asc) !
!****** select via all multiple inv-colums (ascii colums) ***************
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail desc,  ai2 desc, ai1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
!****** select via all multiple inv-colums (other sequence of mindx) ****
explain select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
*FETCH !
!************************************************************************
!**************** fieldlengths not equal *******************************
!***  multiple fields are shorter than fields in seq. table *************
!************************************************************************
drop table t1 !
drop table ti !
drop table tk !
drop table tai !
drop table tak !
create table t1 (s1 char(12),
                 s2 char(17),
                 sl char(20)) !
file mulflds_data ( t1 !
create table ti (i1 char(10),
                 i2 char(15),
                 il char(15)) !
file mulflds_data ( ti !
create table tk (k1 char(10) key,
                 k2 char(15) key,
                 kl char(15) key) !
file mulflds_data ( tk !
create table tai (ai1 char(10) ascii,
                 ai2 char(15) ascii,
                 ail char(15) ascii) !
file mulflds_data ( tai !
create table tak (ak1 char(10) ascii key,
                 ak2 char(15) ascii key,
                 akl char(15) ascii key) !
file mulflds_data ( tak !
update statistics kern.* !
create index mindx on ti (i1, i2, il) !
commit work !
!****** select via all multiple inv-colums *****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
!****** select via start multiple inv-colums => table scan *****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 !
select * from t1, ti where s1 = i1 and s2 = i2 !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il, i2, i1) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 asc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il desc,  i2 desc, i1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
drop index mindx on ti !
create index mindx on ti (il asc,  i2 desc, i1 asc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
select * from t1, ti where s1 = i1 and s2 = i2 and sl = il !
*FETCH !
!****** select via all multiple key colums *****
!****** (t1.cols > ti.cols)
explain select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
*FETCH !
!****** select via all multiple key colums (other sequence) *****
!****** (t1.cols > ti.cols)
explain select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
select * from t1, tk where sl = kl and s2 = k2 and s1 = k1 !
*FETCH !
!****** select via first multiple key colums *****
!****** (t1.cols > ti.cols)
explain select * from t1, tk where s1 = k1 and s2 = k2 !
select * from t1, tk where s1 = k1 and s2 = k2 !
*FETCH !
create index mindx on tai (ail desc,  ai2 asc, ai1 desc) !
!****** select via all multiple inv-colums (ascii columns **************
!****** (t1.cols > ti.cols)
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail asc,  ai2 desc, ai1 asc) !
!****** select via all multiple inv-colums (ascii colums) ***************
!****** (t1.cols > ti.cols)
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
drop index mindx on tai !
create index mindx on tai (ail desc,  ai2 desc, ai1 desc) !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
select * from t1, tai where s1 = ai1 and s2 = ai2 and sl = ail !
*FETCH !
!****** select via all multiple inv-colums (other sequence of mindx) ****
!****** (t1.cols > ti.cols)
explain select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
select * from t1, tak where s1 = ak1 and s2 = ak2 and sl = akl !
*FETCH !
!************************************************************************
!****** char-Byte-Feld als letztes multiples Feld ***********************
!************************************************************************
drop table t1 !
drop table ti !
drop table tk !
create table t1 (s1 char (10),
                 s2 char (10),
                 sl char (10) BYTE) !
insert t1 values ('aaaaaaaaaa',  '          ', X'40408101')!
insert t1 values ('aaaaaaaaaa',  '          ', X'404081')!
create table ti (i1 char(10),
                 i2 char(11),
                 il char(12) BYTE) !
insert ti values ('aaaaaaaaaa',  '          ', X'40408101')!
insert ti values ('aaaaaaaaaa',  '          ', X'404081')!
create table tk (k1 char(10) key,
                 k2 char(11) key,
                 kl char(12) byte key) !
insert tk values ('aaaaaaaaaa',  '          ', X'40408101')!
insert tk values ('aaaaaaaaaa',  '          ', X'404081')!
create index mindx on ti (i1, i2, il) !
explain select * from <t1, ti> where s1 = i1 and s2 = i2 and sl = il !
select * from <t1, ti> where s1 = i1 and s2 = i2 and sl = il !
FETCH !
explain select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
select * from t1, tk where s1 = k1 and s2 = k2 and sl = kl !
FETCH !
file mulmtki !
file mulfprim !
file mulfrang !
file CONNECT ( KERN TEST !
CREATE TABLE T1 (A1 CHAR(5) ASCII KEY, A2 CHAR(5) ASCII KEY,
      A3 CHAR(5) ASCII KEY, A4 CHAR(5) ASCII key) !
INSERT T1 VALUES ('aaaaa','bbbbb','ccccc', 'ddddd') !
CREATE TABLE T2 (B1 CHAR(5) ASCII KEY, B2 CHAR(5) ASCII KEY,
      B3 CHAR(5) ASCII KEY, B4 CHAR(5) ASCII) !
INSERT T2 VALUES ('aaaaa','bbbbb','ccccc', 'ddddd') !
CREATE TABLE T3 (C1 CHAR(5) ASCII KEY, C2 CHAR(5) ASCII KEY,
      C3 CHAR(5) ASCII KEY, C4 CHAR(5) ASCII) !
INSERT T3 VALUES ('aaaaa','bbbbb','ccccc', 'ddddd') !
explain SELECT * FROM <T3, T1, T2>
     WHERE A2 = B2 AND A3 = 'ccccc' 
       AND A1 = 'aaaaa' AND A2 = C2 !
drop table t1 !
drop table t2 !
drop table t3 !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINMULVAL.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* MULFLDS  VDNTEST  changed on 1992-09-28-13.24.38 by ADMIN     *
file connect ( kern test !
create table t1 (s1 char(05),
                 s2 char(10),
                 sl char(10)) !
insert t1 values ('value',  'bbbbb', 'ccccc')!
insert t1 values ('aaaa1',  'value', 'cccc1')!
insert t1 values ('aaaa2',  'bbbbb', 'value')!
create table t2 (b1 char(05),
                 b2 char(10),
                 bl char(10), primary key (b2, b1, bl)) !
insert t2 values ('value',  'bbbbb', 'ccccc')!
insert t2 values ('aaaa1',  'value', 'cccc1')!
insert t2 values ('aaaa2',  'bbbbb', 'value')!
update statistics kern.* !
commit work !
file jmulval !
file jmulvalv !
drop table t2 !
commit work !
create table t2 (b1 char(05),
                 b2 char(10),
                 bl char(10)) !
insert t2 values ('value',  'bbbbb', 'ccccc')!
insert t2 values ('aaaa1',  'value', 'cccc1')!
insert t2 values ('aaaa2',  'bbbbb', 'value')!
update statistics kern.* !
create index d_ind01 on t2 (syskey, b2, bl) !
create index mindx on t2 (b1, b2, bl) !
create unique index d_ind02 on t2 (b2, syskey) !
create unique index d_ind03 on t2 (syskey, bl, b2) !
create unique index d_ind04 on t2 (b2, syskey, bl) !
create unique index umindx on t2 (b1, b2, bl) !
create unique index d_ind05 on t2 (bl, syskey, b2) !
drop index d_ind01 !
file jmulval !
file jmulvalv !
drop table t1 !
drop table t2 !
COMMIT WORK RELEASE !
file connect ( kern test !
CREATE TABLE T1 ( T1XC1 FIXED(2) KEY, T1XC2 FIXED(2),
                  T1XC3 FIXED(2) ) !
CREATE TABLE T2 ( T2XC1 FIXED(2) KEY, T2XC2 FIXED(2), T2XC3 FIXED(2) ) !
CREATE TABLE T3 ( T3XC1 FIXED(2) KEY, T3XC2 FIXED(2), T3XC3 FIXED(2) ) !
ALTER TABLE T1 FOREIGN KEY T1LT2 (T1XC1) REFERENCES T2 !
ALTER TABLE T2 FOREIGN KEY T2LT3 (T2XC2) REFERENCES T3 !
CREATE VIEW V1 AS SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM T1, T2, T3
   WHERE T1XC1 = T2XC1 AND T2XC2 = T3XC1 WITH CHECK OPTION !
INSERT T3 VALUES ( 2, 3, 3 ) !
INSERT T3 VALUES ( 1, 6, 6 ) !
INSERT T3 VALUES ( 1, 2, 4 ) !
INSERT T3 VALUES ( 5, 2, 3 ) !
INSERT T3 VALUES ( 3, 1, 7 ) !
INSERT T2 VALUES ( 8, 1, 5 ) !
INSERT T2 VALUES ( 2, 5, 5 ) !
INSERT T2 VALUES ( 4, 2, 6 ) !
INSERT T1 VALUES ( 2, 3, 5 ) !
INSERT T1 VALUES ( 4, 1, 3 ) !
INSERT T1 VALUES ( 8, 3, 1 ) !
COMMIT WORK !
SELECT * FROM V1 !
FETCH !
INSERT V1 VALUES ( 5,2,2,6,4,6,1 ) !
INSERT V1 VALUES ( 7,8,3,4,7,8,1 ) !
INSERT INTO V1 VALUES ( 3,4,2,7,8,7,2 ) !
INSERT V1 VALUES ( NULL, 4, NULL, 3,9,4,1 ) !
INSERT V1 VALUES ( 2,5,7, NULL, NULL, 6,9 ) !
INSERT V1 VALUES ( 9,2,3,9, NULL, 8,7 ) !
INSERT V1 ( T1XC1, T1XC2, T1XC3, T2XC2, T3XC1 )
   VALUES (17, 3, 7, 4, 4 ) !
COMMIT WORK !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T1, T2, T3>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T1, T3, T2>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T2, T1, T3>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T2, T3, T1>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T3, T1, T2>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
SELECT T1XC1, T1XC2, T1XC3, T2XC2, T2XC3, T3XC2, T3XC3
   FROM <T3, T2, T1>
   WHERE T1XC1=4 AND T1XC1 = T2XC1 AND T2XC2 = T3XC1 !
fetch !
commit work release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINNEW.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
FILE NEWJOIN ( internal !
file connect ( kern test !

create table LD_TABLE (
LD_LOCATION     char (8),
"LD_SSN"        int,
"LD_NAME_LAST"  char (30),
"LD_NAME_FIRST"  char (30),
"LD_NAME_MIDDLE"  char (30),
"LD_PHONE"        char(12),
"LD_EMAIL_INT"   char (40),
"LD_GOLDID"     int)!

insert LD_TABLE values (
'Berlin', 123, 'Zabach', 'Elke', NULL, '1234', 'email', 123) !

create table PR_TABLE (
"PR_NAME_FULL" char(30),
"PR_CLOCK_NO"  char(4),
PR_SITE        int,
PR_DEPT        int,
PR_SSN         int,
PR_TERM_FLAG   boolean)!

insert PR_TABLE values ( 'Elke', '1', 2, 3, 123, false) !

create view VW_LD_PR as
select
"PR_NAME_FULL", "PR_CLOCK_NO", "LD_LOCATION", "LD_SSN", "LD_NAME_LAST",
"LD_NAME_FIRST", "LD_NAME_MIDDLE", "LD_PHONE", "LD_EMAIL_INT", "LD_GOLDID",
TRIM(CHR("PR_SITE"))&
TRIM(CHR("PR_DEPT") )AS "COST_CNTR"
from "LD_TABLE", "PR_TABLE"
where "LD_SSN" = "PR_SSN"
and "PR_TERM_FLAG" = FALSE !

create table CC_COST_CENTER (
"CC_COST_CNTR"  char(12),
"CC_COMP_CODE"  int,
"CC_BUS_AREA" int)!
insert into CC_COST_CENTER values ( '23',2,3)!  

create table BA_BUS_AREA (
BA_NAME char (30),
BA_BUS_AREA_NO int)!
insert into BA_BUS_AREA values ( 'BA_NAME', 3)!  

create view VW_COST_CNTR as
select
"CC_COST_CNTR", "CC_COMP_CODE", "CC_BUS_AREA", "BA_NAME" 
from "CC_COST_CENTER","BA_BUS_AREA"
where "CC_BUS_AREA" = "BA_BUS_AREA_NO" !

select
VW_LD_PR."LD_LOCATION" & '_' & VW_LD_PR."LD_NAME_LAST" & '_' & 
VW_LD_PR."LD_NAME_FIRST" & '_' & VW_LD_PR."PR_CLOCK_NO"
AS "CN",
VW_LD_PR."PR_NAME_FULL", VW_LD_PR."PR_CLOCK_NO",
VW_LD_PR."LD_LOCATION", VW_LD_PR."LD_SSN",
VW_LD_PR."LD_NAME_LAST", VW_LD_PR."LD_NAME_FIRST",
VW_LD_PR."LD_NAME_MIDDLE", VW_LD_PR."LD_PHONE",
VW_LD_PR."LD_EMAIL_INT", VW_LD_PR."LD_GOLDID",
VW_LD_PR."COST_CNTR", VW_COST_CNTR."CC_COMP_CODE",
VW_COST_CNTR."CC_BUS_AREA", VW_COST_CNTR."BA_NAME"
from VW_LD_PR, VW_COST_CNTR
where "CC_COST_CNTR" = "COST_CNTR" !
fetch!

commit work release!
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINNEXT.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* JOINNEXT VDNTEST  changed on 1992-09-28-13.24.12 by ADMIN     *
file jointes1 !
file jointes2 !
file jointes3 !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINOUTER.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* OUTERJ   VDNTEST  changed on 1992-09-28-13.27.35 by ADMIN     *
file connect ( kern test !
create table tab1 ( a fixed(6), b char(3) )!
create table tab2 ( c fixed(6), d char(3) )!
create table tab3 ( e fixed(6), f char(3) )!
insert into tab1 values (1, 'abc')!
insert into tab1 values (2, 'abc')!
insert into tab1 values (3, 'def')!
insert into tab1 values (4, 'def')!
insert into tab1 values (5, 'qwe')!
insert into tab2 values (1, 'abc')!
insert into tab2 values (2, 'def')!
insert into tab2 values (3, 'def')!
insert into tab2 values (4, 'hij')!
insert into tab2 values (6, 'hij')!
insert into tab3 values (1, 'def')!
insert into tab3 values (2, 'hij')!
insert into tab3 values (3, 'abc')!
insert into tab3 values (4, 'fed')!
insert into tab3 values (8, 'fed')!
commit work!
select * from tab1!
fetch !
select * from tab2!
fetch !
select * from tab3!
fetch !
select * from tab1, tab2 where a (+) = 100 !
select * from tab1, tab2 where a = 100 (+) !
select * from tab1, tab2 where a (+) = 1 !
select * from tab1, tab2 where a = 1 (+) !
select * from tab1, tab2 where b (+) = '100' !
select * from tab1, tab2 where b = '100' (+) !
select * from tab1, tab2 where a (+) = c!
fetch !
select * from tab1, tab2 where a = c(+)!
fetch !
select * from tab1, tab2 where a (+) = c and a > 2!
fetch !
select * from tab1, tab2 where a = c( +) and a > 2!
fetch !
select * from tab1, tab2 where a (+) = c and b (+) = d!
fetch !
select * from tab1, tab2 where a (+) = c and b = d (+)!
fetch !
select * from tab1, tab2 where a = c (+) and b = d (+)!
fetch !
select * from tab1, tab2 where a = c (+) and b (+) = d!
fetch !
select * from tab1, tab2 where a = 2 * c (+) and b = d (+)!
fetch !
select * from tab1, tab2 where 2 * a = c (+) and b = d (+)!
fetch !
select * from tab1, tab2 where a (+) = 2 * c and b (+) = d!
fetch !
select * from tab1, tab2 where 2 * a (+) = c and b (+) = d!
fetch !
select * from tab1, tab2 where a (+) = c or a > 2!
fetch !
select * from tab1, tab2 where a = c( +) or a > 2!
fetch !
select * from tab1, tab2 where a (+) = c or b (+) = d!
fetch !
select * from tab1, tab2 where a (+) = c or b = d (+)!
fetch !
select * from tab1, tab2 where a = c (+) or b = d (+)!
fetch !
select a, b, value(c, 20), c from tab1, tab2 order by c, a!
fetch!
select a, b, value(c, 20), c from tab1, tab2 where tab1.a = tab2.c !
fetch!
select a, b, value(c, 20), c from tab1, tab2 where tab1.a = tab2.c (+)!
fetch!
select a, b, value(a+c,20),c from tab1, tab2 where tab1.a = tab2.c (+)!
fetch!
commit work release !
file connect ( kern test !
create table hotel ( hno fixed (5) key, fstate char (2))!
insert hotel values(1, 'FL')!
insert hotel values(3, 'FL')!
insert hotel values(2, 'NY')!
create table reserv (rno fixed(5) key, hno fixed (5))!
insert reserv values ( 90, 5)!
insert reserv values (100, 1)!
insert reserv values (120, 2)!
insert reserv values (130, 1)!
insert reserv values (140, 4)!
select * from hotel, reserv
where
    hotel.hno (+) = reserv.hno (+)
and hotel.fstate = 'FL'
and reserv.rno <= 130
!
fetch !
select * from hotel, reserv
where
    hotel.hno (+) = reserv.hno (+)
and reserv.rno <= 130
!
fetch !
select * from hotel, reserv
where
    hotel.hno (+) = reserv.hno (+)
and hotel.fstate = 'FL'
!
fetch !
select * from hotel, reserv
where
    hotel.hno (+) = reserv.hno (+)
!
fetch !
select * from reserv, hotel
where
    hotel.hno (+) = reserv.hno (+)
and hotel.fstate = 'FL'
and reserv.rno <= 130
!
fetch !
select * from reserv, hotel
where
    hotel.hno (+) = reserv.hno (+)
and reserv.rno <= 130
!
fetch !
select * from reserv, hotel
where
    hotel.hno (+) = reserv.hno (+)
and hotel.fstate = 'FL'
!
fetch !
select * from reserv, hotel
where
    hotel.hno (+) = reserv.hno (+)
!
fetch !
commit work release !
file connect ( kern test !
create table s (sno char(2), city char(18)) !
insert s values ('S7', 'London') !
insert s values ('S8', 'Paris') !
insert s values ('S9', NULL) !
create table p (pno char(2), city char(18)) !
insert p values ('P7', 'London') !
insert p values ('P8', 'Madrid') !
insert p values ('P9', NULL) !
select sno, s.city, pno, p.city from s,p where s.city(+) = p.city(+) !
fetch !
select sno, s.city, pno, p.city from p,s where s.city(+) = p.city(+) !
fetch !
select sno, s.city, pno, p.city from p,s where s.city = p.city(+) 
union
select sno, s.city, pno, p.city from s,p where s.city(+) = p.city 
order by 1, 3 !
fetch !
rollback release !

* PTS 1103775
*
* Der Fehler -3022 tritt faelschlicherweise bei einem Outer Join
* mit einem Predikat folgender Art auf:
*
* table1.column1 (+) = 'a' AND table1.column1 (+) = table2.column2  
*
file connect ( kern test !

create table t0 ( 
tabname char (10), 
contflag char (10)) !

create table t1 (
tabname char (10))!

create table t2 (
nrtab char (10),
dtelsobj char (10))!

create table t3 (
tabname char (10),
ddtext char (10),
ddlanguage char (10))!

SELECT  DISTINCT  T0."TABNAME" , T0."CONTFLAG" , T3."DDTEXT"  
FROM  T0 ,  T1 , T2 , T3  
WHERE  ( T0."TABNAME" = T1."TABNAME" ) 
AND    ( T2."NRTAB" = T1."TABNAME" ) 
AND    ( T3."TABNAME" (+) = T0."TABNAME" 
AND T3."DDLANGUAGE" (+) = 'a' ) 
AND    T2."DTELSOBJ" = 'a' AND T2."NRTAB" <> 'a' !


rollback release !
* PTS 1113318/9 !
file connect ( kern test !
create table t ( a char (8), b char (8))!
insert t values ('aaa', 'bbb')!
create table u ( v char (8), w char (8))!
insert u values ('aaa', 'www')!
insert u values ('s'  , 'www')!
create view vv as
select a,b,v,w 
  from t,u
 where v = a(+)
!
commit !
* PTS 1113326 !
* null-check AFTER join !
* force into one_phase !
explain select a,b,v,w 
  from t,u
 where v = a(+)
   and a is null !
select a,b,v,w 
  from t,u
 where v = a(+)
   and a is null !
fetch !
select a,b,v,w 
  from t,u
 where (b||w <> 'xyz' or b||w is null)
   and v = a(+)
   and a is null !
fetch!
* force into two_phase !
explain select a,b,v,w,substr(b||w, 1, 16)
  from t,u
 where v = a(+)
   and a is null !
select a,b,v,w,substr(b||w, 1, 16)
  from t,u
 where v = a(+)
   and a is null !
fetch !
select a,b,v,w,substr(b||w, 1, 16)
  from t,u
 where (b||w <> 'xyz' or b||w is null)
   and v = a(+)
   and a is null !
fetch!
****************************!
* null-check BEFORE join !
* force into two_phase !
select a,b,v,w,substr(b||w, 1, 16)
  from t,u
 where v = a(+)
   and a(+) is null !
fetch !
select a,b,v,w,substr(b||w, 1, 16)
  from t,u
 where (b||w <> 'xyz' or b||w is null)
   and v = a(+)
   and a(+) is null !
fetch!
* now the original problem !
explain select a,b,v,w 
  from vv!
select a,b,v,w
  from vv!
fetch !
explain select a,b,v,w 
  from vv
 where a is null !
select a,b,v,w 
  from vv
 where a is null !
fetch!
explain select a,b,v,w,substr(b||w, 1, 16)
  from vv
 where a is null !
select a,b,v,w,substr(b||w, 1, 16)
  from vv
 where a is null !
fetch!
* a(+) is null after view-creatio ?!
explain select a,b,v,w 
  from vv
 where a(+) is null !
select a,b,v,w 
  from vv
 where a(+) is null !
fetch!
explain select a,b,v,w,substr(b||w, 1, 16) 
  from vv
 where a(+) is null !
select a,b,v,w,substr(b||w, 1, 16)
  from vv
 where a(+) is null !
fetch!
drop table t !
drop table u !
commit !

create table t1(
t1c1 int, t1c2 int, t1c3 int, t1c4 int
, primary key(t1c1, t1c2))!
create table t2(
t2c1 int, t2c2 int, t2c3 int, t2c4 int
, primary key(t2c2, t2c1))!

insert into t1(t1c1,t1c2,t1c3,t1c4) values( 1,  7, 12,  4)!
insert into t1(t1c1,t1c2,t1c3,t1c4) values( 1,  8,  4, 23)!
insert into t1(t1c1,t1c2,t1c3,t1c4) values( 2,  9,  1,  1)!

insert into t2 values(22,  1,  2,  3)!
insert into t2 values(44,  1, 23,  5)!
insert into t2 values(15,  3,  2,  3)!

* test if constant expression survive left outer join columns
* which have to be filled with NULL values
select sysdba, t2c1, t1c4 from <t2, t1>
where t2c2 = t1c4 (+)!

fetch!
commit release !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
* JOINOUTR.vdnts
file connect ( sut sut !
create user kern password test dba not exclusive !
commit work release !
*   *ID* OUTRJOIN VDNTEST  changed on 1992-09-28-13.27.38 by ADMIN     *
file outerkey !
file cleark !
file outerinv !
file cleark !
file outernxt !
file cleark !
file outerseq !
file cleark !
file outeror  !
file cleark !
file outersyn !
file connect ( sut sut !
drop user kern !
commit work release !

*****************************************************************************
