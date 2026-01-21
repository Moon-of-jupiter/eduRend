#pragma once
#include "Model.h"
class CubeModel : public Model
{
	unsigned m_number_of_indices = 0;

public:
	/**
	 * @brief Create a model of a quad.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	CubeModel(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

	/**
	 * @brief Render the model.
	*/
	virtual void Render() const;

	void FillVertexVector(std::vector<Vertex>& v, float pitch, float yaw, float scale);

	/**
	 *@brief Destructor.
	*/
	~CubeModel() { }

};

