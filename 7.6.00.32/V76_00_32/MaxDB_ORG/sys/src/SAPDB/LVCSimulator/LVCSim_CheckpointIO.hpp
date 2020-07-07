/****************************************************************************

  module      : LVCSim_CheckpointIO.hpp

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


#ifndef LVCSIM_CHECKPOINTIO_HPP
#define LVCSIM_CHECKPOINTIO_HPP


#include <stdio.h>

class LVCSim_CheckpointException {
public:
	long err_fpos;

	LVCSim_CheckpointException(long fpos);
};


/// Interface for checkpoint writer.
class LVCSim_CheckpointWriter {
public:
	virtual void write(const void *data, size_t count) = 0;
};

/// Interface for checkpoint reader.
class LVCSim_CheckpointReader {
public:
	virtual void read(void *data, size_t count) = 0;
};


/// Checkpoint writer writing to a file.
class LVCSim_CheckpointFileWriter : public LVCSim_CheckpointWriter {
public:
	LVCSim_CheckpointFileWriter(const char *fname);
	~LVCSim_CheckpointFileWriter();

	virtual void write(const void *data, size_t count);
private:
	FILE *f;
};

/// Checkpoint reader reading from a file.
class LVCSim_CheckpointFileReader : public LVCSim_CheckpointReader {
public:
	LVCSim_CheckpointFileReader(const char *fname);
	~LVCSim_CheckpointFileReader();

	virtual void read(void *data, size_t count);
private:
	FILE *f;
};


#endif // LVCSIM_CHECKPOINTIO_HPP
