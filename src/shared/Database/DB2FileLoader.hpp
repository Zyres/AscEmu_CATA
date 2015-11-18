/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DB2FILELOADER_H
#define DB2FILELOADER_H

#include "ByteConverter.hpp"
#include "Common.h"
#include <cassert>


class DB2FileLoader
{
    public:

        DB2FileLoader();
        ~DB2FileLoader();

        bool Load(const char* filename, const char* fmt);

        class Record
        {
            public:

                float getFloat(size_t field) const
                {
                    assert(field < file.fieldCount);
                    float val = *reinterpret_cast<float*>(offset + file.GetOffset(field));
                    EndianConvert(val);
                    return val;
                }
                uint32 getUInt(size_t field) const
                {
                    assert(field < file.fieldCount);
                    uint32 val = *reinterpret_cast<uint32*>(offset + file.GetOffset(field));
                    EndianConvert(val);
                    return val;
                }
                uint8 getUInt8(size_t field) const
                {
                    assert(field < file.fieldCount);
                    return *reinterpret_cast<uint8*>(offset + file.GetOffset(field));
                }

                const char* getString(size_t field) const
                {
                    assert(field < file.fieldCount);
                    size_t stringOffset = getUInt(field);
                    assert(stringOffset < file.stringSize);
                    return reinterpret_cast<char*>(file.stringTable + stringOffset);
                }

            private:

                Record(DB2FileLoader& file_, unsigned char* offset_) : offset(offset_), file(file_) {}
                unsigned char* offset;
                DB2FileLoader& file;

                friend class DB2FileLoader;

        };

        // Get record by id
        Record getRecord(size_t id);
        /// Get begin iterator over records

        uint32 GetNumRows() const { return recordCount; }
        uint32 GetCols() const { return fieldCount; }
        uint32 GetOffset(size_t id) const { return (fieldsOffset != NULL && id < fieldCount) ? fieldsOffset[id] : 0; }
        bool IsLoaded() const { return (data != NULL); }
        char* AutoProduceData(const char* fmt, uint32& count, char**& indexTable);
        char* AutoProduceStringsArrayHolders(const char* fmt, char* dataTable);
        char* AutoProduceStrings(const char* fmt, char* dataTable, LocaleConstant loc);
        static uint32 GetFormatRecordSize(const char* format, int32* index_pos = NULL);
        static uint32 GetFormatStringsFields(const char* format);
    
    private:

        uint32 recordSize;
        uint32 recordCount;
        uint32 fieldCount;
        uint32 stringSize;
        uint32* fieldsOffset;
        unsigned char* data;
        unsigned char* stringTable;

        // WDB2/WCH2 fields
        uint32 tableHash;
        uint32 build;

        int unk1;
        int unk2;
        int unk3;
        int locale;
        int unk5;
};

#endif  //DB2FILELOADER_H
