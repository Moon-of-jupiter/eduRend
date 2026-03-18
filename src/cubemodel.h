#pragma once
#include "Model.h"
class CubeModel : public Model
{
	unsigned m_number_of_indices = 0;

public:
	
	Material m_material;

	// shared material cBuffer
	ID3D11Buffer* m_materialBuffer;
	
	/**
	 * @brief Create a model of a cube.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	CubeModel(Material material, bool invert, ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, ID3D11Buffer* materialBuffer);


	void UpdateMaterialBuffer(const Material& material) const;



	/**
	 * @brief Render the model.
	*/
	virtual void Render() const;

	void FillVertexVector(std::vector<Vertex>& v, float pitch, float yaw, float scale, bool invert);

	/**
	 *@brief Destructor.
	*/
	~CubeModel() 
	{
		SAFE_RELEASE(m_material.DiffuseTexture.TextureView);
	}

};

