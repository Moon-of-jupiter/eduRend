#include "cubemodel.h"
CubeModel::CubeModel(Material material, bool invert,
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context, ID3D11Buffer* materialBuffer)
	: Model(dxdevice, dxdevice_context)
{
	// Vertex and index arrays
	// Once their data is loaded to GPU buffers, they are not needed anymore
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	vec2f sides[6] =
	{
		{ 0, 0 },
		{ 90, 0 },
		{ 180, 0 },
		{ -90, 0 },
		{ 0, 90 },
		{ 0, -90 },
	};

	std::vector<Vertex> sideVertices;

	// Populate the vertex array with 4 Vertices
	Vertex v0, v1, v2, v3;
	v0.Position = { -0.5, -0.5f, 0.0f };
	v0.Normal = { 0, 0, 1 };
	v0.TexCoord = { 0, 0 };

	v1.Position = { 0.5, -0.5f, 0.0f };
	v1.Normal = { 0, 0, 1 };
	v1.TexCoord = { 0, 1 };

	v2.Position = { 0.5, 0.5f, 0.0f };
	v2.Normal = { 0, 0, 1 };
	v2.TexCoord = { 1, 1 };

	v3.Position = { -0.5, 0.5f, 0.0f };
	v3.Normal = { 0, 0, 1 };
	v3.TexCoord = { 1, 0 };

	float degreesToRad = 1.0/180.0* MATH_H::PI;
	for (int i = 0; i < 6; i++) {
		FillVertexVector(sideVertices, sides[i].y * degreesToRad, sides[i].x * degreesToRad, 0.5f, invert);


		vertices.push_back(sideVertices[0]);
		vertices.push_back(sideVertices[1]);
		vertices.push_back(sideVertices[2]);
		vertices.push_back(sideVertices[3]);

		int currentIndexBase = i * 4;

		// Populate the index array with two triangles
		// Triangle #1
		indices.push_back(0 + currentIndexBase);
		indices.push_back(1 + currentIndexBase);
		indices.push_back(3 + currentIndexBase);
		// Triangle #2
		indices.push_back(1 + currentIndexBase);
		indices.push_back(2 + currentIndexBase);
		indices.push_back(3 + currentIndexBase);
	}

	




	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc{ 0 };
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = { 0 };
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData{ 0 };
	indexData.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	m_number_of_indices = (unsigned int)indices.size();





	m_materialBuffer = materialBuffer;



	std::cout << "Loading CubeModel textures..." << std::endl;
	m_material = material;

	HRESULT hr;

	// Load Diffuse texture
	//
	if (m_material.DiffuseTextureFilename.size()) {

		hr = LoadTextureFromFile(
			dxdevice,
			dxdevice_context,
			m_material.DiffuseTextureFilename.c_str(),
			&m_material.DiffuseTexture);
		std::cout << "\t" << m_material.DiffuseTextureFilename
			<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;
	}

	std::cout << "Done." << std::endl;
	
}


void CubeModel::FillVertexVector(std::vector<Vertex> & v, float pitch, float yaw, float scale, bool invert) {
	while (v.size() < 4) {
		Vertex empty;
		v.push_back(empty);
		
	}

	auto rot =	mat4f::rotation(0, yaw, pitch);
	
	float halfScale = scale * 0.5f;

	float normal = invert ? -1 : 1;

	vec4f c[5] =
	{
		{ -scale, -scale,  scale * normal, 1 },
		{  scale, -scale,  scale * normal, 1 },
		{  scale,  scale,  scale * normal, 1 },
		{ -scale,  scale,  scale * normal, 1 },

		{  0, 0, 1, 0 }
	};

	

	auto p = rot * c[4];
	
	

	v[0].Position =		(rot * c[0]).xyz();
	v[0].Normal =		(rot * c[4]).xyz();
	v[0].TexCoord =		{ 0, 0 };

	v[1].Position =		(rot * c[1]).xyz();
	v[1].Normal =		(rot * c[4]).xyz();
	v[1].TexCoord =		{ 0, 1 };

	v[2].Position =		(rot * c[2]).xyz();
	v[2].Normal =		(rot * c[4]).xyz();
	v[2].TexCoord =		{ 1, 1 };

	v[3].Position =		(rot * c[3]).xyz();
	v[3].Normal =		(rot * c[4]).xyz();
	v[3].TexCoord =		{ 1, 0 };

	
	
}

void CubeModel::UpdateMaterialBuffer(const Material& material) const {
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_materialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* materialData = (MaterialBuffer*)resource.pData;

	materialData->ambiantColor = vec4f(0.1f, 0, 0.4f, material.inv_skybox_value);
	materialData->specularColor = vec4f(material.SpecularColour, 1);
	materialData->diffuseColor_Glossyness = vec4f(material.DiffuseColour, 40);

	m_dxdevice_context->Unmap(m_materialBuffer, 0);
}

void CubeModel::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind our index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);



	// Bind diffuse texture to slot t0 of the PS
	m_dxdevice_context->PSSetShaderResources(0, 1, &m_material.DiffuseTexture.TextureView);
	// + bind other textures here, e.g. a normal map, to appropriate slots

	UpdateMaterialBuffer(m_material);


	// Make the drawcall
	m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);
}