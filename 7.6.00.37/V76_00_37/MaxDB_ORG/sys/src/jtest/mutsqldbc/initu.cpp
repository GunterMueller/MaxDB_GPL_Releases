/*
 * @(#) initu.cpp	2005-05-06

	Initialization of super-user, runs one time

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



*/

#include "jtest/mutsqldbc/mut.h"

#undef	CE
#define	CE(x,y,z,a) { if (z != 0 ) pe (x, y, z, a) ; };

typedef unsigned short UCSCHAR;

void tab_unicode();
void tab_cluster();
void update_stat();

char	k0 [240] ;
char    c5 [1501] ;
int     k1 , k2 , k3 ;
char    c1 [21] , c2[21] , c3[21] , c4[21] ;
int     i1 , i2 , i3 , i4 , i5 , i6 ;
int     ii ;
int     nr ;
int		cll_length , cll_length_i ;

char	char5 [6] ;
char	char1a [2], char1b [2], char1c [2];
char	char400 [401] ;

struct {unsigned short len; char arr [32002];} cll;
struct  {long len; char arr[100000];} scl;

UCSCHAR	uchar1[24+1];
UCSCHAR	uchar2[5+1];
UCSCHAR	uchar3[500+1];
UCSCHAR	uchar4[3+1];
UCSCHAR	uchar5[1500+1];
char	nouchar1[24+1];
char	nouchar3[500+1];
char	nouchar4[3+1];

long	ln , nrl, rc;
char	st [2000] ;
time_t  tm ;


int main (int argc ,char **argv )
{
	SQLDBC_UInt2	j = 0;
	int     i = 0;
	int     insert_count ;

	pid = getpid () ;
	srand(pid);

	check_params(argc, argv);
	sprintf(actfile, "%s.act", callfile);
	fout = stderr;
	print_params(argc, argv);

	if ( maxlongraw < 0 || maxlongraw > 32000 )
		maxlongraw = 32000 ;

	env = new  SQLDBC_Environment (getRuntime());
	conn = create_conn(env);

	//the 'CHOPBLANK' property removes blanks that will be automatically returned for varchar types;
	//	otherwise every returned varchar value has the full length defined with create table and every
	//	comparison will failed
	CP.setProperty("CHOPBLANKS", "1");
	CP.setProperty("ISOLATIONLEVEL", "0");
	CP.setProperty("SQL", "TRUE");
#ifndef _WIN32
	CP.setProperty("FILENAME", "./pid%p.pct");
#else
	CP.setProperty("FILENAME", ".\\pid%p.pct");
#endif
	CP.setProperty("FILESIZE", "25000000");	//25.000.000 = 25 MB
	env->setTraceOptions(CP);

	do_connect(conn, DBNODE, DBNAME, id, pw, CP);

	is_database_unicode();

	stmt = conn->createStatement();
	CE("main: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	database_info_own();

	printf ( "DROP TABLE TAB1\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB1");
	CE("main: DROP TABLE TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE TAB2\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB2");
	CE("main: DROP TABLE TAB2", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE TAB3\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB3");
	CE("main: DROP TABLE TAB3", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE TAB4\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB4");
	CE("main: DROP TABLE TAB4", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE Tab20K\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE Tab20K");
	CE("main: DROP TABLE Tab20K", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE Tab1SL\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE Tab1SL");
	CE("main: DROP TABLE Tab1SL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE MAXRAW\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE MAXRAW");
	CE("main: DROP TABLE MAXRAW", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE Tab04SK\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE Tab04SK");
	CE("main: DROP TABLE Tab04SK", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	printf ( "CREATE TABLE TAB1\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("CREATE TABLE TAB1 "
		"(K0 CHAR (240) KEY, K1 FIXED (10) KEY, I1 FIXED (10), I2 FIXED (10), C1 CHAR (20),"
		"C2 VARCHAR(20) NOT NULL, I3 FIXED(10) NOT NULL, CLLEN FIXED (10), CF0 CHAR(1500), CLB LONG,"
		"BI_1 CHAR(5), BI_2 CHAR(5), BI_3 CHAR(5), BI_4 CHAR(5), BI_5 CHAR(5), "
		"BI_6 CHAR(5), BI_7 CHAR(5), BI_8 CHAR(5), BI_9 CHAR(5), BI_10 CHAR(5), "
		"BI_11 CHAR(5), BI_12 CHAR(5), BI_13 CHAR(5), BI_14 CHAR(5), BI_15 CHAR(5), "
		"BI_16 CHAR(5), BI_17 CHAR(5), BI_18 CHAR(5), BI_19 CHAR(5), BI_20 CHAR(5), "
		"BI_21 CHAR(5), BI_22 CHAR(5), BI_23 CHAR(5), BI_24 CHAR(5), BI_25 CHAR(5), "
		"BI_26 CHAR(5), BI_27 CHAR(5), BI_28 CHAR(5), BI_29 CHAR(5), BI_30 CHAR(5), "
		"BI_31 CHAR(5), BI_32 CHAR(5), BI_33 CHAR(5), BI_34 CHAR(5), BI_35 CHAR(5), "
		"BI_36 CHAR(5), BI_37 CHAR(5), BI_38 CHAR(5), BI_39 CHAR(5), BI_40 CHAR(5), "
		"BI_41 CHAR(5), BI_42 CHAR(5), BI_43 CHAR(5), BI_44 CHAR(5), BI_45 CHAR(5), "
		"BI_46 CHAR(5), BI_47 CHAR(5), BI_48 CHAR(5), BI_49 CHAR(5), BI_50 CHAR(5), "
		"BI_51 CHAR(5), BI_52 CHAR(5), BI_53 CHAR(5), BI_54 CHAR(5), BI_55 CHAR(5), "
		"BI_56 CHAR(5), BI_57 CHAR(5), BI_58 CHAR(5), BI_59 CHAR(5), BI_60 CHAR(5), "
		"BI_61 CHAR(5), BI_62 CHAR(5), BI_63 CHAR(5), BI_64 CHAR(5), BI_65 CHAR(5), "
		"BI_66 CHAR(5), BI_67 CHAR(5), BI_68 CHAR(5), BI_69 CHAR(5), BI_70 CHAR(5), "
		"BI_71 CHAR(5), BI_72 CHAR(5), BI_73 CHAR(5), BI_74 CHAR(5), BI_75 CHAR(5), "
		"BI_76 CHAR(5), BI_77 CHAR(5), BI_78 CHAR(5), BI_79 CHAR(5), BI_80 CHAR(5), "
		"BI_81 CHAR(5), BI_82 CHAR(5), BI_83 CHAR(5), BI_84 CHAR(5), BI_85 CHAR(5), "
		"BI_86 CHAR(5), BI_87 CHAR(5), BI_88 CHAR(5), BI_89 CHAR(5), BI_90 CHAR(5), "
		"BI_91 CHAR(5), BI_92 CHAR(5), BI_93 CHAR(5), BI_94 CHAR(5), BI_95 CHAR(5), "
		"BI_96 CHAR(5), BI_97 CHAR(5), BI_98 CHAR(5), BI_99 CHAR(5), BI_100 CHAR(5), "
		"BI_101 CHAR(1), BI_102 CHAR(1), BI_103 CHAR(1), BI_104 CHAR(1), BI_105 CHAR(1), "
		"BI_106 CHAR(1), BI_107 CHAR(1), BI_108 CHAR(1), BI_109 CHAR(1), BI_110 CHAR(1), "
		"BI_111 CHAR(1), BI_112 CHAR(1), BI_113 CHAR(1), BI_114 CHAR(1), BI_115 CHAR(1), "
		"BI_116 CHAR(1), BI_117 CHAR(1), BI_118 CHAR(1), BI_119 CHAR(1), BI_120 CHAR(1), "
		"BI_121 CHAR(1), BI_122 CHAR(1), BI_123 CHAR(1), BI_124 CHAR(1), BI_125 CHAR(1), "
		"BI_126 CHAR(1), BI_127 CHAR(1), BI_128 CHAR(1), BI_129 CHAR(1), BI_130 CHAR(1), "
		"BI_131 CHAR(1), BI_132 CHAR(1), BI_133 CHAR(1), BI_134 CHAR(1), BI_135 CHAR(1), "
		"BI_136 CHAR(1), BI_137 CHAR(1), BI_138 CHAR(1), BI_139 CHAR(1), BI_140 CHAR(1), "
		"BI_141 CHAR(1), BI_142 CHAR(1), BI_143 CHAR(1), BI_144 CHAR(1), BI_145 CHAR(1), "
		"BI_146 CHAR(1), BI_147 CHAR(1), BI_148 CHAR(1), BI_149 CHAR(1), BI_150 CHAR(1), "
		"BI_151 CHAR(1), BI_152 CHAR(1), BI_153 CHAR(1), BI_154 CHAR(1), BI_155 CHAR(1), "
		"BI_156 CHAR(1), BI_157 CHAR(1), BI_158 CHAR(1), BI_159 CHAR(1), BI_160 CHAR(1), "
		"BI_161 CHAR(1), BI_162 CHAR(1), BI_163 CHAR(1), BI_164 CHAR(1), BI_165 CHAR(1), "
		"BI_166 CHAR(1), BI_167 CHAR(1), BI_168 CHAR(1), BI_169 CHAR(1), BI_170 CHAR(1), "
		"BI_171 CHAR(1), BI_172 CHAR(1), BI_173 CHAR(1), BI_174 CHAR(1), BI_175 CHAR(1), "
		"BI_176 CHAR(1), BI_177 CHAR(1), BI_178 CHAR(1), BI_179 CHAR(1), BI_180 CHAR(1), "
		"BI_181 CHAR(1), BI_182 CHAR(1), BI_183 CHAR(1), BI_184 CHAR(1), BI_185 CHAR(1), "
		"BI_186 CHAR(1), BI_187 CHAR(1), BI_188 CHAR(1), BI_189 CHAR(1), BI_190 CHAR(1), "
		"BI_191 CHAR(1), BI_192 CHAR(1), BI_193 CHAR(1), BI_194 CHAR(1), BI_195 CHAR(1), "
		"BI_196 CHAR(1), BI_197 CHAR(1), BI_198 CHAR(1), BI_199 CHAR(1), BI_200 CHAR(1), "
		"BI_201 CHAR(1), BI_202 CHAR(1), BI_203 CHAR(1), BI_204 CHAR(1), BI_205 CHAR(1), "
		"BI_206 CHAR(1), BI_207 CHAR(1), BI_208 CHAR(1), BI_209 CHAR(1), BI_210 CHAR(1), "
		"BI_211 CHAR(1), BI_212 CHAR(1), BI_213 CHAR(1), BI_214 CHAR(1), BI_215 CHAR(1), "
		"BI_216 CHAR(1), BI_217 CHAR(1), BI_218 CHAR(1), BI_219 CHAR(1), BI_220 CHAR(1), "
		"BI_221 CHAR(1), BI_222 CHAR(1), BI_223 CHAR(1), BI_224 CHAR(1), BI_225 CHAR(1), "
		"BI_226 CHAR(1), BI_227 CHAR(1), BI_228 CHAR(1), BI_229 CHAR(1), BI_230 CHAR(1), "
		"BI_231 CHAR(1), BI_232 CHAR(1), BI_233 CHAR(1), BI_234 CHAR(1), BI_235 CHAR(1), "
		"BI_236 CHAR(1), BI_237 CHAR(1), BI_238 CHAR(1), BI_239 CHAR(1), BI_240 CHAR(1), "
		"BI_241 CHAR(1), BI_242 CHAR(1), BI_243 CHAR(1), BI_244 CHAR(1), BI_245 CHAR(1), "
		"BI_246 CHAR(1), BI_247 CHAR(1), BI_248 CHAR(1), BI_249 CHAR(1), BI_250 CHAR(1), "
		"BI_251 CHAR(1), BI_252 CHAR(1), BI_253 CHAR(1), BI_254 CHAR(1), BI_255 CHAR(1), "
		"BI_256 CHAR(1), BI_257 CHAR(1), BI_258 CHAR(1), BI_259 CHAR(1), BI_260 CHAR(1), "
		"BI_261 CHAR(1), BI_262 CHAR(1), BI_263 CHAR(1), BI_264 CHAR(1), BI_265 CHAR(1), "
		"BI_266 CHAR(1), BI_267 CHAR(1), BI_268 CHAR(1), BI_269 CHAR(1), BI_270 CHAR(1), "
		"BI_271 CHAR(1), BI_272 CHAR(1), BI_273 CHAR(1), BI_274 CHAR(1), BI_275 CHAR(1), "
		"BI_276 CHAR(1), BI_277 CHAR(1), BI_278 CHAR(1), BI_279 CHAR(1), BI_280 CHAR(1), "
		"BI_281 CHAR(1), BI_282 CHAR(1), BI_283 CHAR(1), BI_284 CHAR(1), BI_285 CHAR(1), "
		"BI_286 CHAR(1), BI_287 CHAR(1), BI_288 CHAR(1), BI_289 CHAR(1), BI_290 CHAR(1), "
		"BI_291 CHAR(1), BI_292 CHAR(1), BI_293 CHAR(1), BI_294 CHAR(1), BI_295 CHAR(1), "
		"BI_296 CHAR(1), BI_297 CHAR(1), BI_298 CHAR(1), BI_299 CHAR(1), BI_300 CHAR(1), "
		"BI_301 CHAR(1), BI_302 CHAR(1), BI_303 CHAR(1), BI_304 CHAR(1), BI_305 CHAR(1), "
		"BI_306 CHAR(1), BI_307 CHAR(1), BI_308 CHAR(1), BI_309 CHAR(1), BI_310 CHAR(1), "
		"BI_311 CHAR(1), BI_312 CHAR(1), BI_313 CHAR(1), BI_314 CHAR(1), BI_315 CHAR(1), "
		"BI_316 CHAR(1), BI_317 CHAR(1), BI_318 CHAR(1), BI_319 CHAR(1), BI_320 CHAR(1), "
		"BI_321 FIXED(2) DEFAULT 1, BI_322 FIXED(2) DEFAULT 1, BI_323 FIXED(2) DEFAULT 1, "
		"BI_324 FIXED(2) DEFAULT 1, BI_325 FIXED(2) DEFAULT 1, BI_326 FIXED(2) DEFAULT 1, "
		"BI_327 FIXED(2) DEFAULT 1, BI_328 FIXED(2) DEFAULT 1, BI_329 FIXED(2) DEFAULT 1, "
		"BI_330 FIXED(2) DEFAULT 1, BI_331 FIXED(2) DEFAULT 1, BI_332 FIXED(2) DEFAULT 1, "
		"BI_333 FIXED(2) DEFAULT 1, BI_334 FIXED(2) DEFAULT 1, BI_335 FIXED(2) DEFAULT 1, "
		"BI_336 FIXED(2) DEFAULT 1, BI_337 FIXED(2) DEFAULT 1, BI_338 FIXED(2) DEFAULT 1, "
		"BI_339 FIXED(2) DEFAULT 1, BI_340 FIXED(2) DEFAULT 1, BI_341 FIXED(2) DEFAULT 1, "
		"BI_342 FIXED(2) DEFAULT 1, BI_343 FIXED(2) DEFAULT 1, BI_344 FIXED(2) DEFAULT 1, "
		"BI_345 FIXED(2) DEFAULT 1, BI_346 FIXED(2) DEFAULT 1, BI_347 FIXED(2) DEFAULT 1, "
		"BI_348 FIXED(2) DEFAULT 1, BI_349 FIXED(2) DEFAULT 1, BI_350 FIXED(2) DEFAULT 1, "
		"BI_351 FIXED(2), BI_352 FIXED(2), BI_353 FIXED(2), BI_354 FIXED(2), BI_355 FIXED(2), "
		"BI_356 FIXED(2), BI_357 FIXED(2), BI_358 FIXED(2), BI_359 FIXED(2), BI_360 FIXED(2), "
		"BI_361 FIXED(2), BI_362 FIXED(2), BI_363 FIXED(2), BI_364 FIXED(2), BI_365 FIXED(2), "
		"BI_366 FIXED(2), BI_367 FIXED(2), BI_368 FIXED(2), BI_369 FIXED(2), BI_370 FIXED(2), "
		"BI_371 FIXED(2), BI_372 FIXED(2), BI_373 FIXED(2), BI_374 FIXED(2), BI_375 FIXED(2), "
		"BI_376 FIXED(2), BI_377 FIXED(2), BI_378 FIXED(2), BI_379 FIXED(2), BI_380 FIXED(2), "
		"BI_381 FIXED(2), BI_382 FIXED(2), BI_383 FIXED(2), BI_384 FIXED(2), BI_385 FIXED(2),"
		"I_386 VARCHAR(5) DEFAULT 'a', I_387 VARCHAR(5) DEFAULT 'a', I_388 VARCHAR(5) DEFAULT 'a', "
		"I_389 VARCHAR(5) DEFAULT 'a', I_390 VARCHAR(5) DEFAULT 'a', I_391 VARCHAR(5) DEFAULT 'a', "
		"I_392 VARCHAR(5) DEFAULT 'a', I_393 VARCHAR(5) DEFAULT 'a', I_394 VARCHAR(5) DEFAULT 'a', "
		"I_395 VARCHAR(5) DEFAULT 'a', I_396 VARCHAR(5) DEFAULT 'a', I_397 VARCHAR(5) DEFAULT 'a', "
		"I_398 VARCHAR(5) DEFAULT 'a', I_399 VARCHAR(5) DEFAULT 'a', I_400 VARCHAR(5) DEFAULT 'a', "
		"I_401 VARCHAR(5) DEFAULT 'a', I_402 VARCHAR(5) DEFAULT 'a', I_403 VARCHAR(5) DEFAULT 'a', "
		"I_404 VARCHAR(5) DEFAULT 'a', I_405 VARCHAR(5) DEFAULT 'a', I_406 VARCHAR(5) DEFAULT 'a', "
		"I_407 VARCHAR(5) DEFAULT 'a', I_408 VARCHAR(5) DEFAULT 'a', I_409 VARCHAR(5) DEFAULT 'a', "
		"I_410 VARCHAR(5) DEFAULT 'a', I_411 VARCHAR(5) DEFAULT 'a', I_412 VARCHAR(5) DEFAULT 'a', "
		"I_413 VARCHAR(5) DEFAULT 'a', I_414 VARCHAR(5) DEFAULT 'a', I_415 VARCHAR(5) DEFAULT 'a', "
		"I_416 VARCHAR(5) DEFAULT 'a', I_417 VARCHAR(5) DEFAULT 'a', I_418 VARCHAR(5) DEFAULT 'a', "
		"I_419 VARCHAR(5) DEFAULT 'a', I_420 VARCHAR(5) DEFAULT 'a', I_421 VARCHAR(5) DEFAULT 'a', "
		"I_422 VARCHAR(5) DEFAULT 'a', I_423 VARCHAR(5) DEFAULT 'a', I_424 VARCHAR(5) DEFAULT 'a', "
		"I_425 VARCHAR(5) DEFAULT 'a', I_426 VARCHAR(5) DEFAULT 'a', I_427 VARCHAR(5) DEFAULT 'a', "
		"I_428 VARCHAR(5) DEFAULT 'a', I_429 VARCHAR(5) DEFAULT 'a', I_430 VARCHAR(5) DEFAULT 'a', "
		"I_431 VARCHAR(5) DEFAULT 'a', I_432 VARCHAR(5) DEFAULT 'a', I_433 VARCHAR(5) DEFAULT 'a', "
		"I_434 VARCHAR(5) DEFAULT 'a', I_435 VARCHAR(5) DEFAULT 'a', I_436 VARCHAR(5) DEFAULT 'a', "
		"I_437 VARCHAR(5) DEFAULT 'a', I_438 VARCHAR(5) DEFAULT 'a', I_439 VARCHAR(5) DEFAULT 'a', "
		"I_440 VARCHAR(5) DEFAULT 'a', I_441 VARCHAR(5) DEFAULT 'a', I_442 VARCHAR(5) DEFAULT 'a', "
		"I_443 VARCHAR(5) DEFAULT 'a', I_444 VARCHAR(5) DEFAULT 'a', I_445 VARCHAR(5) DEFAULT 'a', "
		"I_446 VARCHAR(5), I_447 VARCHAR(5), I_448 VARCHAR(5), I_449 VARCHAR(5), I_450 VARCHAR(5), "
		"I_451 VARCHAR(5), I_452 VARCHAR(5), I_453 VARCHAR(5), I_454 VARCHAR(5), I_455 VARCHAR(5), "
		"I_456 VARCHAR(5), I_457 VARCHAR(5), I_458 VARCHAR(5), I_459 VARCHAR(5), I_460 VARCHAR(5), "
		"I_461 VARCHAR(5), I_462 VARCHAR(5), I_463 VARCHAR(5), I_464 VARCHAR(5), I_465 VARCHAR(5), "
		"I_466 VARCHAR(5), I_467 VARCHAR(5), I_468 VARCHAR(5), I_469 VARCHAR(5), I_470 VARCHAR(5), "
		"I_471 VARCHAR(5), I_472 VARCHAR(5), I_473 VARCHAR(5), I_474 VARCHAR(5), I_475 VARCHAR(5), "
		"I_476 VARCHAR(5), I_477 VARCHAR(5), I_478 VARCHAR(5), I_479 VARCHAR(5), I_480 VARCHAR(5), "
		"I_481 VARCHAR(5), I_482 VARCHAR(5), I_483 VARCHAR(5), I_484 VARCHAR(5), I_485 VARCHAR(5), "
		"I_486 VARCHAR(5), I_487 VARCHAR(5), I_488 VARCHAR(5), I_489 VARCHAR(5), I_490 VARCHAR(5), "
		"I_491 VARCHAR(5), I_492 VARCHAR(5), I_493 VARCHAR(5), I_494 VARCHAR(5), I_495 VARCHAR(5), "
		"I_496 VARCHAR(5), I_497 VARCHAR(5), I_498 VARCHAR(5), I_499 VARCHAR(5), I_500 VARCHAR(5))") ;

	CE("main: CREATE TABLE TAB1", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE TABLE TAB2\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB2"
		"(I3_2 FIXED (10) KEY, "
		"I4    FIXED (10), "
		"C3    CHAR (20) NOT NULL)");
	CE("main: CREATE TABLE TAB2", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE TAB3 ... DYNAMIC\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB3"
		"(I5    FIXED (10) KEY ,"
		 "I6     FIXED (10) KEY ,"
		 "C4     CHAR(20) NOT NULL ,"
		 "C5     CHAR(100) NOT NULL ) DYNAMIC");
	CE("main: CREATE TABLE TAB3 ... DYNAMIC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE TABLE TAB4 ... DYNAMIC\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB4"
		"(I3     FIXED (10) KEY ,"
		 "I4     FIXED (10) ,"
		 "C3     CHAR(20) NOT NULL ) DYNAMIC");
	CE("main: CREATE TABLE TAB4 ... DYNAMIC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE TAB1SL\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB1SL"
        "(NRLL     FIXED (10) ,"
         "LNLL     FIXED (10) ,"
         "CLLL     LONG )");
	CE("main: CREATE TABLE TAB1SL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE TAB20K\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB20K"
		"(VC1 VARCHAR(5) KEY, C2 CHAR(5) KEY, I3 FIXED(5) KEY, "
		 "VC4 VARCHAR(5) KEY  DEFAULT ' ', C5 CHAR(5) KEY DEFAULT ' ', "
		 "I6 FIXED(5) KEY DEFAULT 1, "
		 "VC7 VARCHAR(5) KEY, C8 CHAR(5) KEY, I9 FIXED(5) KEY, "
		 "VC10 VARCHAR(5) KEY, C11 CHAR(5) KEY, I12 FIXED(5) KEY, "
		 "VC13 VARCHAR(5) KEY, C14 CHAR(5) KEY, I15 FIXED(5) KEY, "
		 "VC16 VARCHAR(5) KEY, C17 CHAR(5) KEY, I18 FIXED(5) KEY, "
		 "VC19 VARCHAR(400) KEY, C20 CHAR(400) KEY, "
		 "MYPID INTEGER KEY DEFAULT 0, MYTIME INTEGER KEY DEFAULT 0,"
		 "I21 FIXED(5) DEFAULT 1, C22 CHAR(5), VC23 VARCHAR(5), C24 CHAR(5), C25 CHAR(5), "
		 "I26 FIXED(5) DEFAULT 1, C27 CHAR(400), VC28 VARCHAR(400), C29 CHAR(5), C30 CHAR(5), "
		 "I31 FIXED(5), C32 CHAR(5), VC33 VARCHAR(5), C34 CHAR(5), C35 CHAR(5), "
		 "I36 FIXED(5), C37 CHAR(5), VC38 VARCHAR(5), C39 CHAR(5), C40 CHAR(5))");
	CE("main: CREATE TABLE TAB20K", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE TAB04SK\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB04SK"
		"(VC1 CHAR(1) BYTE KEY, C2 CHAR(1) BYTE KEY, I3 FIXED(1) KEY, "
		 "VC4 CHAR(1) KEY  DEFAULT ' ', C5 CHAR(5)  DEFAULT ' ', "
		 "I6 FIXED(5)  DEFAULT 1, "
		 "VC7 VARCHAR(5) , C8 CHAR(5) , I9 FIXED(5) , "
		 "VC10 VARCHAR(5) , C11 CHAR(5) , I12 FIXED(5) , "
		 "VC13 VARCHAR(5) , C14 CHAR(5) , I15 FIXED(5) , "
		 "VC16 VARCHAR(5) , C17 CHAR(5) , I18 FIXED(5) , "
		 "VC19 VARCHAR(5) , C20 CHAR(5),"
		 "MYPID INTEGER  DEFAULT 0, MYTIME INTEGER  DEFAULT 0,"
		 "I21 FIXED(5) DEFAULT 1, C22 CHAR(5), VC23 VARCHAR(5), C24 CHAR(5), C25 CHAR(5), "
		 "I26 FIXED(5) DEFAULT 1, C27 CHAR(5), VC28 VARCHAR(5), C29 CHAR(5), C30 CHAR(5), "
		 "I31 FIXED(5), C32 CHAR(5), VC33 VARCHAR(5), C34 CHAR(5), C35 CHAR(5), "
		 "I36 FIXED(5), C37 CHAR(5), VC38 VARCHAR(5), C39 CHAR(5), C40 CHAR(5) )");
	CE("main: CREATE TABLE TAB04SK", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE MAXRAW\n" ) ;
	returncode = stmt->execute("CREATE TABLE MAXRAW"
		"(MAXR 	FIXED (10) NOT NULL)");
	CE("main: CREATE TABLE MAXRAW", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	printf ("GRANT ... ON MAXRAW\n" ) ;
	returncode = stmt->execute("GRANT SELECT ON MAXRAW TO PUBLIC");
	CE("main: GRANT ... ON MAXRAW", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT ... ON TAB1\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB1 TO PUBLIC");
	CE("main: GRANT ... ON TAB1", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT ... ON TAB2\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB2 TO PUBLIC");
	CE("main: GRANT ... ON TAB2", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB1SL\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB1SL TO PUBLIC");
	CE("main: GRANT ... ON TAB1SL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT ... ON TAB4\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB4 "
		      "TO A1,A2,A3,A4,A5,A6,A7,A8,A9 "
		      "WITH GRANT OPTION");
	CE("main: GRANT ... ON TAB4", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB20K\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB20K TO PUBLIC");
	CE("main: GRANT ... ON TAB20K", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB04SK\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB04SK TO PUBLIC");
	CE("main: GRANT ... ON TAB04SK", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	printf ("INSERT TAB2\n") ;
	returncode = checkPreparedStatement(pstmt, "INSERT TAB2 VALUES ( ? , ? , ? )");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	for ( i = 0 ; i < rows / 10 + 1 ; i++ )
	{
		i3 = i ;
		i4 = rows - i ;
		sprintf ( c3 , "tab2%d" , i3 ) ;
			SQLDBC_Length indi1=sizeof(i3), indi2=sizeof(i4), indi3=SQLDBC_NTS  ;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i3, &indi1, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i4, &indi2, sizeof(i4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c3, &indi3, sizeof(c3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("main: INSERT TAB2 ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB1 , maxrows = %d\n",rows ) ;
    memset ( k0 , 'x' , 240 ) ;
    k0[239] = 0 ;
    memset ( c5 , 'y' , 1500 ) ;
    c5 [1499] = 0 ;

	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB1 (K0, K1, I1, I2, C1, C2, I3, CLLEN, CF0, CLB) "
		"VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	for ( insert_count = 1 , i = 1 ; i < rows ; i += 3 , insert_count++ )
	{
		k1 = i ;
		sprintf ( c1 , "Name_%d" , i ) ;
		sprintf ( c2 , "Name.%d" , i / 10 ) ;
		i1 = k1 * 100 ;
		i2 = k1 / 10 ;
		i3 = i / 10 ;
		memset ( c5 , 'y' , 1500 ) ;
		c5 [(rand()%1250)+250] = 0 ;
		cll.len = rand () % maxlongraw ;
		cll_length = cll.len ;
		memset ( cll.arr , '0' , cll.len ) ;
		cll.arr[0] = ( cll.len >> 8 ) & 0x7f  ;
		cll.arr[1] = cll.len & 0x7f ;
		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(k1), indi3=sizeof(i1),
			indi4=sizeof(i2), indi5=SQLDBC_NTS, indi6=SQLDBC_NTS, indi7=sizeof(i3),
			indi8=sizeof(cll_length), indi9=SQLDBC_NTS, indi10=cll.len;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &k0, &indi1, sizeof(k0));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &k1, &indi2, sizeof(k1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i1, &indi3, sizeof(i1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &i2, &indi4, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c1, &indi5, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &c2, &indi6, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 9)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_INT4, &i3, &indi7, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 10)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_INT4, &cll_length, &indi8, sizeof(cll_length));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 11)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (9, SQLDBC_HOSTTYPE_ASCII, &c5, &indi9, sizeof(c5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 12)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (10, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &indi10, sizeof(cll.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 13)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("main: INSERT TAB1 ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		if ( insert_count > 200 )
		{
			printf ("Commit\n");
			commit();
			insert_count = 0 ;
		}
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB20K\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB20K (VC1, C2, VC4, C5, VC7, C8, VC10, C11, VC13, C14, VC16, C17, "
		"C22, VC23, C24, C25, C29, C30, C32, VC33, C34, C35, C37, VC38, C39, C40, VC19, C20, C27, VC28, "
		"I3, I9, I15, I21, I6, I12, I18, I26, I31, I36) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	for ( i = 0 ; i < rows ; i += 3  )
	{
		i3 = i ;
		i4 = rows - i ;
		sprintf ( char5 , "%d" , i3 ) ;
		memset ( char400, (char)('a'+rand()%25), 399) ;
		char400[399] = (char) 0 ;

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=SQLDBC_NTS, indi3=sizeof(i3),	indi4=sizeof(i4);

		//bind for char5
		for (j = 1; j < 27; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_ASCII, &char5, &indi1, sizeof(char5));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 14)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		//bind for char400
		for (j = 27; j < 31; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 15)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		//bind for i3
		for (j = 31; j < 35; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_INT4, &i3, &indi3, sizeof(i3));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 16)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		//bind for i4
		for (j = 35; j < 41; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_INT4, &i4, &indi4, sizeof(i4));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 17)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}

		returncode = pstmt->execute();
		CE("main: INSERT TAB20K ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		j = 0;
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB04SK\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB04SK (VC1, C2, VC4, C5, VC7, C8, VC10, C11, VC13, C14, VC16, C17, "
		"VC19, C20, C22, VC23, C24, C25, C27, VC28, C29, C30, C32, VC33, C34, C35, C37, VC38, C39, C40, "
		"I3, I9, I15, I21, I6, I12, I18, I26, I31, I36) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
		"?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	char1a[1] = char1b[1] = char1c[1] = (char) 0 ;
	for ( i = 0 ; i < rows ; i++ )
	{
		i3 = ( i >> 16 ) & 0xff ;
		i4 = rows - i ;
		char1a[0] = ( i ) & 0xff ;
		char1b[0] = ( ( i >> 8 ) ) & 0xff ;
		char1c[0] = ( 'a' + (char) (i%13) ) ;
		sprintf ( char5 , "%d" , i3 ) ;

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=SQLDBC_NTS, indi3=SQLDBC_NTS,	indi4=SQLDBC_NTS, indi5=sizeof(i3), indi6=sizeof(i4);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char1a, &indi1, sizeof(char1a));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 18)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &char1b, &indi2, sizeof(char1b));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 19)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &char1c, &indi3, sizeof(char1c));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 20)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		//bind for char5
		for (j = 4; j < 31; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_ASCII, &char5, &indi4, sizeof(char5));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 21)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		//bind for i3
		for (j = 31; j < 35; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_INT4, &i3, &indi5, sizeof(i3));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 22)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		//bind for i4
		for (j = 35; j < 41; j++)
		{
			returncode = pstmt->bindParameter (j, SQLDBC_HOSTTYPE_INT4, &i4, &indi6, sizeof(i4));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 23)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		}
		returncode = pstmt->execute();
		CE("main: INSERT TAB04SK ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		j = 0;
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB1SL\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB1SL VALUES (?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	memset ( scl.arr , '1' , 100000 ) ;
	scl.arr [99999] = (char) 0 ;
	scl.arr [0] = scl.arr [99998] = 'j' ;
	ln = scl.len = 99999 ;
	for ( nrl = 0 ; nrl < 20 ; nrl++ )
	{
		SQLDBC_Length indi1=sizeof(long), indi2=sizeof(long), indi3=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nrl, &indi1, sizeof(nrl));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 24)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &ln, &indi2, sizeof(ln));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 25)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &scl.arr, &indi3, sizeof(scl.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 26)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE("main: INSERT TAB1SL ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("ALTER TABLE TAB1 DROP PRIMARY KEY\n" ) ;
	returncode = stmt->execute("ALTER TABLE TAB1 DROP PRIMARY KEY");
	CE("main: ALTER TABLE TAB1 DROP PRIMARY KEY", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("ALTER TABLE TAB1 ADD PRIMARY KEY (K0,K1)\n" ) ;
	returncode = stmt->execute("ALTER TABLE TAB1 ADD PRIMARY KEY (K0,K1)");
	CE("main: ALTER TABLE TAB1 ADD PRIMARY KEY (K0,K1)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("ALTER TABLE TAB1 FOREIGN KEY ...\n" ) ;
	returncode = stmt->execute("ALTER TABLE TAB1 FOREIGN KEY LLLL ( I3 ) REFERENCES TAB2 ON DELETE CASCADE");
	CE("main: ALTER TABLE TAB1 FOREIGN KEY ...", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW TABV1\n" ) ;
	returncode = stmt->execute("CREATE VIEW TABV1 AS "
            "SELECT K0 , K1 , I1 , I2 , C1 , C2 , I3 , I4 , C3 "
            "FROM TAB1 , TAB2 "
            "WHERE TAB1.I3 = TAB2.I3_2 "
            "WITH CHECK OPTION");
	CE("main: CREATE VIEW TABV1", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TABV1\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,DELETE ON TABV1 TO PUBLIC");
	CE("main: GRANT ... ON TABV1", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	returncode = conn->commit();
	CE("main: Commit", 1, conn->ERRORCODE, conn->ERRORTEXT);

	printf ("CREATE INDEX TAB1_K1 ON TAB1(K1)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB1_K1 ON TAB1(K1)");
	CE("main: CREATE INDEX TAB1_K1 ON TAB1(K1)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB1_I3 ON TAB1(I3)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB1_I3 ON TAB1(I3)");
	CE("main: CREATE INDEX TAB1_I3 ON TAB1(I3)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX SIDX ON TAB1 (BI_321)\n" ) ;
	returncode = stmt->execute("CREATE INDEX SIDX ON TAB1 (BI_321)");
	CE("main: CREATE INDEX SIDX ON TAB1 (BI_321)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX MULTIDX ON TAB1 (C1,I3)\n" ) ;
	returncode = stmt->execute("CREATE INDEX MULTIDX ON TAB1 (C1,I3)");
	CE("main: CREATE INDEX MULTIDX ON TAB1 (C1,I3)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX MULTIDX2 ON TAB1 (BI_321,I_386)\n" ) ;
	returncode = stmt->execute("CREATE INDEX MULTIDX2 ON TAB1 (BI_321,I_386)");
	CE("main: CREATE INDEX MULTIDX2 ON TAB1 (BI_321,I_386)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX MULTIDX3 ON TAB1 ( BI_324, BI_325, BI_311, BI_313 ...)\n" ) ;
	returncode = stmt->execute("CREATE INDEX MULTIDX3 ON TAB1 (BI_324, BI_325, BI_311, BI_313 , "
		"BI_314, BI_315, BI_316 , BI_317 , BI_318 , BI_319 , "
		"BI_320 , BI_321 , K1 , BI_323 , BI_326 , I3)");
	CE("main: CREATE INDEX MULTIDX3 ON TAB1 ( BI_324, BI_325, BI_311, BI_313 ...)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX SIDX20K ON TAB20K (I36 DESC)\n" ) ;
	returncode = stmt->execute("CREATE INDEX SIDX20K ON TAB20K (I36 DESC)");
	CE("main: CREATE INDEX SIDX20K ON TAB20K (I36 DESC)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB20K_MYPID ON TAB20K (MYPID,MYTIME,I21)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB20K_MYPID ON TAB20K (MYPID,MYTIME,I21)");
	CE("main: CREATE INDEX TAB20K_MYPID ON TAB20K (MYPID,MYTIME,I21)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE UNIQUE INDEX UIDX20K ON TAB20K (I21 ...)\n" ) ;
	returncode = stmt->execute("CREATE UNIQUE INDEX UIDX20K ON TAB20K ("
			"I21, C22, VC23, C24, C25, I26, C27, VC28, C29, C30, "
			"I31, C32, VC33, C34, C35)");
	CE("main: CREATE UNIQUE INDEX UIDX20K ON TAB20K (I21 ...)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB20K_VC19S ON TAB20K (VC19)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB20K_VC19S ON TAB20K (VC19)");
	CE("main: CREATE INDEX TAB20K_VC19S ON TAB20K (VC19)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB20K_VC19M ON TAB20K (VC19,C20)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB20K_VC19M ON TAB20K (VC19,C20)");
	CE("main: CREATE INDEX TAB20K_VC19M ON TAB20K (VC19,C20)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB20K_C27M ON TAB20K (C27,C20)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB20K_C27M ON TAB20K (C27,C20)");
	CE("main: CREATE INDEX TAB20K_C27M ON TAB20K (C27,C20)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX MIDX04SK ON TAB04SK (I36,I18,VC1,C2)\n" ) ;
	returncode = stmt->execute("CREATE INDEX MIDX04SK ON TAB04SK (I36,I18,VC1,C2)");
	CE("main: CREATE INDEX MIDX04SK ON TAB04SK (I36,I18,VC1,C2)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX SIDX04SK_I3 ON TAB04SK (I3)\n" ) ;
	returncode = stmt->execute("CREATE INDEX SIDX04SK_I3 ON TAB04SK (I3)");
	CE("main: CREATE INDEX SIDX04SK_I3 ON TAB04SK (I3)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX SIDX04SK_VC4 ON TAB04SK (VC4)\n" ) ;
	returncode = stmt->execute("CREATE INDEX SIDX04SK_VC4 ON TAB04SK (VC4)");
	CE("main: CREATE INDEX SIDX04SK_VC4 ON TAB04SK (VC4)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE UNIQUE INDEX TAB1SL_0 ON TAB1SL (NRLL DESC)\n" ) ;
	returncode = stmt->execute("CREATE UNIQUE INDEX TAB1SL_0 ON TAB1SL (NRLL DESC)");
	CE("main: CREATE UNIQUE INDEX TAB1SL_0 ON TAB1SL (NRLL DESC)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB1SL_1 ON TAB1SL (LNLL)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB1SL_1 ON TAB1SL (LNLL)");
	CE("main: CREATE INDEX TAB1SL_1 ON TAB1SL (LNLL)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE INDEX TAB1SL_2 ON TAB1SL (LNLL DESC,NRLL)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB1SL_2 ON TAB1SL (LNLL DESC,NRLL)");
	CE("main: CREATE INDEX TAB1SL_2 ON TAB1SL (LNLL DESC,NRLL)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	printf ("INSERT MAXRAW\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT MAXRAW VALUES (?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(maxlongraw);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &maxlongraw, &indi1, sizeof(maxlongraw));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 27)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("main: INSERT MAXRAW ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB4\n") ;
	returncode = stmt->execute("INSERT TAB4 SELECT * FROM Tab2");
	CE("main: INSERT TAB4", 1, conn->ERRORCODE, conn->ERRORTEXT);

    printf ("GRANT ... ON TAB4\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB4 TO PUBLIC");
	CE("main: GRANT ... ON TAB4", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	for ( i = 1 ; i <= 64 ; i++ )
	{
		printf ("DROP/CREATE/INSERT TABJ_%d\n", i ) ;

		sprintf (st,"DROP TABLE TAB3J_%d",i ) ;
		returncode = stmt->execute(st);
		CE(st, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
		sprintf (st,"CREATE TABLE TAB3J_%d LIKE TAB3",i ) ;
		returncode = stmt->execute(st);
		CE(st, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
		sprintf (st,"GRANT SELECT,UPDATE,INSERT,DELETE ON TAB3J_%d TO PUBLIC",i ) ;
		returncode = stmt->execute(st);
		CE(st, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
		sprintf (st,"INSERT TAB3J_%d SELECT I3,I4,C3,C3 FROM TAB4",i ) ;
		returncode = stmt->execute(st);
		CE(st, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
		sprintf (st,"CREATE INDEX ITAB3J_%d ON TAB3J_%d (C4)",i,i) ;
		returncode = stmt->execute(st);
		CE(st, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
	}

	printf ("DROP VIEW BVIEW_K_P\n") ;
	returncode = stmt->execute("DROP VIEW BVIEW_K_P");
	CE("main: DROP VIEW BVIEW_K_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW BVIEW_K_P\n") ;
	returncode = stmt->execute("CREATE VIEW BVIEW_K_P "
		"AS SELECT T1.I5, T1.I6, T1.C4, T4.C5 "
		"FROM "
			"TAB3J_1 T1, TAB3J_2 T2, TAB3J_3 T3, TAB3J_4 T4, TAB3J_5 T5, TAB3J_6 T6, "
			"TAB3J_7 T7, TAB3J_8 T8, TAB3J_9 T9, TAB3J_10 T10, TAB3J_11 T11, "
			"TAB3J_12 T12, TAB3J_13 T13, TAB3J_14 T14, TAB3J_15 T15, TAB3J_16 T16, "
			"TAB3J_17 T17, TAB3J_18 T18, TAB3J_19 T19, TAB3J_20 T20, TAB3J_21 T21, "
			"TAB3J_22 T22, TAB3J_23 T23, TAB3J_24 T24, TAB3J_25 T25, TAB3J_26 T26, "
			"TAB3J_27 T27, TAB3J_28 T28, TAB3J_29 T29, TAB3J_30 T30, TAB3J_31 T31, "
			"TAB3J_32 T32, TAB3J_33 T33, TAB3J_34 T34, TAB3J_35 T35, TAB3J_36 T36, "
			"TAB3J_37 T37, TAB3J_38 T38, TAB3J_39 T39, TAB3J_40 T40, TAB3J_41 T41, "
			"TAB3J_42 T42, TAB3J_43 T43, TAB3J_44 T44, TAB3J_45 T45, TAB3J_46 T46, "
			"TAB3J_47 T47, TAB3J_48 T48, TAB3J_49 T49, TAB3J_50 T50, TAB3J_51 T51, "
			"TAB3J_52 T52, TAB3J_53 T53, TAB3J_54 T54, TAB3J_55 T55, TAB3J_56 T56, "
			"TAB3J_57 T57, TAB3J_58 T58, TAB3J_59 T59, TAB3J_60 T60, TAB3J_61 T61, "
			"TAB3J_62 T62, TAB3J_63 T63, TAB3J_64 T64 "
		"WHERE "
			"T2.I5 = T1.I5 AND T3.I5 = T2.I5 AND T4.I5 = T3.I5 AND T5.I5 = T4.I5 AND "
			"T6.I5 = T5.I5 AND T7.I5 = T6.I5 AND T8.I5 = T7.I5 AND "
			"T9.I5 = T8.I5 AND T10.I5 = T9.I5 AND T11.I5 = T10.I5 AND "
			"T12.I5 = T11.I5 AND T13.I5 = T12.I5 AND T14.I5 = T13.I5 AND "
			"T15.I5 = T14.I5 AND T16.I5 = T15.I5 AND T17.I5 = T16.I5 AND "
			"T18.I5 = T17.I5 AND T19.I5 = T18.I5 AND T20.I5 = T19.I5 AND "
			"T21.I5 = T20.I5 AND T22.I5 = T21.I5 AND T23.I5 = T22.I5 AND "
			"T24.I5 = T23.I5 AND T25.I5 = T24.I5 AND T26.I5 = T25.I5 AND "
			"T27.I5 = T26.I5 AND T28.I5 = T27.I5 AND T29.I5 = T28.I5 AND "
			"T30.I5 = T29.I5 AND T31.I5 = T30.I5 AND T32.I5 = T31.I5 AND "
			"T33.I5 = T32.I5 AND T34.I5 = T33.I5 AND T35.I5 = T34.I5 AND "
			"T36.I5 = T35.I5 AND T37.I5 = T36.I5 AND T38.I5 = T37.I5 AND "
			"T39.I5 = T38.I5 AND T40.I5 = T39.I5 AND T41.I5 = T40.I5 AND "
			"T42.I5 = T41.I5 AND T43.I5 = T42.I5 AND T44.I5 = T43.I5 AND "
			"T45.I5 = T44.I5 AND T46.I5 = T45.I5 AND T47.I5 = T46.I5 AND "
			"T48.I5 = T47.I5 AND T49.I5 = T48.I5 AND T50.I5 = T49.I5 AND "
			"T51.I5 = T50.I5 AND T52.I5 = T51.I5 AND T53.I5 = T52.I5 AND "
			"T54.I5 = T53.I5 AND T55.I5 = T54.I5 AND T56.I5 = T55.I5 AND "
			"T57.I5 = T56.I5 AND T58.I5 = T57.I5 AND T59.I5 = T58.I5 AND "
			"T60.I5 = T59.I5 AND T61.I5 = T60.I5 AND T62.I5 = T61.I5 AND "
			"T63.I5 = T62.I5 AND T64.I5 = T63.I5 AND "
			"T2.I6 = T1.I6 AND T3.I6 = T2.I6 AND T4.I6 = T3.I6 AND T5.I6 = T4.I6 AND "
			"T6.I6 = T5.I6 AND T7.I6 = T6.I6 AND T8.I6 = T7.I6 AND "
			"T9.I6 = T8.I6 AND T10.I6 = T9.I6 AND T11.I6 = T10.I6 AND "
			"T12.I6 = T11.I6 AND T13.I6 = T12.I6 AND T14.I6 = T13.I6 AND "
			"T15.I6 = T14.I6 AND T16.I6 = T15.I6 AND T17.I6 = T16.I6 AND "
			"T18.I6 = T17.I6 AND T19.I6 = T18.I6 AND T20.I6 = T19.I6 AND "
			"T21.I6 = T20.I6 AND T22.I6 = T21.I6 AND T23.I6 = T22.I6 AND "
			"T24.I6 = T23.I6 AND T25.I6 = T24.I6 AND T26.I6 = T25.I6 AND "
			"T27.I6 = T26.I6 AND T28.I6 = T27.I6 AND T29.I6 = T28.I6 AND "
			"T30.I6 = T29.I6 AND T31.I6 = T30.I6 AND T32.I6 = T31.I6 AND "
			"T33.I6 = T32.I6 AND T34.I6 = T33.I6 AND T35.I6 = T34.I6 AND "
			"T36.I6 = T35.I6 AND T37.I6 = T36.I6 AND T38.I6 = T37.I6 AND "
			"T39.I6 = T38.I6 AND T40.I6 = T39.I6 AND T41.I6 = T40.I6 AND "
			"T42.I6 = T41.I6 AND T43.I6 = T42.I6 AND T44.I6 = T43.I6 AND "
			"T45.I6 = T44.I6 AND T46.I6 = T45.I6 AND T47.I6 = T46.I6 AND "
			"T48.I6 = T47.I6 AND T49.I6 = T48.I6 AND T50.I6 = T49.I6 AND "
			"T51.I6 = T50.I6 AND T52.I6 = T51.I6 AND T53.I6 = T52.I6 AND "
			"T54.I6 = T53.I6 AND T55.I6 = T54.I6 AND T56.I6 = T55.I6 AND "
			"T57.I6 = T56.I6 AND T58.I6 = T57.I6 AND T59.I6 = T58.I6 AND "
			"T60.I6 = T59.I6 AND T61.I6 = T60.I6 AND T62.I6 = T61.I6 AND "
			"T63.I6 = T62.I6 AND T64.I6 = T63.I6");
	CE("main: CREATE VIEW BVIEW_K_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON BVIEW_K_P TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON BVIEW_K_P TO PUBLIC");
	CE("main: GRANT SELECT ON BVIEW_K_P TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("DROP VIEW BVIEW_I_P\n") ;
	returncode = stmt->execute("DROP VIEW BVIEW_I_P");
	CE("main: DROP VIEW BVIEW_I_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW BVIEW_I_P\n") ;
	returncode = stmt->execute("CREATE VIEW BVIEW_I_P "
		"AS SELECT T1.I5, T2.I6, T3.C4, T4.C5 "
		"FROM "
			"TAB3J_1 T1, TAB3J_2 T2, TAB3J_3 T3, TAB3J_4 T4, TAB3J_5 T5, TAB3J_6 T6, "
			"TAB3J_7 T7, TAB3J_8 T8, TAB3J_9 T9, TAB3J_10 T10, TAB3J_11 T11, "
			"TAB3J_12 T12, TAB3J_13 T13, TAB3J_14 T14, TAB3J_15 T15, TAB3J_16 T16, "
			"TAB3J_17 T17, TAB3J_18 T18, TAB3J_19 T19, TAB3J_20 T20, TAB3J_21 T21, "
			"TAB3J_22 T22, TAB3J_23 T23, TAB3J_24 T24, TAB3J_25 T25, TAB3J_26 T26, "
			"TAB3J_27 T27, TAB3J_28 T28, TAB3J_29 T29, TAB3J_30 T30, TAB3J_31 T31, "
			"TAB3J_32 T32, TAB3J_33 T33, TAB3J_34 T34, TAB3J_35 T35, TAB3J_36 T36, "
			"TAB3J_37 T37, TAB3J_38 T38, TAB3J_39 T39, TAB3J_40 T40, TAB3J_41 T41, "
			"TAB3J_42 T42, TAB3J_43 T43, TAB3J_44 T44, TAB3J_45 T45, TAB3J_46 T46, "
			"TAB3J_47 T47, TAB3J_48 T48, TAB3J_49 T49, TAB3J_50 T50, TAB3J_51 T51, "
			"TAB3J_52 T52, TAB3J_53 T53, TAB3J_54 T54, TAB3J_55 T55, TAB3J_56 T56, "
			"TAB3J_57 T57, TAB3J_58 T58, TAB3J_59 T59, TAB3J_60 T60, TAB3J_61 T61, "
			"TAB3J_62 T62, TAB3J_63 T63, TAB3J_64 T64 "
		"WHERE "
			"T2.C4 = T1.C4 AND T3.C4 = T2.C4 AND T4.C4 = T3.C4 AND T5.C4 = T4.C4 AND "
			"T6.C4 = T5.C4 AND T7.C4 = T6.C4 AND T8.C4 = T7.C4 AND "
			"T9.C4 = T8.C4 AND T10.C4 = T9.C4 AND T11.C4 = T10.C4 AND "
			"T12.C4 = T11.C4 AND T13.C4 = T12.C4 AND T14.C4 = T13.C4 AND "
			"T15.C4 = T14.C4 AND T16.C4 = T15.C4 AND T17.C4 = T16.C4 AND "
			"T18.C4 = T17.C4 AND T19.C4 = T18.C4 AND T20.C4 = T19.C4 AND "
			"T21.C4 = T20.C4 AND T22.C4 = T21.C4 AND T23.C4 = T22.C4 AND "
			"T24.C4 = T23.C4 AND T25.C4 = T24.C4 AND T26.C4 = T25.C4 AND "
			"T27.C4 = T26.C4 AND T28.C4 = T27.C4 AND T29.C4 = T28.C4 AND "
			"T30.C4 = T29.C4 AND T31.C4 = T30.C4 AND T32.C4 = T31.C4 AND "
			"T33.C4 = T32.C4 AND T34.C4 = T33.C4 AND T35.C4 = T34.C4 AND "
			"T36.C4 = T35.C4 AND T37.C4 = T36.C4 AND T38.C4 = T37.C4 AND "
			"T39.C4 = T38.C4 AND T40.C4 = T39.C4 AND T41.C4 = T40.C4 AND "
			"T42.C4 = T41.C4 AND T43.C4 = T42.C4 AND T44.C4 = T43.C4 AND "
			"T45.C4 = T44.C4 AND T46.C4 = T45.C4 AND T47.C4 = T46.C4 AND "
			"T48.C4 = T47.C4 AND T49.C4 = T48.C4 AND T50.C4 = T49.C4 AND "
			"T51.C4 = T50.C4 AND T52.C4 = T51.C4 AND T53.C4 = T52.C4 AND "
			"T54.C4 = T53.C4 AND T55.C4 = T54.C4 AND T56.C4 = T55.C4 AND "
			"T57.C4 = T56.C4 AND T58.C4 = T57.C4 AND T59.C4 = T58.C4 AND "
			"T60.C4 = T59.C4 AND T61.C4 = T60.C4 AND T62.C4 = T61.C4 AND "
			"T63.C4 = T62.C4 AND T64.C4 = T63.C4");
	CE("main: CREATE VIEW BVIEW_I_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON BVIEW_I_P TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON BVIEW_I_P TO PUBLIC");
	CE("main: GRANT SELECT ON BVIEW_I_P TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("DROP VIEW SBVIEW_I_P\n") ;
	returncode = stmt->execute("DROP VIEW SBVIEW_I_P");
	CE("main: DROP VIEW SBVIEW_I_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW SBVIEW_I_P\n") ;
	returncode = stmt->execute("CREATE VIEW SBVIEW_I_P "
	"AS SELECT T1.K0, T1.K1, T1.I1, T1.I2, T1.C1, "
				"T1.C2, T1.I3, T1.CLLEN, T1.CF0, T1.CLB "
	"FROM "
		"TAB1 T1, TAB1 T2, TAB1 T3, TAB1 T4, TAB1 T5, TAB1 T6, "
		"TAB1 T7, TAB1 T8, TAB1 T9, TAB1 T10, TAB1 T11, "
		"TAB1 T12, TAB1 T13, TAB1 T14, TAB1 T15, TAB1 T16, "
		"TAB1 T17, TAB1 T18, TAB1 T19, TAB1 T20, TAB1 T21, "
		"TAB1 T22, TAB1 T23, TAB1 T24, TAB1 T25, TAB1 T26, "
		"TAB1 T27, TAB1 T28, TAB1 T29, TAB1 T30, TAB1 T31, "
		"TAB1 T32, TAB1 T33, TAB1 T34, TAB1 T35, TAB1 T36, "
		"TAB1 T37, TAB1 T38, TAB1 T39, TAB1 T40, TAB1 T41, "
		"TAB1 T42, TAB1 T43, TAB1 T44, TAB1 T45, TAB1 T46, "
		"TAB1 T47, TAB1 T48, TAB1 T49, TAB1 T50, TAB1 T51, "
		"TAB1 T52, TAB1 T53, TAB1 T54, TAB1 T55, TAB1 T56, "
		"TAB1 T57, TAB1 T58, TAB1 T59, TAB1 T60, TAB1 T61, "
		"TAB1 T62, TAB1 T63, TAB1 T64 "
	"WHERE "
		"T2.K1 = T1.K1 AND T3.K1 = T2.K1 AND T4.K1 = T3.K1 AND T5.K1 = T4.K1 AND "
		"T6.K1 = T5.K1 AND T7.K1 = T6.K1 AND T8.K1 = T7.K1 AND "
		"T9.K1 = T8.K1 AND T10.K1 = T9.K1 AND T11.K1 = T10.K1 AND "
		"T12.K1 = T11.K1 AND T13.K1 = T12.K1 AND T14.K1 = T13.K1 AND "
		"T15.K1 = T14.K1 AND T16.K1 = T15.K1 AND T17.K1 = T16.K1 AND "
		"T18.K1 = T17.K1 AND T19.K1 = T18.K1 AND T20.K1 = T19.K1 AND "
		"T21.K1 = T20.K1 AND T22.K1 = T21.K1 AND T23.K1 = T22.K1 AND "
		"T24.K1 = T23.K1 AND T25.K1 = T24.K1 AND T26.K1 = T25.K1 AND "
		"T27.K1 = T26.K1 AND T28.K1 = T27.K1 AND T29.K1 = T28.K1 AND "
		"T30.K1 = T29.K1 AND T31.K1 = T30.K1 AND T32.K1 = T31.K1 AND "
		"T33.K1 = T32.K1 AND T34.K1 = T33.K1 AND T35.K1 = T34.K1 AND "
		"T36.K1 = T35.K1 AND T37.K1 = T36.K1 AND T38.K1 = T37.K1 AND "
		"T39.K1 = T38.K1 AND T40.K1 = T39.K1 AND T41.K1 = T40.K1 AND "
		"T42.K1 = T41.K1 AND T43.K1 = T42.K1 AND T44.K1 = T43.K1 AND "
		"T45.K1 = T44.K1 AND T46.K1 = T45.K1 AND T47.K1 = T46.K1 AND "
		"T48.K1 = T47.K1 AND T49.K1 = T48.K1 AND T50.K1 = T49.K1 AND "
		"T51.K1 = T50.K1 AND T52.K1 = T51.K1 AND T53.K1 = T52.K1 AND "
		"T54.K1 = T53.K1 AND T55.K1 = T54.K1 AND T56.K1 = T55.K1 AND "
		"T57.K1 = T56.K1 AND T58.K1 = T57.K1 AND T59.K1 = T58.K1 AND "
		"T60.K1 = T59.K1 AND T61.K1 = T60.K1 AND T62.K1 = T61.K1 AND "
		"T63.K1 = T62.K1 AND T64.K1 = T63.K1");
	CE("main: CREATE VIEW SBVIEW_I_P", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON SBVIEW_I_P TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON SBVIEW_I_P TO PUBLIC");
	CE("main: GRANT SELECT ON SBVIEW_I_P TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("DROP VIEW BVIEW_K_P_16\n") ;
	returncode = stmt->execute("DROP VIEW BVIEW_K_P_16");
	CE("main: DROP VIEW BVIEW_K_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW BVIEW_K_P_16\n") ;
	returncode = stmt->execute("CREATE VIEW BVIEW_K_P_16 "
		"AS SELECT T1.I5, T1.I6, T1.C4, T4.C5 "
		"FROM "
			"TAB3J_1 T1, TAB3J_2 T2, TAB3J_3 T3, TAB3J_4 T4, TAB3J_5 T5, TAB3J_6 T6, "
			"TAB3J_7 T7, TAB3J_8 T8, TAB3J_9 T9, TAB3J_10 T10, TAB3J_11 T11, "
			"TAB3J_12 T12, TAB3J_13 T13, TAB3J_14 T14, TAB3J_15 T15, TAB3J_16 T16 "
		"WHERE "
			"T2.I5 = T1.I5 AND T3.I5 = T2.I5 AND T4.I5 = T3.I5 AND T5.I5 = T4.I5 AND "
			"T6.I5 = T5.I5 AND T7.I5 = T6.I5 AND T8.I5 = T7.I5 AND "
			"T9.I5 = T8.I5 AND T10.I5 = T9.I5 AND T11.I5 = T10.I5 AND "
			"T12.I5 = T11.I5 AND T13.I5 = T12.I5 AND T14.I5 = T13.I5 AND "
			"T15.I5 = T14.I5 AND T16.I5 = T15.I5 AND "
			"T2.I6 = T1.I6 AND T3.I6 = T2.I6 AND T4.I6 = T3.I6 AND T5.I6 = T4.I6 AND "
			"T6.I6 = T5.I6 AND T7.I6 = T6.I6 AND T8.I6 = T7.I6 AND "
			"T9.I6 = T8.I6 AND T10.I6 = T9.I6 AND T11.I6 = T10.I6 AND "
			"T12.I6 = T11.I6 AND T13.I6 = T12.I6 AND T14.I6 = T13.I6 AND "
			"T15.I6 = T14.I6 AND T16.I6 = T15.I6");
	CE("main: CREATE VIEW BVIEW_K_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON BVIEW_K_P_16 TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON BVIEW_K_P_16 TO PUBLIC");
	CE("main: GRANT SELECT ON BVIEW_K_P_16 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("DROP VIEW BVIEW_I_P_16\n") ;
	returncode = stmt->execute("DROP VIEW BVIEW_I_P_16");
	CE("main: DROP VIEW BVIEW_I_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW BVIEW_I_P_16\n") ;
	returncode = stmt->execute("CREATE VIEW BVIEW_I_P_16 "
		"AS SELECT T1.I5, T2.I6, T3.C4, T4.C5 "
		"FROM "
			"TAB3J_1 T1, TAB3J_2 T2, TAB3J_3 T3, TAB3J_4 T4, TAB3J_5 T5, TAB3J_6 T6, "
			"TAB3J_7 T7, TAB3J_8 T8, TAB3J_9 T9, TAB3J_10 T10, TAB3J_11 T11, "
			"TAB3J_12 T12, TAB3J_13 T13, TAB3J_14 T14, TAB3J_15 T15, TAB3J_16 T16 "
		"WHERE "
			"T2.C4 = T1.C4 AND T3.C4 = T2.C4 AND T4.C4 = T3.C4 AND T5.C4 = T4.C4 AND "
			"T6.C4 = T5.C4 AND T7.C4 = T6.C4 AND T8.C4 = T7.C4 AND "
			"T9.C4 = T8.C4 AND T10.C4 = T9.C4 AND T11.C4 = T10.C4 AND "
			"T12.C4 = T11.C4 AND T13.C4 = T12.C4 AND T14.C4 = T13.C4 AND "
			"T15.C4 = T14.C4 AND T16.C4 = T15.C4");
	CE("main: CREATE VIEW BVIEW_I_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON BVIEW_I_P_16 TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON BVIEW_I_P_16 TO PUBLIC");
	CE("main: GRANT SELECT ON BVIEW_I_P_16 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("DROP VIEW SBVIEW_I_P_16\n") ;
	returncode = stmt->execute("DROP VIEW SBVIEW_I_P_16");
	CE("main: DROP VIEW SBVIEW_I_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE VIEW SBVIEW_I_P_16\n") ;
	returncode = stmt->execute("CREATE VIEW SBVIEW_I_P_16 "
		"AS SELECT T1.K0, T1.K1, T1.I1, T1.I2, T1.C1, "
			"T1.C2, T1.I3, T1.CLLEN, T1.CF0, T1.CLB "
		"FROM "
			"TAB1 T1, TAB1 T2, TAB1 T3, TAB1 T4, TAB1 T5, TAB1 T6, "
			"TAB1 T7, TAB1 T8, TAB1 T9, TAB1 T10, TAB1 T11, "
			"TAB1 T12, TAB1 T13, TAB1 T14, TAB1 T15, TAB1 T16 "
		"WHERE "
			"T2.K1 = T1.K1 AND T3.K1 = T2.K1 AND T4.K1 = T3.K1 AND T5.K1 = T4.K1 AND "
			"T6.K1 = T5.K1 AND T7.K1 = T6.K1 AND T8.K1 = T7.K1 AND "
			"T9.K1 = T8.K1 AND T10.K1 = T9.K1 AND T11.K1 = T10.K1 AND "
			"T12.K1 = T11.K1 AND T13.K1 = T12.K1 AND T14.K1 = T13.K1 AND "
			"T15.K1 = T14.K1 AND T16.K1 = T15.K1");
	CE("main: CREATE VIEW SBVIEW_I_P_16", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("GRANT SELECT ON SBVIEW_I_P_16 TO PUBLIC\n") ;
	returncode = stmt->execute("GRANT SELECT ON SBVIEW_I_P_16 TO PUBLIC");
	CE("main: GRANT SELECT ON SBVIEW_I_P_16 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	if (use_unicode == SQLDBC_TRUE)
		tab_unicode();

	if (use_cluster == 1)
		tab_cluster();

	update_stat();

	finish();

	return 0;
}
/*------------------------------------------------------------------------*/
void tab_unicode()
/*
	If database supports unicode ("putparam _UNICODE YES") unicode tables will be created, granted and inserted
*/
{
	printf("*****************\n");
	printf("UNICODE activated\n");

	printf ( "DROP TABLE TAB1UC\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB1UC");
	CE("main: DROP TABLE TAB1UC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ( "DROP TABLE TAB2UC\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB2UC");
	CE("main: DROP TABLE TAB2UC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("CREATE TABLE TAB1UC\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB1UC"
		"(CU1 CHAR(24) UNICODE, I2 FIXED (10) NOT NULL, I3 FIXED (10), "
		 "CU4 CHAR(5) UNICODE, I5 FIXED (5) DEFAULT 42, I6 FIXED (1), "
		 "VCU7 VARCHAR(500) UNICODE, VCU8 VARCHAR(3) UNICODE, VCU9 VARCHAR(1500) UNICODE)");
	CE("main: CREATE TABLE TAB1UC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("CREATE TABLE TAB2UC\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB2UC"
		"(C1 CHAR(24), I2 FIXED (10) NOT NULL, I3 FIXED (10), "
		 "CU4 CHAR(5) UNICODE, I5 FIXED (5) DEFAULT 69, I6 FIXED (1), "
		 "VC7 VARCHAR(500), VC8 VARCHAR(3), VCU9 VARCHAR(1500) UNICODE)");
	CE("main: CREATE TABLE TAB2UC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

    printf ("GRANT ... ON TAB1UC\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB1UC TO PUBLIC");
	CE("main: GRANT ... ON TAB1UC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB2UC\n" ) ;
	returncode = stmt->execute("GRANT SELECT,UPDATE,INSERT,DELETE ON TAB2UC TO PUBLIC");
	CE("main: GRANT ... ON TAB2UC", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	printf ("INSERT TAB1UC\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB1UC VALUES (?, ?, ?, ?, 42, ?, ?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	int y = 0;

	/* ATTENTION: currently MaxDB supports only Unicode characters with 2 byte, so something like 0x100ef is NOT POSSIBLE */
	for (y = 0; y < sizeof(uchar1)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar1[y] = (UCSCHAR)0x2603;	//Unicode 2603 = Schneemann-Symbol (Miscellaneous Symbol)
	}
	uchar1[sizeof(uchar1)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar2)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar2[y] = (UCSCHAR)0x06bf;	//Unicode 06bf = arabisches Schriftzeichen (Arabic)
	}
	uchar2[sizeof(uchar2)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar3)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar3[y] = (UCSCHAR)0x4e32;	//Unicode 4e32 = chin. Schriftzeichen (CJK Unified Ideographs)
	}
	uchar3[sizeof(uchar3)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar4)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar4[y] = (UCSCHAR)0x263a;	//Unicode 263a = Smiley-Symbol (Miscellaneous Symbol)
	}
	uchar4[sizeof(uchar4)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar5)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar5[y] = (UCSCHAR)0x0914;	//Unicode 0914 = Schriftsymbol (Devanagari)
	}
	uchar5[sizeof(uchar5)/sizeof(UCSCHAR)-1] = 0;

	for ( nrl = 0 ; nrl < (50 + (rand() % 500)) ; nrl++ )
	{
		i3 = rand() % 100;
		i6 = rand() % 2;

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(nrl) , indi3=sizeof(i3) , indi4=SQLDBC_NTS, 
			indi5=sizeof(i6), indi6=SQLDBC_NTS, indi7=SQLDBC_NTS, indi8=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar1, &indi1, sizeof(uchar1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 28)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &nrl, &indi2, sizeof(nrl));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 29)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i3, &indi3, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 30)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar2, &indi4, sizeof(uchar2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 31)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &i6, &indi5, sizeof(i6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 32)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar3, &indi6, sizeof(uchar3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 33)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar4, &indi7, sizeof(uchar4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 34)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar5, &indi8, sizeof(uchar5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 35)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE("main: INSERT TAB1UC ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("INSERT TAB2UC\n") ;
	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB2UC VALUES (?, ?, ?, ?, 69, ?, ?, ?, ?)");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	sprintf ( nouchar1 , "Schneemann-Symbol" ) ;

	/* ATTENTION: currently MaxDB supports only Unicode characters with 2 byte, so something like 0x100ef is NOT POSSIBLE */
	for (y = 0; y < sizeof(uchar2)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar2[y] = (UCSCHAR)0x06b7;	//Unicode 06b7 = arabisches Schriftzeichen (Arabic)
	}
	uchar2[sizeof(uchar2)/sizeof(UCSCHAR)-1] = 0;

	sprintf ( nouchar3 , "chin. Schriftzeichen" ) ;
	sprintf ( nouchar4 , ":-)" ) ;

	for (y = 0; y < sizeof(uchar5)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar5[y] = (UCSCHAR)0x0914;	//Unicode 0914 = Schriftsymbol (Devanagari)
	}
	uchar5[sizeof(uchar5)/sizeof(UCSCHAR)-1] = 0;

	for ( nrl = 0 ; nrl < (50 + (rand() % 200)) ; nrl++ )
	{
		i3 = rand() % 200;
		i6 = rand() % 10;

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(nrl) , indi3=sizeof(i3) , indi4=SQLDBC_NTS, 
			indi5=sizeof(i6), indi6=SQLDBC_NTS, indi7=SQLDBC_NTS, indi8=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &nouchar1, &indi1, sizeof(nouchar1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 36)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &nrl, &indi2, sizeof(nrl));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 37)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i3, &indi3, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 38)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_UCS2_NATIVE, uchar2, &indi4, sizeof(uchar2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 39)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &i6, &indi5, sizeof(i6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 40)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &nouchar3, &indi6, sizeof(nouchar3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 41)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_ASCII, &nouchar4, &indi7, sizeof(nouchar4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 42)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_UCS2_NATIVE, uchar5, &indi8, sizeof(uchar5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 43)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE("main: INSERT TAB1UC ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}

	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf("UNICODE activated ... done\n");
	printf("*****************\n");
}
/*------------------------------------------------------------------------*/
void tab_cluster()
/*
	1.	DROP & CREATE TAB1_CL with attribute CLUSTER
		GRANT, INSERT
	2.	DROP & CREATE TAB2_CL without attribute CLUSTER
		GRANT, INSERT with batch, ALTER tab to CLUSTER for one column
	3.	DROP & CREATE TAB3_CL with attribute CLUSTER and PACKED
		GRANT, INSERT with batch
*/
{
	get_current_time(da);
	printf("*********************\n");
	printf("CLUSTER activated ... on %s", da);

	int i = 0;
	int rows_to_insert = rows * 200;
	int rows_to_insert2 = rows_to_insert / 30;
	int *i1a = new int[rows_to_insert];
	int *i2a = new int[rows_to_insert];
	int *i3a = new int[rows_to_insert];
	int *i4a = new int[rows_to_insert];
	int *i5a = new int[rows_to_insert];
	SQLDBC_Length *indi1=new SQLDBC_Length[rows_to_insert];
	SQLDBC_Length *indi2=new SQLDBC_Length[rows_to_insert];
	SQLDBC_Length *indi3=new SQLDBC_Length[rows_to_insert];
	SQLDBC_Length *indi4=new SQLDBC_Length[rows_to_insert];
	SQLDBC_Length *indi5=new SQLDBC_Length[rows_to_insert];

	printf ( "DROP TABLE TAB1_CL\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB1_CL");
	CE("tab_cluster: DROP TABLE TAB1_CL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE TABLE TAB1_CL CLUSTER\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB1_CL"
		"(I1 FIXED (10) KEY, "
		"I2 FIXED (10), "
		"I3 FIXED (20) NOT NULL, "
		"C1 CHAR(4000)) "
		"CLUSTER PRIMARY KEY ");
	CE("tab_cluster: CREATE TABLE TAB1_CL CLUSTER", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB1_CL\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB1_CL TO PUBLIC");
	CE("tab_cluster: GRANT ... ON TAB1_CL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	pstmt = conn->createPreparedStatement();
	CE("tab_cluster: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	printf ("INSERT TAB1_CL (%d entries)\n", rows_to_insert2) ;
	returncode = checkPreparedStatement(pstmt, "INSERT TAB1_CL VALUES ( ? , ? , ?, ? )");
	CE("tab_cluster: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	for ( i = 0 ; i < rows_to_insert2; i++ )
	{
		i1 = i ;
		i2 = i / 10 ;
		i3 = i / 3;
		char cl_c1[4000]={0};
		int insert_len = (i%3999)+1;
		memset(cl_c1, 'c', insert_len);
		cl_c1[insert_len] = (char) 0;
		SQLDBC_Length indi1=sizeof(i1), indi2=sizeof(i2), indi3=sizeof(i3), indi4=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i1, &indi1, sizeof(i1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 1.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i2, &indi2, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 1.2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i3, &indi3, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 1.3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &cl_c1, &indi4, sizeof(cl_c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 1.4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("tab_cluster: INSERT TAB1_CL ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}
	printf ("Commit\n");
	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;


	printf ( "DROP TABLE TAB2_CL\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB2_CL");
	CE("tab_cluster: DROP TABLE TAB2_CL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE TABLE TAB2_CL\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB2_CL"
		"(I1 FIXED (10), "
		"I2 FIXED (10), "
		"I3 FIXED (20) NOT NULL)");
	CE("tab_cluster: CREATE TABLE TAB2_CL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB2_CL\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB2_CL TO PUBLIC");
	CE("tab_cluster: GRANT ... ON TAB2_CL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("INSERT TAB2_CL (%d entries)\n", rows_to_insert) ;
	for ( i = 0 ; i < rows_to_insert ; i++ )
    {
		i1a[i] = i ;
		i2a[i] = i / 10 ;
		i3a[i] = i / 3;
	    indi1[i]=sizeof(int);
		indi2[i]=sizeof(int);
		indi3[i]=sizeof(int);
    }
	pstmt = conn->createPreparedStatement();
	CE("tab_cluster: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB2_CL VALUES ( ? , ? , ? )");
	CE("tab_cluster: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, i1a, indi1, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 2.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, i2a, indi2, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 2.2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, i3a, indi3, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 2.3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->setBatchSize(rows_to_insert);
	CE("tab_cluster: setBatchSize", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("tab_cluster: INSERT TAB2_CL ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	conn->releaseStatement(pstmt);
	pstmt = 0;

	printf ("ALTER TABLE TAB2_CL CLUSTER(I2)\n" ) ;
	returncode = stmt->execute("ALTER TABLE TAB2_CL CLUSTER(I2)");
	CE("tab_cluster: ALTER TABLE TAB2_CL CLUSTER(I2)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);
	printf ("Commit\n");
	commit();


	printf ( "DROP TABLE TAB3_CL\n" ) ;
	fflush ( stdout ) ;
	returncode = stmt->execute("DROP TABLE TAB3_CL");
	CE("tab_cluster: DROP TABLE TAB3_CL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("CREATE TABLE TAB3_CL CLUSTER PACKED\n" ) ;
	returncode = stmt->execute("CREATE TABLE TAB3_CL"
		"(I1 FIXED (10) KEY, "
		"I2 FIXED (10), "
		"I3 FIXED (20) NOT NULL, "
		"I4 FIXED (15), "
		"I5 FIXED (3)) "
		"CLUSTER PRIMARY KEY PACKED");
	CE("tab_cluster: CREATE TABLE TAB3_CL CLUSTER PACKED", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("GRANT ... ON TAB3_CL\n" ) ;
	returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB3_CL TO PUBLIC");
	CE("tab_cluster: GRANT ... ON TAB3_CL", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("INSERT TAB3_CL (%d entries)\n", rows_to_insert) ;
	for ( i = 0 ; i < rows_to_insert ; i++ )
    {
		i1a[i] = i ;
		i2a[i] = i / 10 ;
		i3a[i] = i / 3;
		i4a[i] = i % 100;
		i5a[i] = 2;
	    indi1[i]=sizeof(int);
		indi2[i]=sizeof(int);
		indi3[i]=sizeof(int);
		indi4[i]=sizeof(int);
		indi5[i]=sizeof(int);
    }
	pstmt = conn->createPreparedStatement();
	CE("tab_cluster: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "INSERT TAB3_CL VALUES ( ? , ? , ?, ?, ? )");
	CE("tab_cluster: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, i1a, indi1, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 3.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, i2a, indi2, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 3.2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, i3a, indi3, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 3.3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, i4a, indi4, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 3.4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, i5a, indi5, sizeof(int));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in tab_cluster (col 3.5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->setBatchSize(rows_to_insert);
	CE("tab_cluster: setBatchSize", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("tab_cluster: INSERT TAB3_CL ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	conn->releaseStatement(pstmt);
	pstmt = 0;

	delete[] i1a;
	delete[] i2a;
	delete[] i3a;
	delete[] i4a;
	delete[] i5a;
	delete[] indi1;
	delete[] indi2;
	delete[] indi3;
	delete[] indi4;
	delete[] indi5;

	printf ("CREATE INDEX TAB3_CL_I4 ON TAB3_CL(I4)\n" ) ;
	returncode = stmt->execute("CREATE INDEX TAB3_CL_I4 ON TAB3_CL(I4)");
	CE("tab_cluster: CREATE INDEX TAB3_CL_I4 ON TAB3_CL(I4)", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();

	get_current_time(da);
	printf("CLUSTER activated ... done on %s", da);
	printf("*********************\n");
}
/*------------------------------------------------------------------------*/
void update_stat ()
/*
	UPDATE STATISTICS if it is first superu
*/
{
    printf ("UPDATE STATISTICS *\n") ;
	returncode = stmt->execute("UPDATE STATISTICS *");
	CE("update_stat: UPDATE STATISTICS *", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

    printf ("UPDATE STATISTICS COLUMN (*) FOR TAB1\n") ;
	returncode = stmt->execute("UPDATE STATISTICS COLUMN (*) FOR TAB1");
	CE("update_stat: UPDATE STATISTICS COLUMN (*) FOR TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	printf ("Commit\n");
	commit();
}
/*------------------------------------------------------------------------*/
void pe (const char *s, int leave, int errorcode, const char *errortext)
/*
	pe = "print error"
	checks the error from the last command
	with "leave"-flag or some special errors stop kernel (sqladump)
*/
{
//To-Do:
	char    zeile [200]={0};

	if ( ( errorcode > 200 ) || ( errorcode < 0 ) )
		print_f (s, errorcode, errortext) ;

	sprintf ( zeile , "%s :  ERR = %d\n%s" , s , errorcode, errortext ) ;

/*NEW because sqladump breaks down connection and with connection break down no further statement should be sent*/
	if (errorcode == -10807 || errorcode == -10709)
	{
		print_f(s, errorcode, errortext);
		remove( actfile );
		exit(1);
	}
/*ENDNEW*/

	if (leave || errorcode == -108 || errorcode <= -9000 && errorcode >= -10000 || errorcode <= -800 && errorcode > -900)
	{
		print_f (s, errorcode, errortext) ;
		Sqldump (s, errorcode, errortext, conn) ;
	}
}
