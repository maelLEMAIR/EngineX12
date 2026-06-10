#ifndef DEPTH_STENCIL_H_DEFINED
#define DEPTH_STENCIL_H_DEFINED

class DepthStencil
{
public:
    virtual ~DepthStencil() = default;

    virtual void Resize(int _width, int _height) = 0;
    virtual void Clear() = 0;
    
protected:
    int m_width = 0;
    int m_height = 0;

    friend class Device;
};
#endif
