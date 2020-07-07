/*!
  \file    DBMSrvExpHlp_FuzzyStringDifference.cpp
  \author  TiloH
  \ingroup Basics for DBMServer commands explain and help
  \brief   definition of a classe for calculating fuzzy differences
           between strings

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#include <string.h>

#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_FuzzyStringDifference.hpp"


bool DBMSrvExpHlp_FuzzyStringDifference::calculateMinimalDifferenceToAllSubstrings(
        const char * s1,
        const char * s2,
        size_t     & difference)
{
    bool          rc=true;                              // return code, true if needed memory can be allocated, false otherwise

    difference=strlen(s1);

    if(0<difference) //is s1 not empty (otherwise difference is already 0, as it should be)
    {
        const char ** stack=new const char *[strlen(s1)+1]; // stack[s] will be the position of s1[s] in s2, if s1[s] is left
                                                            // out, it is the first position where it could have been
        int         * state=new int [strlen(s1)+1];         // state[s]==0 -> s1[s] was not handled before,
                                                            // state[s]==1 -> s1[s] is left out from s2
                                                            // state[s]==2 -> s1[s] is at position stack[s] in s2
        size_t      * d=new size_t[strlen(s1)+1];           // d[s] is the difference of s1 within s2 up to position s-1
        size_t        s=0;                                  // position within s1
        bool          reachedEnd=false;                     // true if all cases were calculated, false otherwise

        if(0==stack || 0==state || 0==d)
            rc=false;
        else
        {
            d[0]=0;      //no difference so far
            state[0]=0;  //first character was not handled before
            stack[0]=s2; //all characters of s2 can be used

            while(!reachedEnd)
            {
                if(state[s]==0)     //first step for s1[s] -> leave the character out 
                {
                    d[s+1]=d[s]+1;  //increase difference for s+1, because character s1[s] is left out
                    state[s]++;     //in next time don't leave s1[s] out of the game
                }
                else
                {
                    if(state[s]==1) //start going through all possible positions of s1[s] in s2
                    {
                        d[s+1]=d[s];                      //character s1[s] is no longer left out, so difference is left the same
                        stack[s]=strchr(stack[s], s1[s]); //search the first occurrence of that character
                        state[s]++;
                    }
                    else
                        if(state[s]==2) //we had already avalid position of s1[s] in s2 and it was stack[s]
                            stack[s]=strchr(stack[s]+1, s1[s]); //search the next occurrence of that character

                    if(1==state[s] || 2==state[s])
                    {
                        if(0==stack[s])
                            state[s]=3; //we have finished s1[s]
                        else
                        {
                            //is s1[s] the first character "found" in s2?
                            if(s==0 || (state[s-1]==1 && stack[s-1]==s2))
                                d[s+1]=d[s];    // as we calculate the difference to all substrings, the distance (stack[s]-s2) plays no role;
                            else
                                d[s+1]=d[s]+(stack[s]-(stack[s-1]+(state[s-1]==1?0:1))); //count the characters left out of s2
                        }
                    }
                }

                if(state[s]<3) //we have a new position for s1[s] in s2 -> try to find the rest of s1 in the rest of s2
                {   
                    if(d[s+1]<difference) //only position to next character, if difference so far is smaller then minimum found so far
                    {
                        if(s+1<strlen(s1))  //s1[s] is not the last character of s1 -> position to s1[s+1]
                        {
                            state[s+1]=0;
                            stack[s+1]=stack[s];

                            if(2==state[s]) //if s1[s] is set, s1[s+1] can be positioned within s2 only starting at the next character
                                stack[s+1]++;

                            s++; //now position to the next character for the next iteration
                        }
                        else //we are at the last character of s1 and d[s+1] is the actual difference of this distribution of s1 into s2 
                            difference=d[s+1];
                    }
                }
                else //we have gone through all possibly positions of s1[s]
                {
                    if(0==s)
                        reachedEnd=true;
                    else
                        s--;
                }
            }
        }

        //cleanup memory
        if(0!=stack)
            delete [] stack;

        if(0!=state)
            delete [] state;

        if(0!=d)
            delete [] d;
    }

    return rc;
}
