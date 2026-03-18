/**
 * @file model.h
 * @brief Contains model definitions
 * @author Carl Johan Gribel 2016, cjgribel@gmail.com
*/

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "Drawcall.h"
#include "OBJLoader.h"
#include "Texture.h"

using namespace linalg;

/**
 * @brief Abstract class. Defines the Render method and contains mesh data needed for a model.
*/
class Model
{
protected:
	// Pointers to the current device and device context
	ID3D11Device* const			m_dxdevice; //!< Graphics device, use for creating resources.
	ID3D11DeviceContext* const	m_dxdevice_context; //!< Graphics context, use for binding resources and draw commands.

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* m_vertex_buffer = nullptr; //!< Pointer to gpu side vertex buffer
	ID3D11Buffer* m_index_buffer = nullptr; //!< Pointer to gpu side index buffer

	void Compute_TB(Vertex& v0, Vertex& v1, Vertex& v2) {
		vec3f tangent = vec3f(0,1,0);
		vec3f binormal = vec3f(0,1,0);
		
		// TO DO: add stuff

		vec3f D = v1.Position - v0.Position;
		vec3f E = v2.Position - v0.Position;

		vec2f F = v1.TexCoord - v0.TexCoord;
		vec2f G = v2.TexCoord - v0.TexCoord;

		float Fu = F.y ;
		float Fv = F.x ;

		float Gu = G.y;
		float Gv = G.x ;


		// [ Gv,-Fv, 0  ]
		// [-Gu, Fu, 0  ]
		// [ 0 , 0 , 1  ]
		mat3f FG_inv_mat = mat3f(
			float3( Gv,-Fv, 0),
			float3(-Gu, Fu, 0),
			float3( 0 , 0 , 1)
			);
		FG_inv_mat.transpose();
		FG_inv_mat.inverse();
		

		// [ Dx, Dy, Dz ]
		// [ Ex, Ey, Ez ]
		// [ 0 , 0 , 1  ]
		mat3f DE_mat = mat3f(
			D,
			E,
			float3(0,0,1)
			);
		DE_mat.transpose();
		DE_mat.inverse();
		

		// [ Tx, Ty, Tz ]
		// [ Bx, By, Bz ]
		// [ 0 , 0 , 1  ]

		float n = (1.0f / (Fu * Gv - Fv * Gu));
		
		mat3f TB_mat =  ( FG_inv_mat * DE_mat) * n;

		tangent =	vec3f(TB_mat.m11, TB_mat.m12, TB_mat.m13);
		binormal =	vec3f(TB_mat.m21, TB_mat.m22, TB_mat.m23);
		

		v0.Tangent = v1.Tangent = v2.Tangent = tangent;
		v0.Binormal = v1.Binormal = v2.Binormal = binormal;
	}


public:

	/**
	 * @brief Sets the protected member variables to the input params.
	 * @param dxdevice ID3D11Device to be used in the model.
	 * @param dxdevice_context ID3D11DeviceContext to be used in the model.
	*/
	Model(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context) 
		:	m_dxdevice(dxdevice), m_dxdevice_context(dxdevice_context) { }

	/**
	 * @brief Abstract render method: must be implemented by derived classes
	*/
	virtual void Render() const = 0;

	/**
	 * @brief Destructor.
	 * @details Releases the vertex and index buffers of the Model.
	*/
	virtual ~Model()
	{ 
		SAFE_RELEASE(m_vertex_buffer);
		SAFE_RELEASE(m_index_buffer);
	}
};

#endif