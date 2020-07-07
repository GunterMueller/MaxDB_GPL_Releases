/*!
  @defgroup       DBProc DB-Procedures and DB-Functions
  @file           DBProc_Handler.hpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          methods for db-procedure and db-function execution

\if EMIT_LICENCE

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



\endif
*/

#ifndef DBPROC_HANDLER_HPP
#define DBPROC_HANDLER_HPP

struct  tgg00_TransContext;
struct  tgg00_StackEntry;
struct  tgg00_SelectFieldsParam;
class   SAPDBFields_FieldMap;
class   SQLMan_Context;
class   SQLMan_MessBlock;
class   Catalog_MessBlockObject;
class   DBProc_DBProcDescriptor;
class   Msg_List;

/*!
   @brief provides functions concerning internal db-procedures und db-functions
*/

class DBProc_Handler
{
public :

    /*!
       @brief descriptor for required length of db-function parameters
    */
    struct ParameterDescriptor
    {
        bool m_toUnicode;
        int  m_fixedLength;
    };
    typedef ParameterDescriptor ParameterDescriptors[16];

    /*!
       @brief calculates the result of a user defined function
       @param transContext the current transaction context
       @param functionId   the internal identification of the function
       @param inParameters the input parameter values
       @param parameterDescriptors description of parameters of fixed length
       @param pResult the buffer containing the function result
       @param resultLen the length of the result
    */
    static void CalculateFunctionResult (
        tgg00_TransContext                   &transContext,
        const tgg00_StackEntry               &functionId,
        const SAPDBFields_FieldMap           &inParameters,
        const ParameterDescriptors           &parameterDescriptors,
        const tgg00_Rec*                     &pResult,
        int                                  &resultLen);
    /*!
      @brief execute a sql statement inside a db-procedure, db-function or trigger
      @param context the current sql manager context
      @param interpreterContext the current interpreter context (pc, stack, etc)
      @param pReturnCode pointer to the sql number storing the sql code
      @param pointer to the stack code of the caller
      @param currQualIndex current 'program counter'
     */
    static tgg00_BasisError ExecuteSqlStatement (
                                     SQLMan_Context*          pContext,
                                     tgg00_SelectFieldsParam& interpreterContext,
                                     tsp00_MoveObjPtr         pReturnCode,
                                     tgg00_StackEntry*        pCode,
                                     tsp00_Int4&              currQualIndex);

    /*!
       @brief executes a stored procedure call
       @param context the current sql manager context. The call code is located in the mess block of the context
       @param outParamCnt [out] returns the number of output parameters
    */
    static void ProcedureCall (SQLMan_Context& context,
                               int&            outParamCnt);
    /*!
       @brief calls a user defined function
    */
    static void UserDefinedFunctionCall  (
        tgg00_TransContext&      transContext,
        tgg00_SelectFieldsParam& interpreterContext,
        const tgg00_StackEntry&  functionCode,
        const tgg00_StackEntry&  functionId,
        short&                   error);

    enum CallResult {call_ok, call_failed, call_return_error};

private :
    /*!
       @brief calls a db-procedure written in PL
     */
    static void PLDBProcCall (SQLMan_Context&          context,
                              Catalog_MessBlockObject* pCode,
                              SQLMan_MessBlock&        dbprocCode);
     /*!
       @brief calls an external db-procedure written in java, perl, python ...
     */
    static CallResult ExternalProcCall (
                            SQLMan_Context&          context,
                            DBProc_DBProcDescriptor& DBProcDescriptor,
                            SAPDBFields_FieldMap&    parameterMap);
     /*!
       @brief check external db-procedure written in java, perl, python ...

       This should be called after the CREATE DBPROC to make
       sure that the code can be found and that the parameters match.
     */
    static bool CheckExternalProc (SQLMan_Context&          context,
                                   DBProc_DBProcDescriptor& DBProcDescriptor);

};

#endif