/*PRETTY */
/*
 * ptoc00 0.9.5
 * vsp001c generated at 'Fri Oct 18 22:31:36 1996'
 * from vsp001 (Date 'Thu May 30 22:01:00 1996', Size 22657)
 */


/* CONST */

 /* function codes as in ORACLE Call Interface */
#define  CSP1_NIL_FC  0
#define  CSP1_CREATE_TABLE_FC  1
#define  CSP1_SET_ROLE_FC  2    /* unused */
#define  CSP1_INSERT_FC  3
#define  CSP1_SELECT_FC  4
#define  CSP1_UPDATE_FC  5
#define  CSP1_DROP_ROLE_FC  6   /* unused */
#define  CSP1_DROP_VIEW_FC  7
#define  CSP1_DROP_TABLE_FC  8
#define  CSP1_DELETE_FC  9
#define  CSP1_CREATE_VIEW_FC  10
#define  CSP1_DROP_USER_FC  11
#define  CSP1_CREATE_ROLE_FC  12  /* unused */
#define  CSP1_CREATE_SEQUENCE_FC  13
#define  CSP1_ALTER_SEQUENCE_FC  14  /* unused */
#define  CSP1_UNUSED1_FC  15    /* unused */
#define  CSP1_DROP_SEQUENCE_FC  16
#define  CSP1_CREATE_SCHEMA_FC  17
#define  CSP1_CREATE_CLUSTER_FC  18  /* unused */
#define  CSP1_CREATE_USER_FC  19
#define  CSP1_CREATE_INDEX_FC  20
#define  CSP1_DROP_INDEX_FC  21
#define  CSP1_DROP_CLUSTER_FC  22  /* unused */
#define  CSP1_VALIDATE_INDEX_FC  23  /* unused */
#define  CSP1_CREATE_PROCEDURE_FC  24
#define  CSP1_ALTER_PROCEDURE_FC  25  /* unused */
#define  CSP1_ALTER_TABLE_FC  26
#define  CSP1_EXPLAIN_FC  27
#define  CSP1_GRANT_FC  28
#define  CSP1_REVOKE_FC  29
#define  CSP1_CREATE_SYNONYM_FC  30
#define  CSP1_DROP_SYNONYM_FC  31
#define  CSP1_ALTER_SYSTEM_SWITH_LOG_FC  32  /* unused */
#define  CSP1_SET_TRANSACTION_FC  33  /* unused */
#define  CSP1_PL_SQL_EXECUTE_FC  34  /* unused */
#define  CSP1_LOCK_TABLE_FC  35
#define  CSP1_UNUSED2_FC  36    /* unused */
#define  CSP1_RENAME_FC  37
#define  CSP1_COMMENT_FC  38
#define  CSP1_AUDIT_FC  39      /* unused */
#define  CSP1_NOAUDIT_FC  40    /* unused */
#define  CSP1_ALTER_INDEX_FC  41  /* unused */
#define  CSP1_CREATE_EXTERNAL_DATABASE_FC  42  /* unused */
#define  CSP1_DROP_EXTERNAL_DATABASE_FC  43  /* unused */
#define  CSP1_CREATE_DATABASE_FC  44  /* unused */
#define  CSP1_ALTER_DATABASE_FC  45  /* unused */
#define  CSP1_CREATE_ROLLBACK_SEGMENT_FC  46  /* unused */
#define  CSP1_ALTER_ROLLBACK_SEGMENT_FC  47  /* unused */
#define  CSP1_DROP_ROLLBACK_SEGMENT_FC  48  /* unused */
#define  CSP1_CREATE_TS_TABLESPACE_FC  49  /* unused */
#define  CSP1_ALTER_TS_TABLESPACE_FC  50  /* unused */
#define  CSP1_DROP_TS_TABLESPACE_FC  51  /* unused */
#define  CSP1_ALTER_SESSION_FC  52  /* unused */
#define  CSP1_ALTER_USER_FC  53
#define  CSP1_COMMIT_FC  54
#define  CSP1_ROLLBACK_FC  55
#define  CSP1_SAVEPOINT_FC  56
#define  CSP1_CREATE_CONTROL_FILE_FC  57  /* unused */
#define  CSP1_ALTER_TRACING_FC  58  /* unused */
#define  CSP1_CREATE_TRIGGER_FC  59
#define  CSP1_ALTER_TRIGGER_FC  60  /* unused */
#define  CSP1_DROP_TRIGGER_FC  61
#define  CSP1_ANALYZE_TABLE_FC  62
#define  CSP1_ANALYZE_INDEX_FC  63  /* unused */
#define  CSP1_ANALYZE_CLUSTER_FC  64  /* unused */
#define  CSP1_CREATE_PROFILE_FC  65  /* unused */
#define  CSP1_DROP_PROFILE_FC  66  /* unused */
#define  CSP1_ALTER_PROFILE_FC  67  /* unused */
#define  CSP1_DROP_PROCEDURE_FC  68
#define  CSP1_UNUSED3_FC  69    /* unused */
#define  CSP1_ALTER_RESOURCE_COST_FC  70  /* unused */
#define  CSP1_CREATE_SL_SNAPSHOT_LOG_FC  71
#define  CSP1_ALTER_SL_SNAPSHOT_LOG_FC  72  /* unused */
#define  CSP1_DROP_SL_SNAPSHOT_LOG_FC  73
#define  CSP1_CREATE_SNAPSHOT_FC  74
#define  CSP1_ALTER_SNAPSHOT_FC  75  /* unused */
#define  CSP1_DROP_SNAPSHOT_FC  76  /* unused */
 /* */
#define  CSP1_MAX_OCI_FC  76    /* boundary check */
 /* */
#define  CSP1_SELECT_DIRECT_FC  201  /* DB-special */
#define  CSP1_SELECT_FIRST_FC  202  /* DB-special */
#define  CSP1_SELECT_LAST_FC  203  /* DB-special */
#define  CSP1_SELECT_NEXT_FC  204  /* DB-special */
#define  CSP1_SELECT_PREV_FC  205  /* DB-special */
#define  CSP1_FETCH_FIRST_FC  206  /* DB-special */
#define  CSP1_FETCH_LAST_FC  207  /* DB-special */
#define  CSP1_FETCH_NEXT_FC  208  /* DB-special */
#define  CSP1_FETCH_PREV_FC  209  /* DB-special */
#define  CSP1_FETCH_POS_FC  210 /* DB-special */
#define  CSP1_FETCH_SAME_FC  211  /* DB-special */
#define  CSP1_STRING_COMMAND_FC  212  /* DB-special */
#define  CSP1_CONNECT_FC  213   /* DB-special */
#define  CSP1_DROP_PARSEID_FC  214  /* DB-special */
#define  CSP1_CLOSE_FC  215     /* DB-special */
#define  CSP1_SHOW_FC  216      /* DB-special */
#define  CSP1_USAGE_FC  217     /* DB-special */
#define  CSP1_SERVERDB_FC  218  /* DB-special */
#define  CSP1_MONITOR_FC  219   /* DB-special */
#define  CSP1_SET_FC  220       /* DB-special */
#define  CSP1_LINK_FC  221      /* DB-special */
#define  CSP1_CREATE_DOMAIN_FC  222  /* DB-special */
#define  CSP1_DROP_DOMAIN_FC  223  /* DB-special */
#define  CSP1_DESCRIBE_FC  224  /* DB-special */
#define  CSP1_ALTER_PASSWORD_FC  225  /* DB-special */
#define  CSP1_PUTVAL_FC  233    /* DB-special */
#define  CSP1_GETVAL_FC  234    /* DB-special */
#define  CSP1_DIAGNOSE_FC  235  /* DB-special */
#define  CSP1_UNLOCK_FC  236    /* DB-special */
#define  CSP1_REFRESH_FC  237   /* DB-special */
#define  CSP1_CLEAR_SNAPSHOT_LOG_FC  238  /* DB-special */
#define  CSP1_NEXT_STAMP_FC  239  /* DB-special */
#define  CSP1_EXISTS_TABLE_FC  240  /* DB-special */
#define  CSP1_COMMIT_RELEASE_FC  241  /* DB-special */
#define  CSP1_ROLLBACK_RELEASE_FC  242  /* DB-special */
#define  CSP1_DRDA_NATIVE_DDL_FC  243  /* DB-special */
#define  CSP1_SELECT_INTO_FC  244  /* DB-special */
#define  CSP1_FETCH_RELATIVE_FC 247  /* DB-special */
#define  CSP1_CURSOR_PL_SQL_EXECUTE_FC 248  /* DB-special */
#define  CSP1_ADMINISTRATOR_FC 249 /* DB-special */
 /* */
#define  CSP1_MAX_SPECIAL_FC  246  /* boundary check */
 /* */
#define  CSP1_MIN_MASSCMD_FC  1000  /* boundary check */
 /* */
#define  CSP1_MINSERT_FC  1003  /* DB-masscmd */
#define  CSP1_MSELECT_FC  1004  /* DB-masscmd */
#define  CSP1_MUPDATE_FC  1005  /* DB-masscmd */
#define  CSP1_MDELETE_FC  1009  /* DB-masscmd */
#define  CSP1_MLOCK_OBJECTS_FC  1035  /* DB-masscmd */
#define  CSP1_MFETCH_FIRST_FC  1206  /* DB-masscmd */
#define  CSP1_MFETCH_LAST_FC  1207  /* DB-masscmd */
#define  CSP1_MFETCH_NEXT_FC  1208  /* DB-masscmd */
#define  CSP1_MFETCH_PREV_FC  1209  /* DB-masscmd */
#define  CSP1_MFETCH_POS_FC  1210  /* DB-masscmd */
#define  CSP1_MFETCH_SAME_FC  1211  /* DB-masscmd */
 /* */
#define  CSP1_MSELECT_INTO_FC  1244  /* DB-masscmd */
#define  CSP1_MAX_MASSCMD_FC  1244  /* boundary check */
 /* */
#define  CSP1_MASSCMD_FC_OFFSET  1000
#define  CSP1_OLD_VARPART_SIZE  8240
#define  CSP1_PART_ALIGN_LENGTH  8
#define  CSP1_MAX_PACKET_CNT  2
 /* */
#define  CSP1_FIRST_SP1_VERSION  "62000"

/* TYPE */

#define CSP1_VT_LC_CHAR            201
#define CSP1_VT_LC_WCHAR           202
#define CSP1_VT_LC_BYTE            203
#define CSP1_VT_LC_BOOL            204
#define CSP1_VT_LC_ABAP_TAB_HANDLE 205


#define  SP1M_NIL  0

#define  SP1M_CMD_LOWER_BOUND  1

#define  SP1M_DBS  2

#define  SP1M_PARSE  3

#define  SP1M_GETPARSE  4

#define  SP1M_SYNTAX  5

#define  SP1M_CFILL1  6

#define  SP1M_CFILL2  7

#define  SP1M_CFILL3  8

#define  SP1M_CFILL4  9

#define  SP1M_CFILL5  10

#define  SP1M_CMD_UPPER_BOUND  11

#define  SP1M_NO_CMD_LOWER_BOUND  12

#define  SP1M_EXECUTE  13

#define  SP1M_GETEXECUTE  14

#define  SP1M_PUTVAL  15

#define  SP1M_GETVAL  16

#define  SP1M_LOAD  17

#define  SP1M_UNLOAD  18

#define  SP1M_NCFILL1  19

#define  SP1M_NCFILL2  20

#define  SP1M_NCFILL3  21

#define  SP1M_NCFILL4  22

#define  SP1M_NCFILL5  23

#define  SP1M_NO_CMD_UPPER_BOUND  24

#define  SP1M_HELLO  25

#define  SP1M_UTIL_LOWER_BOUND  26

#define  SP1M_UTILITY  27

#define  SP1M_INCOPY  28

#define  SP1M_UFILL1  29

#define  SP1M_OUTCOPY  30

#define  SP1M_DIAG_OUTCOPY  31

#define  SP1M_UFILL3  32

#define  SP1M_UFILL4  33

#define  SP1M_UFILL5  34

#define  SP1M_UFILL6  35

#define  SP1M_UFILL7  36

#define  SP1M_UTIL_UPPER_BOUND  37

#define  SP1M_SPECIALS_LOWER_BOUND  38

#define  SP1M_SWITCH  39

#define  SP1M_SWITCHLIMIT  40

#define  SP1M_BUFLENGTH  41

#define  SP1M_MINBUF  42

#define  SP1M_MAXBUF  43

#define  SP1M_STATE_UTILITY  44

#define  SP1M_SFILL2  45

#define  SP1M_SFILL3  46

#define  SP1M_SFILL4  47

#define  SP1M_SFILL5  48

#define  SP1M_SPECIALS_UPPER_BOUND  49
typedef signed char tsp1_cmd_mess_type;

 /* */


#define  SP1CE_OK  0

#define  SP1CE_NOTOK  1

#define  SP1CE_TASKLIMIT  2

#define  SP1CE_TIMEOUT  3

#define  SP1CE_CRASH  4

#define  SP1CE_START_REQUIRED  5

#define  SP1CE_SHUTDOWN  6

#define  SP1CE_SEND_LINE_DOWN  7

#define  SP1CE_RECEIVE_LINE_DOWN  8

#define  SP1CE_PACKET_LIMIT  9

#define  SP1CE_RELEASED  10

#define  SP1CE_WOULDBLOCK  11
typedef signed char tsp1_comm_error;

 /* */


#define  SP1US_CONNECT  0

#define  SP1US_RESTART  1

#define  SP1US_SHUTDOWN  2
typedef signed char tsp1_utility_startup;

 /* */


#define  SP1IW_WARN0  0

#define  SP1IW_WARN1  1

#define  SP1IW_WARN2  2

#define  SP1IW_WARN3  3

#define  SP1IW_WARN4  4

#define  SP1IW_WARN5  5

#define  SP1IW_WARN6  6

#define  SP1IW_WARN7  7

#define  SP1IW_WARN8  8

#define  SP1IW_WARN9  9

#define  SP1IW_WARN10  10

#define  SP1IW_WARN11  11

#define  SP1IW_WARN12  12

#define  SP1IW_WARN13  13

#define  SP1IW_WARN14  14

#define  SP1IW_WARN15  15
typedef signed char tsp1_intern_warning[2];

 /* */
/* SET */ typedef signed char tsp1_warningset_intern;

 /* */


#define  SP1PK_NIL  0

#define  SP1PK_APPL_PARAMETER_DESCRIPTION  1

#define  SP1PK_COLUMNNAMES  2

#define  SP1PK_COMMAND  3

#define  SP1PK_CONV_TABLES_RETURNED  4

#define  SP1PK_DATA  5

#define  SP1PK_ERRORTEXT  6

#define  SP1PK_GETINFO  7

#define  SP1PK_MODULNAME  8

#define  SP1PK_PAGE  9

#define  SP1PK_PARSID  10

#define  SP1PK_PARSID_OF_SELECT  11

#define  SP1PK_RESULTCOUNT  12

#define  SP1PK_RESULTTABLENAME  13

#define  SP1PK_SHORTINFO  14

#define  SP1PK_USER_INFO_RETURNED  15

#define  SP1PK_SURROGATE  16

#define  SP1PK_BDINFO  17

#define  SP1PK_LONGDATA  18

#define  SP1PK_TABLENAME  19

#define  SP1PK_UNUSED1  20

#define  SP1PK_UNUSED2  21

#define  SP1PK_UNUSED3  22
typedef signed char tsp1_part_kind;

 /* */


#define  SP1PA_LAST_PACKET  0

#define  SP1PA_FILL1  1

#define  SP1PA_FILL2  2

#define  SP1PA_FILL3  3

#define  SP1PA_FILL4  4

#define  SP1PA_FILL5  5

#define  SP1PA_FILL6  6

#define  SP1PA_FILL7  7
typedef signed char tsp1_part_attributes;

 /* */
/* SET */ typedef signed char tsp1_part_attr;

 /* */


#define  SP1PR_NIL  0

#define  SP1PR_USER_CMD  1

#define  SP1PR_INTERNAL_CMD  2

#define  SP1PR_KERNEL  3

#define  SP1PR_INSTALLATION  4
typedef signed char tsp1_producer;

 /*                sp1pr_internal_cmd given for example by LOAD, */
 /*                if command was not given by the user but      */
 /*                needed by LOAD for internal use               */
 /* */


#define  SP1SK_NIL  0

#define  SP1SK_CMD  1

#define  SP1SK_RETURN  2
typedef signed char tsp1_segment_kind;

 /* */


#define  SP1SM_NIL  0

#define  SP1SM_SESSION_SQLMODE  1

#define  SP1SM_INTERNAL  2

#define  SP1SM_ANSI  3

#define  SP1SM_DB2  4

#define  SP1SM_ORACLE  5

#define  CSP1_VT_EMPTY  0
#define  CSP1_VT_NULL  1
#define  CSP1_VT_I2  2
#define  CSP1_VT_I4  3
#define  CSP1_VT_R4  4
#define  CSP1_VT_R8  5
#define  CSP1_VT_CY  6
#define  CSP1_VT_DATE  7
#define  CSP1_VT_BSTR  8
#define  CSP1_VT_DISPATCH  9
#define  CSP1_VT_ERROR  10
#define  CSP1_VT_BOOL  11
#define  CSP1_VT_VARIANT  12
#define  CSP1_VT_UNKNOWN  13
#define  CSP1_VT_DECIMAL  14
#define  CSP1_VT_I1  16
#define  CSP1_VT_UI1  17
#define  CSP1_VT_UI2  18
#define  CSP1_VT_UI4  19
#define  CSP1_VT_I8  20
#define  CSP1_VT_UI8  21
#define  CSP1_VT_INT  22
#define  CSP1_VT_UINT  23
#define  CSP1_VT_VOID  24
#define  CSP1_VT_HRESULT  25
#define  CSP1_VT_PTR  26
#define  CSP1_VT_SAFEARRARY  27
#define  CSP1_VT_CARRAY  28
#define  CSP1_VT_USERDEFINED  29
#define  CSP1_VT_LPSTR  30
#define  CSP1_VT_LPWSTR  31
#define  CSP1_VT_FILETIME  64
#define  CSP1_VT_BLOB  65
#define  CSP1_VT_STREAM  66
#define  CSP1_VT_STORAGE  67
#define  CSP1_VT_STREAMED_OBJECT  68
#define  CSP1_VT_STORED_OBJECT  69
#define  CSP1_VT_BLOB_OBJECT  70
#define  CSP1_VT_CF  71
#define  CSP1_VT_CLSID  72
#define  CSP1_VT_VECTOR  4096
#define  CSP1_VT_ARRAY  8192
#define  CSP1_VT_BYREF  16384


typedef signed char tsp1_sqlmode;


typedef struct tsp1_part_header
    {
    tsp1_part_kind  sp1p_part_kind;
    tsp1_part_attr  sp1p_attributes;
    tsp00_Int2        sp1p_arg_count;
    tsp00_Int4        sp1p_segm_offset;
    /* */
    tsp00_Int4        sp1p_buf_len;

    tsp00_Int4        sp1p_buf_size;
    }
tsp1_part_header;



typedef struct tsp1_part
    {

    union
        {

        struct tsp1_part_01
            {
            tsp1_part_header sp1p_part_header;
            tsp00_MoveObj     sp1p_buf;
            }
        C_1;

        struct tsp1_part_02
            {
            tsp1_part_kind  sp1p_part_kind;
            tsp1_part_attr  sp1p_attributes;
            tsp00_Int2        sp1p_arg_count;
            tsp00_Int4        sp1p_segm_offset;
            tsp00_Int4        sp1p_buf_len;
            tsp00_Int4        sp1p_buf_size;
            tsp00_ResNum      sp1p_resnum;
            }
        C_2;
        }
    integer;
    }
tsp1_part;

 /*ENDCASE */

typedef tsp1_part *tsp1_part_ptr;


typedef struct tsp1_segment_header
    {

    union
        {

        struct tsp1_segment_header_01
            {
            tsp00_Int4        sp1s_segm_len;
            tsp00_Int4        sp1s_segm_offset;
            /*  */
            tsp00_Int2        sp1s_no_of_parts;
            tsp00_Int2        sp1s_own_index;
            tsp1_segment_kind sp1s_segm_kind;
            tsp1_cmd_mess_type sp1c_mess_type;
            tsp1_sqlmode    sp1c_sqlmode;
            tsp1_producer   sp1c_producer;
            /*  */
            boolean         sp1c_commit_immediately;
            boolean         sp1c_ignore_costwarning;
            boolean         sp1c_prepare;
            boolean         sp1c_with_info;
            boolean         sp1c_mass_cmd;
            boolean         sp1c_parsing_again;
            tsp00_C2          sp1c_filler1;
            /*  */
            tsp00_C8          sp1c_filler2;
            tsp00_C8          sp1c_filler3;
            }
        C_1;

        struct tsp1_segment_header_02
            {
            tsp00_C8          sp1s_space2;
            /*  */
            tsp00_C4          sp1s_space3;
            tsp1_segment_kind sp1s_space4;
            tsp00_SqlState    sp1r_sqlstate;  /* 5 bytes */
            tsp00_Int2        sp1r_returncode;
            tsp00_Int4        sp1r_errorpos;
            /*  */
            tsp00_WarningSet  sp1r_extern_warning;
            tsp1_warningset_intern sp1r_intern_warning;
            tsp00_Int2        sp1r_function_code;  /* csp1_.._fc */
            tsp00_Int2        sp1r_filler1;
            tsp00_C8          sp1r_filler2;
            }
        C_2;
        }
    integer;
    }
tsp1_segment_header;

 /*ENDCASE */



typedef struct tsp1_segment
    {

    union
        {

        struct tsp1_segment_01
            {
            tsp1_segment_header sp1s_segm_header;
            tsp1_part       sp1p_part;
            }
        C_1;

        struct tsp1_segment_02
            {
            tsp1_segment_header sp1s_space1;
            tsp1_part_header sp1p_part_header;
            tsp00_MoveObj     sp1p_buf;
            }
        C_2;

        struct tsp1_segment_03
            {
            tsp00_Int4        sp1s_segm_len;
            tsp00_Int4        sp1s_segm_offset;
            tsp00_Int2        sp1s_no_of_parts;
            tsp00_Int2        sp1s_own_index;
            tsp1_segment_kind sp1s_segm_kind;
            /* --- cmd segment header --- */
            tsp1_cmd_mess_type sp1c_mess_type;
            tsp1_sqlmode    sp1c_sqlmode;
            tsp1_producer   sp1c_producer;
            boolean         sp1c_commit_immediately;
            boolean         sp1c_ignore_costwarning;
            boolean         sp1c_prepare;
            boolean         sp1c_with_info;
            boolean         sp1c_mass_cmd;
            boolean         sp1c_parsing_again;
            tsp00_C2          sp1c_filler1;
            tsp00_C8          sp1c_filler2;
            tsp00_C8          sp1c_filler3;
            }
        C_3;

        struct tsp1_segment_04
            {
            tsp00_C8          sp1s_space2;
            tsp00_C4          sp1s_space3;
            tsp1_segment_kind sp1s_space4;
            /* --- return segment header --- */
            tsp00_SqlState    sp1r_sqlstate;  /* 5 bytes */
            tsp00_Int2        sp1r_returncode;
            tsp00_Int4        sp1r_errorpos;
            tsp00_WarningSet  sp1r_extern_warning;
            tsp1_warningset_intern sp1r_intern_warning;
            tsp00_Int2        sp1r_function_code;  /* csp1_.._fc */
            tsp00_Int2        sp1r_filler1;
            tsp00_C8          sp1r_filler2;
            /* --- first part --- */
            tsp1_part_kind  sp1p_part_kind;  /* |     */
            tsp1_part_attr  sp1p_attributes;  /* |first */
            tsp00_Int2        sp1p_arg_count;  /* |part */
            tsp00_Int4        sp1p_segm_offset;  /* |head */
            tsp00_Int4        sp1p_buf_len;  /* |     */
            tsp00_Int4        sp1p_buf_size;  /* |     */
            tsp00_ResNum      sp1p_resnum;
            }
        C_4;
        }
    integer;
    }
tsp1_segment;

 /*ENDCASE */

typedef tsp1_segment *tsp1_segment_ptr;

 /* */


typedef struct tsp1_packet_header
    {
    tsp00_CodeType   sp1h_mess_code;  /* code of old packet */
    tsp00_SwapKind   sp1h_mess_swap;  /* swap of old packet */
    tsp00_Int2        sp1h_filler1;
    tsp00_C5          sp1h_appl_version;  /* | senderid of      */
    tsp00_C3          sp1h_application;  /* |    old packet    */
    tsp00_Int4        sp1h_varpart_size;
    /*  */
    tsp00_Int4        sp1h_varpart_len;
    tsp00_Int2        sp1h_filler2;
    tsp00_Int2        sp1h_no_of_segm;

    tsp00_C8          sp1h_filler3;
    }
tsp1_packet_header;



typedef struct tsp1_packet
    {
    tsp1_packet_header sp1_header;

    union
        {

        struct tsp1_packet_01
            {
            tsp00_MoveObj     sp1_varpart;
            }
        C_1;

        struct tsp1_packet_02
            {
            tsp1_segment    sp1_segm;
            }
        C_2;
        }
    integer;
    }
tsp1_packet;

 /*ENDCASE */

typedef tsp1_packet *tsp1_packet_ptr;

 /* */
typedef tsp1_packet_ptr tsp1_packet_list[CSP1_MAX_PACKET_CNT];

 /* */


#define  SP1OT_MANDATORY  0

#define  SP1OT_OPTIONAL  1

#define  SP1OT_DEFAULT  2

#define  SP1OT_ESCAPE_CHAR  3
typedef signed char tsp1_param_opt_type;

 /* */
/* SET */ typedef signed char tsp1_param_opt_set;

 /* */


#define  SP1IO_INPUT  0

#define  SP1IO_OUTPUT  1

#define  SP1IO_INOUT  2
typedef signed char tsp1_param_io_type;

typedef  struct tsp1_param_info {
 tsp1_param_opt_set sp1i_mode;
 tsp1_param_io_type sp1i_io_type;
 tsp00_DataType sp1i_data_type;
 tsp00_Uint1 sp1i_frac;
 tsp00_Int2 sp1i_length;
 tsp00_Int2 sp1i_in_out_len;
 tsp00_Int4 sp1i_bufpos;
} tsp1_param_info;

typedef struct tsp1_dcom_param_info {
 tsp1_param_opt_set sp1i_dcom_mode;
 tsp1_param_io_type sp1i_dcom_io_type;
 tsp00_Int2 sp1i_dcom_datatype;
 tsp00_Int2 sp1i_dcom_sub_datatype;
 tsp00_Int2 sp1i_dcom_length;
 tsp00_Int2 sp1i_dcom_dim;
 tsp00_Uint1 sp1i_dcom_frac;
 tsp00_Uint1 sp1i_dcom_filler;
 tsp00_C16  sp1i_dcom_type_uid;
} tsp1_dcom_param_info;
	/*ENDCASE*/
	/* die beiden naechsten Typen ersetzen die vsp00-Typen */



typedef  struct tsp1_param_spec {
 tsp00_DataType para_type;
 char para_frac;
 tsp00_Int2 para_length;
} tsp1_param_spec;


typedef  tsp1_param_spec tsp1_param_spec_list[ csp_maxvariables ] ;


