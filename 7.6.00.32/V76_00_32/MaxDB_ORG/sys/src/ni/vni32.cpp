/*******************************************************************************
  module:       vni32.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  implementation of classes for parameter handling
  last change:  1999-10-04 15:30
  version:      7.2
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




*******************************************************************************/


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni32.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


ni32_AParameter::ni32_AParameter(State s,const char *EnvironmentName)
  :ParamIsSet(0),ParamState(s)
{
    if(0!=EnvironmentName)
    {
        strncpy(EnvName,EnvironmentName,255);
        EnvName[255]='\0';
    }
    else
        EnvName[0]='\0';
}

ni32_AParameter::~ni32_AParameter()
{
}

void ni32_AParameter::SetParameterToFound()
{
    ParamIsSet=1;
}

int ni32_AParameter::IsSet()
{
    return ParamIsSet;
}

int ni32_AParameter::IsOptional()
{
    return (ParamState==Optional);
}

void ni32_AParameter::CheckEnvironment()
{
    if(EnvName[0]!='\0' && !IsSet())
    {
        char *pc=getenv(EnvName);

        if(0!=pc)
        {
            TakeValue(pc);
            ParamIsSet=2;
        }
    }
}

ni32_ProgramParameters::ni32_ProgramParameters(int argc, const char * const * argv, ni31_List<ni32_AParameter *> & ParamList)
:Param(ParamList),ArgC(argc),ArgV(argv)
{
    if(&ParamList==&TempParamList)
        TempParamList.DelAll();
}

int ni32_ProgramParameters::Check()
{
    const ni31_ListElem<ni32_AParameter *> *h;

    int NoErrorOccured=1;
    
    if(ArgC>1)
    {
        int i;
        ni31_ListElem<const char *> *last=Arguments.Add(ArgV[1]);
        for(i=2;i<ArgC;i++)
            last=Arguments.Add(ArgV[i],*last);
        
        while(Arguments.IsNotEmpty())
        {
            h=Param.First();

            while(h!=0 && !(h->GiveInfo()->Check(Arguments)))
                h=h->Next();

            if(0!=h)
                Arguments.Del(h->GiveInfo()->TakeValue(Arguments));
            else
            {
                if(0!=UsageMessageAvailable())
                    fprintf(stderr, "%s\n", (const char *)UsageMessage());
                else
                    fprintf(stderr, "Unknown or incorrect used argument '%s'.\n", (const char *)(*Arguments.First()));

                OnError();
                
                NoErrorOccured=0;
                Arguments.DelAll();
            }
        }
    }

    if(NoErrorOccured)
    {
        /* try to get unset parameters from the environment */
        h=Param.First();
        while(0!=h)
        {
            h->GiveInfo()->CheckEnvironment();
            h=h->Next();
        }

        /* are all none-optional parameters set?*/
        h=Param.First();
        while(0!=h)
        {
            if(!h->GiveInfo()->IsOptional() && !h->GiveInfo()->IsSet())
            {
                if(0!=UsageMessageAvailable())
	                fprintf(stderr, "%s\n", (const char *)UsageMessage());
                else
                    fprintf(stderr, "A required parameter was not supplied.\n");

                OnError();

                NoErrorOccured=0;
                h=0; // end while-loop
            }
            else
                h=h->Next();
        }

        // do further parameter checking
        if(NoErrorOccured && !ExtendedParameterCheck())
            NoErrorOccured=0;
    }

    return NoErrorOccured;
}

int ni32_ProgramParameters::UsageMessageAvailable()
{
    return 0;
}

const char * ni32_ProgramParameters::UsageMessage()
{
    return "";
}

void ni32_ProgramParameters::OnError()
{
}

int ni32_ProgramParameters::ExtendedParameterCheck()
{
    return 1;
}

ni31_List<ni32_AParameter *> ni32_ProgramParameters::TempParamList;



const ni32_AParameter & ni32_ProgramParameters::GiveParameter(int i)
{
    return *(ni32_AParameter *)(*Param[i]);
}

const ni31_List<ni32_AParameter *> & ni32_ProgramParameters::GiveParameterList()
{
    return Param;
}

ni32_SingleParameter::ni32_SingleParameter(const char * ParName)
{
    strncpy(ParamName,ParName,256-1);
    ParamName[255]=0;
}

int ni32_SingleParameter::Check(const ni31_List<const char*> & Arguments)
{
    return !strcmp(*Arguments.First(),ParamName);
}


int ni32_SingleParameter::TakeValue(const ni31_List<const char*> & Arguments)
{
    SetParameterToFound();
    return 1; 
}

void ni32_SingleParameter::TakeValue(const char *value)
{
    SetParameterToFound();
}


ni32_MergedParameter::ni32_MergedParameter(const char * ParName)
:ni32_SingleParameter(ParName)
{
    ParamValue[0]='\0';
}

int ni32_MergedParameter::Check(const ni31_List<const char*> & Arguments)
{
    return (!strncmp(*Arguments.First(),ParamName,strlen(ParamName)) && Arguments.First()->Next()!=0);
}

int ni32_MergedParameter::TakeValue(const ni31_List<const char*> & Arguments)
{
    TakeValue(*(Arguments.First()));

    return 2;
}

void ni32_MergedParameter::TakeValue(const char *value)
{
    SetParameterToFound();
    strncpy(ParamValue,value+strlen(ParamName),255);
    ParamValue[255]=0;
}

const char * ni32_MergedParameter::GiveValue()
{
    return ParamValue;
}



ni32_DoubleParameter::ni32_DoubleParameter(const char * ParName,State s,const char *EnvironmentName)
  :ni32_AParameter(s,EnvironmentName)
{
    strncpy(ParamName,ParName,255);
    ParamName[255]='\0';
    ParamValue[0]='\0';
}

int ni32_DoubleParameter::Check(const ni31_List<const char*> & Arguments)
{
    return (!strcmp(*Arguments.First(),ParamName) && 0!=Arguments.First()->Next());
}

void ni32_DoubleParameter::TakeValue(const char *value)
{
    SetParameterToFound();
    SetValueTo(value);
}

int ni32_DoubleParameter::TakeValue(const ni31_List<const char*> & Arguments)
{
    SetParameterToFound();
    SetValueTo(Arguments.First()->Next()->GiveInfo());

    return 2;
}

const char * ni32_DoubleParameter::GiveValue()
{
    return ParamValue;
}

void ni32_DoubleParameter::SetValueTo(const char * v)
{
    strncpy(ParamValue,v,255);
    ParamValue[255]=0;
}


ni32_RestrictedDoubleParameter::ni32_RestrictedDoubleParameter(const char * ParName, const ni31_List<const char*> & Values, State s)
  :ni32_DoubleParameter(ParName,s),PossibleValues(Values)
{
    if(&Values==&TempValueList)
        TempValueList.DelAll();

    if(PossibleValues.First()!=0)
        SetValueTo(PossibleValues.First()->GiveInfo());
}

ni31_List<const char*> ni32_RestrictedDoubleParameter::TempValueList;

int ni32_RestrictedDoubleParameter::Check(const ni31_List<const char*> & Arguments)
{
    int ReturnValue=0;
    
    if(ni32_DoubleParameter::Check(Arguments))
    {
        const ni31_ListElem<const char *> *h=PossibleValues.First();

        while(0!=h && strcmp(*Arguments.First()->Next(),*h))
            h=h->Next();

        if(0!=h)
            ReturnValue=1;
    }

    return ReturnValue;
}


// -----------------------------------------------------------------------------
// class ni32_TrippleParameter
// -----------------------------------------------------------------------------

ni32_TrippleParameter::ni32_TrippleParameter(const char * ParName, State s)
  :ni32_AParameter(s)
{
    strncpy(ParamName, ParName, 255);
    ParamName[255]='\0';
    ParamValue1[0]=ParamValue2[0]='\0';
}

int ni32_TrippleParameter::Check(const ni31_List<const char*> & Arguments)
{
    return (!strcmp(*Arguments.First(), ParamName) &&
            0!=Arguments.First()->Next() &&
            0!=Arguments.First()->Next()->Next());
}

int ni32_TrippleParameter::TakeValue(const ni31_List<const char*> & Arguments)
{
    SetParameterToFound();

    strncpy(ParamValue1, Arguments.First()->Next()->GiveInfo(), 255);
    ParamValue1[255]=0;
    
    strncpy(ParamValue2, Arguments.First()->Next()->Next()->GiveInfo(), 255);
    ParamValue2[255]=0;

    return 3;
}

void ni32_TrippleParameter::TakeValue(const char *value)
{
    SetParameterToFound();

    strncpy(ParamValue1, value, 255);
    ParamValue1[255]=0;

    ParamValue2[0]='\0';
}

void ni32_TrippleParameter::GiveValues(const char *& Value1, const char *& Value2)
{
    Value1=ParamValue1;
    Value2=ParamValue2;
}
