#ifndef FONTLOADER_H_DEFINED
#define FONTLOADER_H_DEFINED

#include <fstream>

#include "../../Common/Common.h"
#include "FontTables.hpp"

using ifstream = std::ifstream;

class FontLoader 
{
public:
    FontLoader() = default;
    
    void Load(WString const& _path);

    uint16 GetGlyphIndex(uint16 _charCode);

    Map<uint16, Glyph>& GetGlyphs() { return m_glyphs; }
    
private:
    uint8 ReadByte(uint32 _offset);
    uint16 Read2Byte(uint32 _offset);
    uint32 Read4Byte(uint32 _offset);
    uint64 Read8Byte(uint32 _offset);

    void SkipBytes(uint32 _byteCount) { cursor += _byteCount; }
    
    void ReadHeader();
    void ReadTableDirectory();
    void ReadTableDirectoryElement();
    void ReadHeadTable();
    void ReadMaxpTable();
    void ReadCmapTable();
    void ReadLocaTable();
    void ReadGlyphTable();
    void ReadCompoundGlyph(Glyph& _glyph, uint16 _index);
    void ReadHheaTable();
    void ReadHmtxTable();

    void BuildCodePointTable();
    void RemapGlyph(Glyph& _glyph);
    
    ifstream file;

    Vector<char> buffer;
    uint64 bufferSize = 0;
    uint32 cursor = 0;

    Header m_header;
    UnorderedMap<String, Table> m_tables;
    HeadTable m_head;
    MaxpTable m_maxp;
    CmapTable m_cmap;
    HheaTable m_hhea;
    Vector<uint32> m_loca;
    
    Map<uint16, uint16> m_codepointToGlyph;
    Map<uint16, Glyph> m_glyphs;
    Map<uint16, GlyphMetrics> m_glyphMetrics;
    
    friend class RenderFont;
};

#endif
