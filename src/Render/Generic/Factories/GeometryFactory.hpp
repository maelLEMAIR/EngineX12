#ifndef GEOMETRY_FACTORY_HPP_INCLUDED
#define GEOMETRY_FACTORY_HPP_INCLUDED

#include "../../Common/Common.h"
#include "../RenderItems/Geometry.h"

#include <fstream>
#include "Core/nlohmann.hpp"
using json = nlohmann::json;

struct GeometryFactory
{
private:
	static Vector<Vertex> CalculateNormalsAndTangentsSeamless(Vector<Vertex>& _vertices, Vector<uint32>& _indices)
	 {
	 	struct AccumData
	     {
	 	    XMFLOAT3 normal = {0.0f, 0.0f, 0.0f };
			XMFLOAT3 tangent = {0.0f, 0.0f, 0.0f };
	     };
	
	 	static constexpr float MERGE_EPSILON = 1e-5f;
	 	
	 	struct XMFLOAT3Hash {
	 		size_t operator()(const XMFLOAT3& v) const {
	 			auto quantize = [](float f) { return static_cast<int>(std::round(f / MERGE_EPSILON)); };
	 			size_t h1 = std::hash<int>{}(quantize(v.x));
	 			size_t h2 = std::hash<int>{}(quantize(v.y));
	 			size_t h3 = std::hash<int>{}(quantize(v.z));
	 			return h1 ^ (h2 << 16) ^ (h3 << 32);
	 		}
	 	};
	
	 	struct XMFLOAT3Eq
	 	{
	 		bool operator()(const XMFLOAT3& a, const XMFLOAT3& b) const {
	 			return std::abs(a.x - b.x) <= MERGE_EPSILON &&
	 				   std::abs(a.y - b.y) <= MERGE_EPSILON &&
	 				   std::abs(a.z - b.z) <= MERGE_EPSILON;
	 		}
	 	};
	
	 	UnorderedMap<XMFLOAT3, AccumData, XMFLOAT3Hash, XMFLOAT3Eq> accum;
	
	 	uint32 numTriangle = (uint32)_indices.size() / 3;
	
	 	for (uint32 i = 0; i < numTriangle; i++)
	 	{
	 	    uint32 i0 = _indices[i * 3 + 0];
	 	    uint32 i1 = _indices[i * 3 + 1];
	 	    uint32 i2 = _indices[i * 3 + 2];
	
	 	    Vertex& vertex0 = _vertices[i0];
	 	    Vertex& vertex1 = _vertices[i1];
	 	    Vertex& vertex2 = _vertices[i2];
	
	 	    XMVECTOR e0 = XMVectorSubtract(XMLoadFloat3(&vertex1.position), XMLoadFloat3(&vertex0.position));
	 	    XMVECTOR e1 = XMVectorSubtract(XMLoadFloat3(&vertex2.position), XMLoadFloat3(&vertex0.position));
	
	 	    XMVECTOR normal = XMVector3Cross(e0, e1);
	
	 	    XMFLOAT2 deltaUV1 = { vertex1.uv.x - vertex0.uv.x, vertex1.uv.y - vertex0.uv.y };
	 	    XMFLOAT2 deltaUV2 = { vertex2.uv.x - vertex0.uv.x, vertex2.uv.y - vertex0.uv.y };
	
	 	    XMVECTOR tangent = XMVectorZero();
	 	    float denom = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	 	    if (std::abs(denom) >= 1e-6f)
	 	    {
	 	        float f = 1.0f / denom;
	 	        XMFLOAT3 t;
	 	        t.x = f * (deltaUV2.y * XMVectorGetX(e0) - deltaUV1.y * XMVectorGetX(e1));
	 	        t.y = f * (deltaUV2.y * XMVectorGetY(e0) - deltaUV1.y * XMVectorGetY(e1));
	 	        t.z = f * (deltaUV2.y * XMVectorGetZ(e0) - deltaUV1.y * XMVectorGetZ(e1));
	 	        tangent = XMLoadFloat3(&t);
	 	    }
	
	 	    // Accumulate directly into the map for all 3 vertices
	 	    for (uint32 idx : { i0, i1, i2 })
	 	    {
	 	        AccumData& data = accum[_vertices[idx].position];
	 	        XMStoreFloat3(&data.normal,  XMVectorAdd(XMLoadFloat3(&data.normal),  normal));
	 	        XMStoreFloat3(&data.tangent, XMVectorAdd(XMLoadFloat3(&data.tangent), tangent));
	 	    }
	 	}
	
	 	for (uint32 i = 0; i < _vertices.size(); i++)
	 	{
	 	    AccumData& data = accum[_vertices[i].position];
	
	 	    XMVECTOR N = XMVector3Normalize(XMLoadFloat3(&data.normal));
	 	    XMVECTOR T = XMLoadFloat3(&data.tangent);
	
	 	    T = XMVector3Normalize(XMVectorSubtract(T, XMVectorMultiply(XMVector3Dot(T, N), N)));
	
	 	    XMStoreFloat3(&_vertices[i].normal,  N);
	 	    XMStoreFloat3(&_vertices[i].tangent, T);
	 	}
	
	 	return _vertices;
	 }

	static Vector<Vertex> CalculateNormalsAndTangents(Vector<Vertex>& _vertices, const Vector<uint32>& _indices)
	{
		// 1. Utiliser un vecteur simple au lieu d'une map (beaucoup plus rapide)
		struct Accum { XMVECTOR n = XMVectorZero(); XMVECTOR t = XMVectorZero(); };
		std::vector<Accum> accum(_vertices.size());
	
		// 2. Premier passage : Accumulation par face
		for (size_t i = 0; i < _indices.size(); i += 3)
		{
			uint32 i0 = _indices[i], i1 = _indices[i+1], i2 = _indices[i+2];
			Vertex& v0 = _vertices[i0]; Vertex& v1 = _vertices[i1]; Vertex& v2 = _vertices[i2];
	
			XMVECTOR p0 = XMLoadFloat3(&v0.position), p1 = XMLoadFloat3(&v1.position), p2 = XMLoadFloat3(&v2.position);
			XMVECTOR e1 = p1 - p0, e2 = p2 - p0;
	
			// Normale de la face (non normalisée pour pondérer par l'aire de la face)
			XMVECTOR faceNormal = XMVector3Cross(e1, e2);
	
			// Tangente
			float du1 = v1.uv.x - v0.uv.x, dv1 = v1.uv.y - v0.uv.y;
			float du2 = v2.uv.x - v0.uv.x, dv2 = v2.uv.y - v0.uv.y;
			float f = 1.0f / (du1 * dv2 - du2 * dv1);
	
			XMVECTOR faceTangent = f * (dv2 * e1 - dv1 * e2);
	
			for (uint32 idx : {i0, i1, i2}) {
				accum[idx].n += faceNormal;
				accum[idx].t += faceTangent;
			}
		}
	
		// 3. Second passage : Orthogonalisation et stockage
		for (size_t i = 0; i < _vertices.size(); i++)
		{
			XMVECTOR N = XMVector3Normalize(accum[i].n);
			XMVECTOR T = accum[i].t;
	
			// Gram-Schmidt : T' = Normalize(T - (N.T) * N)
			T = XMVector3Normalize(T - XMVector3Dot(T, N) * N);
	
			XMStoreFloat3(&_vertices[i].normal, N);
			// On stocke souvent la tangente en float4 (x, y, z, w) pour la bitangente
			XMStoreFloat3(&_vertices[i].tangent, T); 
		}
	
		return _vertices;
	}

	static XMFLOAT3 Normalize(const XMFLOAT3& v, float radius)
	{
		XMVECTOR vec = XMLoadFloat3(&v);
		vec = XMVector3Normalize(vec) * radius;

		XMFLOAT3 out;
		XMStoreFloat3(&out, vec);
		return out;
	}

	static Vertex MidPoint(Vertex const& v0, Vertex const& v1)
	{

		XMFLOAT3 pos;
		pos.x = ( v0.position.x + v1.position.x ) * 0.5f;
		pos.y = ( v0.position.y + v1.position.y ) * 0.5f;
		pos.z = ( v0.position.z + v1.position.z ) * 0.5f;

		XMFLOAT2 uv;
		uv.x = (v0.uv.x + v1.uv.x) * 0.5f;
		uv.y = (v0.uv.y + v1.uv.y) * 0.5f;

		Vertex v = { pos, uv };

		return v;
	}

	static void Subdivide(Vector<uint32>& indices, Vector<Vertex>& vertices)
	{
		Vector<uint32> indicesCopy = indices;
		Vector<Vertex> verticesCopy = vertices;

		vertices.resize(0);
		indices.resize(0);

		uint32 numTris = static_cast<uint32>(indicesCopy.size()) / 3;
		for (uint32 i = 0; i < numTris; ++i)
		{
			Vertex v0 = verticesCopy[indicesCopy[i * 3 + 0]];
			Vertex v1 = verticesCopy[indicesCopy[i * 3 + 1]];
			Vertex v2 = verticesCopy[indicesCopy[i * 3 + 2]];

			Vertex m0 = MidPoint(v0, v1);
			Vertex m1 = MidPoint(v1, v2);
			Vertex m2 = MidPoint(v0, v2);
			
			vertices.push_back(v0); // 0
			vertices.push_back(v1); // 1
			vertices.push_back(v2); // 2
			vertices.push_back(m0); // 3
			vertices.push_back(m1); // 4
			vertices.push_back(m2); // 5

			indices.push_back(i * 6 + 0);
			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 5);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 2);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 1);
			indices.push_back(i * 6 + 4);
		}
	}

	static void Circle(Vector<Vertex>& vertices, Vector<uint32>& indices, int _stackCount, float _posY, bool _isTop)
	{
		vertices.push_back(
			Vertex{ XMFLOAT3(0.0f, _posY, 0.0f), XMFLOAT2(0.5f, 0.5f) }
		);

		int indexCenter = (UINT)vertices.size() - 1;
		float angle = XM_PI / (float)_stackCount * 2.0f;
    	
		for (int i = 0; i < _stackCount + 1; ++i)
		{
			float c = cosf(angle * (float)i);
			float s = sinf(angle * (float)i);
			float posX = c * 0.5f;
			float posZ = s * 0.5f;

			XMFLOAT2 uv = { 0.5f + c * 0.5f, 0.5f + s * 0.5f };
			vertices.push_back(Vertex{ XMFLOAT3(posX, _posY, posZ), uv });
		}
		for (int i = indexCenter; i < indexCenter + _stackCount; ++i)
		{
			indices.push_back(indexCenter);
			if (_isTop)
			{
				indices.push_back(i + 2);
				indices.push_back(i + 1);
			}
			else
			{
				indices.push_back(i + 1);
				indices.push_back(i + 2);
			}
		}
	}
public:
	////////////////////////////////////////////
	//// PYRAMID
	////////////////////////////////////////////
	
    static Geometry* BuildPyramid(Device* _pDevice, bool _isDynamic = false)
    {
    	Geometry* pGeometry = _pDevice->CreateGeometry(_isDynamic);

    	Vector<Vertex> vertices;
    	Vector<uint32> indices;


    	vertices = {
    		// Front face
    		Vertex{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},

    		// Right Face
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
    		Vertex{XMFLOAT3( 0.5f, -0.5f, 00.5f), XMFLOAT2(1.0f, 1.0f)},

    		// Back Face
    		Vertex{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( -0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f)},

			// Left Face
    		Vertex{XMFLOAT3( -0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( -0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
    		
			Vertex{XMFLOAT3( 0.0f,  0.5f,  0.0f), XMFLOAT2(0.5f, 0.0f)},

    		// Bottom Face
    		Vertex{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
    		Vertex{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3( -0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f)}
		};
    	indices = {
    		9, 10, 11,
    		9, 11, 12,
    		
			0, 8, 1, // Front Face
			2, 8, 3, // Right Face
    		4, 8, 5, // Back Face
    		6, 8, 7, // Left Face
		};

    	vertices = CalculateNormalsAndTangents(vertices, indices);

    	pGeometry->SetVertexData(vertices.data(), vertices.size());
    	pGeometry->SetIndexData(indices.data(), indices.size());

    	return pGeometry;
    }

	////////////////////////////////////////////
	//// CUBE
	////////////////////////////////////////////
	
	static Geometry* BuildCube(Device* pDevice, bool _isDynamic = false)
    {
		Geometry* pGeometry = pDevice->CreateGeometry(_isDynamic);

    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	vertices = {
    		// DOWN  (y = -0.5)
    		Vertex{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f)},
			// UP    (y = +0.5)
			Vertex{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
			Vertex{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f)},
			// LEFT  (x = -0.5)
			Vertex{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
			Vertex{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f)},
			// RIGHT (x = +0.5)
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f)},
			// FRONT (z = -0.5)
			Vertex{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
			// BACK  (z = +0.5)
			Vertex{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f)},
			Vertex{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f)},
		};

    	indices = {
    		// DOWN
    		0,  2,  1,   3,  1,  2,
		   // UP
			4,  5,  6,   7,  6,  5,
		   // LEFT
			8,  9, 10,   9, 11, 10,
		   // RIGHT
		   12, 14, 13,  13, 14, 15,
		   // FRONT
		   16, 18, 17,  17, 18, 19,
		   // BACK
		   20, 21, 22,  21, 23, 22,
	   };

    	vertices = CalculateNormalsAndTangents(vertices, indices);

    	pGeometry->SetVertexData(vertices.data(), vertices.size());
		pGeometry->SetIndexData(indices.data(), indices.size());

		return pGeometry;
    }

	////////////////////////////////////////////
	//// TRIANGLE
	////////////////////////////////////////////
	
	static Geometry* BuildTriangle(Device* _pDevice, bool _isDynamic = false)
    {
		Geometry* pGeometry = _pDevice->CreateGeometry(_isDynamic);

    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	vertices = {
    		Vertex{XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
			Vertex{XMFLOAT3( 0.0f,  0.5f, 0.0f), XMFLOAT2(0.5f, 0.0f)},
		};
    	indices = { 0, 2, 1 };

    	vertices = CalculateNormalsAndTangents(vertices, indices);

		pGeometry->SetVertexData(vertices.data(), vertices.size());
		pGeometry->SetIndexData(indices.data(), indices.size());

    	return pGeometry;
    }

    ////////////////////////////////////////////
	//// ICOSPHERE
	////////////////////////////////////////////
	
	static Geometry* BuildIcosphere(Device* _pDevice, int _subdivisions, bool _isDynamic = false)
    {
		Geometry* pGeometry = _pDevice->CreateGeometry(_isDynamic);

    	if (_subdivisions >= 6)
    		_subdivisions = 6;

    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	float phi = (0.5f + sqrtf(5.0f)) * 0.5f;

    	vertices = {
    		Vertex{ XMFLOAT3(-0.5f,    0,  phi), XMFLOAT2() },
			Vertex{ XMFLOAT3(0.5f,    0,  phi), XMFLOAT2() },
			Vertex{ XMFLOAT3(-0.5f,    0, -phi), XMFLOAT2() },
			Vertex{ XMFLOAT3(0.5f,    0, -phi), XMFLOAT2() },
			Vertex{ XMFLOAT3( 0,  phi,    0.5f), XMFLOAT2() },
			Vertex{ XMFLOAT3( 0,  phi,   -0.5f), XMFLOAT2() },
			Vertex{ XMFLOAT3( 0, -phi,    0.5f), XMFLOAT2() },
			Vertex{ XMFLOAT3( 0, -phi,   -0.5f), XMFLOAT2() },
			Vertex{ XMFLOAT3( phi,  0.5f,    0), XMFLOAT2() },
			Vertex{ XMFLOAT3(-phi,  0.5f,    0), XMFLOAT2() },
			Vertex{ XMFLOAT3( phi, -0.5f,    0), XMFLOAT2() },
			Vertex{ XMFLOAT3(-phi, -0.5f,    0), XMFLOAT2() },
		};
    	indices = {
    		1, 10, 8,   1,  0,  6,   1,  6, 10,   1,  4, 0,   1, 8,  4,
			2, 11, 9,   2,  7, 11,   2,  3,  7,   2,  9, 5,   2, 5,  3,
			3,  5, 8,   3,  8, 10,   3, 10,  7,
			4,  8, 5,   4,  5,  9,   4,  9,  0,
			6, 11, 7,   6,  7, 10,   6,  0, 11,
			11, 0, 9
		};

    	for (int i = 0; i < _subdivisions; ++i)
    		Subdivide(indices, vertices);

    	for (uint32 i = 0; i < (uint32)vertices.size(); ++i)
    	{
    		XMFLOAT3 n = Normalize(vertices[i].position, 0.5f);
    		vertices[i].position = { n.x, n.y, n.z };
    		
    		float u = 0.5f + atan2f(n.z, n.x) / XM_2PI;
    		float v = 0.5f - asinf(n.y) / XM_PI;
    		vertices[i].uv = XMFLOAT2(u, v);
    	}

    	for (uint32 i = 0; i < (uint32)indices.size(); i += 3)
    	{
    		uint32 i0 = indices[i];
    		uint32 i1 = indices[i + 1];
    		uint32 i2 = indices[i + 2];

    		float u0 = vertices[i0].uv.x;
    		float u1 = vertices[i1].uv.x;
    		float u2 = vertices[i2].uv.x;

    		if (abs(u0 - u1) > 0.5f || abs(u1 - u2) > 0.5f || abs(u0 - u2) > 0.5f)
    		{
    			if (u0 < 0.5f)
    			{
    				vertices.push_back(vertices[i0]);
    				vertices.back().uv.x += 1.0f;
    				indices[i] = (uint32)vertices.size() - 1;
    			}
    			if (u1 < 0.5f)
    			{
    				vertices.push_back(vertices[i1]);
    				vertices.back().uv.x += 1.0f;
    				indices[i + 1] = (uint32)vertices.size() - 1;
    			}
    			if (u2 < 0.5f)
    			{
    				vertices.push_back(vertices[i2]);
    				vertices.back().uv.x += 1.0f;
    				indices[i + 2] = (uint32)vertices.size() - 1;
    			}
    		}
    	}

    	// Step 3: fix pole vertices
    	for (uint32 i = 0; i < (uint32)indices.size(); i += 3)
    	{
    		uint32 i0 = indices[i];
    		uint32 i1 = indices[i + 1];
    		uint32 i2 = indices[i + 2];

    		auto fixPole = [&](uint32 poleIdx, uint32 a, uint32 b) -> uint32
    		{
    			float y = vertices[poleIdx].position.y;
    			if (abs(y - 1.0f) < 0.001f || abs(y + 1.0f) < 0.001f)
    			{
    				vertices.push_back(vertices[poleIdx]);
    				vertices.back().uv.x = (vertices[a].uv.x + vertices[b].uv.x) * 0.5f;
    				return (uint32)vertices.size() - 1;
    			}
    			return poleIdx;
    		};

    		indices[i]     = fixPole(i0, i1, i2);
    		indices[i + 1] = fixPole(i1, i0, i2);
    		indices[i + 2] = fixPole(i2, i0, i1);
    	}
    	
    	vertices = CalculateNormalsAndTangentsSeamless(vertices, indices);

		pGeometry->SetVertexData(vertices.data(), vertices.size());
		pGeometry->SetIndexData(indices.data(), indices.size());

		return pGeometry;
    }

	////////////////////////////////////////////
	//// UV SPHERE
	////////////////////////////////////////////

	static Geometry* BuildUVSphere(Device* _pDevice, uint8 _sliceCount, uint8 _stackCount, bool _isDynamic = false)
	{
	    Vector<Vertex> vertices;
	    Vector<uint32> indices;

	    // top pole
	    vertices.push_back(Vertex{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.5f, 0.0f) });

	    float phiStep   = XM_PI / _stackCount;
	    float thetaStep = XM_2PI / _sliceCount;

	    for (uint32 i = 1; i < _stackCount; ++i)
	    {
	        float phi = i * phiStep;
	        for (uint32 j = 0; j <= _sliceCount; ++j)
	        {
	            float theta = j * thetaStep;
	            Vertex v;
	            v.position = XMFLOAT3(
	                sinf(phi) * cosf(theta),
	                cosf(phi),
	                sinf(phi) * sinf(theta)
	            );
	            v.normal = XMFLOAT3();
	        	v.uv     = XMFLOAT2(
					(float)j / (float)_sliceCount,
					(float)i / (float)_stackCount
				);
	            vertices.push_back(v);
	        }
	    }

	    // bottom pole
	    vertices.push_back(Vertex{ XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.5f, 1.0f) });

	    // top cap - connect pole to first ring
	    for (uint32 i = 1; i <= _sliceCount; ++i)
	    {
	        indices.push_back(0);
	        indices.push_back(i + 1);
	        indices.push_back(i);
	    }

	    // rings
	    uint32 baseIndex = 1;
	    uint32 ringVertexCount = _sliceCount + 1;
	    for (uint32 i = 0; i < (uint32)_stackCount - 2; ++i)
	    {
	        for (uint32 j = 0; j < _sliceCount; ++j)
	        {
	            indices.push_back(baseIndex + i * ringVertexCount + j);
	            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
	            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

	            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
	            indices.push_back(baseIndex + i * ringVertexCount + j + 1);
	            indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
	        }
	    }

	    // bottom cap - connect last ring to pole
	    uint32 southPoleIndex = (uint32)vertices.size() - 1;
	    baseIndex = southPoleIndex - ringVertexCount;
	    for (uint32 i = 0; i < _sliceCount; ++i)
	    {
	        indices.push_back(southPoleIndex);
	        indices.push_back(baseIndex + i);
	        indices.push_back(baseIndex + i + 1);
	    }

	    vertices = CalculateNormalsAndTangentsSeamless(vertices, indices);

    	Geometry* geo = _pDevice->CreateGeometry(_isDynamic);
	    geo->SetVertexData(vertices.data(), vertices.size());
	    geo->SetIndexData(indices.data(), indices.size());
    	return geo;
	}
	
	////////////////////////////////////////////
	//// CIRCLE
	////////////////////////////////////////////
	
	static Geometry* BuildCircle(Device* _pDevice, int _stackCount, float _posY, bool _isTop, bool _isDynamic = false)
    {
    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	Circle(vertices, indices, _stackCount, _posY, _isTop);

    	if (_isTop == false)
    		std::reverse(indices.begin(), indices.end());

    	vertices = CalculateNormalsAndTangentsSeamless(vertices, indices);

		Geometry* geo = _pDevice->CreateGeometry(_isDynamic);
    	
    	geo->SetVertexData(vertices.data(), vertices.size());
    	geo->SetIndexData(indices.data(), indices.size());

    	return geo;
    }

	////////////////////////////////////////////
	//// Cylinder
	////////////////////////////////////////////
	
	static Geometry* BuildCylinder(Device* _pDevice, int _stackCount, bool _isDynamic = false)
    {
    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	Circle(vertices, indices, _stackCount, -0.5f, false);
    	Circle(vertices, indices, _stackCount,  0.5f, true);

    	int sideBaseVertex = (int)vertices.size();
    	float angle = XM_PI / (float)_stackCount * 2.0f;
    	for (int i = 0; i <= _stackCount; ++i)
    	{
    		float u  = (float)i / (float)_stackCount;
    		float c  = cosf(angle * (float)i);
    		float s  = sinf(angle * (float)i);

    		vertices.push_back(Vertex{ XMFLOAT3(c * 0.5f, -0.5f, s * 0.5f), XMFLOAT2(u, 1.0f) });
    		vertices.push_back(Vertex{ XMFLOAT3(c * 0.5f,  0.5f, s * 0.5f), XMFLOAT2(u, 0.0f) });
    	}

    	for (int i = 0; i < _stackCount; ++i)
    	{
    		int b  = sideBaseVertex + i * 2;
    		// bottom-left, top-left, bottom-right
    		indices.push_back(b + 0);
    		indices.push_back(b + 1);
    		indices.push_back(b + 2);
    		// top-left, top-right, bottom-right
    		indices.push_back(b + 1);
    		indices.push_back(b + 3);
    		indices.push_back(b + 2);
    	}

    	vertices = CalculateNormalsAndTangents(vertices, indices);

		Geometry* geo = _pDevice->CreateGeometry(_isDynamic);
    	
    	geo->SetVertexData(vertices.data(), vertices.size());
    	geo->SetIndexData(indices.data(), indices.size());

    	return geo;
    }

	////////////////////////////////////////////
	//// Donut
	////////////////////////////////////////////
	
	static Geometry* BuildDonut(Device* _pDevice, float _majorRadius, float _minorRadius, uint32 _majorSlices, uint32 _minorSlices, bool _isDynamic = false)
    {
	    Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	for (uint32 i = 0; i <= _majorSlices; ++i)
    	{
			float u = (float)i / (float)_majorSlices;
    		float theta = u * 2.0f * XM_PI;
    		
    		for (uint32 j = 0; j <= _minorSlices; ++j)
    		{
    			float v = (float)j / (float)_minorSlices;
    			float phi = v * 2.0f * XM_PI;

    			float x = (_majorRadius + _minorRadius * XMScalarCos(phi)) * XMScalarCos(theta);
    			float y = _minorRadius * XMScalarSin(phi);
    			float z = (_majorRadius + _minorRadius * XMScalarCos(phi)) * XMScalarSin(theta);

    			Vertex vertex;
    			vertex.position = XMFLOAT3(x, y, z);
    			vertex.uv = XMFLOAT2(u, v);

    			vertices.push_back(vertex);
    		}
    	}

    	for (uint32 i = 0; i < _majorSlices; ++i)
    	{
    		for (uint32 j = 0; j < _minorSlices; ++j)
    		{
    			uint32 first = i * (_minorSlices + 1) + j;
    			uint32 second = first + _minorSlices + 1;
    			
    			indices.push_back(first + 1);
    			indices.push_back(second);
    			indices.push_back(first);
    			
    			indices.push_back(first + 1);
    			indices.push_back(second + 1);
    			indices.push_back(second);
    		}
    	}

    	vertices = CalculateNormalsAndTangentsSeamless(vertices, indices);

    	Geometry* geo = _pDevice->CreateGeometry(_isDynamic);
    	
    	geo->SetVertexData(vertices.data(), vertices.size());
    	geo->SetIndexData(indices.data(), indices.size());

    	return geo;
    }
	
	////////////////////////////////////////////
	//// GeometryCustom
	////////////////////////////////////////////

	static Geometry* LoadGeometry(Device* _pDevice, std::string _path, bool _isDynamic = false)
    {
		Geometry* pGeo = _pDevice->CreateGeometry(_isDynamic);
    	
    	Vector<XMFLOAT3> rawPositions;
    	Vector<XMFLOAT2> rawUVs;
    	
    	Vector<Vertex>   vertices;
    	Vector<uint32>   indices;

    	FILE* file = nullptr;
    	errno_t err = fopen_s(&file, _path.c_str(), "r");
    	if (err != 0 || file == nullptr)
    	{
    		// std::cout << "Impossible to open the file: " << _path << '\n';
    		return nullptr;
    	}

    	while (1)
    	{
    		char lineHeader[128];
    		int res = fscanf_s(file, "%s", lineHeader, (unsigned)_countof(lineHeader));
    		if (res == EOF)
    			break;

    		if (strcmp(lineHeader, "v") == 0)
    		{
    			XMFLOAT3 pos;
    			fscanf_s(file, "%f %f %f\n", &pos.x, &pos.y, &pos.z);
    			rawPositions.push_back(pos);
    		}
    		else if (strcmp(lineHeader, "vt") == 0)
    		{
    			XMFLOAT2 uv;
    			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
    			// Flip V to match DirectX convention (OBJ origin is bottom-left)
    			uv.y = 1.0f - uv.y;
    			rawUVs.push_back(uv);
    		}
    		else if (strcmp(lineHeader, "f") == 0)
    		{
    			unsigned int posIdx[3], uvIdx[3], normalIdx[3];
    			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
					&posIdx[0], &uvIdx[0], &normalIdx[0],
					&posIdx[1], &uvIdx[1], &normalIdx[1],
					&posIdx[2], &uvIdx[2], &normalIdx[2]);

    			if (matches != 9)
    			{
    				printf("File can't be read by our simple parser\n");
    				fclose(file);
    				return nullptr;
    			}

    			for (int k = 0; k < 3; ++k)
    			{
    				Vertex v;
    				v.position = rawPositions[posIdx[k] - 1];
    				v.uv       = rawUVs.empty() ? XMFLOAT2() : rawUVs[uvIdx[k] - 1];
    				v.normal   = XMFLOAT3();
    				indices.push_back((uint32)vertices.size());
    				vertices.push_back(v);
    			}
    		}
    	}

    	fclose(file);

    	vertices = CalculateNormalsAndTangents(vertices, indices);

    	pGeo->SetVertexData(vertices.data(), vertices.size());
    	pGeo->SetIndexData(indices.data(), indices.size());

		return pGeo;
    }

	static void LoadJsonObj(json const& object, Vector<Vertex>& _vVertex, Vector<uint32>& _vIndices)
    {
    	json mesh = object["mesh"];
    	json vVertices = mesh["vertices"];
    	json indices = mesh["indices"];
    	json uvs = mesh["uvs"];

    	for (int i = 0; i < indices.size(); i++)
    	{
    		int vIndex = indices[i].get<int>();

    		Vertex vertex;
    		
    		// Position from vertex index
    		XMFLOAT3 pos;
    		pos.x = vVertices[vIndex * 3 + 0].get<float>();
    		pos.y = vVertices[vIndex * 3 + 2].get<float>();
    		pos.z = vVertices[vIndex * 3 + 1].get<float>();
    		vertex.position = pos;

    		
    		// UV from loop index
    		XMFLOAT2 texCoord;
    		texCoord.x = -uvs[i * 2 + 0].get<float>();
    		texCoord.y = uvs[i * 2 + 1].get<float>();
			vertex.uv = texCoord;
    		
    		_vVertex.push_back(vertex);
    	}
		
    	for (int i = 0; i < indices.size(); i++)
    		_vIndices.push_back(i);
    	
    }
	
	static Geometry* LoadJsonGeometry(Device* _pDevice, json const& object)
    {
    	Geometry* pGeo = _pDevice->CreateGeometry();

    	Vector<Vertex> vertices;
    	Vector<uint32> indices;

    	LoadJsonObj(object, vertices, indices);
    	vertices = CalculateNormalsAndTangents(vertices, indices);
    	
    	pGeo->SetVertexData(vertices.data(), vertices.size());
    	pGeo->SetIndexData(indices.data(), indices.size());

    	return pGeo;
    }
};

#endif