#ifndef FONT_TABLES_HPP_DEFINED
#define FONT_TABLES_HPP_DEFINED

struct FontBoundingBox
{
    int16 xMin, yMin, xMax, yMax;
};

struct Header
{
    uint32 version = 0;
    uint16 tableCount = 0;
    uint16 searchRange = 0;
    uint16 entrySelector = 0;
    uint16 rangeShift = 0;
};

struct Table
{
    String name;
    uint32 checksum = 0;
    uint32 offset = 0;
    uint32 length = 0;
};

struct HeadTable
{
    uint32 magicNumber = 0;
    uint16 unitsPerEm = 0;
    uint16 indexToLocFormat = 0;
    FontBoundingBox boundingBox;
};

struct MaxpTable
{
    uint32 version = 0;
    uint16 glyphCount = 0;
};

struct CmapTable
{
    uint16 format = 0;
    uint16 length = 0;
    uint16 language = 0;
    uint16 segCount = 0;
    uint16 searchRange = 0;
    uint16 entrySelector = 0;
    uint16 rangeShift = 0;
    
    Vector<uint16> endCodes;
    Vector<uint16> startCodes;
    
    Vector<int16> idDelta;
    Vector<uint16> idRangeOffset;

    Vector<uint16> glyphIdArray;
};

struct HheaTable
{
    int16  ascender;
    int16  descender;
    int16  lineGap;
    uint16 advanceWidthMax;
    int16  minLeftSideBearing;
    int16  minRightSideBearing;
    int16  xMaxExtent;
    int16  caretSlopeRise;
    int16  caretSlopeRun;
    int16  caretOffset;
    int16  metricDataFormat;
    uint16 numberOfHMetrics;
};

struct GlyphMetrics
{
    uint16 advanceWidth;
    int16  leftSideBearing;
};

struct GlyphPoint
{
    int16 x, y;
    bool onCurve = false;
};

struct CharacterPoint
{
    float x, y;
    bool onCurve = false;
};

enum GlyphFlags : uint8
{
    ON_CURVE =           0b00000001,
    X_SHORT =            0b00000010,
    Y_SHORT =            0b00000100,
    REPEAT =             0b00001000,
    X_SAME_OR_POSITIVE = 0b00010000,
    Y_SAME_OR_POSITIVE = 0b00100000,
};

enum CompoundGlyphFlags
{
    ARG_1_AND_2_ARE_WORDS    = 0b00000001,
    ARGS_ARE_XY_VALUES       = 0b00000010,
    WE_HAVE_A_SCALE          = 0b00001000,
    MORE_COMPONENTS          = 0b00100000,
    WE_HAVE_AN_X_AND_Y_SCALE = 0b01000000,
    WE_HAVE_A_TWO_BY_TWO     = 0b10000000,
};

struct Glyph
{
    int16 contourCount = 0;
    FontBoundingBox boundingBox;
    
    Vector<uint16> endPtsContours;
    Vector<uint8> flags;
    Vector<GlyphPoint> points;
};

struct Line
{
    int16 x1, x2, y1, y2;
};

struct Contour
{
    Vector<Line> lines;
    bool isClockwise = true;
};

struct Character
{
    Vector<CharacterPoint> points;
    Vector<Line> lines;

    int16  leftSideBearing  = 0;
    uint16 advanceWidth     = 0;

    // Atlas info
    uint16 atlasX           = 0;
    uint16 atlasY           = 0;
    uint16 atlasWidth       = 0;
    uint16 atlasHeight      = 0;

    float minX;
    float minY;
    float maxX;
    float maxY;
};

struct CharData
{
    float minX, minY, maxX, maxY;
    
    float penX, penY;
    float spread;
    float smoothing;
    
    uint32 lineCount;
    uint32 canvasWidth;
    uint32 canvasHeight;
    float _pad;
};

#endif