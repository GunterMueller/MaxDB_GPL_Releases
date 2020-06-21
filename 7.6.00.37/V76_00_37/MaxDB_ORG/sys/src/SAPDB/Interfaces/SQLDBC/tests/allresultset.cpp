/*!
  @file           allresultset.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Checks the methods of the SQLDBC_ResultSet.

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <string.h>

#define DATA_SIZE 40000

//----------------------------------------------------------------------------------------------------------------------
int test_pos[] = {
    43529,4840,7665,3468,23617,5476,37841,9528,403,684,12076,615,43914,2149,24709,3393,30175,3650,3010,6095,41788,6982,24793,4204,38901,5612,
    19716,5926,33408,6411,49587,257,24192,4761,37980,5459,26201,3359,44484,764,1198,15,3899,9105,22968,4780,25678,3703,34827,1687,23934,6308,
    32222,5448,23337,9918,28231,8394,11190,3776,34803,7992,31014,3473,17522,227,37957,3954,6720,9189,11312,8252,22754,8158,44501,9753,48256,
    2503,7793,812,5155,9713,4102,5526,19320,6872,13151,9484,30591,878,44343,4294,31046,1745,43619,9721,39301,4267,24648,1221,43650,8380,
    19636,5706,21874,3534,20075,3344,20524,7371,2758,8201,1473,4040,11347,5549,38593,2447,38828,8645,22310,9766,31151,2794,4915,506,7384,
    9510,44378,6473,16671,8590,29534,7905,27327,902,44419,1124,18377,9724,4577,2999,18834,3410,23159,7540,15067,7238,39612,4932,21520,7210,
    18784,8320,35152,3654,3246,8671,18856,7415,26658,3001,34203,6454,20974,6303,18825,5679,25310,2475,28727,7800,29285,4566,40851,8044,8652,
    9651,36220,517,36016,8643,8948,9047,24465,6370,21447,315,36941,2200,1140,2070,19115,6568,48984,4269,38639,3674,5127,5936,38537,5695,
    42006,5545,37583,8812,30198,7769,35457,6370,47330,4373,5498,9464,25390,2090,32874,302,6698,5278,30964,8615,3908,6172,43783,6454,34318,
    554,17255,1680,23794,1637,47147,5453,658,9992,36461,4042,24823,2313,20136,258,22480,6856,39823,4837,21686,8332,26590,3070,9128,108,
    24338,7000,3396,6482,11754,8501,15072,2888,5617,3547,38949,6809,3254,2984,48690,7237,46981,119,39243,5470,18317,439,37317,5076,44811,
    3769,25245,3831,27257,4187,48527,5433,20156,8002,2395,3823,23192,5113,17670,7914,16578,4061,41277,9540,48032,6081,31670,2439,4258,8472,
    14534,5092,24692,2599,19053,5210,40348,6829,40030,8706,45580,9730,46245,942,35564,3830,30263,3080,17316,5258,20109,1339,27053,9574,48029,
    1980,11716,9104,46727,9820,7477,2094,18337,3096,23011,8987,46456,2493,10381,6147,46576,7140,31327,2338,25193,5389,1780,8257,42923,3201,
    22410,4466,1935,7930,28022,7997,3014,343,13608,2973,42560,2662,19883,5535,44851,5461,36091,8413,49862,1225,2920,5419,1970,7377,23328,
    1212,9849,6588,14077,6617,22858,7195,5819,4829,44271,1518,24526,8806,26607,4905,12067,5169,19422,3312,18779,4913,14841,2228,25806,6251,
    11170,4768,30040,9900,39872,1724,49477,2118,26246,1417,31792,5807,29111,2137,23952,5002,29790,4996,35669,3039,15745,4,4520,9740,29133,
    3178,45540,7905,25635,7502,43100,4438,13837,5545,17875,6409,20493,6550,26729,7381,1989,5814,28330,4287,47291,9160,22283,202,42734,320,
    44619,3295,43923,4042,27453,7293,25072,2733,5596,3753,1120,9450,27974,1471,42885,5044,6621,3328,30619,2289,11103,5890,16237,4784,41152,
    2078,39570,3464,35341,2208,18909,8507,48603,3737,11204,9480,6412,4998,12186,1509,18441,3259,41749,299,9491,2412,34900,3814,541,600,
    33588,3434,47678,3099,49717,2391,21784,765,44678,4307,36725,9364,45676,9638,8913,91,44211,2846,6014,6588,46979,8104,17632,8789,24535,
    6342,5782,8884,28028,1467,17956,9732,19306,8543,20600,1019,34717,1762,35814,3495,27912,5013,39094,6574,11968,2856,905,9643,46132,4728,
    29931,2358,37522,174,789,4271,3057,7932,24035,9073,11979,6677,41253,3792,4241,235,3071,8229,5085,5087,12894,4977,31130,2967,44169,
    5305,8271,6367,13993,1244,46715,9777,20899,9167,43444,4442,29595,5196,7451,9487,10502,7729,28025,7181,16877,7412,31423,5193,7361,9867,
    46235,3620,46737,1484,22751,9617,42731,3588,26642,7439,1656,5307,2268,6705,10718,435,25940,6692,29795,8618,44422,9484,498,6161,18999,
    9778,5340,6786,20443,4161,34800,7636,45142,7673,45119,849,47761,472,14631,6013,10695,2402,31438,3806,45669,8289,20250,6145,47768,1769,
    41193,3151,47231,467,18401,2244,29300,331,31215,9670,17457,1946,25782,9798,40345,6626,29877,2470,27703,5107,28774,3737,5608,6650,34942,
    279,36350,1367,9420,2841,23244,3582,17661,8975,25707,9037,27651,412,4886,3763,33138,5383,45180,7418,9760,1181,21941,8918,36913,7307,
    10685,7156,43131,4476,8123,4638,7831,9827,10312,6841,27213,3633,18252,5103,27132,7830,16237,5863,30750,1666,43281,4061,13804,4805,2895,
    3509,5256,3174,31594,2051,16192,7881,21378,4316,4738,2510,16543,9884,9839,9111,30115,9673,11681,8191,232,8365,1598,3019,31777,5673,
    19265,3566,43566,4274,39975,383,16442,7214,19848,3729,12529,2591,31736,8276,11014,8348,49691,8681,30732,4968,14756,6358,34882,8890,39933,
    2746,38195,4464,37118,9408,11835,7259,23819,236,6391,6516,42475,6120,38227,9589,179,7360,9536,6164,11637,4141,7390,5076,3147,395,
    2120,1534,10433,1354,40302,7579,28144,96,2706,3559,43812,3425,22492,3474,14106,2361,33965,1959,16187,7587,47217,5755,31955,5143,18961,
    7461,48043,2683,14864,4383,29328,1619,33696,288,48790,5449,38619,2891,31553,9598,15899,4872,45861,3060,6386,1235,48877,7190,30059,840,
    20058,1915,48942,4893,39945,4718,38720,235,47170,2675,14354,218,31518,2881,30016,997,45806,1558,48649,6041,44455,7171,1467,6685,31508,
    7868,32718,1522,25947,8534,8,2459,27102,7797,25492,248,37978,286,18285,7300,29293,4453,13344,591,2062,1276,47194,2492,31521,4153,
    15761,2304,217,5300,34665,6346,3064,8964,47254,9375,32193,6767,709,5178,24389,5490,35526,9128,28948,2611,8696,533,28750,9911,45881,
    7575,22370,456,42908,2665,39,9273,11893,1018,21259,3029,42315,262,26897,9026,22320,3899,43557,8825,40621,6420,13056,6813,14258,2902,
    4810,9021,12552,875,43940,2511,22571,7305,12912,640,26398,8474,9527,4518,35544,5584,19071,5966,2794,3905,33439,2294,6801,8259,25037,
    8206,29766,2674,8298,5147,23469,7001,46583,3404,34017,8328,37330,7274,30585,9979,33223,8608,8469,5935,32215,9051,11776,3466,44201,6283,
    46050,1809,17183,4572,26156,9686,30703,4971,29729,9487,39377,5435,1500,921,24346,6212,28815,1956,35444,784,33657,9857,31899,7702,1487,
    129,4392,3597,42209,6189,36260,2710,3586,7569,36689,3227,32393,9471,3679,5744,3657,1473,49616,4185,13257,8219,47245,4459,4233,9889,
    28870,1189,22080,5262,46599,7826,19378,5199,26413,444,5033,9342,23923,546,8474,3846,14574,7184,4093,2967,26157,3711,16259,2305,46399,
    9465,208,6515,13482,1721,19123,9314,22260,8198,12714,8579,4043,5448,27634,4351,12696,3440,4993,2575,24927,1646,45342,2359,37522,6937,
    46392,9592,19388,8620,33808,4719,1177,907,30382,7490,10328,8139,33895,6485,34328,3082,48980,6255,20015,3597,43308,9854,41720,8000,23403,
    9603,46071,4431,387,3892,6276,4537,40993,2823,43688,6783,24137,517,49456,9703,48371,6889,14110,413,10526,6717,47847,8677,858,8802,
    49361,1866,35016,3036,6023,1098,18136,8200,33217,3885,11364,8292,35645,3601,46160,203,48433,1072,6644,558,40812,8612,25330,4519,40226,
    1808,12552,4387,9658,9380,14895,792,35991,6410,22938,4292,12214,1271,23626,206,21290,1553,17591,4467,4499,9307,46720,3336,22065,925,
    2288,4768,38877,1951,8976,4250,43126,2150,5076,2936,30662,1149,14200,860,12876,9946,28811,3078,30956,8621,872,1521,34275,5542,9951,
    389,5767,8395,4061,7888,42084,2339,33591,2133,21352,5601,23589,4015,16334,7829,4757,4864,35819,1507,48441,1703,2047,1754,25940,7910,
    46978,4809,37436,2108,35222,523,30886,3091,46447,9169,6138,3588,25058,7876,24637,702,29996,7008,20011,5807,42426,1796,29822,1738,14158,
    9538,43854,83,14603,7676,39191,6370,9073,3060,47443,5994,26244,7729,24404,8669,47593,1495,17460,4228,26612,6659,1075,7589,20632,5176,
    25647,5684,43047,3565,60,4186,18767,5890,3232,6821,702,116,44991,5101,30095,8163,21576,1267,48516,4956,36072,2283,44327,6800,29107,
    7662,41436,4374,40113,8965,13143,675,44466,3765,28902,9214,36818,1896,16797,2771,5933,6804,49566,8042,21334,9808,36960,6467,27086,77,
    32271,6561,13024,8152,35830,8075,8718,5806,3284,3269,20522,70,41480,5637,6995,4233,49759,1381,13012,3621,39460,3954,7879,8979,39477,
    6985,47770,5260,20647,5884,7784,8639,46813,3465,3953,9562,35347,9874,493,4639,34969,2157,22638,5201,3652,3754,20328,5332,44449,9267,
    3113,7176,32033,4773,41552,6355,37781,9036,5414,5286,36197,5891,34041,7227,499,3150,9113,8858,20186,8313,25199,4202,48371,6409,28357,
    7721,11731,284,18119,9369,40865,8637,33162,9440,3855,2126,15996,6007,889,2849,26948,2781,2715,7524,2794,181,40758,9662,30324,7448,
    31716,1069,30159,8600,44426,7963,21140,55,41854,2045,10586,2715,5962,1818,30759,6350,28325,4471,16820,9068,24270,3643,46610,9699,3367,
    3383,37543,5901,4075,3610,20971,5805,25368,7383,39266,6000,13141,5316,36007,3688,34218,7419,34507,5800,45759,7699,25065,2911,42908,1830,
    6197,738,42458,3197,203,7059,35381,7632,38349,6851,15996,2162,35991,3461,20636,8059,2970,8630,33579,3531,9543,9170,43082,6350,3226,
    3120,5335,1732,46804,3187,49128,5789,22855,276,29052,9853,25095,4941,10050,5996,31833,9556,41138,9053,31897,9013,1081,5462,48284,2781,
    23867,4230,29896,6113,36770,343,6649,3232,18836,5930,28266,305,12577,9494,32094,1004,46706,8516,10212,2489,2727,5761,21184,4605,33173,
    4578,16755,120,14321,3521,28786,8808,41195,5599,36774,3191,47666,2468,11240,8756,9464,8870,3301,6456,12902,7654,25770,309,47144,653,
    46045,7907,2831,5778,36149,6648,34838,5594,194,3503,22036,7725,24614,1128,31815,2387,21451,8141,22220,7355,10030,6373,39490,5692,7320,
    2891,43818,6541,22301,9638,21151,4295,24648,8465,13837,6460,33039,4400,27932,5522,43412,4945,49972,5130,42472,8218,31043,5906,26760,7763,
    20940,1200,22612,6829,10242,8097,27300,9927,13100,3991,16674,4500,48571,4033,39637,492,3290,9781,44727,636,33317,4345,9163,9292,37935,
    4349,15621,2621,13256,4027,47098,7919,18401,1758,45142,4929,12116,4350,1777,6939,42588,9399,30433,4470,48311,1023,36188,9381,2791,9280,
    30651,2194,10917,8608,11767,455,39797,5561,3890,941,41894,1189,28425,3531,3235,8348,335,9545,24564,9276,20852,2369,18183,3726,11684,
    3182,28347,2980,11045,9589,8010,57,26809,9975,9719,1420,17104,1402,23490,508,43667,3844,19459,8388,18262,8588,28144,1851,37472,3607,
    16108,4266,26188,3718,39846,9494,35100,8334,35709,4187,12510,4632,28673,8773,45760,8943,17774,2450,42618,4169,21006,2492,47904,6059,37516,
    3187,15696,6139,9885,4711,32395,4112,44326,7270,11008,7551,21605,786,1912,1686,6493,5483,40447,7093,38572,3654,26630,8123,12191,7932,
    7317,9852,28405,6379,6116,6379,26694,9708,21369,8295,29282,936,37508,2115,5861,2803,20101,9497,41547,3250,39917,9060,15860,4275,46287,
    8613,25562,4488,35103,605,9145,661,31768,8608,5803,8597,34379,7656,6681,8637,37496,6231,31218,6092,31778,2737,36420,1942,49819,4245,
    41825,4095,3473,344,28031,4530,33017,9899,41559,2625,13430,3538,13875,9994,38103,9660,27387,7138,25332,6807,36229,7224,17846,184,24645,
    7313,20737,5160,10770,2325,48070,5547,28649,2093,23499,9482,46288,6777,5195,719,2543,977,30419,8136,20678,1853,17,1291,31399,1553,
    48176,4811,13157,9280,17754,9453,32570,3291,36477,2628,42742,8155,14335,6218,22298,7580,7477,4487,12978,8109,18934,9902,38553,4020,31446,
    9675,12917,8720,42765,847,20144,2959,48676,6710,24338,5177,44117,2399,42067,4282,37650,2448,35228,196,49646,8136,7110,2451,39455,7432,
    9510,2180,31911,5343,43696,7791,38447,7494,13016,9640,43165,4737,9995,4035,49158,1608,45713,1852,5394,6448,4271,2107,9476,5990,30185,
    5773,6606,2241,31484,166,30040,4450,6354,9209,3368,3996,26136,9890,49918,4993,32461,223,29599,8432,31383,9020,48513,2671
};

class ResultSetFeatures
    : public SQLDBC_TestCase
{
public:
    ResultSetFeatures()
    :SQLDBC_TestCase("ResultSetFeatures")
    {}

    virtual SQLDBC_Bool prepare()
    {
        loginfo("prepare");
        dropTable("SQLDBC_RESULTSET");
        m_success = m_success && execSQL("CREATE TABLE SQLDBC_RESULTSET(K INTEGER PRIMARY KEY)");
        SQLDBC_Int4 i;
        SQLDBC_Int4 iarray[DATA_SIZE];
	SQLDBC_Length ilength[DATA_SIZE];
        SQLDBC_PreparedStatement *insert_statement=tc_errorcheck(connection())->createPreparedStatement();
        insert_statement->prepare("INSERT INTO SQLDBC_RESULTSET VALUES (?)");
        tc_errorcheck(insert_statement)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &iarray, ilength, sizeof(SQLDBC_Int4));
        for(i=1; i <= DATA_SIZE; ++i) {
            iarray[i-1]=i;
	    ilength[i-1] = sizeof(SQLDBC_Int4);
        }
        tc_errorcheck(insert_statement)->setBatchSize(DATA_SIZE);
        tc_errorcheck(insert_statement)->executeBatch();
        connection()->commit();
        connection()->releaseStatement(insert_statement);
        loginfo("prepare finished");
        return m_success;
    }
    
    virtual SQLDBC_Bool run()
    {
        m_success = m_success && runEmptyResultSet();
        m_success = m_success && runEmptyResultSetPrepared();
        m_success = m_success && runClosedResultSet();
        runMFetch(1,10);
        runMFetch(10,100);
        runMFetch(1000,10);
        runMFetch(1000,999);
        runMFetch(999,1000);
        runForward();
        runRelative();
        runRandomPlaceAndFetchSizeRelative();
        runRandomPlaceAndFetchSize();
        return m_success;
    }
    
    SQLDBC_Bool runMFetch(SQLDBC_Int4 fetchsize, SQLDBC_Int4 rowsetsize)
    {
        loginfo("Running mass command with fetch size %d and row set size %d.", fetchsize, rowsetsize);

        SQLDBC_Int4 *rowsetdata=new SQLDBC_Int4[rowsetsize];
        for(SQLDBC_Int4 i=0; i<rowsetsize; ++i) {
            rowsetdata[i]=-1;
        }
        SQLDBC_Int4 pos=1;
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        tc_errorcheck(rs1)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, rowsetdata, 0, 0);
        tc_errorcheck(rs1)->setRowSetSize(rowsetsize);
        tc_errorcheck(rs1)->setFetchSize(fetchsize);
        tc_errorcheck(rs1)->first();
        SQLDBC_Int4 fetcheddata;
        do {
            tc_errorcheck(rs1)->getRowSet()->fetch();
            fetcheddata = tc_errorcheck(rs1)->getRowSet()->getRowsAffected();
            for(SQLDBC_Int4 i=0; i<fetcheddata; ++i) {
                if(i + pos != rowsetdata[i]) {
                    logerror("Rowset fetch: Expected %d found %d.", (i+pos), rowsetdata[i]);
                    delete [] rowsetdata;
                    tc_errorcheck(connection())->releaseStatement(s1);
                    return false;
                }
            }
            pos += fetcheddata;
            if(fetcheddata != 0 && pos < DATA_SIZE) {
                tc_errorcheck(rs1)->next();
            }
        } while(fetcheddata!=0 && pos < DATA_SIZE);
        delete [] rowsetdata;
        connection()->releaseStatement(s1);
        return true;
    }

    SQLDBC_Bool runEmptyResultSet()
    {
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET WHERE K >65536");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();

        if(rs1 == 0) {
            logerror("Expected result set, getResultSet() returned a null pointer!");
            return false;
        }

        if(tc_errorcheck(rs1)->getResultCount() != 0) {
            logerror("Expected result count == 0, found %d", rs1->getResultCount());
            return false;
        }
        if(tc_errorcheck(rs1)->getStatement() != s1) {
            logerror("getStatement() did not retrieve the original statement.");
            return false;
        }
        if(tc_errorcheck(rs1)->getRowNumber() != 0) {
            logerror("Expected row number 0, found %d", rs1->getRowNumber());
            return false;
        }
        if(tc_errorcheck(rs1)->next() != SQLDBC_NO_DATA_FOUND) {
            logerror("next() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->previous() != SQLDBC_NO_DATA_FOUND) {
            logerror("previous() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->last() != SQLDBC_NO_DATA_FOUND) {
            logerror("last() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->absolute(5) != SQLDBC_NO_DATA_FOUND) {
            logerror("absolute(5) on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->relative(5) != SQLDBC_NO_DATA_FOUND) {
            logerror("relative(5) on empty result set - expected NO DATA FOUND.");
        }
        connection()->releaseStatement(s1);
        return m_success;
    }

    SQLDBC_Bool runEmptyResultSetPrepared()
    {
        SQLDBC_PreparedStatement *ps1=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps1)->prepare("SELECT * FROM SQLDBC_RESULTSET WHERE K >65536");
        ps1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(ps1)->execute();
        SQLDBC_ResultSet *rs1 = tc_errorcheck(ps1)->getResultSet();

        if(rs1 == 0) {
            logerror("Expected result set, getResultSet() returned a null pointer!");
            return false;
        }

        if(tc_errorcheck(rs1)->getResultCount() != 0) {
            logerror("Expected result count == 0, found %d", rs1->getResultCount());
            return false;
        }
        if(tc_errorcheck(rs1)->getStatement() != ps1) {
            logerror("getStatement() did not retrieve the original statement.");
            return false;
        }
        if(tc_errorcheck(rs1)->getRowNumber() != 0) {
            logerror("Expected row number 0, found %d", rs1->getRowNumber());
            return false;
        }
        if(tc_errorcheck(rs1)->next() != SQLDBC_NO_DATA_FOUND) {
            logerror("next() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->previous() != SQLDBC_NO_DATA_FOUND) {
            logerror("previous() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->last() != SQLDBC_NO_DATA_FOUND) {
            logerror("last() on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->absolute(5) != SQLDBC_NO_DATA_FOUND) {
            logerror("absolute(5) on empty result set - expected NO DATA FOUND.");
        }
        if(tc_errorcheck(rs1)->relative(5) != SQLDBC_NO_DATA_FOUND) {
            logerror("relative(5) on empty result set - expected NO DATA FOUND.");
        }
        connection()->releaseStatement(ps1);
        return m_success;
    }

    SQLDBC_Bool runClosedResultSet()
    {
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        rs1->close();
        tc_expectederrorcheck(rs1, -10500)->absolute(6);
        connection()->releaseStatement(s1);
        return m_success;
    }

    SQLDBC_Bool runForward()
    {
        m_success = m_success && runForwardWithFetchSize(1);
        m_success = m_success && runForwardWithFetchSize(13);
        m_success = m_success && runForwardWithFetchSize(100);
        m_success = m_success && runForwardWithFetchSize(1000);
        m_success = m_success && runForwardWithFetchSize(10000);
        m_success = m_success && runForwardWithFetchSize(32766);
        m_success = m_success && runForwardWithFetchSize(32767);
        return m_success;
    }

    SQLDBC_Bool runForwardWithFetchSize(int fetchsize)
    {
        loginfo("Forward fetching test with fetch size %d", fetchsize);
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        tc_errorcheck(rs1)->setFetchSize((SQLDBC_Int2)fetchsize);
        SQLDBC_Int4 val;
        for(SQLDBC_Int4 i=1; i<=DATA_SIZE; ++i) {
            if(tc_errorcheck(rs1)->next() == SQLDBC_NO_DATA_FOUND) {
                logerror("Data expected at position %d", i);
                break;
            }
            getInteger(rs1, val);
            if(val != i) {
                logerror("Expected %d, found %d while fetching with size %d", i, val, fetchsize);
                break;
            }
        }
        connection()->releaseStatement(s1);
        return m_success;
    }

    SQLDBC_Bool runRelative()
    {
        m_success = m_success && runRelativeWithFetchSize(10, 20);
        m_success = m_success && runRelativeWithFetchSize(30, 20);
        return m_success;
    }

    SQLDBC_Bool runRelativeWithFetchSize(SQLDBC_Int4 fetchsize, SQLDBC_Int4 rowsetsize)
    {
        loginfo("Relative fetching test with fetch size %d", fetchsize);
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        tc_errorcheck(rs1)->setRowSetSize(rowsetsize);
        tc_errorcheck(rs1)->setFetchSize((SQLDBC_Int2)fetchsize);

        tc_errorcheck(rs1)->next();
        tc_errorcheck(rs1)->next();
        if (tc_errorcheck(rs1)->relative(-2*rowsetsize) != SQLDBC_NO_DATA_FOUND) {
          logerror("No data expected");
        }
        connection()->releaseStatement(s1);
        return m_success;
    }

    SQLDBC_Bool runRandomPlaceAndFetchSize()
    {
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        SQLDBC_Int4 val;
        
        for(int i=0; i<sizeof(test_pos)/sizeof(int); i+=2) {
            if(i % 500 == 0) { 
                loginfo(" *random* absolute/fetch size: %d of %d", (i+1), sizeof(test_pos)/sizeof(int));
            }
            int rnd_pos=test_pos[i] % DATA_SIZE;
            int rnd_fs=test_pos[i+1] % DATA_SIZE;
            tc_errorcheck(rs1)->setFetchSize((SQLDBC_Int2)rnd_fs);            
            tc_errorcheck(rs1)->absolute(rnd_pos);
            getInteger(rs1, val);
            int rnd_val = rnd_pos;
            if(val != rnd_val) {
                logerror("Expected %d at %d found %d fetch size %d", rnd_val, rnd_pos, val, rnd_fs);
                break;
            }
        }
        connection()->releaseStatement(s1);
        return m_success;
    }

    SQLDBC_Bool runRandomPlaceAndFetchSizeRelative()
    {
        SQLDBC_Statement *s1=tc_errorcheck(connection())->createStatement();
        s1->setResultSetType(SQLDBC_Statement::SCROLL_SENSITIVE);
        tc_errorcheck(s1)->execute("SELECT * FROM SQLDBC_RESULTSET ORDER BY K");
        SQLDBC_ResultSet *rs1 = tc_errorcheck(s1)->getResultSet();
        SQLDBC_Int4 val;

        int rnd_pos=test_pos[0] % DATA_SIZE;
        int rnd_fs=test_pos[1] % DATA_SIZE;
        tc_errorcheck(rs1)->setFetchSize((SQLDBC_Int2)rnd_fs);            
        tc_errorcheck(rs1)->absolute(rnd_pos);
        getInteger(rs1, val);
        int rnd_val = rnd_pos;
        if(val != rnd_val) {
            logerror("Expected %d at %d found %d fetch size %d", rnd_val, rnd_pos, val, rnd_fs);
        }
        
        for(int i=1; i<sizeof(test_pos)/sizeof(int); i+=2) {
            if(i % 500 == 1) {
                loginfo(" *random* relative/fetch size: %d of %d", (i+1), sizeof(test_pos)/sizeof(int));
            }
            int new_rnd_pos=test_pos[i] % DATA_SIZE;
            int rnd_fs=test_pos[i+1] % DATA_SIZE;
            int rel_pos = new_rnd_pos - rnd_pos;
            tc_errorcheck(rs1)->setFetchSize((SQLDBC_Int2)rnd_fs);            
            tc_errorcheck(rs1)->relative(rel_pos);
            getInteger(rs1, val);
            rnd_pos = new_rnd_pos;
            int rnd_val = rnd_pos;
            if(val != rnd_val) {
                logerror("Expected %d at %d found %d fetch size %d", rnd_val, rnd_pos, val, rnd_fs);
                break;
            }
        }
        connection()->releaseStatement(s1);
        return m_success;
    }

    
    void getInteger(SQLDBC_ResultSet* rs, SQLDBC_Int4& value)
    {
        tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &value, 0, 0);
    }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new ResultSetFeatures());
  if (testframe.run ()) 
      return 0;
  
  return 1;
}


