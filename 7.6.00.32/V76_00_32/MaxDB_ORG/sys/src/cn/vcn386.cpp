/*!*****************************************************************************
  module:       vcn386.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  output and error output for Backint for SAP DB/Backint for
                Oracle adapter
  last change:  2002-07-18 12:56
  version:      7.3.0
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include <time.h>
#include <stdio.h>

#include "hcn36.h"
#include "hcn385.h"
#include "hcn386.h"

tcn386_BackintOut::tcn386_BackintOut()
:OutFileName(0),
 OutFile(0),
 OutFileIsOpen(0),
 FirstOpen(1)
{
}

tcn386_BackintOut::~tcn386_BackintOut()
{
    cn36_StrDealloc(OutFileName);
    
    Close();

    if(0!=OutFile)
        delete OutFile;
}

int tcn386_BackintOut::SetOutputTo(const char * NewOutFileName)
{
    int rc=1;

    Close();

    if(0!=OutFile)
    {
        delete OutFile;
        OutFile=0;
    }

    if(0!=NewOutFileName)
    {
        rc=cn36_StrAllocCpy(OutFileName, NewOutFileName);

        if(rc)
        {
            OutFile=new tni34_ATextFile(OutFileName, tni34_AFile::ToDiskAppend, 1, 0);   //create if not existing, do not open now

            if(0==OutFile)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            }
            else
                if(!OutFile->IsGood())
                    rc=0;
        }
    }
    else
        cn36_StrDealloc(OutFileName);

    return rc;
}

int tcn386_BackintOut::Open()
{
    int rc=1;

    if(0!=OutFile && !OutFileIsOpen)
    {
        if(FirstOpen)
        {
            if(OutFile->Exists() && !OutFile->Remove())
                rc=0;

            FirstOpen=0;
        }

        if(rc)
            rc=OutFile->Open();

        if(rc)
            OutFileIsOpen=1;
        else
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenOutFile));
        }
    }

    return rc;
}

/*int tcn386_BackintOut::Write(const char * Line)
{
    int rc=1;

    if(0==OutFileName)
        cout<<Line<<endl;
    else
    {
        int OpenedNow=0;

        if(!OutFileIsOpen)
            rc=OutFile->Open();

        //tni34_ATextFile OutFile(OutFileName, tni34_AFile::ToDiskAppend);

        if(rc)
        {
        //if(OutFile.IsGood())
            rc=OutFile->WriteLine(Line);
        }
        //else
        //{
        //    rc=0;
        //    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenOutFile));
        //}

        if(OpenedNow && !OutFile->Close())
            rc=0;
    }

    return rc;
}*/

int tcn386_BackintOut::Write(const char * Part1,
                             const char * Part2,
                             const char * Part3,
                             const char * Part4,
                             const char * Part5,
                             const char * Part6,
                             const char * Part7)
{
    int rc=1,
        i,
        LastSetPart=0;

    const char *Part[7];

    if(0!=Part1)
        Part[LastSetPart++]=Part1;

    if(0!=Part2)
        Part[LastSetPart++]=Part2;

    if(0!=Part3)
        Part[LastSetPart++]=Part3;

    if(0!=Part4)
        Part[LastSetPart++]=Part4;

    if(0!=Part5)
        Part[LastSetPart++]=Part5;
    
    if(0!=Part6)
        Part[LastSetPart++]=Part6;

    if(0!=Part7)
        Part[LastSetPart++]=Part7;

    if(0==OutFileName)
    {
        for(i=0; i<LastSetPart; i++)
            printf("%s", (const char *)(Part[i]));
        
        printf("\n");
        fflush(stdout);
    }
    else
    {
        int OpenedNow=0;

        if(!OutFileIsOpen)
        {
            if(Open())
                OpenedNow=1;
            else
                rc=0;
        }

        if(rc)
        {
            long BytesWritten;

            for(i=0; rc && i<LastSetPart-1; i++)
            {
                if(!OutFile->Write(Part[i], long(strlen(Part[i])), BytesWritten))
                    rc=0;
            }

            if(0<LastSetPart)
            {
                if(!OutFile->WriteLine(Part[LastSetPart-1]))
                    rc=0;
            }
            else
            {
                if(!OutFile->WriteLine(""))
                    rc=0;
            }

/*            if(!OutFile->Write(Part1, strlen(Part1), BytesWritten) ||
               !OutFile->Write(Part2, strlen(Part2), BytesWritten) ||
               !OutFile->WriteLine(Part3))
            {
                rc=0;
            }*/
        }
        /*else
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenOutFile));
        }*/
        
        if(OpenedNow && !Close()/*OutFile->Close()*/)
            rc=0;
    }

    return rc;
}

int tcn386_BackintOut::Close()
{
    int rc=1;

    if(OutFileIsOpen)
    {
        if(0!=OutFile)
        {
            if(OutFile->Close())
                OutFileIsOpen=0;
            else
                rc=0;
        }
        else
        {
            fflush(stdout);
            OutFileIsOpen=0;
        }
    }

    return rc;
}
