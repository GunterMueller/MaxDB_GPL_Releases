/*******************************************************************************
  module:       hni32.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  classes for parameter handling
  last change:  1999-01-22 16:05
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

#ifndef _HNI32_H_
#define _HNI32_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include"hni00b.h"
#include"hni31.h"

class ni32_AParameter;

// -----------------------------------------------------------------------------
// class ni32_ProgramParameters
// -----------------------------------------------------------------------------

class ni32_ProgramParameters
{
  private:
    ni31_List<ni32_AParameter *> Param;
    ni31_List<const char *>      Arguments;
    const int                    ArgC;     
    const char * const * const   ArgV;
  public:
    ni32_ProgramParameters(int argc, const char * const * argv, ni31_List<ni32_AParameter *> & ParamList);
    
    int                                  Check();
    const ni32_AParameter &              GiveParameter(int i);
    const ni31_List<ni32_AParameter *> & GiveParameterList();
    //int                                AreParametersOK(){return NoErrorOccured;}
    virtual int                          UsageMessageAvailable();
    virtual const char *                 UsageMessage();
    virtual void                         OnError();
    virtual int                          ExtendedParameterCheck();


    static ni31_List<ni32_AParameter *> TempParamList;
};


// -----------------------------------------------------------------------------
// class ni32_AParameter
// -----------------------------------------------------------------------------

class ni32_AParameter
{
  public:
    enum State{Optional,Required};
  private:
    int   ParamIsSet;
    State ParamState;
    char  EnvName[256];
  protected:
    void        SetParameterToFound();
  public:
                ni32_AParameter(State s=Optional,const char *EnvironmentName=0);
    virtual     ~ni32_AParameter();

    virtual int  Check(const ni31_List<const char*> & Arguments)=0;
    virtual void TakeValue(const char *value)=0;
    virtual int  TakeValue(const ni31_List<const char*> & Arguments)=0;
    void         CheckEnvironment();
    int          IsSet();
    int          IsOptional();

};


// -----------------------------------------------------------------------------
// class ni32_SingleParameter
// -----------------------------------------------------------------------------

class ni32_SingleParameter:public ni32_AParameter
{
  protected:
    char ParamName[256];
  public:
    ni32_SingleParameter(const char * ParName);

    virtual int Check(const ni31_List<const char*> & Arguments);
    virtual int TakeValue(const ni31_List<const char*> & Arguments);
    virtual void TakeValue(const char *value);
};


// -----------------------------------------------------------------------------
// class ni32_MergedParameter
// -----------------------------------------------------------------------------

class ni32_MergedParameter:public ni32_SingleParameter
{
    char ParamValue[256];
  public:
    ni32_MergedParameter(const char * ParName);
    virtual int  Check(const ni31_List<const char*> & Arguments);
    virtual int  TakeValue(const ni31_List<const char*> & Arguments);
    virtual void TakeValue(const char *value);
    const char * GiveValue();
};


// -----------------------------------------------------------------------------
// class ni32_DoubleParameter
// -----------------------------------------------------------------------------

class ni32_DoubleParameter:public ni32_AParameter
{
    char ParamName[256];
    char ParamValue[256];
  protected:
    void SetValueTo(const char *v);
  public:
    ni32_DoubleParameter(const char * ParName, State s=Optional, const char *EnvironmentName=0);
    virtual int Check(const ni31_List<const char*> & Arguments);
    virtual int TakeValue(const ni31_List<const char*> & Arguments);
    const char * GiveValue();
    void        TakeValue(const char *value);
};


// -----------------------------------------------------------------------------
// class ni32_RestrictedDoubleParameter
// -----------------------------------------------------------------------------

class ni32_RestrictedDoubleParameter:public ni32_DoubleParameter
{
    const ni31_List<const char*> PossibleValues;
  public:
    ni32_RestrictedDoubleParameter(const char * ParName, const ni31_List<const char*> & Values, State s=Optional);
    virtual int Check(const ni31_List<const char*> & Arguments);

    static ni31_List<const char*> TempValueList;
};

// -----------------------------------------------------------------------------
// class ni32_TrippleParameter
// -----------------------------------------------------------------------------

class ni32_TrippleParameter:public ni32_AParameter
{
    char ParamName[256];
    char ParamValue1[256];
    char ParamValue2[256];
  //protected:
  //  void SetValueTo(const char *v);
  public:
    ni32_TrippleParameter(const char * ParName, State s=Optional);

    virtual int Check(const ni31_List<const char*> & Arguments);
    virtual int TakeValue(const ni31_List<const char*> & Arguments);
    void        TakeValue(const char *value);

    void GiveValues(const char *& Value1, const char *& Value2);
};

#endif
