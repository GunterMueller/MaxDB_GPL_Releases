/*!*********************************************************************

    @file         SAPDBFields_VarData.cpp

    @author       DanielD

    @brief

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
***********************************************************************/

#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"

#include "vsp001.h"

/*----------------------------------------*/

inline void
writeInt2 (
    unsigned char * target,
    int             value)
{
    target [0] = value / 256;
    target [1] = value % 256;
}

/*----------------------------------------*/

inline int
readInt2 (
    const unsigned char * source)
{
    return source [0] * 256 + source [1];
}

/*----------------------------------------*/

inline int
ptrDiff (
    const void * highPtr,
    const void * lowPtr)
{
    return ((const char *)highPtr) - ((const char *)lowPtr);
}

/*----------------------------------------*/

inline bool
lenRequires2Bytes (
    unsigned int len)
{
    return len <= csp1_fi_max_1byte_length;
}

/*----------------------------------------*/

SAPDBFields_VarData::Reader::
Reader (
    const void * buf_init)
{
    unsigned char * buf = (unsigned char *) buf_init;
    this->fieldsRemaining = readInt2 (buf);
    this->readPos = buf + 2;
}

/*----------------------------------------*/

SAPDBFields_VarData::Reader::
Reader ()
{
    this->fieldsRemaining = 0;
    this->readPos = NULL;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Reader::
next (
    const void * & fieldptr,
    int          & fieldlen,
    int          & indicator)
{
    if (this->fieldsRemaining <= 0) {
        return false;
    }
    switch (this->readPos [0]) {
        case csp1_fi_ignored:
        case csp1_fi_special_null:
        case csp1_fi_blob_description:
        case csp1_fi_default_value:
        case csp1_fi_null_value:
            fieldptr = NULL;
            fieldlen = 0;
            indicator = this->readPos [0];
            this->readPos += 1;
            break;
        case csp1_fi_2byte_length:
            fieldptr = readPos + 3;
            fieldlen = readInt2 (this->readPos + 1);
            indicator = 0;
            this->readPos += 3 + fieldlen;
            break;
        default:
            fieldptr = readPos + 1;
            fieldlen = this->readPos [0];
            indicator = 0;
            this->readPos += 1 + fieldlen;
            break;
    }
    --this->fieldsRemaining;
    return true;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Reader::
next (
    const void * & fieldptr,
    int          & fieldlen)
{
    int indicator;
    bool ok = this->next (fieldptr, fieldlen, indicator);
    if (!ok) {
        return false;
    }
    if (indicator != 0) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Reader::
skip (
    int count)
{
    const void * fieldptr;
    int          fieldlen;
    int          indicator;

    for (int i = 0; i < count; ++i) {
        if (! this->next (fieldptr, fieldlen, indicator)) {
            return false;
        }
    }
    return true;
}

/*----------------------------------------*/

class Verifier
    : public SAPDBFields_VarData::Reader
{
public:
    Verifier (const void * buf, int buflen);
    int verify ();
private:
    unsigned char * endPos;
};

/*----------------------------------------*/

Verifier::
Verifier (
    const void * buf,
    int buflen)
: SAPDBFields_VarData::Reader (buf)
{
    this->endPos = ((unsigned char*) buf) + buflen;
}

/*----------------------------------------*/

int Verifier::
verify ()
{
    const void * fieldptr;
    int          fieldlen;
    int          indicator;

    while (this->next (fieldptr, fieldlen, indicator)) {
        if (this->readPos > this->endPos) {
            return 1;
        }
    }
    if (this->readPos == this->endPos) {
        return 0;
    }
    else {
        return -1;
    }
}

/*----------------------------------------*/

bool SAPDBFields_VarData::
Verify (
    const void * buf,
    int buflen,
    bool allowUnderflow)
{
    Verifier ver (buf, buflen);
    int result = ver.verify ();
    if (allowUnderflow) {
        return result <= 0;
    }
    else {
        return result == 0;
    }
}

/*----------------------------------------*/

unsigned int SAPDBFields_VarData::
GetFieldCount (
    const void * buf)
{
    return readInt2 ((const unsigned char *) buf);
}

/*----------------------------------------*/

unsigned int SAPDBFields_VarData::
GetTotalLength (
    const void * buf)
{
    Reader reader (buf);
    const void * fieldptr;
    int          fieldlen = 0;
    int          indicator;
    unsigned int result = 2; // u2 fieldCount

    while (reader.next (fieldptr, fieldlen, indicator)) {
        if (indicator == 0) {
            result = ptrDiff (fieldptr, buf) + fieldlen;
        }
        else {
            // +1 for indicator byte
            ++result;
        }
    }
    return result;
}

/*----------------------------------------*/

SAPDBFields_VarData::Writer::
Writer ()
{
}

/*----------------------------------------*/

SAPDBFields_VarData::Writer::
Writer (
    void * buf,
    int bufsize)
{
    this->fieldCount = 0;
    this->startPos = (unsigned char *) buf;
    this->writePos = startPos + 2;
    this->endPos = startPos + bufsize;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Writer::
addField (
    const void * field,
    unsigned int fieldlen)
{
    void * target = this->reserveField (fieldlen);
    if (target == NULL) {
        return false;
    }
    memcpy (target, field, fieldlen);
    return true;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Writer::
addField (
    const void * field,
    unsigned int fieldlen,
    int indicator)
{
    if (indicator == 0) {
        return this->addField (field, fieldlen);
    }
    else {
        return this->addSpecial (indicator);
    }
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Writer::
addSpecial (
    int indicator)
{
    if ((this->endPos - this->writePos) < 1) {
        return false;
    }
    this->writePos [0]   = (unsigned int) indicator;
    ++this->fieldCount;
    ++this->writePos;
    return true;
}

/*----------------------------------------*/

void * SAPDBFields_VarData::Writer::
reserveField (
    unsigned int fieldlen)
{
    int indicatorLen;
    if (lenRequires2Bytes (fieldlen)) {
        indicatorLen = 1;
    }
    else {
        indicatorLen = 3;
    }
    if ((this->writePos + indicatorLen + fieldlen) > this->endPos) {
        return NULL;
    }
    if (lenRequires2Bytes (fieldlen)) {
        this->writePos [0] = (unsigned int) fieldlen;
    }
    else {
        this->writePos [0] = csp1_fi_2byte_length;
        writeInt2 (this->writePos + 1, fieldlen);
    }
    void * result = this->writePos + indicatorLen;
    this->writePos += indicatorLen + fieldlen;
    ++this->fieldCount;
    return result;
}

/*----------------------------------------*/

void SAPDBFields_VarData::Writer::
close (
    int & fieldCount_out,
    int & buflen)
{
    writeInt2 (this->startPos, this->fieldCount);
    fieldCount_out = this->fieldCount;
    buflen = this->writePos - this->startPos;
}

/*----------------------------------------*/

void SAPDBFields_VarData::Writer::
openSubfields (
    Writer & subfields)
{
    unsigned char * fieldDataPos = this->writePos + 3;
    int remaining = this->endPos - fieldDataPos;
    subfields = Writer (fieldDataPos, remaining);
}

/*----------------------------------------*/

void SAPDBFields_VarData::Writer::
closeSubfields (
    Writer & subfields)
{
    int subFieldCount;
    int subLen;
    int indicatorLen;

    subfields.close (subFieldCount, subLen);
    if (lenRequires2Bytes (subLen)) {
        indicatorLen = 1;
    }
    else {
        indicatorLen = 3;
    }
    if (lenRequires2Bytes (subLen)) {
        this->writePos [0] = (unsigned int) subLen;
        memmove (this->writePos + 1, subfields.startPos, subLen);
    }
    else {
        this->writePos [0] = csp1_fi_2byte_length;
        writeInt2 (this->writePos + 1, subLen);
    }
    this->writePos += indicatorLen + subLen;
    ++this->fieldCount;
}

/*----------------------------------------*/

bool SAPDBFields_VarData::Writer::
copyReader (
    Reader & input)
{
    const void * fieldptr;
    int          fieldlen = 0;
    int          indicator;
    while (input.next (fieldptr, fieldlen, indicator)) {
        if (!this->addField (fieldptr, fieldlen, indicator)) {
            return false;
        }
    }
    return true;
}

/*----------------------------------------*/

