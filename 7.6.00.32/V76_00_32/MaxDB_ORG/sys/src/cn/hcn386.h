/*******************************************************************************
  module:       hcn386.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  basic definitions for output and error output of Backint for
                SAP DB/Backint for Oracle adapter
  last change:  2000-11-29 11:25
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


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



*******************************************************************************/

#if !defined(_HCN386_H_)
#define _HCN386_H_

class tcn386_BackintOut
{
  public:
    tcn386_BackintOut();
    ~tcn386_BackintOut();

    int SetOutputTo(const char * NewOutFileName);

    int Open();
    //int Write(const char * Line);
    int Write(const char * Part1,
              const char * Part2=0,
              const char * Part3=0,
              const char * Part4=0,
              const char * Part5=0,
              const char * Part6=0,
              const char * Part7=0);
    int Close();

  private:
    char * OutFileName;

    tni34_ATextFile * OutFile;
    int               OutFileIsOpen;
    int               FirstOpen;
};

class BackintError
{
};

#endif
