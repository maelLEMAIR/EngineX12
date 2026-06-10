#ifndef RENDER_H_DEFINED
#define RENDER_H_DEFINED

#include "../Common/Common.h"

#include "Base/Window.h"
#include "Base/Camera.h"
#include "Base/DepthStencil.h"
#include "Base/Device.h"
#include "Base/RenderTarget.h"
#include "Base/RenderTexture.h"
#include "Base/SwapChain.h"

#include "RenderItems/Geometry.h"
#include "RenderItems/Sprite.h"

#include "Shader-Mat/ComputeShader.h"
#include "Shader-Mat/Lights.hpp"
#include "Shader-Mat/Material.h"
#include "Shader-Mat/Texture.h"

#include "Factories/ComputeShaderFactory.hpp"
#include "Factories/GeometryFactory.hpp"
#include "Factories/ShaderFactory.hpp"
#include "Factories/SpriteFactory.hpp"

#include "FontRendering/Font.h"
#include "FontRendering/Text.hpp"

#endif
