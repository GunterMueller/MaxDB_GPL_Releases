/*


    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct opt_id
        { char *opt_srch_str ;
          char *opt_repl_op ;
          int  opt_type ;
          char *found_pos ;
          char *full_line ;
          int num_found ;
          int num_replaced ;
        } OPT_ID ;

#define OPT_SET_OP 1
#define OPT_DIV    2
#define OPT_MULT   3
#define OPT_MODULO 4

#define MAX_LINE_LEN 4096
#define TRUE 1
#define FALSE 0
#undef NULL
#define NULL (char *)0
#define SRCH_PATTERN_1 "(ptoc_Var"
#define COMMENT_ACTIV "COMMENT"
#define COMMENT_OPT_FAILED     "/*===== OPERATION_TO_OPT_FOUND (FAILED) =====*/\n" 
#define COMMENT_OLD_LINE_START "/*===== OPERATION_TO_OPT_FOUND ( START ) =====\n" 
#define COMMENT_OLD_LINE_END   "===== OPERATION_TO_OPT_FOUND ( END   ) =====*/\n" 
#define COMMENT_TOTAL_REPLACED \
        "\n/*===== Total optimized lines: %d  set-ops: %d / mult_ops %d / div_ops %d =====*/\n"

static int  get_int_from_str () ;
static char *del_in_str ();
static char *fh_strstr() ;
static char *srch_str_bw() ;
static int  check_set_op (char **line, char *varname, int *initval);
static int  convert_expression (
    char *line,
    int comment_old_line,
    OPT_ID *opt_ids);
static int  srch_pos_to_opt (char *line, OPT_ID *opt_ids, OPT_ID **opt_type);
static int  replace_set_op (char **conv_beg_pos, OPT_ID *to_opt);
static int  opt_arithmetic (char **conv_beg_pos, OPT_ID *to_opt);
static int  get_int_from_str () ;
static char *del_in_str ();
static char *fh_strstr() ;
static char *srch_str_bw() ;
static char *srch_para_beg_pos (char *line, char *act_pos);
static int  get_shift_val (int val);

#define MAX_OPT_TO_SRCH 4
/*----------------------------------------------------------------*/
main ( argc, argv ) 
/*----------------------------------------------------------------*/
int argc ; 
char **argv ;
{
  static OPT_ID opt_ids [ MAX_OPT_TO_SRCH ] = 
             {
               { "(ptoc_Var", NULL, OPT_SET_OP, NULL, NULL, 0, 0 }, 
               { " * "      , "<<", OPT_MULT,   NULL, NULL, 0, 0 },
               { " / "      , ">>", OPT_DIV   , NULL, NULL, 0, 0 }, 
               { " % "      , "& ", OPT_MODULO, NULL, NULL, 0, 0 } 
             } ;
  char line[MAX_LINE_LEN+1];
  int 
      num_replaced=0, total_lines_repl=0, total_replaced = 0 ,
      comment = argc == 2 && !strcmp ( argv[1], COMMENT_ACTIV ) ;

  while ( gets ( line ) != NULL )
    { if ( strchr ( line, '"' ) == NULL &&  /* don't modify in strings */
           (num_replaced = convert_expression ( line, comment, opt_ids ) ) )
        { total_lines_repl++; total_replaced += num_replaced ; }
      puts ( line ) ; 
    }
  if ( comment )
    {
      printf ( "\n/* ==== STATISTIK - BEGIN ==== ");
      printf ( "\n   ==== Total optimized lines           : %3d", 
                                                           total_lines_repl); 
      printf ( "\n   ==== set-operations found / replaced : %3d / %3d ", 
                          opt_ids[0].num_found, opt_ids[0].num_replaced);
      printf ( "\n   ==== '*'-operations found / replaced : %3d / %3d ", 
                          opt_ids[1].num_found, opt_ids[1].num_replaced);
      printf ( "\n   ==== '/'-operations found / replaced : %3d / %3d ", 
                          opt_ids[2].num_found, opt_ids[2].num_replaced);
      printf ( "\n   ==== '%%'-operations found / replaced : %3d / %3d ", 
                          opt_ids[3].num_found, opt_ids[3].num_replaced);
      printf ( "\n   ==== STATISTIK - END   ==== */\n");
    }

  exit (0) ;  
}

/*----------------------------------------------------------------*/
static int convert_expression ( line , comment_old_line, opt_ids ) 
/*----------------------------------------------------------------*/
char *line ;
int comment_old_line ;
OPT_ID *opt_ids ;
{ 
  int first = TRUE , replaced=FALSE, num_replaced = 0, last_num_found;
  char *conv_beg_pos = line , sav_line[MAX_LINE_LEN+1];
  OPT_ID *to_opt ;

  do 
    if ( srch_pos_to_opt ( conv_beg_pos , opt_ids, &to_opt ) )
      { if ( first )
          strcpy ( sav_line, line ) ;
        last_num_found= to_opt->num_found ; 
        conv_beg_pos   =to_opt->found_pos ; 
        switch ( to_opt->opt_type )
          { case OPT_SET_OP : replaced=replace_set_op (&conv_beg_pos, to_opt);
                              break ;
            case OPT_MULT   : replaced=opt_arithmetic (&conv_beg_pos, to_opt);
                              break ;
            case OPT_DIV    : replaced=opt_arithmetic (&conv_beg_pos, to_opt);
                              break ;
            case OPT_MODULO : replaced=opt_arithmetic (&conv_beg_pos, to_opt);
                              break ;
          }
        if ( comment_old_line && first )
          { if ( replaced ) 
              { printf( COMMENT_OLD_LINE_START );
                puts ( sav_line ) ;
                printf( COMMENT_OLD_LINE_END );
              }
            else
             if ( last_num_found != to_opt->num_found ) 
               printf( COMMENT_OPT_FAILED );
            first = FALSE ;
          }
        if ( replaced ) 
          { replaced = FALSE ; num_replaced++ ; to_opt->num_replaced++ ; }
        else
          conv_beg_pos++;
      }
    else
      conv_beg_pos = NULL ;
  while ( conv_beg_pos ); 

  return ( num_replaced );
}

/*----------------------------------------------------------------*/
static int srch_pos_to_opt ( line, opt_ids, opt_type )
/*----------------------------------------------------------------*/
char *line ;
OPT_ID *opt_ids ;
OPT_ID **opt_type ;
{
  int i, first_opt_index ;
  char *pos_to_opt ;

  for ( i=0, first_opt_index = 0 ; i< MAX_OPT_TO_SRCH ; i++ )
    {
      opt_ids[i].found_pos = fh_strstr ( line , opt_ids[i].opt_srch_str) ;
      if ( i>0 && opt_ids[i].found_pos > opt_ids[i-1].found_pos )
        first_opt_index = i ; 
    }
  *opt_type = & ( opt_ids [ first_opt_index ] ) ;
  if ( (pos_to_opt = opt_ids [ first_opt_index ] . found_pos) )
    (*opt_type)->full_line = line ;
   
  return ( opt_ids [ first_opt_index ] . found_pos != NULL ) ;
}

#define SRCH_INIT " = "
#define SET_BEG_ID "(("
#define SET_END_ID "))"
#define SHIFT_RIGHT_ID " >> "
/*----------------------------------------------------------------*/
static int replace_set_op ( conv_beg_pos, to_opt ) 
/*----------------------------------------------------------------*/
char **conv_beg_pos ;
OPT_ID *to_opt ;
{
  char *ret_pos, varname[30]; 
  int initval, replace_ok = FALSE;

  if ( check_set_op ( conv_beg_pos, varname, &initval ) )
    {  char *set_op_beg_pos = fh_strstr ( *conv_beg_pos, SET_BEG_ID ) ;
       to_opt->num_found++ ;
       if ( set_op_beg_pos )
         { char *index_beg_pos, *index_end_pos, *shift_right_par;
           char *bit_and_expr, int_buf[10];
           int shr, int_buf_len ;
           set_op_beg_pos = 
                  del_in_str ( *conv_beg_pos, set_op_beg_pos - *conv_beg_pos ) ;
           index_beg_pos =  fh_strstr ( set_op_beg_pos, varname );
           shift_right_par= fh_strstr (index_beg_pos, SHIFT_RIGHT_ID ) ;
           index_end_pos =  strchr ( index_beg_pos , ']' ) ;
           shr = get_int_from_str ( shift_right_par  
                                    + strlen ( SHIFT_RIGHT_ID ), &int_buf_len );
           sprintf ( int_buf, "%d", initval >> shr ) ;
           int_buf_len = strlen(int_buf) ;
           ret_pos = del_in_str ( index_beg_pos, 
                             index_end_pos-index_beg_pos-int_buf_len ) ;
           strncpy ( index_beg_pos, int_buf, int_buf_len ) ; 
           if ( (bit_and_expr = fh_strstr ( index_beg_pos , varname ) ) )
             { char *bit_and_right_arg = strchr ( bit_and_expr , '&' ) ;
               int bit_and_const   = get_int_from_str ( bit_and_right_arg+2,
                                                        &int_buf_len ) ;
               sprintf ( int_buf, "%d", initval & bit_and_const );
               int_buf_len = strlen(int_buf) ;
               ret_pos = del_in_str ( bit_and_expr, strchr ( bit_and_expr , ')')
                                               - bit_and_expr - int_buf_len) ;
               strncpy ( bit_and_expr, int_buf, int_buf_len ) ; 
             } 
           replace_ok = TRUE ;
         }
    }

  return  ( replace_ok );
}

/*----------------------------------------------------------------*/
static int check_set_op ( line , varname, initval ) 
/*----------------------------------------------------------------*/
char **line ;
char *varname ;
int *initval ;
{ 
  int something_to_do, dummy ;
  char *hptr, *beg_of_varname, *end_of_varname ; 
  char *conv_beg_pos = *line ;

  beg_of_varname = conv_beg_pos+1 ;
  hptr = conv_beg_pos + strlen ( SRCH_PATTERN_1 ) ; 
  end_of_varname = strchr ( hptr, ' ' ) ;
  something_to_do = end_of_varname != NULL              &&
   		(int ) (end_of_varname - hptr) <= 4 &&
                        end_of_varname[1] == '='    &&
                        end_of_varname[2] == ' '    &&
                        isdigit ( end_of_varname[3] ) ;
  if ( something_to_do )
    { int varname_len = end_of_varname - beg_of_varname ;
      strncpy ( varname, beg_of_varname, varname_len ) ; 
      varname[varname_len] = '\0' ;
      *initval = get_int_from_str ( end_of_varname+3, &dummy ) ;
    }
  else
    *line = end_of_varname != NULL ? end_of_varname : hptr ;

  return ( something_to_do ) ;
}

/*----------------------------------------------------------------*/
static int opt_arithmetic ( conv_beg_pos, to_opt ) 
/*----------------------------------------------------------------*/
char **conv_beg_pos ;
OPT_ID *to_opt ;
{
  int power_of_2, op_digits, replaced_ok = FALSE ;
  char *op_pos = *conv_beg_pos + strlen ( to_opt->opt_srch_str ) ;  
  int op = get_int_from_str ( op_pos, &op_digits ) ;
  to_opt->num_found++ ;
  if ( op > 0 && ( power_of_2 = get_shift_val ( op ) ) > 0 ) 
    {
      int sh_op_digits = power_of_2 < 10 ? 1 : 2 ;
      char *para_beg_pos, *para_end_pos,
           hbuf[10], save_chr1, save_chr2;
      static char *srch_para_beg_pos() ;

      para_beg_pos = srch_para_beg_pos ( to_opt->full_line, *conv_beg_pos-1);
      if ( para_beg_pos && 
          ( *(para_end_pos = op_pos + op_digits) == ' ' ||
             sh_op_digits <= op_digits ||
             (*para_end_pos == ';' && *(para_end_pos+1) == '\0' )
          ) )
        {
           int hbuf_len ;
           *para_beg_pos = '(' ; 
           save_chr1 = *para_end_pos ;
           sprintf ( hbuf, "%s%d)", to_opt->opt_repl_op,
                     to_opt->opt_type == OPT_MODULO ? op -1 : power_of_2 ) ;
           hbuf_len = strlen ( hbuf ) ;
           strncpy ( *conv_beg_pos, hbuf, hbuf_len ) ;
           *conv_beg_pos = del_in_str ( *conv_beg_pos + hbuf_len, 
                                   para_end_pos - *conv_beg_pos - hbuf_len ) ;
           if ( save_chr1 == ';' ) 
             { **conv_beg_pos = save_chr1; *conv_beg_pos+=1;
               **conv_beg_pos = '\0'; 
             }
           replaced_ok = TRUE ;
        }
      else 
        *conv_beg_pos+=2 ;
    }
  else 
    *conv_beg_pos+=2 ;
  return ( replaced_ok ) ;
}

/*----------------------------------------------------------------*/
static char *srch_para_beg_pos ( line, act_pos ) 
/*----------------------------------------------------------------*/
char *line, *act_pos;
{
  int end = FALSE, p1_beg=0, p1_end=0, p2_beg=0, p2_end=0 ;
  char *beg_pos=act_pos ;

  while ( !end && beg_pos != NULL &&
          (beg_pos = srch_str_bw ( line, beg_pos, "[]() " )) )
    { switch ( *beg_pos )
        {
          case ' ' : if ( *(beg_pos-1) == '/' || *(beg_pos-1) == '*' || 
                          *(beg_pos-1) == '%' )
                        beg_pos = NULL;
                     else
                        if ( ! (end = p1_beg == p1_end && p2_beg == p2_end ) )
                           beg_pos-- ;
                     break ;
          case ')' : p1_end++; beg_pos--; break ;
          case '(' : if ( p1_end == p1_beg ) 
                       { beg_pos = NULL ; end = TRUE ; }
                     else 
                       { beg_pos--;p1_beg++ ; }
                     break ;
          case ']' : p2_end++; beg_pos--; break ;
          case '[' : if ( p2_end == p2_beg ) 
                       { beg_pos = NULL ; end = TRUE ; }
                     else 
                       { beg_pos--;p2_beg++ ; }
                     break ;
          default  : beg_pos = NULL ; break ;
        }
    }
/*
  else
    { if ( strchr ( beg_pos, '[' ) && !strchr ( beg_pos, ']' ) ||
           strchr ( beg_pos, ']' ) && !strchr ( beg_pos, '[' ) || 
           strchr ( beg_pos, ')' ) ||  strchr ( beg_pos, '(' ) )
         beg_pos = NULL ;
    }
*/
  return ( beg_pos ) ;
}

/*----------------------------------------------------------------*/
static int replace_modulo ( conv_beg_pos, to_opt ) 
/*----------------------------------------------------------------*/
char **conv_beg_pos ;
OPT_ID *to_opt ;
{
  int replaced_ok = FALSE ;
  *conv_beg_pos+=2 ;
  return ( replaced_ok ) ;
}

#define MAX_INT_LEN 10
/*----------------------------------------------------------------*/
static int get_int_from_str ( str, num_digits )
/*----------------------------------------------------------------*/
char *str ;
int *num_digits ;
{
  char int_buf[MAX_INT_LEN+1];
  int i ;

  for ( i=0; i < MAX_INT_LEN && isdigit ( str[i] ) ; int_buf[i] = str[i], i++ );
  int_buf[i] = '\0' ;
  *num_digits=i;
  return ( atoi ( int_buf ) ) ;
}

/*----------------------------------------------------------------*/
static int get_shift_val ( val )
/*----------------------------------------------------------------*/
int  val;
{
  int i, to_shift, shift_val ;

  for ( i=0, to_shift = val; (to_shift = to_shift >> 1) > 0; i++ ) ;
  shift_val = ( 1 << i ) == val ? i : 0 ;
  return ( shift_val ) ;
}


/*----------------------------------------------------------------*/
static char *del_in_str ( str, del_len )
/*----------------------------------------------------------------*/
char *str ;
int del_len ;
{
  char *hptr = str + del_len , *str_beg_pos = str ;
  if ( del_len > 0 )
    while ( *hptr ) 
      { *str = *hptr ;
        str++; hptr++ ;
      }
  *str = *hptr ;
  return ( str_beg_pos );
}

/*----------------------------------------------------------------*/
static char *fh_strstr ( str1, str2 )
/*----------------------------------------------------------------*/
char *str1, *str2 ;
{
  char *found_pos = str1 , *cmp_str = str2+1; 
  int  found = FALSE , cmp_len = strlen ( cmp_str ) ;

  while ( !found && (found_pos = strchr ( found_pos, *str2 )) != NULL )
    if ( !strncmp ( found_pos+1, cmp_str, cmp_len ) )
      found = TRUE;
    else
      found_pos++ ;
      
  return ( found_pos ) ;
}

/*----------------------------------------------------------------*/
static char *srch_str_bw ( str_beg, srch_beg_pos, chrs_to_srch )
/*----------------------------------------------------------------*/
char *str_beg, *srch_beg_pos, *chrs_to_srch ;
{
  char *act_chr, *found_pos = srch_beg_pos;
  int found = FALSE ;

  while ( found_pos >= str_beg && ! found )
   { for ( act_chr = chrs_to_srch; 
           *act_chr != '\0' && ! (found = *found_pos == *act_chr) ; act_chr++ );
     if ( !found ) found_pos--; 
   }
  return ( found ? found_pos : NULL ) ;
}
