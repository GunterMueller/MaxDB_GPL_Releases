/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Types.h
  @author         FerdiF
  @brief          types used by the XPath indexer
*/
/*

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

#ifndef XMLXPATH_TYPES_H
#define XMLXPATH_TYPES_H

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/*!
    XPATH Indexing error codes
*/
typedef enum
{
    XMLXPath_NOERR                    = 0,   /*!< ok                             */
    XMLXPath_BAD_NUMBER               = -1,  /*!< bad number                     */   
    XMLXPath_BAD_TOKEN                = -2,  /*!< bad token                      */
    XMLXPath_INFINITE_LITERAL         = -3,  /*!< literal not terminated         */
    XMLXPath_BAD_VAR                  = -4,  /*!< bad variable                   */
    XMLXPath_EXTRA_COLON              = -5,  /*!< extra colon                    */
    XMLXPath_ENTITYREF_NOT_SUPP       = -6,  /*!< entity reference not supported */
    XMLXPath_NO_VALUE_PATHEXPR        = -7,  /*!< no value path expression       */
    XMLXPath_NOT_SUPPORTED            = -8,  /*!< not supported                  */
    XMLXPath_EXPR_NOT_TERMINATED      = -9,  /*!< expr not terminated            */
    XMLXPath_NO_ABSOLUTE_PATH_ALLOWED = -10, /*!< absolute path not allowed      */
    XMLXPath_NO_MORE_MEMORY           = -11, /*!< no more memory                 */
    XMLXPath_CREATE_PARSER_FAILED     = -12, /*!< create of sax parser failed    */
    XMLXPath_XML_READ_HANDLER_FAILED  = -13, /*!< XML read handler failed        */
    XMLXPath_XML_SAX_PARSER_ERROR     = -14, /*!< XML SAX parser error           */
    XMLXPath_NOT_YET_IMPLEMENTED      = -15, /*!< not yet implemented            */
    XMLXPath_MIX_SYNC_ASYNC_READ      = -16  /*!< ??                             */
} XMLXPath_ErrorNo;

/*!
    XPATH Tokens
*/
typedef enum
{
    XMLXPath_TOK_NULL,         /*!< used as end indicator               */

    /* names */
    XMLXPath_TOK_NAME,         /*!< name test, incl. '*' and 'prefix:*' */
    XMLXPath_TOK_AXISNAME,     /*!< name followed by ::                 */
    XMLXPath_TOK_NODENAME,     /*!< node(), text() etc.                 */
    XMLXPath_TOK_FUNCNAME,     /*!< other names followed by (           */

    /* symbols */
    XMLXPath_TOK_LPAREN,       /*!< left parent     */
    XMLXPath_TOK_RPAREN,       /*!< right parent    */
    XMLXPath_TOK_LBRACKET,     /*!< left bracket    */
    XMLXPath_TOK_RBRACKET,     /*!< right bracket   */  
    XMLXPath_TOK_PERIOD,       /*!< period          */
    XMLXPath_TOK_DPERIOD,      /*!< double period   */
    XMLXPath_TOK_ATSIGN,       /*!< at sign         */
    XMLXPath_TOK_COMMA,        /*!< comma           */
    XMLXPath_TOK_DCOLON,       /*!< double colon    */  
    XMLXPath_TOK_DSLASH,       /*!< double slash    */
    XMLXPath_TOK_SLASH,        /*!< slash           */

    /* variable reference ($...) */
    XMLXPath_TOK_VAR,          /*!< variable        */

    /* string in quotes */
    XMLXPath_TOK_LITERAL,      /*!< literal         */

    /* real number */
    XMLXPath_TOK_NUMBER,       /*!< number          */

    /* operators */
    XMLXPath_TOK_OR,           /*!< or              */
    XMLXPath_TOK_AND,          /*!< and             */
    XMLXPath_TOK_EQ,           /*!< eq              */
    XMLXPath_TOK_NEQ,          /*!< neq             */
    XMLXPath_TOK_LT,           /*!< lt              */
    XMLXPath_TOK_GT,           /*!< gt              */
    XMLXPath_TOK_LE,           /*!< le              */
    XMLXPath_TOK_GE,           /*!< ge              */
    XMLXPath_TOK_PLUS,         /*!< plus            */
    XMLXPath_TOK_SUBT,         /*!< subtraction     */
    XMLXPath_TOK_MULT,         /*!< multiplication  */
    XMLXPath_TOK_MOD,          /*!< modula          */
    XMLXPath_TOK_DIV,          /*!< division        */
    XMLXPath_TOK_MINUS,        /*!< minus           */
    XMLXPath_TOK_VERT,         /*!< vert            */
    XMLXPath_TOK_STAR,         /*!< star            */
    XMLXPath_TOK_NONE          /*!< noe             */
} XMLXPath_TokenEnum;

/*!
    XMLXPath_Error
*/
typedef struct
{
    short             rc;       /*!< error code     */
    SAPDB_UTF8        msg[256]; /*!< error message  */
} XMLXPath_Error;  
/*!
    XMLXPath_Error pointer type definition
*/
typedef XMLXPath_Error *XMLXPath_ErrorPtr;

/*!
    pointer and description of one parsed document item
*/
typedef struct
{
    SAPDB_UTF8         *startChar; /*!< start pos of item */
    short              len;        /*!< lenght of item    */
    XMLXPath_TokenEnum token;      /*!< matched token     */
} XMLXPath_Item;

/*!
    XMLXPath_Item pointer type definition
*/
typedef XMLXPath_Item *XMLXPath_ItemPtr;

/*!
    XML node types
*/
typedef enum
{
    XMLXPath_NodeType_Element       = 1, /*!< element   */
    XMLXPath_NodeType_Attribute     = 2, /*!< attribute */
    XMLXPath_NodeType_Nodename      = 3  /*!< nodename  */
} XMLXPath_NodeType;

/*!
    XML step types
*/
typedef enum
{
    XMLXPath_Step_Absolute      = 1,    /*!< absolute step */
    XMLXPath_Step_Relative      = 2     /*!< relative step */
} XMLXPath_StepType;

/*!
    XML step operation type
*/
typedef enum
{
    XMLXPath_StepOp_NONE  = 0,      /*!< none       */
    XMLXPath_StepOp_EQ    = 1,      /*!< equal      */
    XMLXPath_StepOp_NEQ   = 2       /*!< not equal  */
} XMLXPath_StepOpType;

/*!
    forward declaration for struct XMLXPath_Predicate pointer
*/
typedef struct XMLXPath_Predicate XMLXPath_Predicate, *XMLXPath_PredicatePtr;

/*!
    XMLXPath_Predicate 
*/
struct XMLXPath_Predicate
{
    XMLXPath_PredicatePtr   next;         /*!< next pointer                         */
    SAPDB_UTF8             *name;         /*!< name of attribute                    */
    SAPDB_UTF8             *value;        /*!< value to compare to                  */
    short                   nameLen;      /*!< name length                          */
    short                   valueLen;     /*!< value length                         */
    XMLXPath_NodeType       nodeType;     /*!< node type                            */
    XMLXPath_StepOpType     selectOP;     /*!< level of element or op for selection */
};

/*!
    XMLXPath_Step 
*/
typedef struct
{
    XMLXPath_PredicatePtr  predicate;     /*!< pointer to predicate                 */
    SAPDB_UTF8            *name;          /*!< name of element or attribute         */
    SAPDB_UTF8            *value;         /*!< value to compare to                  */
    short                  nameLen;       /*!< name length                          */
    XMLXPath_NodeType      nodeType;      /*!< node type                            */ 
    XMLXPath_StepType      stepType;      /*!< step type                            */
} XMLXPath_Step;

/*!
    XMLXPath_Step pointer definition 
*/
typedef  XMLXPath_Step *XMLXPath_StepPtr;

/*!
    XMLXPath_TokenList 
*/
typedef struct
{
    SAPDB_UTF8           *xpathStr;       /*!< xpath string                         */
    XMLXPath_ItemPtr      firstToken;     /*!< token list built by tokenizer        */
    XMLXPath_StepPtr      firstStep;      /*!< execution steps to build index       */
    short                 tokenUsed;      /*!< token used                           */
    short                 tokenAllocated; /*!< token allocated                      */
    short                 stepsUsed;      /*!< steps used                           */
    short                 stepsAllocated; /*!< steps allocated                      */
    XMLXPath_Idx_Category category;       /*!< category                             */
} XMLXPath_TokenList;

/*!
    XMLXPath_TokenList pointer definition 
*/
typedef XMLXPath_TokenList *XMLXPath_TokenListPtr;

/*!
    forward declaration for struct XMLXPath_IndexList pointer
*/
typedef struct XMLXPath_IndexList XMLXPath_IndexList, *XMLXPath_IndexListPtr;

/*!
    XMLXPath_IndexList 
*/
struct XMLXPath_IndexList
{
    XMLXPath_IndexListPtr     next;             /*!< next               */
    XMLXPath_TokenListPtr     itemListBase;     /*!< base               */  
    XMLXPath_TokenListPtr     itemListValue;    /*!< value              */
    XMLXPath_Idx_IndexHandler indexHandler;     /*!< handler            */
    void                     *userData;         /*!< user data          */
    SAPDB_Bool                attrUsed;         /*!< attribute used     */
    SAPDB_Bool                textUsed;         /*!< text used          */
};

/*!
    XMLXPath_Context definition 
*/
typedef struct
{
    void                     *theParser;       /*!< pointer to SAX parser            */
    void                     *idxActiveList;   /*!< index list used by SAX handlers  */
    XMLXPath_IndexListPtr     idxList;         /*!< list of all defined indexes      */
    XMLXPath_IndexListPtr     idxListInWork;   /*!< used by XMLXPath_Idx_DefineIndex */
    XMLXPath_Error            error;           /*!< error description                */
    SAPDB_Int2                actualTreeLevel; /*!< actual level within XML document */
    SAPDB_UTF8                tmpWorkBuf[256]; /*!< working area                     */
    XMLXPath_Idx_RetCode      retCode;         /*!< needed for SAX parser logic      */
    SAPDB_Bool                readCallBackFlg; /*!< use callback for reading document*/
    void                     *userData;        /*!< user defined data                */
} XMLXPath_Context;

/*!
    XMLXPath_TokenList pointer definition 
*/
typedef XMLXPath_Context *XMLXPath_ContextPtr;

#endif /* XMLXPATH_TYPES_H */
