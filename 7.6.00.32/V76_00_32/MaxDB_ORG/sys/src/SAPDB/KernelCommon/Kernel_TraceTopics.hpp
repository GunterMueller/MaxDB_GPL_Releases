/*!
  @file           Kernel_TraceTopics.hpp
  @author         DanielD
  @brief          exports array with all kernel trace topics

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


#ifndef KERNEL_TRACETOPICS_HPP
#define KERNEL_TRACETOPICS_HPP

class SAPDBDiag_Topic;
class SAPDBTrace_Topic;

/*!
   @brief          variable to be used as parameter for
    &lt;Method>SAPDBTrace_Topic::MultiSwitch&lt;/>

 */

extern SAPDBTrace_Topic * const Kernel_AllTraceTopics [];

/* enddeclaration: */

/*!
   @brief          parse and executes a line with check instructions


       The line is of the form "&lt;id> [&lt;int>] [&lt;id> [&lt;int>] ] ...
       An id '*' serves as a wildcard for all topics.
       interface in vgg941.

 */

externPascal void Kernel_CheckSwitch (const char*  TopicStr, /* c20 */
                                      int          TopicStrLen);

/*!
   @brief          parse and executes a line with trace instructions


       The line is of the form "&lt;id> [&lt;int>] [&lt;id> [&lt;int>] ] ...
       An id '*' serves as a wildcard for all topics.
       interface in vgg941.

 */

externPascal void Kernel_TraceSwitch (const char*  TopicStr, /* c20 */
                                      int          TopicStrLen);

/*!
   @brief          returns next check/vtrace option and its current value


arguments:
    requested [IN]    an index into the array of topics
    kindOut   [OUT]   "Vtrace" or "Check"
    topicKeyOut [OUT] the string used to switch the topic
    levelOut [OUT]    the current level
    topicDescOut [OUT]the description of the switch

 */

bool Kernel_DiagInfo (
    int          requested,
    SAPDB_Char * kindOut,
    int          maxKindLen,
    SAPDB_Char * topicKeyOut,
    int          maxTopicKey,
    int        & levelOut,
    SAPDB_Char * topicDescOut,
    int          maxTopicDesc);

/*!
    @brief      return the next check option

arguments:
    requested [IN]    an index into the array of topics
    topic [OUT]       the topic at index requested
 */
bool Kernel_DiagTopicsIterator (
    int requested,
    SAPDBDiag_Topic ** topic);

/*!
    @brief      set the check level for the named check option
 */
void Kernel_SwitchDiagByName (
    const SAPDB_Char * topicName,
    int                newLevel);

/*!
    @brief      return the next trace option

arguments:
    requested [IN]    an index into the array of topics
    topic [OUT]       the topic at index requested
 */
bool Kernel_TraceTopicsIterator (
    int requested,
    SAPDBDiag_Topic ** topic);

/*!
    @brief      set the check level for the named tracce option
 */
void Kernel_SwitchTraceByName (
    const SAPDB_Char * topicName,
    int                newLevel);

#endif

