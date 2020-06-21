/*!***************************************************************************

  module      : LVCSim_CheckpointIO.cpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : checkpoint writer and reader for simulator state dump

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "LVCSimulator/LVCSim_CheckpointIO.hpp"


LVCSim_CheckpointException::LVCSim_CheckpointException(long fpos)
{
	err_fpos = fpos;
}

LVCSim_CheckpointFileWriter::LVCSim_CheckpointFileWriter(const char *fname)
{
	f = fopen(fname, "wb");
	if (!f) throw LVCSim_CheckpointException(-1);
}

LVCSim_CheckpointFileWriter::~LVCSim_CheckpointFileWriter()
{
	fflush(f);
	fclose(f);
}

void LVCSim_CheckpointFileWriter::write(const void *data, size_t count)
{
	if (fwrite(data, 1, count, f) != count)
		throw LVCSim_CheckpointException(ftell(f));
}


LVCSim_CheckpointFileReader::LVCSim_CheckpointFileReader(const char *fname)
{
	f = fopen(fname, "rb");
	if (!f) throw LVCSim_CheckpointException(-1);
}

LVCSim_CheckpointFileReader::~LVCSim_CheckpointFileReader()
{
	fclose(f);
}

void LVCSim_CheckpointFileReader::read(void *data, size_t count)
{
	if (fread(data, 1, count, f) != count)
		throw LVCSim_CheckpointException(ftell(f));
}

