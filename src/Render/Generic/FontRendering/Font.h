#ifndef FONT_H_DEFINED
#define FONT_H_DEFINED

#include "../../Common/Common.h"

#include "FontTables.hpp"

class RenderTarget;
class FontLoader;

class RenderFont
{
public:
	RenderFont() = default;
	RenderFont(RenderFont&& other) = delete;

	virtual ~RenderFont() {};

	void Load(WString const& _name, float _fontSize = 1.0f);

	float GetLineHeight() const { return m_lineHeight; };
	float GetAscender() const { return m_ascender; };
	float GetDescender() const { return m_descender; };
	float GetScale() const { return m_scale; };
	float GetFontSize() const { return m_fontSize; };

	Character* GetCharacter(uint16 _characterCode) { return &m_characters[_characterCode]; };
	
	virtual RenderTarget* GetAtlas() = 0;
protected:
	FontLoader* m_fontLoader = nullptr;

	uint16 m_unitsPerEm = 0;
	float m_fontSize = 1.0f;
	float m_scale = 1.0f;
	float m_lineHeight = 0.0f;
	float m_ascender = 0.0f;
	float m_descender = 0.0f;

	Vector<uint16> m_availableCharacterCodes;
	Map<uint16, Character> m_characters;
	Map<uint16, Glyph> m_glyphs;
	
	void CreateCharacters(int _bezierPrecision);
	Vector<CharacterPoint> InterpolateBezier(CharacterPoint _p0, CharacterPoint _control, CharacterPoint _p1, int _steps);
	
	virtual void Bake() = 0;
};

#endif