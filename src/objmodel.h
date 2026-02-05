/**
 * @file objmodel.h
 * @brief Contains the OBJModel
 * @author Carl Johan Gribel 2016, cjgribel@gmail.com
*/

#pragma once
#include "Model.h"
#include "drawcall.h"
/**
 * @brief Model representing a 3D object.
 * @see OBJLoader
*/
class OBJModel : public Model
{
	// index ranges, representing Drawcalls, within an index array
	struct IndexRange
	{
		unsigned int Start;
		unsigned int Size;
		unsigned Offset;
		int MaterialIndex;
	};

	// data 
	std::vector<IndexRange> m_index_ranges;
	std::vector<Material> m_materials;


	// shared material cBuffer
	ID3D11Buffer* m_materialBuffer;


	void append_materials(const std::vector<Material>& mtl_vec)
	{
		m_materials.insert(m_materials.end(), mtl_vec.begin(), mtl_vec.end());
	}

	void UpdateMaterialBuffer(const Material& material) const;

public:

	/**
	 * @brief Creates a .obj model.
	 * @details Uses OBJLoader internaly.
	 * @param objfile Path to the .obj file.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	OBJModel(const std::string& objfile, ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, ID3D11Buffer* sharedMaterialBuffer);

	/**
	 * @brief Renders the model.
	*/
	virtual void Render() const;

	/**
	 * @brief Destructor 
	*/
	~OBJModel();
};