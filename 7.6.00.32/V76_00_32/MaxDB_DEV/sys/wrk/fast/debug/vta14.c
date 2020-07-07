#include <ptoc.h>

typedef
  unsigned char tsp00_Uint1;
typedef
  short tsp00_Int2;
typedef
  tsp00_Int2 tsp00_Uint2;
typedef
  int tsp00_Int4;
typedef
  unsigned char tsp00_C1[1];
typedef
  unsigned char tsp00_C2[2];
typedef
  unsigned char tsp00_C4[4];
typedef
  unsigned char tsp00_C6[6];
typedef
  unsigned char tsp00_C8[8];
typedef
  unsigned char tsp00_C10[10];
typedef
  unsigned char tsp00_C16[16];
typedef
  unsigned char tsp00_C18[18];
typedef
  unsigned char tsp00_C20[20];
typedef
  unsigned char tsp00_C24[24];
typedef
  unsigned char tsp00_C30[30];
typedef
  unsigned char tsp00_C32[32];
typedef
  unsigned char tsp00_C42[42];
typedef
  unsigned char tsp00_C50[50];
typedef
  unsigned char tsp00_C60[60];
typedef
  unsigned char tsp00_Line[132];
typedef
  unsigned char tsp00_MoveObj[8192000];
typedef
  tsp00_C18 tsp00_Name;
typedef
  tsp00_Int4 tsp00_PageNo;
typedef
  unsigned char tsp00_ScreenBuf[8488];
typedef
  unsigned char tsp00_Sname[12];
typedef
  unsigned char tsp00_VFilename[256];
typedef
  tsp00_C8 tsp00_VtLabel;
typedef
  tsp00_MoveObj *tsp00_MoveObjPtr;
typedef
  tsp00_ScreenBuf *tsp00_ScreenBufAddr;
typedef
  struct ptoc_Type0
    {
    union  
      {
      struct  
        {
        tsp00_Int2 mapInt_sp00;
        } C_true;
      struct  
        {
        tsp00_C2 mapC2_sp00;
        } C_false;
      } boolean;
    } tsp00_IntMapC2;
typedef
  tsp00_C8 tsp00_VtLlabels[9][2];
typedef
  tsp00_C8 tsp00_VtSlabels[9];
typedef
  struct ptoc_Type1
    {
    tsp00_VtLabel text;
    tsp00_Int2 lno;
    tsp00_Int2 colnr;
    tsp00_C1 att;
    char fill1;
    tsp00_Int2 fill2;
    } tsp00_VtField;
typedef
  struct ptoc_Type2
    {
    tsp00_VtSlabels slabels;
    tsp00_VtLlabels llabels;
    tsp00_Line sys_text;
    tsp00_Line sys_attr;
    tsp00_VtField insert_label;
    } tsp00_HifParms;
typedef
  struct ptoc_Type3
    {
    tsp00_C4 ci4_gg00;
    } tgg90_Cint4;
typedef
  struct ptoc_Type4
    {
    tsp00_C2 ci2_gg00;
    } tgg91_FileVersion;
typedef
  struct ptoc_Type5
    {
    tsp00_C6 ci6_gg00;
    } tgg91_TransNo;
typedef
  tsp00_Int4 tgg00_ConverterVersion;
typedef
  unsigned char tgg00_MessType;
typedef
  unsigned char tgg00_MessType2;
typedef
  unsigned char tgg00_PageMode[1];
typedef
  unsigned char tgg00_PageType2;
typedef
  tsp00_Uint2 tgg00_ObjPagePos;
typedef
  unsigned char tgg00_PageHeadStr[8];
typedef
  tsp00_Int4 tgg00_PagePos;
typedef
  tsp00_C8 tgg00_Surrogate;
typedef
  struct ptoc_Type6
    {
    union  
      {
      struct  
        {
        tsp00_PageNo pageId_gg00;
        unsigned char pageType_gg00;
        tgg00_PageType2 pageType2_gg00;
        unsigned char pageCheck_gg00;
        tgg00_PageMode pageMode_gg00;
        } C_1;
      struct  
        {
        tgg00_PageHeadStr pageHeaderStr_gg00;
        } C_2;
      } integer;
    } tgg00_PageHeader;
typedef
  tgg00_PageHeader tgg00_PageTrailer;
typedef
  struct ptoc_Type7
    {
    tsp00_Int4 trlChecksum_gg00;
    tgg00_PageTrailer trlTrailer_gg00;
    } tgg00_PageCheckTrailer;
typedef
  unsigned char tut_diag_scan[4];
typedef
  struct ptoc_Type8
    {
    tsp00_Int4 utvf_handle;
    tsp00_Int4 utvf_filler;
    } tut_vf_fileref;
typedef
  struct ptoc_Type9
    {
    tsp00_Int4 utfp_block_no;
    tsp00_Int2 utfp_page_no;
    tsp00_Int2 utfp_pages_per_block;
    } tut_file_pos;
typedef
  unsigned char tut_pfkey;
typedef
  struct ptoc_Type10
    {
    tut_vf_fileref inputfile[25];
    unsigned char inputfn[25][256];
    int inputfilpos[25];
    unsigned char param_line[25][512];
    short param_lwb[25][10];
    short param_upb[25][10];
    short count_cmd_lines[20];
    unsigned char last_cmd_lines[20][20][132];
    unsigned char lines[1500][132];
    struct ptoc_Type11
      {
      unsigned char ld_type;
      char ld_new_page;
      char ld_in_code1;
      char ld_in_code2;
      tsp00_C2 ld_attr1;
      tsp00_C2 ld_attr2;
      tsp00_Int2 ld_in_len1;
      tsp00_Int2 ld_in_len2;
      } ln_desc[1500];
    int prime_number[506];
    tsp00_HifParms hif;
    tsp00_Line blankline;
    tsp00_Line blank_window_line;
    tsp00_ScreenBufAddr screen_buf;
    tsp00_ScreenBufAddr screen_att;
    tsp00_VFilename blankfilename;
    tsp00_Int4 bottom_frame_offset;
    tsp00_Int4 help_line_offset;
    tsp00_Int4 msg_line_offset;
    tsp00_Int4 screen_chars;
    tsp00_Int4 term_ref;
    tsp00_Int4 top_frame_offset;
    tsp00_Int2 bottom;
    tsp00_Int2 curr_inputfile;
    tsp00_Int2 currcol;
    tsp00_Int2 currline;
    tsp00_Int2 first_cmd_line;
    tsp00_Int2 header_lines;
    tsp00_Int2 last_cmd_line;
    tsp00_Int2 last_cmd_no;
    tsp00_Int2 put_linecount;
    tsp00_Int2 retcode;
    tsp00_Int2 retcode_external;
    tsp00_Int2 screen_cols;
    tsp00_Int2 screen_lines;
    tsp00_Int2 top;
    tsp00_Int2 window_bottom;
    tsp00_Int2 window_cols;
    tsp00_Int2 window_currline;
    tsp00_Int2 window_lines;
    tsp00_Int2 window_top;
    tsp00_Int2 used_window_lines;
    char has_sysline;
    char help_state;
    char is_batch;
    char is_cmd_read;
    char is_display;
    char is_frame;
    char is_holding;
    char is_inputfile;
    char is_mask;
    char is_reset_called;
    char is_virt_term;
    char screen_available;
    char screen_changed;
    char use_pfkeys;
    char io_ok;
    char use_script;
    } tut_terminal;
typedef
  unsigned char tbd_file_state_set[1];
typedef
  unsigned char tbd_objpage_state_set[1];
typedef
  tsp00_Int4 tbd00_LockRequestSeqNo;
typedef
  unsigned char tbd_node_header[80];
typedef
  unsigned char tbd_node_body[8100];
typedef
  int tbd_pointer_list[2025];
typedef
  unsigned char tbd00_ConsistencyInfo[1];
typedef
  struct ptoc_Type12
    {
    tsp00_C50 filler1;
    tsp00_Int2 fixRecordLength;
    tsp00_Int4 filler2;
    tsp00_C24 filler3;
    } tbd00_PageFixSizeSpace;
typedef
  struct ptoc_Type13
    {
    tsp00_C42 filler1;
    tsp00_Int2 filler2;
    tsp00_Int4 firstEntrySequence;
    tsp00_Int4 filler3;
    tsp00_Int4 lastEntrySequence;
    tsp00_C8 filler4;
    tgg91_TransNo transno;
    tsp00_C10 filler5;
    } tbd00_RedoPage;
typedef
  struct ptoc_Type14
    {
    tsp00_C50 filler1;
    tsp00_Int2 filler2;
    tsp00_Int4 lastEntrySequence;
    tsp00_Int4 pagecount;
    char relevantForGC;
    char filler3;
    tsp00_Int2 filler4;
    tgg91_TransNo transno;
    tsp00_Int2 lastGC_offset;
    tsp00_PageNo lastGC_pageno;
    tsp00_Int4 filler5;
    } tbd00_UndoPage;
typedef
  struct ptoc_Type15
    {
    tsp00_C18 filler1;
    char filler2;
    char isCreatedDuringSavepoint;
    tsp00_C32 filler5;
    tsp00_Int4 oldestEOTSequence;
    tsp00_C16 filler7;
    tsp00_Int4 savepointSequence;
    tsp00_Int4 filler8;
    } tbd00_OpenTransPage;
typedef
  struct ptoc_Type16
    {
    union  
      {
      struct  
        {
        tgg00_PageHeader nd_header;
        } C_1;
      struct  
        {
        tbd_node_header nd_full_header;
        tbd_node_body nd_body;
        tgg00_PageCheckTrailer nd_full_trailer;
        } C_2;
      struct  
        {
        tsp00_PageNo nd_id;
        unsigned char nd_pt;
        tgg00_PageType2 nd_pt2;
        unsigned char nd_checktype;
        tgg00_PageMode nd_pmode;
        tgg00_PagePos nd_bottom;
        tsp00_Int4 nd_record_cnt;
        tsp00_Int2 nd_level;
        tbd_file_state_set nd_file_state;
        char nd_sorted;
        tsp00_PageNo nd_root;
        tsp00_PageNo nd_right;
        tsp00_PageNo nd_left;
        tsp00_PageNo nd_last;
        tsp00_Int4 nd_filler2;
        tgg00_ConverterVersion nd_conv_version;
        tsp00_Int4 nd_str_version;
        tgg91_FileVersion nd_file_version;
        tbd00_ConsistencyInfo nd_history_info;
        tsp00_Uint1 ndPageVersion_bd00;
        tsp00_Int4 ndInvUsageCnt_bd00;
        tsp00_Int4 ndLeafCount_bd00;
        tsp00_Int4 ndTreeLeaves_bd00;
        tgg91_TransNo nd_trans_id;
        tsp00_Int2 nd_filler4;
        tsp00_PageNo ndInvRoot_bd00;
        tsp00_Int4 nd_write_cnt;
        tbd_pointer_list nd_pointer_list;
        tsp00_Int4 nd_checksum;
        tgg00_PageTrailer nd_trailer;
        } C_3;
      struct  
        {
        tsp00_C8 nd_header_space;
        tsp00_Uint2 nd_max_obj_cnt;
        tsp00_Uint2 nd_occ_obj_cnt;
        tsp00_Uint2 nd_free_eot_obj_cnt;
        tsp00_Uint2 nd_free_obj_cnt;
        tsp00_Int2 nd_level_space;
        tbd_file_state_set nd_file_state_space;
        tbd_objpage_state_set nd_objpage_state;
        tsp00_PageNo nd_root_space;
        tsp00_PageNo nd_right_space;
        tsp00_PageNo nd_left_space;
        tsp00_Int2 nd_filler5_2;
        char nd_in_free_chain;
        tsp00_Uint1 nd_num_key_files;
        tbd00_LockRequestSeqNo ndLockRequestSeqNoLow_bd00;
        tgg00_ConverterVersion nd_conv_vers_space;
        tsp00_Uint2 nd_obj_frame_len;
        tsp00_Uint2 nd_obj_key_len;
        tgg91_FileVersion nd_file_vers_space;
        tgg00_ObjPagePos nd_first_free_obj;
        tgg00_ObjPagePos nd_first_obj;
        tsp00_Int2 nd_filler5_4;
        tsp00_C4 nd_obj_file_no1;
        tbd00_LockRequestSeqNo ndLockRequestSeqNoHigh_bd00;
        tsp00_PageNo nd_next_free;
        tsp00_PageNo nd_prev_free;
        tsp00_C4 nd_obj_file_no2;
        tsp00_Int4 nd_write_cnt_space;
        } C_4;
      struct  
        {
        tsp00_C8 ndStrHeaderSpace_bd00;
        tgg00_PagePos ndStrBottomSpace_bd00;
        tsp00_Int4 ndStrFiller1_bd00;
        tsp00_Int2 ndStrLevelSpace_bd00;
        tbd_file_state_set ndStrFileStateSpace_bd00;
        char ndStrFiller2_bd00;
        tsp00_PageNo ndStrRootSpace_bd00;
        tsp00_PageNo ndStrRightSpace_bd00;
        tsp00_PageNo ndStrLeftSpace_bd00;
        tgg00_Surrogate ndStrParentID_bd00;
        tgg00_ConverterVersion ndStrConvVersionSpace_bd00;
        tsp00_Int4 ndStrVersion_bd00;
        tgg91_FileVersion ndStrFileVersionSpace_bd00;
        tsp00_C6 ndStrFiller4_bd00;
        tsp00_Int4 ndStrFileSize_bd00;
        tsp00_Int4 ndStrFiller5_bd00;
        tsp00_C8 ndStrFiller6_bd00;
        tsp00_Int4 ndStrFiller7_bd00;
        tsp00_Int4 ndStrWriteCntSpace_bd00;
        } C_5;
      struct  
        {
        tsp00_C8 ndHeaderSpace_bd00;
        tgg00_PagePos ndFirstOffset_bd00;
        tsp00_Int4 ndFieldCount_bd00;
        tsp00_Int2 nd_level_space2;
        tbd_file_state_set nd_file_state_space2;
        char nd_sorted_space2;
        tsp00_PageNo nd_root_space2;
        tsp00_PageNo nd_right_space2;
        tsp00_PageNo nd_left_space2;
        tgg00_PagePos ndFirstFreeOffset_bd00;
        tsp00_Int4 ndAllFieldInQueueCount_bd00;
        } C_7;
      struct  
        {
        tbd00_PageFixSizeSpace ndPageFixSizeSpace_bd00;
        } C_8;
      struct  
        {
        tbd00_UndoPage ndUndoPage_bd00;
        } C_9;
      struct  
        {
        tbd00_RedoPage ndRedoPage_bd00;
        } C_10;
      struct  
        {
        tbd00_OpenTransPage ndOpenTransPage_bd00;
        } C_11;
      } integer;
    } tbd_node;
typedef
  tbd_node *tbd_nodeptr;
typedef
  struct ptoc_Type17
    {
    union  
      {
      struct  
        {
        tsp00_Line ln;
        } C_1;
      struct  
        {
        tsp00_C4 prefix;
        } C_2;
      } integer;
    } hexbuf_line;
typedef
  struct ptoc_Type18
    {
    union  
      {
      struct  
        {
        tsp00_Int4 map_i4;
        } C_true;
      struct  
        {
        tsp00_C4 map_c4;
        } C_false;
      } boolean;
    } int_map_c4;
int gg06Int4FromUint2();
int gg06Int4ToUint2();
int gg06PnoGet();
int gg06PnoPut();
int g17hexto_line();
int g17int4to_line();
int g17messtype_to_line();
int g17mess2type_to_line();
char g17printable_char();
int g17sname_to_line();
int g17trimint4_to_line();
int t09getcursor_line();
int t09get1name();
int t09get2name();
int t09goto_bottom();
int t09holding();
char t09is_empty_window();
char t09is_window_full();
int t09newscreen_page();
int t09pfkeys();
int t09put();
int t09put60();
int t09putmsg();
int t09setpf1();
int t09window_cols();
int t09unused_window_lines();
int t09uppercase_line();
int t12write_prot();
int x04afterimagehead_display();
int x07debug_page();
int x07edit_page();
int x07keys_display();
int x08beforeimage_edit_display();
int x08fdir_rec_edit();
int x08obj_edit();
int x08rec_edit();
int x08tree_index_rec_edit();
int SAPDB_PascalForcedMove();
int SAPDB_PascalForcedOverlappingMove();
int SAPDB_PascalForcedFill();
int t14bufdimension();
int t14bufdisplay();
int t14display_msg();
int t14editbuf();
int t14int4from_line();
int t14logentry_edit();
int t14object_edit();
int t14record_edit();
int ta14display_leave_msg();
int ta14edit_bufdimension();
int ta14fill();
int ta14help();
int ta14hex_from_line();
char ta14is_nil_pno();
int ta14move();
int ta14mtype_assign();
int ta14mtype2_assign();
int ta14set_assign();



int t14bufdimension(term, ptoc_ptr_scan, bytes_per_line, line_size, lines_per_buf)
tut_terminal *term;
tut_diag_scan ptoc_ptr_scan;
int *bytes_per_line;
int *line_size;
int *lines_per_buf;
    {
    tut_diag_scan scan;
    _ptoc_MOVE(char, (long)sizeof(tut_diag_scan ), scan, ptoc_ptr_scan);
    if (1 && (scan[1] & 128) != 0)
        {
        *lines_per_buf = 2;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        *bytes_per_line = (t09window_cols(&(*term)) + -5) / 2 / 10 * 10;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        if (*bytes_per_line * 2 + (*bytes_per_line + -1) / 10 + 5 > t09window_cols(&(*term)))
            {
            *bytes_per_line = *bytes_per_line + -10;
            }
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        *line_size = *bytes_per_line * 2 + (*bytes_per_line + -1) / 10 + 5;
        }
    else
        {
        *lines_per_buf = 5;
        if (1 && (scan[2] & 2) != 0)
            {
            *lines_per_buf = *lines_per_buf + 1;
            }
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        *bytes_per_line = (t09window_cols(&(*term)) + -4) / 3 / 10 * 10;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        *line_size = *bytes_per_line * 3 + 4;
        }
    }



int t14bufdisplay(term, protfile, ptoc_ptr_scan, ptoc_ptr_msg, buf, buf_size, startpos, endpos, start_numbering, break_pos, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tut_diag_scan ptoc_ptr_scan;
tsp00_C30 ptoc_ptr_msg;
tsp00_MoveObj buf;
tsp00_Int4 buf_size;
tsp00_Int4 startpos;
tsp00_Int4 endpos;
tsp00_Int4 start_numbering;
tsp00_Int4 *break_pos;
tut_pfkey *pfkey;
    {
    char skip_output;
    char last_skipped;
    char is_blank;
    char blank_only;
    unsigned char vt_attr;
    tgg90_Cint4 cint4;
    tsp00_Int4 upb;
    tsp00_Int4 buf_pos;
    tsp00_Int4 numb_pos;
    tsp00_Int4 out_pos;
    tsp00_Int4 pos_displ;
    tsp00_Int4 skip_numb_pos;
    int dummy_int;
    int line_size;
    int lines_per_buf;
    int ln_len;
    int i;
    int err;
    int bytes_per_line;
    tsp00_Int4 byte_count;
    tsp00_Int4 char_count;
    tsp00_Int4 prev_char_cnt;
    tsp00_IntMapC2 i2;
    int_map_c4 i4;
    int_map_c4 i4_overflow;
    hexbuf_line chrline;
    hexbuf_line c2_line;
    hexbuf_line c4_line;
    hexbuf_line decline;
    hexbuf_line hexline;
    hexbuf_line int2_line;
    hexbuf_line int4_line;
    hexbuf_line posline;
    hexbuf_line posline_2;
    tsp00_Line spaceline;
    hexbuf_line u2_line;
    char odd_bytecount;
    int ptoc_Var0;
    tut_diag_scan scan;
    tsp00_C30 msg;
    _ptoc_MOVE(char, (long)sizeof(tut_diag_scan ), scan, ptoc_ptr_scan);
    _ptoc_MOVE(char, (long)sizeof(tsp00_C30 ), msg, ptoc_ptr_msg);
    *break_pos = 0;
    *pfkey = 0;
    if (1 && (scan[2] & 1) != 0)
        {
        vt_attr = (unsigned char)'\200';
        ta14edit_bufdimension(&(*term) , buf_size - startpos + start_numbering , &dummy_int , &bytes_per_line , &line_size , &lines_per_buf);
        i4_overflow.boolean.C_true.map_i4 = -2147483647;
        i = 1;
        do
            {
            if ((int ) i4_overflow.boolean.C_false.map_c4[i + -1] == 1)
                {
                i4_overflow.boolean.C_false.map_c4[i + -1] = (unsigned char)'\0';
                }
            if (i == 4)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    else
        {
        vt_attr = (unsigned char)'\0';
        t14bufdimension(&(*term) , scan , &bytes_per_line , &line_size , &lines_per_buf);
        }
    if (startpos < 1 || startpos > buf_size)
        {
        buf_pos = 1;
        upb = bytes_per_line;
        }
    else
        {
        buf_pos = startpos;
        if (endpos < startpos || endpos > buf_size)
            {
            upb = buf_size;
            }
        else
            {
            upb = endpos;
            }
        }
    numb_pos = start_numbering;
    if (1 && (scan[2] & 1) != 0)
        {
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        t12write_prot(&(*protfile) , (*term).blankline , 1 , &err);
        }
    else
        {
        _ptoc_ASSIGN(unsigned char , 132, chrline.integer.C_1.ln, (*term).blankline)
        ln_len = 0;
        g17sname_to_line("BUFFER FROM " , &ln_len , chrline.integer.C_1.ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(numb_pos , &ln_len , chrline.integer.C_1.ln);
        g17sname_to_line(" TO         " , &ln_len , chrline.integer.C_1.ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(numb_pos + upb - buf_pos , &ln_len , chrline.integer.C_1.ln);
        if (1 && (scan[2] & 2) != 0 && (0 || (scan[1] & 128) == 0))
            {
            g17sname_to_line(" (FROM      " , &ln_len , chrline.integer.C_1.ln);
            ln_len = ln_len + 1;
            g17trimint4_to_line(buf_pos , &ln_len , chrline.integer.C_1.ln);
            g17sname_to_line(" TO         " , &ln_len , chrline.integer.C_1.ln);
            ln_len = ln_len + 1;
            g17trimint4_to_line(upb , &ln_len , chrline.integer.C_1.ln);
            ln_len = ln_len + 1;
            chrline.integer.C_1.ln[ln_len + -1] = (unsigned char)')';
            }
        if (buf_pos != startpos)
            {
            g17sname_to_line("   *** from " , &ln_len , chrline.integer.C_1.ln);
            ln_len = ln_len + 1;
            g17trimint4_to_line(startpos , &ln_len , chrline.integer.C_1.ln);
            }
        if (upb != endpos)
            {
            g17sname_to_line("   *** to   " , &ln_len , chrline.integer.C_1.ln);
            ln_len = ln_len + 1;
            g17trimint4_to_line(endpos , &ln_len , chrline.integer.C_1.ln);
            }
        if ((1 && (scan[1] & 128) != 0 || upb - buf_pos + 1 < bytes_per_line) && (_ptoc_MEMCMP(unsigned char , 30, msg , "                              ") == 0 ? 1 : 0))
            {
            posline = chrline;
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 132, posline.integer.C_1.ln, (*term).blankline)
            if (_ptoc_MEMCMP(unsigned char , 30, msg , "                              ") == 0 ? 1 : 0)
                {
                SAPDB_PascalForcedMove((long)sizeof(unsigned char [132]) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) chrline.integer.C_1.ln , 1 , (tsp00_MoveObj *) posline.integer.C_1.ln , line_size - ln_len + 1 , ln_len);
                }
            else
                {
                SAPDB_PascalForcedMove((long)sizeof(tsp00_C30 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) msg , 1 , (tsp00_MoveObj *) posline.integer.C_1.ln , 1 , (long)sizeof(tsp00_C30 ));
                if (upb - buf_pos >= bytes_per_line)
                    {
                    SAPDB_PascalForcedMove((long)sizeof(unsigned char [132]) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) chrline.integer.C_1.ln , 1 , (tsp00_MoveObj *) posline.integer.C_1.ln , line_size - ln_len + 1 , ln_len);
                    }
                }
            }
        t12write_prot(&(*protfile) , posline.integer.C_1.ln , (long)sizeof(unsigned char [132]) , &err);
        if (1 && (scan[1] & 64) != 0)
            {
            if (_ptoc_MEMCMP(unsigned char , 30, msg , "                              ") == 0 ? 1 : 0)
                {
                t09put(&(*term) , posline.integer.C_1.ln , (unsigned char)'\0');
                }
            else
                {
                t09put(&(*term) , posline.integer.C_1.ln , (unsigned char)'\001');
                }
            }
        }
    last_skipped = 0;
    prev_char_cnt = 0;
    char_count = 0;
    blank_only = 1;
    do
        {
        skip_output = 0;
        if (buf_pos > startpos && buf_pos + bytes_per_line + -1 < upb && (0 || (scan[2] & 1) == 0))
            {
            i = buf_pos;
            skip_output = 1;
            do
                {
                if ((int ) buf[i - bytes_per_line + -1] != (int ) buf[i + -1])
                    {
                    skip_output = 0;
                    }
                i = i + 1;
                }
            while (!(!skip_output || i >= buf_pos + bytes_per_line));
            }
        if (skip_output && !last_skipped)
            {
            last_skipped = 1;
            skip_numb_pos = numb_pos;
            }
        if (skip_output)
            {
            buf_pos = buf_pos + bytes_per_line;
            numb_pos = numb_pos + bytes_per_line;
            }
        else
            {
            if (last_skipped || (0 || (scan[1] & 128) == 0) && (0 || (scan[2] & 1) == 0) && buf_pos > startpos && (blank_only || char_count == 0))
                {
                if (1 && (scan[1] & 64) != 0 && t09is_window_full(&(*term) , lines_per_buf + 1))
                    {
                    t09holding(&(*term) , &(*pfkey));
                    if ((int ) *pfkey == 0)
                        {
                        t09newscreen_page(&(*term));
                        }
                    if ((int ) *pfkey != 0)
                        {
                        *break_pos = buf_pos;
                        }
                    }
                if (last_skipped && (int ) *pfkey == 0)
                    {
                    last_skipped = 0;
                    _ptoc_ASSIGN(unsigned char , 132, chrline.integer.C_1.ln, (*term).blankline)
                    ln_len = 0;
                    g17sname_to_line("     ...    " , &ln_len , chrline.integer.C_1.ln);
                    ln_len = ln_len + 1;
                    g17trimint4_to_line(skip_numb_pos , &ln_len , chrline.integer.C_1.ln);
                    g17sname_to_line(" TO         " , &ln_len , chrline.integer.C_1.ln);
                    ln_len = ln_len + 1;
                    g17trimint4_to_line(numb_pos + -1 , &ln_len , chrline.integer.C_1.ln);
                    if (1 && (scan[2] & 2) != 0 && (0 || (scan[1] & 128) == 0))
                        {
                        ln_len = ln_len + 2;
                        chrline.integer.C_1.ln[ln_len + -1] = (unsigned char)'(';
                        g17trimint4_to_line(buf_pos - numb_pos + skip_numb_pos , &ln_len , chrline.integer.C_1.ln);
                        g17sname_to_line(" TO         " , &ln_len , chrline.integer.C_1.ln);
                        ln_len = ln_len + 1;
                        g17trimint4_to_line(buf_pos + -1 , &ln_len , chrline.integer.C_1.ln);
                        ln_len = ln_len + 1;
                        chrline.integer.C_1.ln[ln_len + -1] = (unsigned char)')';
                        }
                    t12write_prot(&(*protfile) , chrline.integer.C_1.ln , ln_len , &err);
                    if ((int ) *pfkey == 0 && (1 && (scan[1] & 64) != 0))
                        {
                        t09put(&(*term) , chrline.integer.C_1.ln , (unsigned char)'\0');
                        }
                    }
                else
                    {
                    if ((int ) *pfkey == 0)
                        {
                        t12write_prot(&(*protfile) , (*term).blankline , 1 , &err);
                        if (1 && (scan[1] & 64) != 0 && !t09is_empty_window(&(*term)))
                            {
                            t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
                            }
                        }
                    }
                }
            }
        if (!skip_output && (int ) *pfkey == 0)
            {
            _ptoc_ASSIGN(unsigned char , 132, hexline.integer.C_1.ln, (*term).blankline)
            _ptoc_ASSIGN(unsigned char , 132, chrline.integer.C_1.ln, (*term).blankline)
            if (1 && (scan[1] & 128) != 0)
                {
                g17int4to_line(numb_pos , 0 , 5 , 1 , chrline.integer.C_1.ln);
                out_pos = 6;
                }
            else
                {
                out_pos = 5;
                _ptoc_ASSIGN(unsigned char , 132, posline.integer.C_1.ln, (*term).blankline)
                _ptoc_ASSIGN(unsigned char , 132, posline_2.integer.C_1.ln, (*term).blankline)
                _ptoc_ASSIGN(unsigned char , 132, decline.integer.C_1.ln, (*term).blankline)
                _ptoc_ASSIGN(unsigned char , 4, posline.integer.C_2.prefix, "pos:")
                _ptoc_ASSIGN(unsigned char , 4, decline.integer.C_2.prefix, "dec:")
                _ptoc_ASSIGN(unsigned char , 4, hexline.integer.C_2.prefix, "hex:")
                _ptoc_ASSIGN(unsigned char , 4, chrline.integer.C_2.prefix, "chr:")
                if (1 && (scan[2] & 1) != 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 132, c2_line.integer.C_1.ln, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 132, c4_line.integer.C_1.ln, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 132, int2_line.integer.C_1.ln, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 132, int4_line.integer.C_1.ln, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 132, spaceline, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 132, u2_line.integer.C_1.ln, (*term).blankline)
                    _ptoc_ASSIGN(unsigned char , 4, c2_line.integer.C_2.prefix, "c2 :")
                    _ptoc_ASSIGN(unsigned char , 4, c4_line.integer.C_2.prefix, "c4 :")
                    _ptoc_ASSIGN(unsigned char , 4, int2_line.integer.C_2.prefix, "int2")
                    _ptoc_ASSIGN(unsigned char , 4, int4_line.integer.C_2.prefix, "int4")
                    _ptoc_ASSIGN(unsigned char , 4, u2_line.integer.C_2.prefix, "u2 :")
                    }
                }
            byte_count = 0;
            char_count = 0;
            blank_only = 1;
            do
                {
                byte_count = byte_count + 1;
                if (0 || (scan[1] & 128) == 0)
                    {
                    if (1 && (scan[2] & 1) != 0)
                        {
                        if (byte_count == 1)
                            {
                            int2_line.integer.C_1.ln[out_pos + -1] = (unsigned char)':';
                            int4_line.integer.C_1.ln[out_pos + -1] = (unsigned char)':';
                            }
                        out_pos = out_pos + 1;
                        if (byte_count > 1)
                            {
                            posline.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                            posline_2.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                            decline.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                            hexline.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                            chrline.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                            spaceline[out_pos + -1] = (unsigned char)'+';
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                            if (((ptoc_Var0 = byte_count % 2) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                            if (((ptoc_Var0 =(byte_count& 1)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
                                {
                                c2_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                                u2_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                                int2_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                                }
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                            if (((ptoc_Var0 = byte_count % 4) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) == 1)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                            if (((ptoc_Var0 =(byte_count& 3)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) == 1)
                                {
                                c4_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                                int4_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'|';
                                }
                            out_pos = out_pos + 1;
                            }
                        if (numb_pos < 1000)
                            {
                            i = 3;
                            }
                        else
                            {
                            i = 4;
                            }
                        if (buf_size - startpos + start_numbering >= 10000)
                            {
                            i = i + 1;
                            }
                        g17int4to_line(numb_pos , 0 , i , out_pos , posline.integer.C_1.ln);
                        if (1 && (scan[2] & 2) != 0)
                            {
                            if (buf_pos < 1000)
                                {
                                i = 3;
                                }
                            else
                                {
                                i = 4;
                                }
                            if (buf_size >= 10000)
                                {
                                i = i + 1;
                                }
                            g17int4to_line(buf_pos , 0 , i , out_pos , posline_2.integer.C_1.ln);
                            }
                        if (buf_size >= 10000)
                            {
                            spaceline[out_pos + -1] = (unsigned char)'-';
                            out_pos = out_pos + 1;
                            }
                        spaceline[out_pos + -1] = (unsigned char)'-';
                        spaceline[out_pos] = (unsigned char)'-';
                        spaceline[out_pos + 1] = (unsigned char)'-';
                        spaceline[out_pos + 2] = (unsigned char)'-';
                        }
                    else
                        {
                        if (byte_count == 1)
                            {
                            if (numb_pos < 1000 && (0 || (scan[2] & 2) == 0))
                                {
                                g17int4to_line(numb_pos , 0 , 3 , out_pos , posline.integer.C_1.ln);
                                }
                            else
                                {
                                g17int4to_line(numb_pos , 0 , 7 , 1 , posline.integer.C_1.ln);
                                }
                            }
                        else
                            {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                            if (numb_pos < 100 || ((ptoc_Var0 = byte_count % 2) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                            if (numb_pos < 100 || ((ptoc_Var0 =(byte_count& 1)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
                                {
                                if (numb_pos < 1000)
                                    {
                                    pos_displ = numb_pos;
                                    }
                                else
                                    {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                    pos_displ = (ptoc_Var0 = numb_pos % 1000) >= 0 ? ptoc_Var0 : ptoc_Var0 + 1000;
                                    }
                                g17int4to_line(pos_displ , numb_pos >= 1000 , 3 , out_pos , posline.integer.C_1.ln);
                                }
                            }
                        if (1 && (scan[2] & 2) != 0)
                            {
                            if (byte_count == 1)
                                {
                                g17int4to_line(buf_pos , 0 , 7 , 1 , posline_2.integer.C_1.ln);
                                }
                            else
                                {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                                if (buf_pos < 100 || ((ptoc_Var0 = byte_count % 2) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                                if (buf_pos < 100 || ((ptoc_Var0 =(byte_count& 1)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1)
                                    {
                                    if (buf_pos < 1000)
                                        {
                                        pos_displ = buf_pos;
                                        }
                                    else
                                        {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                        pos_displ = (ptoc_Var0 = buf_pos % 1000) >= 0 ? ptoc_Var0 : ptoc_Var0 + 1000;
                                        }
                                    g17int4to_line(pos_displ , buf_pos >= 1000 , 3 , out_pos , posline_2.integer.C_1.ln);
                                    }
                                }
                            }
                        }
                    g17int4to_line(sql__ord((int ) buf[buf_pos + -1]) , 0 , 3 , out_pos , decline.integer.C_1.ln);
                    }
                else
                    {
                    out_pos = out_pos + -1;
                    }
                ln_len = out_pos;
                g17hexto_line(buf[buf_pos + -1] , &ln_len , hexline.integer.C_1.ln);
                if (g17printable_char(buf[buf_pos + -1]))
                    {
                    chrline.integer.C_1.ln[out_pos + 1] = buf[buf_pos + -1];
                    }
                else
                    {
                    if (1 && (scan[1] & 128) != 0)
                        {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                        odd_bytecount = ((ptoc_Var0 = byte_count % 2) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                        odd_bytecount = ((ptoc_Var0 =(byte_count& 1)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 1;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                        if (odd_bytecount && ((ptoc_Var0 = byte_count % 10) >= 0 ? ptoc_Var0 : ptoc_Var0 + 10) != 1)
                            {
                            chrline.integer.C_1.ln[out_pos] = (unsigned char)'|';
                            }
                        }
                    }
                if (1 && (scan[2] & 1) != 0)
                    {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    if (((ptoc_Var0 = byte_count % 2) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 0)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    if (((ptoc_Var0 =(byte_count& 1)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 2) == 0)
                        {
                        cint4.ci4_gg00[0] = (unsigned char)'\0';
                        cint4.ci4_gg00[1] = (unsigned char)'\0';
                        cint4.ci4_gg00[2] = buf[buf_pos + -2];
                        cint4.ci4_gg00[3] = buf[buf_pos + -1];
                        i2.boolean.C_false.mapC2_sp00[0] = buf[buf_pos + -2];
                        i2.boolean.C_false.mapC2_sp00[1] = buf[buf_pos + -1];
                        g17int4to_line(gg06Int4FromUint2(i2.boolean.C_true.mapInt_sp00) , 0 , 6 , out_pos + -3 , u2_line.integer.C_1.ln);
                        g17int4to_line(gg06PnoGet(&cint4) , 0 , 6 , out_pos + -3 , c2_line.integer.C_1.ln);
                        g17int4to_line((int ) i2.boolean.C_true.mapInt_sp00 , 0 , 6 , out_pos + -3 , int2_line.integer.C_1.ln);
                        }
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    if (((ptoc_Var0 = byte_count % 4) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) == 0)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    if (((ptoc_Var0 =(byte_count& 3)) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) == 0)
                        {
                        i = 1;
                        do
                            {
                            i4.boolean.C_false.map_c4[i + -1] = buf[buf_pos + i + -5];
                            if (i == 4)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        _ptoc_ASSIGN(unsigned char , 4, cint4.ci4_gg00, i4.boolean.C_false.map_c4)
                        if (_ptoc_MEMCMP(unsigned char , 4, i4.boolean.C_false.map_c4 , i4_overflow.boolean.C_false.map_c4) == 0 ? 1 : 0)
                            {
                            int4_line.integer.C_1.ln[out_pos + -2] = (unsigned char)'-';
                            int4_line.integer.C_1.ln[out_pos + -1] = (unsigned char)'.';
                            int4_line.integer.C_1.ln[out_pos] = (unsigned char)'-';
                            }
                        else
                            {
                            g17int4to_line(i4.boolean.C_true.map_i4 , 0 , 12 , out_pos + -9 , int4_line.integer.C_1.ln);
                            }
                        if (gg06PnoGet(&cint4) >= 0)
                            {
                            g17int4to_line(gg06PnoGet(&cint4) , 0 , 12 , out_pos + -9 , c4_line.integer.C_1.ln);
                            }
                        }
                    }
                if (1 && (scan[1] & 128) != 0)
                    {
                    out_pos = out_pos + 3;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                    if (((ptoc_Var0 = byte_count % 10) >= 0 ? ptoc_Var0 : ptoc_Var0 + 10) == 0)
                        {
                        out_pos = out_pos + 1;
                        }
                    }
                else
                    {
                    out_pos = out_pos + 3;
                    }
                buf_pos = buf_pos + 1;
                numb_pos = numb_pos + 1;
                }
            while (!(byte_count >= bytes_per_line || buf_pos > upb));
            if (1 && (scan[1] & 64) != 0 && t09is_window_full(&(*term) , lines_per_buf))
                {
                t09holding(&(*term) , &(*pfkey));
                if ((int ) *pfkey == 0)
                    {
                    t09newscreen_page(&(*term));
                    }
                if ((int ) *pfkey != 0)
                    {
                    *break_pos = buf_pos;
                    }
                }
            if ((int ) *pfkey == 0 && (1 && (scan[1] & 64) != 0))
                {
                if (0 || (scan[1] & 128) == 0)
                    {
                    t09put(&(*term) , posline.integer.C_1.ln , (unsigned char)'\0');
                    if (1 && (scan[2] & 2) != 0)
                        {
                        t09put(&(*term) , posline_2.integer.C_1.ln , (unsigned char)'\0');
                        }
                    if (1 && (scan[2] & 1) != 0)
                        {
                        t09put(&(*term) , spaceline , (unsigned char)'\0');
                        }
                    t09put(&(*term) , decline.integer.C_1.ln , vt_attr);
                    }
                t09put(&(*term) , hexline.integer.C_1.ln , vt_attr);
                t09put(&(*term) , chrline.integer.C_1.ln , vt_attr);
                }
            if ((int ) *pfkey == 0)
                {
                if (1 && (scan[2] & 1) != 0)
                    {
                    t09put(&(*term) , c2_line.integer.C_1.ln , vt_attr);
                    t09put(&(*term) , u2_line.integer.C_1.ln , vt_attr);
                    t09put(&(*term) , int2_line.integer.C_1.ln , vt_attr);
                    t09put(&(*term) , c4_line.integer.C_1.ln , vt_attr);
                    t09put(&(*term) , int4_line.integer.C_1.ln , vt_attr);
                    }
                if (1 && (scan[1] & 128) != 0)
                    {
                    hexline.integer.C_1.ln[1] = (unsigned char)'@';
                    hexline.integer.C_1.ln[2] = (unsigned char)'@';
                    }
                else
                    {
                    t12write_prot(&(*protfile) , posline.integer.C_1.ln , out_pos + -1 , &err);
                    if (1 && (scan[2] & 2) != 0)
                        {
                        t12write_prot(&(*protfile) , posline_2.integer.C_1.ln , out_pos + -1 , &err);
                        }
                    t12write_prot(&(*protfile) , decline.integer.C_1.ln , out_pos + -1 , &err);
                    }
                t12write_prot(&(*protfile) , hexline.integer.C_1.ln , out_pos + -1 , &err);
                t12write_prot(&(*protfile) , chrline.integer.C_1.ln , out_pos + -1 , &err);
                if (1 && (scan[2] & 1) != 0)
                    {
                    t12write_prot(&(*protfile) , c2_line.integer.C_1.ln , out_pos + -1 , &err);
                    t12write_prot(&(*protfile) , u2_line.integer.C_1.ln , out_pos + -1 , &err);
                    t12write_prot(&(*protfile) , int2_line.integer.C_1.ln , out_pos + -1 , &err);
                    t12write_prot(&(*protfile) , c4_line.integer.C_1.ln , out_pos + -1 , &err);
                    t12write_prot(&(*protfile) , int4_line.integer.C_1.ln , out_pos + -1 , &err);
                    }
                }
            }
        }
    while (!(buf_pos > upb || (int ) *pfkey != 0));
    }



int t14display_msg(term, ptoc_ptr_msg)
tut_terminal *term;
tsp00_C20 ptoc_ptr_msg;
    {
    tsp00_Line ln;
    tsp00_C20 msg;
    _ptoc_MOVE(char, (long)sizeof(tsp00_C20 ), msg, ptoc_ptr_msg);
    _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
    SAPDB_PascalForcedMove((long)sizeof(tsp00_C20 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) msg , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C20 ));
    t09putmsg(&(*term) , ln , 1 , 0);
    }

static
  tut_diag_scan ptoc_Var1 = 
  {
  (unsigned char)'\0', (unsigned char)'\200', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var2 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var3 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var4[4] = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\002', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var5 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var6 = 
  {
  (unsigned char)'\0', (unsigned char)'\300', (unsigned char)'\0', (unsigned char)'\0'
  };


int t14editbuf(term, protfile, buf, buf_size, buf_offset, file_pos, ptoc_ptr_edit_scan, is_saved, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj buf;
tsp00_Int4 buf_size;
tsp00_Int4 buf_offset;
tut_file_pos *file_pos;
tut_diag_scan ptoc_ptr_edit_scan;
char *is_saved;
tut_pfkey *pfkey;
    {
    char is_ok;
    char is_quit;
    char is_cmd;
    char is_locate_cmd;
    char pfkeys_used;
    unsigned char indicator;
    tut_diag_scan scan;
    unsigned char c;
    int dummy_err;
    tgg90_Cint4 cint4;
    tsp00_Int4 i;
    tsp00_Int4 lwb;
    tsp00_Int4 upb;
    tsp00_Int4 type_upb;
    tsp00_Int4 dummy_pos;
    int dummy_size;
    int bytes_per_line;
    int char_per_byte;
    int curr_col;
    int curr_col_len;
    int col_type_pos;
    int lines_per_buf;
    int minbuf_bytes;
    int minbuf_lines;
    int byte_offset;
    tsp00_IntMapC2 i2;
    int_map_c4 i4;
    tsp00_Int4 int_4;
    tsp00_Int4 new_lwb;
    tsp00_C6 bool_value;
    tsp00_Name mtype_value;
    hexbuf_line in_line;
    tbd_nodeptr node_ptr;
    tsp00_Line ln;
    int ln_len;
    int err;
    tut_diag_scan ptoc_Var7;
    unsigned char ptoc_Var8[4];
    unsigned char ptoc_Var9[4];
    tut_diag_scan edit_scan;
    _ptoc_MOVE(char, (long)sizeof(tut_diag_scan ), edit_scan, ptoc_ptr_edit_scan);
    lwb = buf_offset + 1;
    *is_saved = 0;
    is_quit = 0;
    t14bufdimension(&(*term) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var7, ptoc_Var1) , ptoc_Var7) , &minbuf_bytes , &dummy_size , &minbuf_lines);
    ta14edit_bufdimension(&(*term) , buf_size , &char_per_byte , &bytes_per_line , &dummy_size , &lines_per_buf);
    pfkeys_used = (*term).use_pfkeys;
    t09pfkeys(&(*term) , 0);
    do
        {
        is_ok = 0;
        if (buf_size > lwb + bytes_per_line + -1)
            {
            upb = lwb + bytes_per_line + -1;
            }
        else
            {
            upb = buf_size;
            }
        do
            {
            is_cmd = 0;
            is_locate_cmd = 0;
            t09newscreen_page(&(*term));
            if (sql__nre((long)sizeof(tut_diag_scan ) , edit_scan , ptoc_Var2))
                {
                t12write_prot(&(*protfile) , (*term).blankline , 1 , &dummy_err);
                }
            if (1 && (edit_scan[0] & 2) != 0)
                {
                x07edit_page(&(*term) , &(*protfile) , &(*file_pos) , buf , buf_size , &(*pfkey));
                }
            else
                {
                if (1 && (edit_scan[1] & 1) != 0)
                    {
                    node_ptr = (tbd_node *) buf;
                    if ((int ) node_ptr->integer.C_3.nd_pt == 1 && ((int ) node_ptr->integer.C_3.nd_pt2 == 13 || (int ) node_ptr->integer.C_3.nd_pt2 == 7 || (int ) node_ptr->integer.C_3.nd_pt2 == 8))
                        {
                        if (1 && (node_ptr->integer.C_3.nd_pmode[0] & 4) != 0 && (int ) node_ptr->integer.C_3.nd_level == 0)
                            {
                            x08fdir_rec_edit(&(*term) , &(*protfile) , buf , buf_size , buf_offset , 1 , &(*pfkey));
                            }
                        else
                            {
                            if ((int ) node_ptr->integer.C_3.nd_level == 0)
                                {
                                x08rec_edit(&(*term) , &(*protfile) , buf , buf_size , buf_offset , 1 , 1 && (node_ptr->integer.C_3.nd_pmode[0] & 2) != 0 , node_ptr->integer.C_3.nd_pt2 , &(*pfkey));
                                }
                            else
                                {
                                x08tree_index_rec_edit(&(*term) , &(*protfile) , buf , buf_size , buf_offset , 1 , &(*pfkey));
                                }
                            }
                        }
                    else
                        {
                        if ((int ) node_ptr->integer.C_3.nd_pt == 1 && ((int ) node_ptr->integer.C_3.nd_pt2 == 18 || (int ) node_ptr->integer.C_3.nd_pt2 == 20 || (int ) node_ptr->integer.C_3.nd_pt2 == 21))
                            {
                            x08obj_edit(&(*term) , &(*protfile) , buf , buf_size , buf_offset , 1 , &(*pfkey));
                            }
                        }
                    if ((int ) node_ptr->integer.C_3.nd_pt == 1 && (int ) node_ptr->integer.C_3.nd_pt2 == 22)
                        {
                        x08beforeimage_edit_display(&(*term) , &(*protfile) , edit_scan , buf , buf_size , buf_offset , &(*pfkey));
                        }
                    if ((int ) node_ptr->integer.C_3.nd_pt == 4 || (int ) node_ptr->integer.C_3.nd_pt == 1 && (int ) node_ptr->integer.C_3.nd_pt2 == 23)
                        {
                        x04afterimagehead_display(&(*term) , &(*protfile) , edit_scan , buf , buf_size , (short ) buf_offset , buf_size - buf_offset , -1 , &(*pfkey) , ln , &ln_len , &err);
                        }
                    }
                }
            if (t09is_window_full(&(*term) , 10))
                {
                t09holding(&(*term) , &(*pfkey));
                if ((int ) *pfkey == 0)
                    {
                    t09newscreen_page(&(*term));
                    if (1 && (edit_scan[0] & 2) != 0)
                        {
                        x07debug_page(&(*term) , &(*protfile) , 1 , &(*file_pos) , buf , buf_size , &(*pfkey));
                        }
                    }
                }
            if ((int ) *pfkey == 0)
                {
                _ptoc_MOVE(unsigned char , 4, scan, ptoc_Var3);
                if (buf_offset > 0)
                    {
                    _ptoc_MOVE(unsigned char , 4, scan, *(unsigned char (*)[4]) sql__psetadd(ptoc_Var7 , scan , ptoc_Var4 , (long)sizeof(unsigned char [4])));
                    }
                t14bufdisplay(&(*term) , &(*protfile) , scan , "                              " , buf , buf_size , lwb , upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
                }
            if ((int ) *pfkey == 0)
                {
                t09pfkeys(&(*term) , sql__nre((long)sizeof(tut_diag_scan ) , edit_scan , ptoc_Var5));
                t09setpf1(&(*term) , "help    ");
                t09getcursor_line(&(*term) , in_line.integer.C_1.ln , &(*pfkey));
                t09setpf1(&(*term) , "        ");
                }
            t09pfkeys(&(*term) , 0);
            t14display_msg(&(*term) , "                    ");
            if ((int ) *pfkey == 4 || (int ) *pfkey == 0 && ((_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "HELP") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "help") == 0 ? 1 : 0) || (int ) in_line.integer.C_1.ln[0] == 63))
                {
                *pfkey = 0;
                t09goto_bottom(&(*term));
                t09newscreen_page(&(*term));
                ta14help(&(*term));
                t09holding(&(*term) , &(*pfkey));
                }
            if (((_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "TYPE") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "type") == 0 ? 1 : 0)) && (int ) *pfkey == 0)
                {
                is_cmd = 1;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                type_upb = lwb + (t09unused_window_lines(&(*term)) + -1) / minbuf_lines * minbuf_bytes + -1;
                if (type_upb > buf_size)
                    {
                    type_upb = buf_size;
                    }
                t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var9, ptoc_Var6) , ptoc_Var9) , "                              " , buf , buf_size , lwb , type_upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
                if ((int ) *pfkey == 0)
                    {
                    t09holding(&(*term) , &(*pfkey));
                    }
                }
            if (((_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "FILL") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "fill") == 0 ? 1 : 0)) && (int ) *pfkey == 0)
                {
                is_cmd = 1;
                ta14fill(&(*term) , &(*protfile) , buf , buf_size , buf_offset , lwb , &(*pfkey));
                }
            if (((_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "MOVE") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "move") == 0 ? 1 : 0)) && (int ) *pfkey == 0)
                {
                is_cmd = 1;
                ta14move(&(*term) , &(*protfile) , buf , buf_size , buf_offset , lwb , &(*pfkey));
                }
            if ((_ptoc_MEMCMP(unsigned char , 132, in_line.integer.C_1.ln , (*term).blankline) == 0 ? 1 : 0) && (int ) *pfkey == 0)
                {
                is_cmd = 1;
                }
            }
        while (!(!is_cmd || (int ) *pfkey != 0));
        if ((int ) *pfkey == 0)
            {
            byte_offset = 4;
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "dec:") == 0 ? 1 : 0)
                {
                i = lwb;
                do
                    {
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + char_per_byte + -3 , 3 , &int_4 , &is_ok);
                    if (is_ok && int_4 >= 0 && int_4 <= 255)
                        {
                        buf[i + -1] = (unsigned char ) int_4;
                        }
                    i = i + 1;
                    byte_offset = byte_offset + char_per_byte;
                    }
                while (!(i > upb || !is_ok));
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "hex:") == 0 ? 1 : 0)
                {
                t09uppercase_line(in_line.integer.C_1.ln , 1 , (long)sizeof(unsigned char [132]));
                i = lwb;
                do
                    {
                    ta14hex_from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + char_per_byte + -2 , &c , &is_ok);
                    if (is_ok)
                        {
                        buf[i + -1] = c;
                        }
                    i = i + 1;
                    byte_offset = byte_offset + char_per_byte;
                    }
                while (!(i > upb || !is_ok));
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "chr:") == 0 ? 1 : 0)
                {
                i = lwb;
                do
                    {
                    c = in_line.integer.C_1.ln[byte_offset + char_per_byte + -2];
                    if ((int ) c != 32)
                        {
                        buf[i + -1] = c;
                        }
                    i = i + 1;
                    byte_offset = byte_offset + char_per_byte;
                    }
                while (!(i > upb));
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "c2 :") == 0 ? 1 : 0)
                {
                i = lwb;
                is_ok = 1;
                while (i + 1 <= upb && is_ok)
                    {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 , char_per_byte * 2 + -2 , &int_4 , &is_ok);
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 ,(char_per_byte<<1) + -2 , &int_4 , &is_ok);
                    if (is_ok && int_4 >= 0 && int_4 <= 65535)
                        {
                        gg06PnoPut(&cint4 , int_4);
                        buf[i + -1] = cint4.ci4_gg00[2];
                        buf[i] = cint4.ci4_gg00[3];
                        }
                    i = i + 2;
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    byte_offset = byte_offset + char_per_byte * 2;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    byte_offset = byte_offset +(char_per_byte<<1);
                    }
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "u2 :") == 0 ? 1 : 0)
                {
                i = lwb;
                is_ok = 1;
                while (i + 1 <= upb && is_ok)
                    {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 , char_per_byte * 2 + -2 , &int_4 , &is_ok);
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 ,(char_per_byte<<1) + -2 , &int_4 , &is_ok);
                    if (is_ok && int_4 >= 0 && int_4 <= 65535)
                        {
                        gg06Int4ToUint2(int_4 , &i2.boolean.C_true.mapInt_sp00);
                        buf[i + -1] = i2.boolean.C_false.mapC2_sp00[0];
                        buf[i] = i2.boolean.C_false.mapC2_sp00[1];
                        }
                    i = i + 2;
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    byte_offset = byte_offset + char_per_byte * 2;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    byte_offset = byte_offset +(char_per_byte<<1);
                    }
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "int2") == 0 ? 1 : 0)
                {
                i = lwb;
                is_ok = 1;
                while (i + 1 <= upb && is_ok)
                    {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 , char_per_byte * 2 + -2 , &int_4 , &is_ok);
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 ,(char_per_byte<<1) + -2 , &int_4 , &is_ok);
                    if (is_ok && int_4 >= -32767 && int_4 <= 32767)
                        {
                        i2.boolean.C_true.mapInt_sp00 = (short ) int_4;
                        buf[i + -1] = i2.boolean.C_false.mapC2_sp00[0];
                        buf[i] = i2.boolean.C_false.mapC2_sp00[1];
                        }
                    i = i + 2;
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    byte_offset = byte_offset + char_per_byte * 2;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    byte_offset = byte_offset +(char_per_byte<<1);
                    }
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "c4 :") == 0 ? 1 : 0)
                {
                i = lwb;
                is_ok = 1;
                while (i + 3 <= upb && is_ok)
                    {
                    if (ta14is_nil_pno(in_line.integer.C_1.ln , byte_offset + 1))
                        {
                        i4.boolean.C_true.map_i4 = 2147483647;
                        }
                    else
                        {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                        t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 , char_per_byte * 4 + -2 , &i4.boolean.C_true.map_i4 , &is_ok);
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                        t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 ,(char_per_byte<<2) + -2 , &i4.boolean.C_true.map_i4 , &is_ok);
                        }
                    if (is_ok)
                        {
                        gg06PnoPut(&cint4 , int_4);
                        buf[i + -1] = cint4.ci4_gg00[0];
                        buf[i] = cint4.ci4_gg00[1];
                        buf[i + 1] = cint4.ci4_gg00[2];
                        buf[i + 2] = cint4.ci4_gg00[3];
                        }
                    i = i + 4;
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    byte_offset = byte_offset + char_per_byte * 4;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    byte_offset = byte_offset +(char_per_byte<<2);
                    }
                is_ok = 1;
                }
            if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "int4") == 0 ? 1 : 0)
                {
                i = lwb;
                is_ok = 1;
                while (i + 3 <= upb && is_ok)
                    {
                    if (ta14is_nil_pno(in_line.integer.C_1.ln , byte_offset + 1))
                        {
                        i4.boolean.C_true.map_i4 = 2147483647;
                        }
                    else
                        {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                        t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 , char_per_byte * 4 + -2 , &i4.boolean.C_true.map_i4 , &is_ok);
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                        t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 2 ,(char_per_byte<<2) + -2 , &i4.boolean.C_true.map_i4 , &is_ok);
                        }
                    if (is_ok)
                        {
                        buf[i + -1] = i4.boolean.C_false.map_c4[0];
                        buf[i] = i4.boolean.C_false.map_c4[1];
                        buf[i + 1] = i4.boolean.C_false.map_c4[2];
                        buf[i + 2] = i4.boolean.C_false.map_c4[3];
                        }
                    i = i + 4;
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
                    byte_offset = byte_offset + char_per_byte * 4;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
                    byte_offset = byte_offset +(char_per_byte<<2);
                    }
                is_ok = 1;
                }
            if (!is_ok)
                {
                t09uppercase_line(in_line.integer.C_1.ln , 1 , (long)sizeof(unsigned char [132]));
                if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "SAVE") == 0 ? 1 : 0)
                    {
                    *is_saved = 1;
                    }
                else
                    {
                    if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "QUIT") == 0 ? 1 : 0)
                        {
                        is_quit = 1;
                        *pfkey = 3;
                        }
                    else
                        {
                        if (_ptoc_MEMCMP(unsigned char , 4, in_line.integer.C_2.prefix , "RR  ") == 0 ? 1 : 0)
                            {
                            if (buf_size + -7 > 0)
                                {
                                lwb = buf_size + -7;
                                }
                            }
                        else
                            {
                            if ((int ) in_line.integer.C_1.ln[0] == 82)
                                {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                if (lwb + bytes_per_line / 4 * 4 <= buf_size)
                                    {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                    lwb = lwb + bytes_per_line / 4 * 4;
                                    }
                                }
                            else
                                {
                                if ((int ) in_line.integer.C_1.ln[0] == 76)
                                    {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                    if (lwb - bytes_per_line / 4 * 4 >= buf_offset + 1)
                                        {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                                        lwb = lwb - bytes_per_line / 4 * 4;
                                        }
                                    else
                                        {
                                        lwb = buf_offset + 1;
                                        }
                                    }
                                else
                                    {
                                    t14int4from_line(&(*term) , in_line.integer.C_1.ln , 1 , 6 , &new_lwb , &is_ok);
                                    if (is_ok)
                                        {
                                        new_lwb = new_lwb + buf_offset;
                                        if (new_lwb >= buf_offset + 1 && new_lwb <= buf_size)
                                            {
                                            lwb = new_lwb;
                                            is_locate_cmd = 1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        if ((int ) *pfkey == 0 && is_locate_cmd)
            {
            curr_col = 1;
            curr_col_len = 0;
            i = lwb;
            is_ok = 1;
            do
                {
                if (curr_col == 1)
                    {
                    col_type_pos = 7;
                    byte_offset = 25;
                    }
                else
                    {
                    col_type_pos = 9;
                    byte_offset = 57;
                    }
                if ((int ) in_line.integer.C_1.ln[col_type_pos] >= 48 && (int ) in_line.integer.C_1.ln[col_type_pos] <= 57)
                    {
                    curr_col_len = sql__ord((int ) in_line.integer.C_1.ln[col_type_pos]) - sql__ord(48);
                    }
                else
                    {
                    is_ok = 0;
                    }
                if (is_ok)
                    {
                    indicator = in_line.integer.C_1.ln[col_type_pos + -1];
                    if ((int ) indicator == 73 && (curr_col_len == 1 || curr_col_len == 2 || curr_col_len == 4))
                        {
                        if (ta14is_nil_pno(in_line.integer.C_1.ln , byte_offset))
                            {
                            i4.boolean.C_true.map_i4 = 2147483647;
                            }
                        else
                            {
                            t14int4from_line(&(*term) , in_line.integer.C_1.ln , byte_offset + 1 , 11 , &i4.boolean.C_true.map_i4 , &is_ok);
                            }
                        if (is_ok && curr_col_len == 1)
                            {
                            if (i4.boolean.C_true.map_i4 >= 0 && i4.boolean.C_true.map_i4 <= 255)
                                {
                                buf[i + -1] = (unsigned char ) i4.boolean.C_true.map_i4;
                                }
                            else
                                {
                                is_ok = 0;
                                }
                            }
                        else
                            {
                            if (is_ok && curr_col_len == 2)
                                {
                                if (i4.boolean.C_true.map_i4 >= -32768 && i4.boolean.C_true.map_i4 <= 32767)
                                    {
                                    i2.boolean.C_true.mapInt_sp00 = (short ) i4.boolean.C_true.map_i4;
                                    buf[i + -1] = i2.boolean.C_false.mapC2_sp00[0];
                                    buf[i] = i2.boolean.C_false.mapC2_sp00[1];
                                    }
                                }
                            else
                                {
                                if (is_ok && curr_col_len == 4)
                                    {
                                    buf[i + -1] = i4.boolean.C_false.map_c4[0];
                                    buf[i] = i4.boolean.C_false.map_c4[1];
                                    buf[i + 1] = i4.boolean.C_false.map_c4[2];
                                    buf[i + 2] = i4.boolean.C_false.map_c4[3];
                                    }
                                }
                            }
                        is_ok = 1;
                        }
                    else
                        {
                        if ((int ) indicator == 66 && curr_col_len == 1)
                            {
                            SAPDB_PascalForcedMove((long)sizeof(unsigned char [132]) , (long)sizeof(tsp00_C6 ) , (tsp00_MoveObj *) in_line.integer.C_1.ln , byte_offset + 1 , (tsp00_MoveObj *) bool_value , 1 , (long)sizeof(tsp00_C6 ));
                            if (_ptoc_MEMCMP(unsigned char , 6, bool_value , "TRUE  ") == 0 ? 1 : 0)
                                {
                                buf[i + -1] = (unsigned char ) sql__ord(1);
                                }
                            else
                                {
                                if (_ptoc_MEMCMP(unsigned char , 6, bool_value , "FALSE ") == 0 ? 1 : 0)
                                    {
                                    buf[i + -1] = (unsigned char ) sql__ord(0);
                                    }
                                }
                            }
                        else
                            {
                            if (((int ) indicator == 77 || (int ) indicator == 78) && curr_col_len == 1)
                                {
                                SAPDB_PascalForcedMove((long)sizeof(unsigned char [132]) , (long)sizeof(tsp00_C18 ) , (tsp00_MoveObj *) in_line.integer.C_1.ln , byte_offset + 1 , (tsp00_MoveObj *) mtype_value , 1 , (long)sizeof(tsp00_C18 ));
                                if ((int ) indicator == 77)
                                    {
                                    ta14mtype_assign(mtype_value , buf , i);
                                    }
                                else
                                    {
                                    ta14mtype2_assign(mtype_value , buf , i);
                                    }
                                }
                            else
                                {
                                if ((int ) indicator == 83 && curr_col == 1 && curr_col_len == 1)
                                    {
                                    ta14set_assign(in_line.integer.C_1.ln , byte_offset , buf , i);
                                    }
                                }
                            }
                        }
                    i = i + curr_col_len;
                    }
                curr_col = curr_col + 1;
                }
            while (!(curr_col > 2 || !is_ok));
            }
        if ((int ) *pfkey == 3 && !*is_saved && !is_quit)
            {
            *pfkey = 0;
            ta14display_leave_msg(&(*term));
            t09holding(&(*term) , &(*pfkey));
            }
        }
    while (!(*is_saved || (int ) *pfkey != 0));
    t09pfkeys(&(*term) , pfkeys_used);
    }



int t14int4from_line(term, ln, ln_pos, ln_len, int_, ok)
tut_terminal *term;
tsp00_Line ln;
int ln_pos;
int ln_len;
tsp00_Int4 *int_;
char *ok;
    {
    unsigned char intstring[10];
    tsp00_Int4 factor;
    int intlength;
    int i;
    int pos;
    int startpos;
    tsp00_C20 msg;
    int ptoc_Var10;
    *ok = 1;
    intlength = 0;
    startpos = ln_pos;
    while (startpos < ln_pos + ln_len + -1 && (int ) ln[startpos + -1] == 32)
        startpos = startpos + 1;
    if ((int ) ln[startpos + -1] == 45)
        {
        pos = startpos + 1;
        }
    else
        {
        pos = startpos;
        }
    while (pos < ln_pos + ln_len && (int ) ln[pos + -1] >= 48 && (int ) ln[pos + -1] <= 57)
        {
        intlength = intlength + 1;
        pos = pos + 1;
        }
    if (intlength == 0 || (int ) ln[pos + -1] != 32)
        {
        *ok = 0;
        _ptoc_ASSIGN(unsigned char , 20, msg, "INVALID NUMBER (\' \')")
        msg[17] = ln[pos + -1];
        t14display_msg(&(*term) , msg);
        }
    if (*ok && intlength > 10)
        {
        *ok = 0;
        t14display_msg(&(*term) , "NUMBER TOO LONG     ");
        }
    if (*ok && intlength == 10)
        {
        i = 1;
        do
            {
            intstring[i + -1] = ln[pos + i + -12];
            if (i == 10)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        if (_ptoc_MEMCMP(unsigned char , 10, intstring , "2147483647") > 0 ? 1 : 0)
            {
            *ok = 0;
            t14display_msg(&(*term) , "NUMBER TOO LARGE    ");
            }
        }
    if (*ok)
        {
        pos = pos + -1;
        *int_ = 0;
        factor = 1;
        ptoc_Var10 = intlength;
        if (1 <= ptoc_Var10)
            {
            i = 1;
            do
                {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                *int_ = *int_ + factor * (sql__ord((int ) ln[pos + -1]) - sql__ord(48));
                pos = pos + -1;
                if (i < intlength)
                    {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
                    factor = factor * 10;
                    }
                if (i == ptoc_Var10)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        if ((int ) ln[startpos + -1] == 45)
            {
            *int_ = -*int_;
            }
        }
    }

static
  tut_diag_scan ptoc_Var11 = 
  {
  (unsigned char)'\0', (unsigned char)'\001', (unsigned char)'\0', (unsigned char)'\0'
  };


int t14logentry_edit(term, protfile, node, file_pos, entry_pos, entry_length, is_saved, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj node;
tut_file_pos *file_pos;
tsp00_Int4 entry_pos;
tsp00_Int4 entry_length;
char *is_saved;
tut_pfkey *pfkey;
    {
    char pfkeys_used;
    tbd_nodeptr node_ptr;
    tut_diag_scan ptoc_Var12;
    *is_saved = 0;
    *pfkey = 0;
    pfkeys_used = (*term).use_pfkeys;
    node_ptr = (tbd_node *) node;
    t14editbuf(&(*term) , &(*protfile) , node , entry_pos + entry_length + -1 , entry_pos + -1 , &(*file_pos) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var12, ptoc_Var11) , ptoc_Var12) , &(*is_saved) , &(*pfkey));
    if ((int ) *pfkey == 3)
        {
        *pfkey = 0;
        }
    t09pfkeys(&(*term) , pfkeys_used);
    }

static
  tut_diag_scan ptoc_Var13 = 
  {
  (unsigned char)'\0', (unsigned char)'\001', (unsigned char)'\0', (unsigned char)'\0'
  };


int t14object_edit(term, protfile, node, file_pos, obj_pos, is_saved, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj node;
tut_file_pos *file_pos;
tsp00_Int4 obj_pos;
char *is_saved;
tut_pfkey *pfkey;
    {
    char pfkeys_used;
    tbd_nodeptr node_ptr;
    tut_diag_scan ptoc_Var14;
    *is_saved = 0;
    *pfkey = 0;
    pfkeys_used = (*term).use_pfkeys;
    node_ptr = (tbd_node *) node;
    t14editbuf(&(*term) , &(*protfile) , node , obj_pos + gg06Int4FromUint2(node_ptr->integer.C_4.nd_obj_frame_len) + -1 , obj_pos + -1 , &(*file_pos) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var14, ptoc_Var13) , ptoc_Var14) , &(*is_saved) , &(*pfkey));
    if ((int ) *pfkey == 3)
        {
        *pfkey = 0;
        }
    t09pfkeys(&(*term) , pfkeys_used);
    }



int t14record_edit(term, protfile, node, node_size, file_pos, ptoc_ptr_edit_scan, is_saved, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj node;
tsp00_Int4 node_size;
tut_file_pos *file_pos;
tut_diag_scan ptoc_ptr_edit_scan;
char *is_saved;
tut_pfkey *pfkey;
    {
    char is_ok;
    char curr_saved;
    char pfkeys_used;
    int dummy_err;
    tsp00_Int4 rec_pos;
    tsp00_Int4 rec_len;
    tsp00_IntMapC2 i2;
    tsp00_Line in_line;
    tut_diag_scan edit_scan;
    _ptoc_MOVE(char, (long)sizeof(tut_diag_scan ), edit_scan, ptoc_ptr_edit_scan);
    *is_saved = 0;
    *pfkey = 0;
    pfkeys_used = (*term).use_pfkeys;
    do
        {
        rec_pos = 0;
        rec_len = 0;
        is_ok = 0;
        _ptoc_ASSIGN(unsigned char , 132, in_line, (*term).blankline)
        curr_saved = 0;
        t09newscreen_page(&(*term));
        t12write_prot(&(*protfile) , (*term).blankline , 1 , &dummy_err);
        x07keys_display(&(*term) , &(*protfile) , &(*file_pos) , node , node_size , &(*pfkey));
        if ((int ) *pfkey == 0)
            {
            if (t09is_window_full(&(*term) , 1))
                {
                t09holding(&(*term) , &(*pfkey));
                if ((int ) *pfkey == 0)
                    {
                    t09newscreen_page(&(*term));
                    }
                }
            }
        if ((int ) *pfkey == 0)
            {
            t09pfkeys(&(*term) , 1);
            t09getcursor_line(&(*term) , in_line , &(*pfkey));
            t09pfkeys(&(*term) , 0);
            }
        if ((int ) *pfkey == 0 && (int ) in_line[6] == 112 && (int ) in_line[7] == 111 && (int ) in_line[8] == 115 && (int ) in_line[15] == 41)
            {
            in_line[15] = (unsigned char)' ';
            t14int4from_line(&(*term) , in_line , 11 , 5 , &rec_pos , &is_ok);
            if (is_ok)
                {
                is_ok = rec_pos >= 1 && rec_pos + 3 <= node_size;
                }
            }
        if (is_ok && (int ) *pfkey == 0)
            {
            i2.boolean.C_false.mapC2_sp00[0] = node[rec_pos + -1];
            i2.boolean.C_false.mapC2_sp00[1] = node[rec_pos];
            rec_len = (int ) i2.boolean.C_true.mapInt_sp00;
            is_ok = rec_pos + rec_len + -1 <= node_size;
            }
        if (is_ok && (int ) *pfkey == 0)
            {
            t14editbuf(&(*term) , &(*protfile) , node , rec_pos + rec_len + -1 , rec_pos + -1 , &(*file_pos) , edit_scan , &curr_saved , &(*pfkey));
            if ((int ) *pfkey == 3)
                {
                *pfkey = 0;
                }
            if (curr_saved)
                {
                *is_saved = 1;
                }
            }
        }
    while (!(!is_ok || (int ) *pfkey != 0));
    t09pfkeys(&(*term) , pfkeys_used);
    }



int ta14display_leave_msg(term)
tut_terminal *term;
    {
    t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
    t09put60(&(*term) , "If you want to leave the EDIT mode, restrike the END key    " , (unsigned char)'\001');
    }



int ta14edit_bufdimension(term, buf_size, char_per_byte, bytes_per_line, line_size, lines_per_buf)
tut_terminal *term;
tsp00_Int4 buf_size;
int *char_per_byte;
int *bytes_per_line;
int *line_size;
int *lines_per_buf;
    {
    if (buf_size >= 10000)
        {
        *char_per_byte = 6;
        }
    else
        {
        *char_per_byte = 5;
        }
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
    *bytes_per_line = (t09window_cols(&(*term)) + -4) / *char_per_byte;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
    *line_size = *bytes_per_line * *char_per_byte + 4;
    *lines_per_buf = 8;
    }

static
  tut_diag_scan ptoc_Var15 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var16 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var17 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var18 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var19 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };


int ta14fill(term, protfile, buf, buf_size, buf_offset, init_pos, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj buf;
tsp00_Int4 buf_size;
tsp00_Int4 buf_offset;
tsp00_Int4 init_pos;
tut_pfkey *pfkey;
    {
    char is_ok;
    unsigned char fill_char;
    int ln_pos;
    int ln_len;
    int count;
    int maxbuf_bytes;
    int dummy_int_1;
    int dummy_int_2;
    tsp00_Int4 i;
    tsp00_Int4 lwb;
    tsp00_Int4 upb;
    tsp00_Int4 dummy_pos;
    tsp00_Int4 fill_len;
    tsp00_Int4 fill_pos;
    tsp00_Name n;
    tsp00_Name n_endpos;
    tsp00_Line ln;
    tut_diag_scan ptoc_Var20;
    int ptoc_Var21;
    int ptoc_Var22;
    count = 0;
    is_ok = 1;
    fill_pos = init_pos;
    t14bufdimension(&(*term) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var20, ptoc_Var15) , ptoc_Var20) , &maxbuf_bytes , &dummy_int_1 , &dummy_int_2);
    do
        {
        count = count + 1;
        t09newscreen_page(&(*term));
        lwb = init_pos;
        if (lwb + maxbuf_bytes + -1 > buf_size)
            {
            upb = buf_size;
            }
        else
            {
            upb = lwb + maxbuf_bytes + -1;
            }
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var20, ptoc_Var16) , ptoc_Var20) , "                              " , buf , buf_size , lwb , upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
        ln_len = 0;
        g17trimint4_to_line(init_pos - buf_offset , &ln_len , ln);
        _ptoc_ASSIGN(unsigned char , 18, n, "                  ")
        ptoc_Var21 = (long)sizeof(tsp00_C18 );
        if (1 <= ptoc_Var21)
            {
            i = 1;
            do
                {
                n[i + -1] = ln[i + -1];
                if (i == ptoc_Var21)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        t09get1name(&(*term) , "                POS:" , (unsigned char)'\001' , (unsigned char)'\200' , 11 , 1 , n , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        if ((int ) *pfkey == 0)
            {
            SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
            t14int4from_line(&(*term) , ln , 1 , (long)sizeof(tsp00_C18 ) , &fill_pos , &is_ok);
            if (is_ok)
                {
                fill_pos = fill_pos + buf_offset;
                if (fill_pos <= buf_offset || fill_pos > buf_size)
                    {
                    is_ok = 0;
                    t14display_msg(&(*term) , "ILLEGAL POSITION    ");
                    }
                }
            }
        }
    while (!(is_ok || (int ) *pfkey != 0 || count >= 3));
    if ((int ) *pfkey == 0 && !is_ok)
        {
        *pfkey = 3;
        }
    if ((int ) *pfkey == 0)
        {
        if (fill_pos + -5 > buf_offset)
            {
            lwb = fill_pos + -5;
            }
        else
            {
            lwb = buf_offset + 1;
            }
        upb = lwb + maxbuf_bytes + -1;
        if (upb > buf_size)
            {
            upb = buf_size;
            }
        }
    count = 0;
    is_ok = 0;
    while (!is_ok && (int ) *pfkey == 0 && count < 3)
        {
        count = count + 1;
        t09newscreen_page(&(*term));
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var20, ptoc_Var17) , ptoc_Var20) , "                              " , buf , buf_size , lwb , upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        ln_pos = (fill_pos - lwb) * 3 + 5;
        ln[ln_pos + -1] = (unsigned char)'|';
        ln[ln_pos] = (unsigned char)'=';
        ln[ln_pos + 1] = (unsigned char)'>';
        t09put(&(*term) , ln , (unsigned char)'\001');
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 18, n, "                  ")
        _ptoc_ASSIGN(unsigned char , 18, n_endpos, "                  ")
        t09get2name(&(*term) , "        FILL LENGTH:" , "or      FILL ENDPOS:" , (unsigned char)'\001' , (unsigned char)'\001' , (unsigned char)'\200' , (unsigned char)'\200' , 11 , 11 , 1 , 1 , n , n_endpos , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        if ((int ) *pfkey == 0)
            {
            _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
            if (_ptoc_MEMCMP(unsigned char , 18, n , "                  ") == 0 ? 0 : 1)
                {
                SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
                }
            else
                {
                SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n_endpos , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
                }
            t14int4from_line(&(*term) , ln , 1 , (long)sizeof(tsp00_C18 ) , &fill_len , &is_ok);
            if (is_ok)
                {
                if (_ptoc_MEMCMP(unsigned char , 18, n , "                  ") == 0 ? 0 : 1)
                    {
                    if (fill_len < 1 || fill_pos + fill_len + -1 > buf_size)
                        {
                        t14display_msg(&(*term) , "ILLEGAL LENGTH      ");
                        is_ok = 0;
                        }
                    }
                else
                    {
                    fill_len = fill_len + buf_offset;
                    if (fill_len < fill_pos || fill_len > buf_size)
                        {
                        t14display_msg(&(*term) , "ILLEGAL POSITION    ");
                        is_ok = 0;
                        }
                    else
                        {
                        fill_len = fill_len - fill_pos + 1;
                        }
                    }
                }
            }
        }
    if ((int ) *pfkey == 0 && !is_ok)
        {
        *pfkey = 3;
        }
    if ((int ) *pfkey == 0)
        {
        t09newscreen_page(&(*term));
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var20, ptoc_Var18) , ptoc_Var20) , "                              " , buf , buf_size , lwb , upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
        ln[ln_pos + -1] = (unsigned char)'|';
        if (fill_pos + fill_len + -1 > upb)
            {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
            ln_len = (upb - lwb) * 3 + 7;
            ln[ln_len + -1] = (unsigned char)'>';
            }
        else
            {
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
            ln_len = (fill_pos - lwb + fill_len) * 3 + 4;
            ln[ln_len + -1] = (unsigned char)'|';
            }
        ln_pos = ln_pos + 1;
        while (ln_pos < ln_len)
            {
            ln[ln_pos + -1] = (unsigned char)'=';
            ln_pos = ln_pos + 1;
            }
        t09put(&(*term) , ln , (unsigned char)'\001');
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        if (fill_pos + fill_len + -1 > upb)
            {
            if (fill_pos + fill_len + -1 > upb + maxbuf_bytes)
                {
                upb = fill_pos + fill_len + -1;
                if (upb + 5 <= buf_size)
                    {
                    upb = upb + 5;
                    }
                else
                    {
                    upb = buf_size;
                    }
                lwb = upb - maxbuf_bytes + 1;
                }
            else
                {
                lwb = upb + 1;
                upb = upb + maxbuf_bytes;
                if (upb > buf_size)
                    {
                    upb = buf_size;
                    }
                }
            t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var20, ptoc_Var19) , ptoc_Var20) , "                              " , buf , buf_size , lwb , upb , lwb - buf_offset , &dummy_pos , &(*pfkey));
            _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
            ln_pos = 5;
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
            ln_len = (fill_pos - lwb + fill_len) * 3 + 4;
            ln[ln_len + -1] = (unsigned char)'|';
            while (ln_pos < ln_len)
                {
                ln[ln_pos + -1] = (unsigned char)'=';
                ln_pos = ln_pos + 1;
                }
            t09put(&(*term) , ln , (unsigned char)'\001');
            t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
            }
        _ptoc_ASSIGN(unsigned char , 18, n, "                  ")
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        t09get1name(&(*term) , "    fill char / hex:" , (unsigned char)'\001' , (unsigned char)'\200' , 2 , 0 , n , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        }
    if ((int ) *pfkey == 0)
        {
        if ((int ) n[1] == 32)
            {
            fill_char = n[0];
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
            SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
            t09uppercase_line(ln , 1 , 3);
            if ((int ) ln[0] == 88)
                {
                i = 2;
                }
            else
                {
                i = 1;
                }
            ta14hex_from_line(&(*term) , ln , i , &fill_char , &is_ok);
            if (!is_ok)
                {
                *pfkey = 3;
                }
            }
        }
    if ((int ) *pfkey == 0)
        {
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
        ln_len = 0;
        g17sname_to_line("FILL from   " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(fill_pos - buf_offset , &ln_len , ln);
        g17sname_to_line(" to         " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(fill_pos + fill_len - buf_offset + -1 , &ln_len , ln);
        g17sname_to_line(" (length    " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(fill_len , &ln_len , ln);
        ln_len = ln_len + 1;
        ln[ln_len + -1] = (unsigned char)')';
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        t09put(&(*term) , ln , (unsigned char)'\001');
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 18, n, "y                 ")
        t09get1name(&(*term) , "EXECUTE FILL ? (y/n)" , (unsigned char)'\001' , (unsigned char)'\200' , 2 , 1 , n , &(*pfkey));
        }
    if ((int ) *pfkey == 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 18, n , "Y                 ") == 0 ? 1 : 0)
            {
            ptoc_Var21 = fill_pos;
            ptoc_Var22 = fill_pos + fill_len + -1;
            if (ptoc_Var21 <= ptoc_Var22)
                {
                i = ptoc_Var21;
                do
                    {
                    buf[i + -1] = fill_char;
                    if (i == ptoc_Var22)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        }
    if ((int ) *pfkey == 3)
        {
        *pfkey = 0;
        }
    }



int ta14help(term)
tut_terminal *term;
    {
    t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
    t09put60(&(*term) , "<pos>  goto the specified position                          " , (unsigned char)'\0');
    t09put60(&(*term) , "l      goto left                                            " , (unsigned char)'\0');
    t09put60(&(*term) , "r      goto right                                           " , (unsigned char)'\0');
    t09put60(&(*term) , "rr     goto the end of the page                             " , (unsigned char)'\0');
    t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
    t09put60(&(*term) , "fill                                                        " , (unsigned char)'\0');
    t09put60(&(*term) , "move                                                        " , (unsigned char)'\0');
    t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
    t09put60(&(*term) , "type   displays the buffer (starting at edit position)      " , (unsigned char)'\0');
    t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
    t09put60(&(*term) , "quit   leaves the EDIT mode without any changes             " , (unsigned char)'\0');
    t09put60(&(*term) , "save   leaves the EDIT mode                                 " , (unsigned char)'\0');
    }



int ta14hex_from_line(term, ln, ln_pos, result, ok)
tut_terminal *term;
tsp00_Line ln;
int ln_pos;
unsigned char *result;
char *ok;
    {
    unsigned char c;
    int i;
    int int_result;
    tsp00_C20 msg;
    *ok = 1;
    i = 0;
    int_result = 0;
    do
        {
        c = ln[ln_pos + i + -1];
        if ((int ) c >= 48 && (int ) c <= 57)
            {
            int_result = int_result + sql__ord((int ) c) - sql__ord(48);
            }
        else
            {
            if ((int ) c >= 65 && (int ) c <= 70)
                {
                int_result = int_result + sql__ord((int ) c) - sql__ord(65) + 10;
                }
            else
                {
                *ok = 0;
                _ptoc_ASSIGN(unsigned char , 20, msg, "INVALID HEX (\' \')   ")
                msg[14] = c;
                t14display_msg(&(*term) , msg);
                }
            }
        if (i == 0)
            {
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
            int_result = int_result * 16;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
            int_result =(int_result<<4);
            }
        i = i + 1;
        }
    while (!(i > 1 || !*ok));
    if (*ok)
        {
        *result = (unsigned char ) int_result;
        }
    }



char ta14is_nil_pno(ln, ln_offset)
tsp00_Line ln;
int ln_offset;
    {
    char _ta14is_nil_pno;
    char is_nil_pno;
    int offset;
    int i;
    tsp00_C8 n;
    int ptoc_Var23;
    is_nil_pno = 0;
    offset = ln_offset;
    if (offset + 1 < (long)sizeof(unsigned char [132]))
        {
        while ((int ) ln[offset] == 32 && offset + 1 < (long)sizeof(unsigned char [132]))
            offset = offset + 1;
        if (offset + (long)sizeof(tsp00_C8 ) <= (long)sizeof(unsigned char [132]))
            {
            ptoc_Var23 = (long)sizeof(tsp00_C8 );
            if (1 <= ptoc_Var23)
                {
                i = 1;
                do
                    {
                    n[i + -1] = ln[offset + i + -1];
                    if (i == ptoc_Var23)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            is_nil_pno = (_ptoc_MEMCMP(unsigned char , 8, n , "nil_pno ") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 8, n , "NIL_PNO ") == 0 ? 1 : 0);
            }
        }
    _ta14is_nil_pno = is_nil_pno;
    return(_ta14is_nil_pno);
    }

static
  tut_diag_scan ptoc_Var24 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var25 = 
  {
  (unsigned char)'\0', (unsigned char)'\200', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var26 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var27 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var28 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var29 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var30 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var31 = 
  {
  (unsigned char)'\0', (unsigned char)'\300', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var32 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tut_diag_scan ptoc_Var33 = 
  {
  (unsigned char)'\0', (unsigned char)'@', (unsigned char)'\0', (unsigned char)'\0'
  };


int ta14move(term, protfile, buf, buf_size, buf_offset, init_pos, pfkey)
tut_terminal *term;
tut_vf_fileref *protfile;
tsp00_MoveObj buf;
tsp00_Int4 buf_size;
tsp00_Int4 buf_offset;
tsp00_Int4 init_pos;
tut_pfkey *pfkey;
    {
    char is_ok;
    tut_diag_scan scan;
    tsp00_Int4 source_pos;
    tsp00_Int4 destin_pos;
    tsp00_Int4 move_len;
    tsp00_Int4 source_upb;
    tsp00_Int4 destin_lwb;
    tsp00_Int4 destin_upb;
    tsp00_Int4 dummy_pos;
    tsp00_Int4 count;
    tsp00_Int4 i;
    int maxbuf_bytes;
    int minbuf_bytes;
    int dummy_int_1;
    int dummy_int_2;
    int ln_len;
    int offset;
    tsp00_Name n;
    tsp00_Name n_endpos;
    tsp00_C30 msg_source;
    tsp00_Line ln;
    tut_diag_scan ptoc_Var34;
    int ptoc_Var35;
    _ptoc_ASSIGN(unsigned char , 30, msg_source, "SOURCE:                       ")
    count = 0;
    is_ok = 1;
    t14bufdimension(&(*term) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var24) , ptoc_Var34) , &maxbuf_bytes , &dummy_int_1 , &dummy_int_2);
    t14bufdimension(&(*term) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var25) , ptoc_Var34) , &minbuf_bytes , &dummy_int_1 , &dummy_int_2);
    do
        {
        count = count + 1;
        t09newscreen_page(&(*term));
        if (buf_offset + maxbuf_bytes > buf_size)
            {
            source_upb = buf_size;
            }
        else
            {
            source_upb = buf_offset + maxbuf_bytes;
            }
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var26) , ptoc_Var34) , msg_source , buf , buf_size , init_pos , source_upb , init_pos - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
        ln_len = 0;
        g17trimint4_to_line(init_pos - buf_offset , &ln_len , ln);
        _ptoc_ASSIGN(unsigned char , 18, n, "                  ")
        ptoc_Var35 = (long)sizeof(tsp00_C18 );
        if (1 <= ptoc_Var35)
            {
            i = 1;
            do
                {
                n[i + -1] = ln[i + -1];
                if (i == ptoc_Var35)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        t09get1name(&(*term) , "         SOURCE POS:" , (unsigned char)'\001' , (unsigned char)'\200' , 11 , 1 , n , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        if ((int ) *pfkey == 0)
            {
            SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
            t14int4from_line(&(*term) , ln , 1 , (long)sizeof(tsp00_C18 ) , &source_pos , &is_ok);
            if (is_ok)
                {
                source_pos = source_pos + buf_offset;
                if (source_pos <= buf_offset || source_pos > buf_size)
                    {
                    is_ok = 0;
                    t14display_msg(&(*term) , "ILLEGAL POSITION    ");
                    }
                }
            }
        }
    while (!(is_ok || (int ) *pfkey != 0 || count >= 3));
    if ((int ) *pfkey == 0 && !is_ok)
        {
        *pfkey = 3;
        }
    if ((int ) *pfkey == 0)
        {
        if (source_pos + maxbuf_bytes + -1 <= buf_size)
            {
            source_upb = source_pos + maxbuf_bytes + -1;
            }
        else
            {
            source_upb = buf_size;
            }
        }
    count = 0;
    is_ok = 0;
    while (!is_ok && (int ) *pfkey == 0 && count < 3)
        {
        count = count + 1;
        t09newscreen_page(&(*term));
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var27) , ptoc_Var34) , msg_source , buf , buf_size , source_pos , source_upb , source_pos - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        if (buf_offset + maxbuf_bytes > buf_size)
            {
            destin_upb = buf_size;
            }
        else
            {
            destin_upb = buf_offset + maxbuf_bytes;
            }
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var28) , ptoc_Var34) , "DESTIN:                       " , buf , buf_size , buf_offset + 1 , destin_upb , 1 , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 18, n, "1                 ")
        t09get1name(&(*term) , "         DESTIN POS:" , (unsigned char)'\001' , (unsigned char)'\200' , 11 , 1 , n , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        if ((int ) *pfkey == 0)
            {
            SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
            t14int4from_line(&(*term) , ln , 1 , (long)sizeof(tsp00_C18 ) , &destin_pos , &is_ok);
            if (is_ok)
                {
                destin_pos = destin_pos + buf_offset;
                if (destin_pos <= buf_offset || destin_pos > buf_size)
                    {
                    is_ok = 0;
                    t14display_msg(&(*term) , "ILLEGAL POSITION    ");
                    }
                }
            }
        }
    if ((int ) *pfkey == 0 && !is_ok)
        {
        *pfkey = 3;
        }
    if ((int ) *pfkey == 0)
        {
        if (destin_pos + maxbuf_bytes + -1 <= buf_size)
            {
            destin_upb = destin_pos + maxbuf_bytes + -1;
            }
        else
            {
            destin_upb = buf_size;
            }
        }
    count = 0;
    is_ok = 0;
    while (!is_ok && (int ) *pfkey == 0 && count < 3)
        {
        count = count + 1;
        t09newscreen_page(&(*term));
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var29) , ptoc_Var34) , msg_source , buf , buf_size , source_pos , source_upb , source_pos - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var30) , ptoc_Var34) , "DESTIN:                       " , buf , buf_size , destin_pos , destin_upb , destin_pos - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 18, n, "                  ")
        _ptoc_ASSIGN(unsigned char , 18, n_endpos, "                  ")
        t09get2name(&(*term) , "        MOVE LENGTH:" , "or    SOURCE ENDPOS:" , (unsigned char)'\001' , (unsigned char)'\001' , (unsigned char)'\200' , (unsigned char)'\200' , 11 , 11 , 1 , 1 , n , n_endpos , &(*pfkey));
        t09putmsg(&(*term) , (*term).blankline , 0 , 0);
        if ((int ) *pfkey == 0)
            {
            if (_ptoc_MEMCMP(unsigned char , 18, n , "                  ") == 0 ? 0 : 1)
                {
                SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
                }
            else
                {
                SAPDB_PascalForcedMove((long)sizeof(tsp00_C18 ) , (long)sizeof(unsigned char [132]) , (tsp00_MoveObj *) n_endpos , 1 , (tsp00_MoveObj *) ln , 1 , (long)sizeof(tsp00_C18 ));
                }
            t14int4from_line(&(*term) , ln , 1 , (long)sizeof(tsp00_C18 ) , &move_len , &is_ok);
            if (is_ok)
                {
                if (_ptoc_MEMCMP(unsigned char , 18, n , "                  ") == 0 ? 0 : 1)
                    {
                    if (source_pos + move_len + -1 > buf_size || destin_pos + move_len + -1 > buf_size || move_len < 1)
                        {
                        is_ok = 0;
                        t14display_msg(&(*term) , "ILLEGAL LENGTH      ");
                        }
                    }
                else
                    {
                    move_len = move_len - buf_offset;
                    if (move_len < source_pos || move_len > buf_size)
                        {
                        is_ok = 0;
                        t14display_msg(&(*term) , "ILLEGAL POSITION    ");
                        }
                    else
                        {
                        move_len = move_len - source_pos + 1;
                        if (destin_pos + move_len + -1 > buf_size)
                            {
                            is_ok = 0;
                            t14display_msg(&(*term) , "ILLEGAL LENGTH      ");
                            }
                        }
                    }
                }
            }
        }
    if ((int ) *pfkey == 0 && !is_ok)
        {
        *pfkey = 3;
        }
    if ((int ) *pfkey == 0)
        {
        t09newscreen_page(&(*term));
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
        if (move_len > minbuf_bytes * 4)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
        if (move_len >(minbuf_bytes<<2))
            {
            _ptoc_ASSIGN(unsigned char , 30, msg_source, "SOURCE (trunc):               ")
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
            source_upb = source_pos + minbuf_bytes * 4 + -1;
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
            source_upb = source_pos +(minbuf_bytes<<2) + -1;
            }
        else
            {
            source_upb = source_pos + move_len + -1;
            }
/*===== OPERATION_TO_OPT_FOUND ( START ) =====
        if (move_len > maxbuf_bytes * 2)
===== OPERATION_TO_OPT_FOUND ( END   ) =====*/
        if (move_len >(maxbuf_bytes<<1))
            {
            _ptoc_MOVE(unsigned char , 4, scan, ptoc_Var31);
            }
        else
            {
            _ptoc_MOVE(unsigned char , 4, scan, ptoc_Var32);
            }
        t14bufdisplay(&(*term) , &(*protfile) , scan , msg_source , buf , buf_size , source_pos , source_upb , source_pos - buf_offset , &dummy_pos , &(*pfkey));
        if (destin_pos > 5)
            {
            destin_lwb = destin_pos + -5;
            }
        else
            {
            destin_lwb = 1;
            }
        if (destin_upb + maxbuf_bytes + -1 <= buf_size)
            {
            destin_upb = destin_lwb + maxbuf_bytes + -1;
            }
        else
            {
            destin_upb = buf_size;
            }
        if (destin_upb < destin_pos + move_len + -1)
            {
            count = destin_upb - destin_pos + 1;
            }
        else
            {
            count = move_len;
            }
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        count = count * 3;
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/
        offset = (destin_pos - destin_lwb) * 3 + 4;
        ln[offset + 1] = (unsigned char)'|';
        ptoc_Var35 = count;
        if (3 <= ptoc_Var35)
            {
            i = 3;
            do
                {
                ln[offset + i + -1] = (unsigned char)'=';
                if (i == ptoc_Var35)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        if (destin_upb < destin_pos + move_len + -1)
            {
            ln[offset + count] = (unsigned char)'>';
            }
        else
            {
            ln[offset + count + -1] = (unsigned char)'|';
            }
        t09put(&(*term) , (*term).blankline , (unsigned char)'\001');
        t14bufdisplay(&(*term) , &(*protfile) , *(tut_diag_scan *) (_ptoc_MOVE(unsigned char , 4, ptoc_Var34, ptoc_Var33) , ptoc_Var34) , "DESTIN:                       " , buf , buf_size , destin_lwb , destin_upb , destin_lwb - buf_offset , &dummy_pos , &(*pfkey));
        t09put(&(*term) , ln , (unsigned char)'\001');
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 132, ln, (*term).blankline)
        ln_len = 0;
        g17sname_to_line("MOVE from   " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(source_pos - buf_offset , &ln_len , ln);
        ln_len = ln_len + 1;
        ln[ln_len + -1] = (unsigned char)'-';
        g17trimint4_to_line(source_pos + move_len - buf_offset + -1 , &ln_len , ln);
        g17sname_to_line(" to         " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(destin_pos - buf_offset , &ln_len , ln);
        ln_len = ln_len + 1;
        ln[ln_len + -1] = (unsigned char)'-';
        g17trimint4_to_line(destin_pos + move_len - buf_offset + -1 , &ln_len , ln);
        g17sname_to_line(" (length    " , &ln_len , ln);
        ln_len = ln_len + 1;
        g17trimint4_to_line(move_len , &ln_len , ln);
        ln_len = ln_len + 1;
        ln[ln_len + -1] = (unsigned char)')';
        t09put(&(*term) , ln , (unsigned char)'\001');
        t09put(&(*term) , (*term).blankline , (unsigned char)'\0');
        _ptoc_ASSIGN(unsigned char , 18, n, "y                 ")
        t09get1name(&(*term) , "EXECUTE MOVE ? (y/n)" , (unsigned char)'\001' , (unsigned char)'\200' , 2 , 1 , n , &(*pfkey));
        }
    if ((int ) *pfkey == 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 18, n , "Y                 ") == 0 ? 1 : 0)
            {
            if (source_pos < destin_pos)
                {
                ptoc_Var35 = move_len + -1;
                if (ptoc_Var35 >= 0)
                    {
                    i = ptoc_Var35;
                    do
                        {
                        buf[destin_pos + i + -1] = buf[source_pos + i + -1];
                        if (i == 0)
                            {
                            break;
                            }
                        i -= 1;
                        }
                    while (!0);
                    }
                }
            else
                {
                SAPDB_PascalForcedOverlappingMove(buf_size , buf_size , (tsp00_MoveObj *) buf , source_pos , (tsp00_MoveObj *) buf , destin_pos , move_len);
                }
            }
        }
    if ((int ) *pfkey == 3)
        {
        *pfkey = 0;
        }
    }



int ta14mtype_assign(mtype_value, buf, buf_pos)
tsp00_Name mtype_value;
tsp00_MoveObj buf;
tsp00_Int4 buf_pos;
    {
    char found;
    tgg00_MessType curr;
    int ln_len;
    struct ptoc_Type19
      {
      union  
        {
        struct  
          {
          tsp00_Line ln;
          } C_1;
        struct  
          {
          tsp00_Name n;
          } C_2;
        } integer;
      } univ_ln;
    found = 0;
    curr = 0;
    do
        {
        _ptoc_ASSIGN(unsigned char , 18, univ_ln.integer.C_2.n, "                  ")
        ln_len = 0;
        g17messtype_to_line(curr , &ln_len , univ_ln.integer.C_1.ln);
        t09uppercase_line(univ_ln.integer.C_1.ln , 1 , (long)sizeof(tsp00_C18 ));
        if (_ptoc_MEMCMP(unsigned char , 18, univ_ln.integer.C_2.n , mtype_value) == 0 ? 1 : 0)
            {
            found = 1;
            }
        else
            {
            if ((int ) curr == 92)
                {
                curr = 0;
                }
            else
                {
                curr = (unsigned char ) ((int ) curr + 1);
                }
            }
        }
    while (!(found || (int ) curr == 0));
    if (found)
        {
        buf[buf_pos + -1] = (unsigned char ) sql__ord((int ) curr);
        }
    }



int ta14mtype2_assign(mtype_value, buf, buf_pos)
tsp00_Name mtype_value;
tsp00_MoveObj buf;
tsp00_Int4 buf_pos;
    {
    char found;
    tgg00_MessType2 curr;
    int ln_len;
    struct ptoc_Type20
      {
      union  
        {
        struct  
          {
          tsp00_Line ln;
          } C_1;
        struct  
          {
          tsp00_Name n;
          } C_2;
        } integer;
      } univ_ln;
    found = 0;
    curr = 0;
    do
        {
        _ptoc_ASSIGN(unsigned char , 18, univ_ln.integer.C_2.n, "                  ")
        ln_len = 0;
        g17mess2type_to_line(curr , &ln_len , univ_ln.integer.C_1.ln);
        t09uppercase_line(univ_ln.integer.C_1.ln , 1 , (long)sizeof(tsp00_C18 ));
        if (_ptoc_MEMCMP(unsigned char , 18, univ_ln.integer.C_2.n , mtype_value) == 0 ? 1 : 0)
            {
            found = 1;
            }
        else
            {
            if ((int ) curr == 94)
                {
                curr = 0;
                }
            else
                {
                curr = (unsigned char ) ((int ) curr + 1);
                }
            }
        }
    while (!(found || (int ) curr == 0));
    if (found)
        {
        buf[buf_pos + -1] = (unsigned char ) sql__ord((int ) curr);
        }
    }

static
  unsigned char ptoc_Var36[1] = 
  {
  (unsigned char)'\0'
  };


int ta14set_assign(ln, ln_offset, buf, buf_pos)
tsp00_Line ln;
int ln_offset;
tsp00_MoveObj buf;
tsp00_Int4 buf_pos;
    {
    int pos;
    int curr_bit;
    struct ptoc_Type21
      {
      union  
        {
        struct  
          {
          tsp00_C2 c;
          } C_1;
        struct  
          {
          unsigned char s[1];
          } C_2;
        } integer;
      } univ_set;
    unsigned char ptoc_Var37[1];
    _ptoc_MOVE(unsigned char , 1, univ_set.integer.C_2.s, ptoc_Var36);
    curr_bit = 0;
    pos = ln_offset + 1;
    do
        {
        while ((int ) ln[pos + -1] == 32 && pos < (long)sizeof(unsigned char [132]))
            pos = pos + 1;
        if ((int ) ln[pos + -1] != 45 && (int ) ln[pos + -1] != 32)
            {
            sql__psetadd(univ_set.integer.C_2.s , univ_set.integer.C_2.s , sql__pcnst(ptoc_Var37 , 0 , 7 , 0 , 1 , curr_bit) , (long)sizeof(unsigned char [1]));
            }
        while ((int ) ln[pos + -1] != 32 && pos < (long)sizeof(unsigned char [132]))
            pos = pos + 1;
        curr_bit = curr_bit + 1;
        }
    while (!(curr_bit > 7 || pos >= (long)sizeof(unsigned char [132])));
    buf[buf_pos + -1] = univ_set.integer.C_1.c[0];
    }

/* ==== STATISTIK - BEGIN ==== 
   ==== Total optimized lines           :  21
   ==== set-operations found / replaced :   0 /   0 
   ==== '*'-operations found / replaced :  31 /  14 
   ==== '/'-operations found / replaced :   3 /   0 
   ==== '%'-operations found / replaced :  11 /   7 
   ==== STATISTIK - END   ==== */
static char ident_mf[] = "@(#)vta14 FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF -debug=1";
static char ident_sr[] = "@(#)vta14 dated 2006-07-17 05:56:41 used 2008-05-09 02:39:45";
extern char * vta14_force_data () { return( ident_sr ); }
static char ident_pc[] = "@(#)vta14 ptoc -qn -e";
