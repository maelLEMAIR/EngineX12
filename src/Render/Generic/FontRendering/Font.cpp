#ifndef FONT_CPP_DEFINED
#define FONT_CPP_DEFINED

#include "Font.h"
#include "FontLoader.h"

void RenderFont::Load(WString const& _name, float _fontSize)
{
    m_fontSize = _fontSize;
    m_fontLoader = new FontLoader();
    m_fontLoader->Load(_name);

    m_unitsPerEm = m_fontLoader->m_head.unitsPerEm;
    m_scale = m_fontSize / (float)m_unitsPerEm;
    m_ascender = (float)(m_fontLoader->m_hhea.ascender) * m_scale;
    m_descender = (float)m_fontLoader->m_hhea.descender * m_scale;
    
    m_glyphs = m_fontLoader->GetGlyphs();

    CreateCharacters(3);

    Bake();

    delete m_fontLoader;
}

void RenderFont::CreateCharacters(int _bezierPrecision)
{
    UnorderedMap<uint16, Character> processedGlyphs;
    float scale = m_fontSize / (float)m_unitsPerEm;
    
    for (auto& [codepoint, glyphIndex] : m_fontLoader->m_codepointToGlyph)
    {
        if (m_glyphs.find(glyphIndex) == m_glyphs.end())
        {
            m_characters[codepoint] = m_characters[0];
            continue;
        }
        
        if (processedGlyphs.find(glyphIndex) != processedGlyphs.end())
        {
            m_characters[codepoint] = processedGlyphs[glyphIndex];
            continue;
        }
        
        Glyph& glyph = m_glyphs[glyphIndex];
        Character character;

        character.advanceWidth    = (uint16)(m_fontLoader->m_glyphMetrics[glyphIndex].advanceWidth * m_scale);
        character.leftSideBearing = (uint16)(m_fontLoader->m_glyphMetrics[glyphIndex].leftSideBearing * m_scale);
        character.minX = (float)glyph.boundingBox.xMin * scale;
        character.minY = (float)glyph.boundingBox.yMin * scale;
        character.maxX = (float)glyph.boundingBox.xMax * scale;
        character.maxY = (float)glyph.boundingBox.yMax * scale;
        
        for (size_t i = 0; i < glyph.points.size(); ++i)
        {
            CharacterPoint p;
            p.x       = (float)glyph.points[i].x * scale;
            p.y       = (float)glyph.points[i].y * scale;
            p.onCurve = glyph.points[i].onCurve;
            character.points.push_back(p);
        }

        uint16 contourStart = 0;
        for (int c = 0; c < glyph.contourCount; ++c)
        {
            uint16 contourEnd = glyph.endPtsContours[c];

            int i = contourStart;
            while (i <= (int)contourEnd)
            {
                int next = (i == (int)contourEnd) ? contourStart : i + 1;

                CharacterPoint& curr = character.points[i];
                CharacterPoint& nxt  = character.points[next];

                if (curr.onCurve == 1 && nxt.onCurve == 1)
                {
                    Line line;
                    line.x1 = (int16)curr.x;
                    line.y1 = (int16)curr.y;
                    line.x2 = (int16)nxt.x;
                    line.y2 = (int16)nxt.y;
                    character.lines.push_back(line);
                    i++;
                }
                else if (curr.onCurve == 1 && nxt.onCurve == 0)
                {
                    int afterNext = (next == (int)contourEnd) ? contourStart : next + 1;
                    CharacterPoint& p1 = character.points[afterNext];

                    Vector<CharacterPoint> curvePoints = InterpolateBezier(curr, nxt, p1, _bezierPrecision);

                    CharacterPoint prev = curr;
                    for (auto& cp : curvePoints)
                    {
                        Line line;
                        line.x1 = (int16)prev.x;
                        line.y1 = (int16)prev.y;
                        line.x2 = (int16)cp.x;
                        line.y2 = (int16)cp.y;
                        character.lines.push_back(line);
                        prev = cp;
                    }

                    Line line;
                    line.x1 = (int16)prev.x;
                    line.y1 = (int16)prev.y;
                    line.x2 = (int16)p1.x;
                    line.y2 = (int16)p1.y;
                    character.lines.push_back(line);

                    i += 2;
                }
                else
                {
                    i++;
                }
            }
            
            contourStart = contourEnd + 1;
        }

        m_availableCharacterCodes.push_back(codepoint);
        m_characters[codepoint] = character;
        processedGlyphs[glyphIndex] = character;
    }
}

Vector<CharacterPoint> RenderFont::InterpolateBezier(CharacterPoint _p0, CharacterPoint _control, CharacterPoint _p1, int _steps)
{
    {
        Vector<CharacterPoint> points;
        for (int s = 1; s < _steps; s++)
        {
            float t = (float)s / (float)_steps;
            float u = 1.0f - t;

            CharacterPoint p;
            p.x       = u * u * _p0.x + 2 * u * t * _control.x + t * t * _p1.x;
            p.y       = u * u * _p0.y + 2 * u * t * _control.y + t * t * _p1.y;
            p.onCurve = 1;
            points.push_back(p);
        }
        return points;
    }
}

#endif
