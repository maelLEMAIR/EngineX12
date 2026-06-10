#ifndef TEXT_H_DEFINED
#define TEXT_H_DEFINED

#include "../../Common/Common.h"
#include "Font.h"
#include "../RenderItems/Sprite.h"

class Text
{
public:
    virtual ~Text() = default;

    void SetString(const String& _str)          { m_string = _str;     m_dirty = true; }
    void SetPosition(float _x, float _y)        { m_x = _x; m_y = _y; m_dirty = true; }
    void SetFont(RenderFont* _font)              { m_font = _font;      m_dirty = true; }
    void SetColor(float _r, float _g, float _b) { m_color = XMFLOAT4(_r, _g, _b, 1.0f); m_dirty = true; }

    RenderFont* GetFont() { return m_font; }
    
    const String& GetString()   const { return m_string; }
    Sprite*       GetSprite()   const { return m_sprite; }
    bool          IsDirty()     const { return m_dirty; }

    virtual void Build() = 0;
    virtual void Bind() = 0;

protected:
    String      m_string;
    RenderFont* m_font    = nullptr;
    Sprite*     m_sprite  = nullptr;
    float       m_x       = 0.0f;
    float       m_y       = 0.0f;
    bool        m_dirty   = true;
    XMFLOAT4    m_color   = XMFLOAT4(1, 1, 1, 1);
};

#endif