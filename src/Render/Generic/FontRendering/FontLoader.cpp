#include "FontLoader.h"

void FontLoader::Load(WString const& _path)
{
    ifstream ifd(_path, std::ios::binary | std::ios::ate);
    bufferSize = ifd.tellg();
    ifd.seekg(0, std::ios::beg);
    buffer.resize((size_t)bufferSize);
    ifd.read(buffer.data(), bufferSize);

    ReadHeader();
    ReadTableDirectory();
    ReadHeadTable();
    ReadMaxpTable();
    ReadCmapTable();
    ReadLocaTable();
    ReadGlyphTable();
    ReadHheaTable();
    ReadHmtxTable();
}

uint16 FontLoader::GetGlyphIndex(uint16 charCode)
{
    return m_codepointToGlyph[charCode];
}

void FontLoader::BuildCodePointTable()
{
    m_codepointToGlyph[0] = 0;
    for (int i = 0; i < m_cmap.segCount; i++)
    {
        uint16 start = m_cmap.startCodes[i];
        uint16 end   = m_cmap.endCodes[i];

        if (start == 0xFFFF) continue;

        for (uint32 codepoint = start; codepoint <= end; codepoint++)
        {
            uint16 glyphIndex = 0;

            if (m_cmap.idRangeOffset[i] == 0)
            {
                glyphIndex = (codepoint + m_cmap.idDelta[i]) & 0xFFFF;
            }
            else
            {
                int32 idx = (int32)(m_cmap.idRangeOffset[i] / 2)
                          + (int32)(codepoint - start)
                          + i
                          - m_cmap.segCount;

                if (idx >= 0 && idx < (int32)m_cmap.glyphIdArray.size())
                {
                    glyphIndex = m_cmap.glyphIdArray[idx];
                    if (glyphIndex != 0)
                        glyphIndex = (glyphIndex + m_cmap.idDelta[i]) & 0xFFFF;
                }
            }

            if (glyphIndex != 0)
                m_codepointToGlyph[(uint16)codepoint] = glyphIndex;
        }
    }
}

void FontLoader::RemapGlyph(Glyph& _glyph)
{
    int size = _glyph.points.size() - 1;
    int i = 0;
    
    while (i < size)
    {
        // check if i and i+1 are in the same contour
        bool isBoundary = false;
        for (size_t c = 0; c < _glyph.endPtsContours.size(); c++)
        {
            if (_glyph.endPtsContours[c] == i)
            {
                isBoundary = true;
                break;
            }
        }

        if (isBoundary)
        {
            i++;
            continue;
        }
        
        bool createOfCurve = _glyph.points[i].onCurve && _glyph.points[i + 1].onCurve;
        bool createOnCurve = !_glyph.points[i].onCurve && !_glyph.points[i + 1].onCurve;

        if (!createOfCurve && !createOnCurve)
        {
            i++;
            continue;
        }
    
        int16 x = (_glyph.points[i].x + _glyph.points[i + 1].x) / 2;
        int16 y = (_glyph.points[i].y + _glyph.points[i + 1].y) / 2;

        GlyphPoint p;
        p.onCurve = createOnCurve;
        p.x = x;
        p.y = y;

        _glyph.points.insert(_glyph.points.begin() + i + 1, p);

        for (size_t c = 0; c < _glyph.endPtsContours.size(); c++)
        {
            if (_glyph.endPtsContours[c] >= i + 1)
                _glyph.endPtsContours[c]++;
        }

        size++;
        i += 2;
    }

    for (size_t c = 0; c < _glyph.endPtsContours.size(); c++)
    {
        int endPt   = _glyph.endPtsContours[c];
        int startPt = (c == 0) ? 0 : _glyph.endPtsContours[c - 1] + 1;
    
        bool createOffCurve = _glyph.points[endPt].onCurve && _glyph.points[startPt].onCurve;
        bool createOnCurve  = !_glyph.points[endPt].onCurve && !_glyph.points[startPt].onCurve;
    
        if (!createOffCurve && !createOnCurve) continue;
    
        int16 x = (_glyph.points[endPt].x + _glyph.points[startPt].x) / 2;
        int16 y = (_glyph.points[endPt].y + _glyph.points[startPt].y) / 2;
    
        GlyphPoint p;
        p.onCurve = createOnCurve;
        p.x = x;
        p.y = y;
        
        _glyph.points.insert(_glyph.points.begin() + endPt + 1, p);
        
        for (size_t j = 0; j < _glyph.endPtsContours.size(); j++)
        {
            if (_glyph.endPtsContours[j] >= endPt)
                _glyph.endPtsContours[j]++;
        }
    }
    
}

void FontLoader::ReadHeader()
{
    m_header.version = Read4Byte(cursor);
    m_header.tableCount = Read2Byte(cursor);
    m_header.searchRange = Read2Byte(cursor);
    m_header.entrySelector = Read2Byte(cursor);
    m_header.rangeShift = Read2Byte(cursor);

//     PRINTL("Loading font :");
//     PRINTL("");
//     PRINTL("Header :");
//
//     PRINTL("SF Version : " << m_header.version);
//     PRINTL("numTables  : " << m_header.tableCount);
//     PRINTL("searchRange : " << m_header.searchRange);
//     PRINTL("entrySelector : " << m_header.entrySelector);
//     PRINTL("rangeShift : " << m_header.rangeShift);
}

void FontLoader::ReadTableDirectory()
{
    for (int i = 0; i < m_header.tableCount; i++)
        ReadTableDirectoryElement();

    // PRINTL("");
    // PRINT("Tables :");
    // PRINTL("");
    //
    // for (auto [name, table] : m_tables)
    // {
    //     PRINTL("[" << name << "] : " << "Offset = " << table.offset << ", Size = " << table.length );
    // }
}

void FontLoader::ReadTableDirectoryElement()
{
    Table table;

    table.name.resize(4);
    memcpy(table.name.data(), buffer.data() + cursor, 4);
    SkipBytes(4);

    table.checksum = Read4Byte(cursor);
    table.offset = Read4Byte(cursor);
    table.length = Read4Byte(cursor);

    m_tables[table.name] = table;
}

void FontLoader::ReadHeadTable()
{
    uint32 offset = m_tables["head"].offset;

    m_head.magicNumber = Read4Byte(offset + 12);

    assert(m_head.magicNumber == 0x5F0F3CF5 && "MagicNumber doesn't match");

    m_head.unitsPerEm = Read2Byte(offset + 18);
    m_head.indexToLocFormat = Read2Byte(offset + 50);
    m_head.boundingBox.xMin = (int16)Read2Byte(offset + 36);
    m_head.boundingBox.yMin = (int16)Read2Byte(offset + 38);
    m_head.boundingBox.xMax = (int16)Read2Byte(offset + 40);
    m_head.boundingBox.yMax = (int16)Read2Byte(offset + 42);
    
    //
    // PRINTL("");
    // PRINT("Head Table :");
    // PRINTL("");
    //
    // PRINTL("Units per Elements : " << m_head.unitsPerEm);
    // PRINTL("Index to Loc Format : " << m_head.indexToLocFormat);
    // PRINTL("Bounding Box : { " <<  b.xMin << ", " << b.yMin << ", " << b.xMax << ", " << b.yMax << " }");
}

void FontLoader::ReadMaxpTable()
{
    uint32 offset = m_tables["maxp"].offset;

    m_maxp.version = Read4Byte(offset);
    m_maxp.glyphCount = Read2Byte(offset + 4);

    // PRINTL("");
    // PRINT("Maxp Table :");
    // PRINTL("");
    //
    // PRINTL("Version : " << m_maxp.version);
    // PRINTL("Glyph count : " << m_maxp.glyphCount);
}

void FontLoader::ReadCmapTable()
{
    uint32 offset = m_tables["cmap"].offset;

    uint16 numTables = Read2Byte(offset + 2);

    uint32 encodingRecordsStart = offset + 4;

    uint32 validTableStart = 0;
    bool found = false;
    
    for (int i = 0; i < numTables; i++)
    {
        uint16 platformID = Read2Byte(encodingRecordsStart + i * 8);
        uint16 encodingID = Read2Byte(encodingRecordsStart + i * 8 + 2);
        uint32 stOffset = Read4Byte(encodingRecordsStart + i * 8 + 4);

        if ((platformID != 3 || encodingID != 1) && (platformID != 0 || encodingID != 3))
            continue;

        validTableStart = offset + stOffset;
        found = true;
        
        break;
    }

    assert(found && "Can't find valid format to parse cmap");

    cursor = validTableStart;
    m_cmap.format = Read2Byte(cursor);

    assert(m_cmap.format == 4 && "Format doesn't match");

    m_cmap.length = Read2Byte(cursor);
    m_cmap.language = Read2Byte(cursor);
    m_cmap.segCount = Read2Byte(cursor) / 2;
    m_cmap.searchRange = Read2Byte(cursor);
    m_cmap.entrySelector = Read2Byte(cursor);
    m_cmap.rangeShift = Read2Byte(cursor);

    for (int i = 0; i < m_cmap.segCount; i++)
        m_cmap.endCodes.push_back(Read2Byte(cursor));
    SkipBytes(2);
    for (int i = 0; i < m_cmap.segCount; i++)
        m_cmap.startCodes.push_back(Read2Byte(cursor));
    for (int i = 0; i < m_cmap.segCount; i++)
        m_cmap.idDelta.push_back(Read2Byte(cursor));
    for (int i = 0; i < m_cmap.segCount; i++)
        m_cmap.idRangeOffset.push_back(Read2Byte(cursor));

    uint32 glyphIdArraySize = (m_cmap.length - (cursor - validTableStart)) / 2;
    
    for (uint32 i = 0; i < glyphIdArraySize; i++)
        m_cmap.glyphIdArray.push_back(Read2Byte(cursor));

    BuildCodePointTable();
}

void FontLoader::ReadLocaTable()
{
    uint32 offset = m_tables["loca"].offset;
    uint32 count = m_maxp.glyphCount + 1;

    cursor = offset;
    
    for (uint32 i = 0; i < count; i++)
    {
        if (m_head.indexToLocFormat == 0)
            m_loca.push_back(Read2Byte(cursor) * 2);
        else
            m_loca.push_back(Read4Byte(cursor));
    }

    // PRINTL("");
    // PRINT("Loca Table :");
    // PRINTL("");
    //
    // PRINTL("Format     : " << (m_head.indexToLocFormat == 0 ? "uint16 (x2)" : "uint32"));
    // PRINTL("Entry Count: " << m_loca.size());
    //
    // PRINTL("");
    // PRINTL("First 10 entries :");
    // for (int i = 0; i < 10 && i < m_loca.size(); i++)
    // {
    //     uint32 size = m_loca[i + 1] - m_loca[i];
    //     PRINTL("  glyph[" << i << "] offset=" << m_loca[i] << " size=" << size << (size == 0 ? " (empty)" : ""));
    // }
    //
    // PRINTL("");
    // PRINTL("Entries around 'A' (glyph index from cmap) :");
    // uint16 glyphA = GetGlyphIndex('A');
    // PRINTL("  'A' -> glyph index " << glyphA);
    // PRINTL("  offset=" << m_loca[glyphA] << " size=" << (m_loca[glyphA + 1] - m_loca[glyphA]));
}

void FontLoader::ReadGlyphTable()
{
    uint32 offset = m_tables["glyf"].offset;

    Vector<uint16> compoundGlyphs;
    
    for (size_t k = 0; k < m_loca.size(); k++)
    {
        if (k < m_loca.size() - 1 && m_loca[k] == m_loca[k + 1]) continue;
        
        cursor = offset + m_loca[k];
        Glyph g;
        g.contourCount = (int16)Read2Byte(cursor);

        if (g.contourCount <= 0)
        {
            if (g.contourCount < 0)
                compoundGlyphs.push_back((uint16)k);
            continue;
        }
        
        g.boundingBox.xMin = (int16)Read2Byte(cursor);
        g.boundingBox.yMin = (int16)Read2Byte(cursor);
        g.boundingBox.xMax = (int16)Read2Byte(cursor);
        g.boundingBox.yMax = (int16)Read2Byte(cursor);
        
        for (int i = 0; i < g.contourCount; i++)
            g.endPtsContours.push_back(Read2Byte(cursor));
        uint16 instructionLength = Read2Byte(cursor);
        SkipBytes(instructionLength);

        uint32 pointCount = g.endPtsContours.back() + 1;
        
        while (g.flags.size() < pointCount)
        {
            uint8 flag = ReadByte(cursor);
            g.flags.push_back(flag);
            if (flag & REPEAT)
            {
                uint8 repeatCount = ReadByte(cursor);
                for (int r = 0; r < repeatCount; r++)
                    g.flags.push_back(flag);
            }
        }

        // X COORD
        for (uint32 i = 0; i < pointCount; i++)
        {
            uint8 currFlag = g.flags[i];

            GlyphPoint p;
            p.onCurve = currFlag & GlyphFlags::ON_CURVE;
            p.x = 0;
            if (i > 0)
                p.x = g.points[i - 1].x;
            
            bool xSameOrPos = (currFlag & GlyphFlags::X_SAME_OR_POSITIVE) != 0;
            bool isShort = (currFlag & GlyphFlags::X_SHORT) != 0;

            if (isShort)
            {
                int16 delta = (int16)ReadByte(cursor);
                if (!xSameOrPos)
                    delta = int16(-delta);
                p.x += (int16)delta;
            }
            if (!isShort && !xSameOrPos)
                p.x += (int16)Read2Byte(cursor);

            g.points.push_back(p);
        }
        // Y COORD
        for (uint32 i = 0; i < pointCount; i++)
        {
            uint8 currFlag = g.flags[i];

            GlyphPoint& p = g.points[i];
            p.y = 0;
            if (i > 0)
                p.y = g.points[i - 1].y;
            
            bool ySameOrPos = (currFlag & GlyphFlags::Y_SAME_OR_POSITIVE) != 0;
            bool isShort = (currFlag & GlyphFlags::Y_SHORT) != 0;

            if (isShort)
            {
                int16 delta = (int16)ReadByte(cursor);
                if (!ySameOrPos) delta = -delta;  // negative direction
                p.y += (int16)delta;
            }
            if (!isShort && !ySameOrPos)
                p.y += (int16)Read2Byte(cursor);
        }

        RemapGlyph(g);
        m_glyphs[(uint16)k] = g;
    }

    for (uint16 k : compoundGlyphs)
    {
        cursor = offset + m_loca[k];
        Glyph g;

        SkipBytes(2);
        g.boundingBox.xMin = Read2Byte(cursor);
        g.boundingBox.yMin = Read2Byte(cursor);
        g.boundingBox.xMax = Read2Byte(cursor);
        g.boundingBox.yMax = Read2Byte(cursor);
        
        ReadCompoundGlyph(g, k);
    }
}

void FontLoader::ReadHheaTable()
{
    uint32 offset = m_tables["hhea"].offset;
    cursor = offset;

    Read4Byte(cursor); // version, skip

    m_hhea.ascender             = (int16)Read2Byte(cursor);
    m_hhea.descender            = (int16)Read2Byte(cursor);
    m_hhea.lineGap              = (int16)Read2Byte(cursor);
    m_hhea.advanceWidthMax      = Read2Byte(cursor);
    m_hhea.minLeftSideBearing   = (int16)Read2Byte(cursor);
    m_hhea.minRightSideBearing  = (int16)Read2Byte(cursor);
    m_hhea.xMaxExtent           = (int16)Read2Byte(cursor);
    m_hhea.caretSlopeRise       = (int16)Read2Byte(cursor);
    m_hhea.caretSlopeRun        = (int16)Read2Byte(cursor);
    m_hhea.caretOffset          = (int16)Read2Byte(cursor);
    SkipBytes(8);                // reserved (4 x int16)
    m_hhea.metricDataFormat     = (int16)Read2Byte(cursor);
    m_hhea.numberOfHMetrics     = Read2Byte(cursor);
}

void FontLoader::ReadHmtxTable()
{
    uint32 offset       = m_tables["hmtx"].offset;
    cursor              = offset;

    // numberOfHMetrics comes from hhea table
    for (int i = 0; i < m_hhea.numberOfHMetrics; i++)
    {
        GlyphMetrics metrics;
        metrics.advanceWidth     = Read2Byte(cursor);
        metrics.leftSideBearing  = (int16)Read2Byte(cursor);
        m_glyphMetrics[i]        = metrics;
    }

    // Remaining glyphs reuse the last advanceWidth, only LSB stored
    uint16 lastAdvance = m_glyphMetrics[m_hhea.numberOfHMetrics - 1].advanceWidth;
    for (int i = m_hhea.numberOfHMetrics; i < m_maxp.glyphCount; i++)
    {
        GlyphMetrics metrics;
        metrics.advanceWidth    = lastAdvance;
        metrics.leftSideBearing = (int16)Read2Byte(cursor);
        m_glyphMetrics[i]       = metrics;
    }
}

void FontLoader::ReadCompoundGlyph(Glyph& _g, uint16 _index)
{
    bool hasMoreComponents = true;
    while (hasMoreComponents)
    {
        uint16 flags      = Read2Byte(cursor);
        uint16 glyphIndex = Read2Byte(cursor);

        // Read offset/transform arguments
        int16 dx = 0, dy = 0;
        uint16 basePointIdx      = 0;
        uint16 componentPointIdx = 0;
        bool usePointMatching = false;

        if (flags & ARG_1_AND_2_ARE_WORDS)
        {
            if (flags & ARGS_ARE_XY_VALUES)
            {
                dx = (int16)Read2Byte(cursor);
                dy = (int16)Read2Byte(cursor);
            }
            else
            {
                basePointIdx      = Read2Byte(cursor);
                componentPointIdx = Read2Byte(cursor);
                usePointMatching  = true;
            }
        }
        else
        {
            if (flags & ARGS_ARE_XY_VALUES)
            {
                dx = (int8)ReadByte(cursor);
                dy = (int8)ReadByte(cursor);
            }
            else
            {
                basePointIdx      = ReadByte(cursor);
                componentPointIdx = ReadByte(cursor);
                usePointMatching  = true;
            }
        }

        // Read transform (scale/rotation matrix)
        float xx = 1.0f, yx = 0.0f, xy = 0.0f, yy = 1.0f;

        if (flags & WE_HAVE_A_SCALE)
        {
            xx = yy = (int16)Read2Byte(cursor) / 16384.0f;
        }
        else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
        {
            xx = (int16)Read2Byte(cursor) / 16384.0f;
            yy = (int16)Read2Byte(cursor) / 16384.0f;
        }
        else if (flags & WE_HAVE_A_TWO_BY_TWO)
        {
            xx = (int16)Read2Byte(cursor) / 16384.0f;
            yx = (int16)Read2Byte(cursor) / 16384.0f;
            xy = (int16)Read2Byte(cursor) / 16384.0f;
            yy = (int16)Read2Byte(cursor) / 16384.0f;
        }

        if (m_glyphs.find(glyphIndex) == m_glyphs.end())
        {
            uint32 savedCursor = cursor;
            uint32 offset = m_tables["glyf"].offset;
            cursor = offset + m_loca[glyphIndex];
            
            int16 componentContourCount = (int16)Read2Byte(cursor);
            assert(componentContourCount <= 0 && "Non compound glyph not loaded");
            if (componentContourCount < 0)
            {
                SkipBytes(8);
                Glyph compoundComponent;
                ReadCompoundGlyph(compoundComponent, glyphIndex);
            }
            cursor = savedCursor;
        }
        
        if (m_glyphs.find(glyphIndex) != m_glyphs.end())
        {
            Glyph& component = m_glyphs[glyphIndex];
            uint16 pointOffset = (uint16)_g.points.size();

            // Resolve point matching offset
            if (usePointMatching)
            {
                if (basePointIdx < _g.points.size() && componentPointIdx < component.points.size())
                {
                    GlyphPoint& basePoint = _g.points[basePointIdx];
                    GlyphPoint& compPoint = component.points[componentPointIdx];
                    dx = basePoint.x - compPoint.x;
                    dy = basePoint.y - compPoint.y;
                }
            }

            for (auto& p : component.points)
            {
                GlyphPoint np;
                np.onCurve = p.onCurve;
                np.x = (int16)(xx * p.x + xy * p.y + dx);
                np.y = (int16)(yx * p.x + yy * p.y + dy);
                _g.points.push_back(np);
                _g.flags.push_back(p.onCurve ? ON_CURVE : 0);
            }

            for (auto endPt : component.endPtsContours)
                _g.endPtsContours.push_back(endPt + pointOffset);

            _g.contourCount += component.contourCount;
        }

        hasMoreComponents = (flags & MORE_COMPONENTS) != 0;
    }
    
    RemapGlyph(_g);
    m_glyphs[_index] = _g;
}

uint8 FontLoader::ReadByte(uint32 _offset)
{
    assert(_offset + 1 <= bufferSize && "Offset out of buffer range");
    
    char* start = buffer.data() + _offset;

    uint8 result;
    memcpy(&result, start, sizeof(result));
    SkipBytes(1);
    
    return result;
}

uint16 FontLoader::Read2Byte(uint32 _offset)
{
    assert(_offset + 2 <= bufferSize && "Offset out of buffer range");

    char* start = buffer.data() + _offset;
    
    uint16 result;
    memcpy(&result, start, sizeof(result));

    result = (result >> 8) | (result << 8);
    SkipBytes(2);
    
    return result;
}

uint32 FontLoader::Read4Byte(uint32 _offset)
{
    assert(_offset + 4 <= bufferSize && "Offset out of buffer range");

    char* start = buffer.data() + _offset;
    
    uint32 result;
    memcpy(&result, start, sizeof(result));

    result =  ((result >> 24) & 0x000000FF)
            | ((result >> 8)  & 0x0000FF00)
            | ((result << 8)  & 0x00FF0000)
            | ((result << 24) & 0xFF000000);

    SkipBytes(4);

    return result;
}

uint64 FontLoader::Read8Byte(uint32 _offset)
{
    assert(_offset + 8 <= bufferSize && "Offset out of buffer range");

    char* start = buffer.data() + _offset;
    
    uint64 result;
    memcpy(&result, start, sizeof(result));

    result =  ((result >> 56)  & 0x00000000000000FF)
            | ((result >> 40)  & 0x000000000000FF00)
            | ((result >> 24)  & 0x0000000000FF0000)
            | ((result >> 8)   & 0x00000000FF000000)
            | ((result << 8)   & 0x000000FF00000000)
            | ((result << 24)  & 0x0000FF0000000000)
            | ((result << 40)  & 0x00FF000000000000)
            | ((result << 56)  & 0xFF00000000000000);

    SkipBytes(8);

    return result;
}


