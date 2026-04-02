
#include "Scene.h"
#include "cubemodel.h"
#include "QuadModel.h"
#include "OBJModel.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	InitLightCameraBuffer();
	InitSharedMaterialBuffer();
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	


	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	m_camera->MoveTo({ 0, 0, 5 });


	// rotates camera
	m_camera->RotateTo({ 0,0,0 });


	Material cubeMat = Material();
	cubeMat.AmbientColour = vec3f(1, 0, 0);
	cubeMat.DiffuseTextureFilename = "assets/textures/wood.png";


	// Create objects
	m_quad = new CubeModel(cubeMat, false, m_dxdevice, m_dxdevice_context, m_sharedMaterialBuffer); //new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context, m_sharedMaterialBuffer);
	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context, m_sharedMaterialBuffer);

	Material skymat = Material();
	skymat.AmbientColour = vec3f(1, 0, 0);
	skymat.inv_skybox_value = 0;
	m_skybox = new CubeModel(skymat, true, m_dxdevice, m_dxdevice_context, m_sharedMaterialBuffer);

	
	

	//D3D11_FILTER_MIN_MAG_MIP_POINT
	D3D11_SAMPLER_DESC samplerdesc = {
		D3D11_FILTER_ANISOTROPIC,			// Filter
		D3D11_TEXTURE_ADDRESS_WRAP,			// AddressU
		D3D11_TEXTURE_ADDRESS_WRAP,			// AddressV
		D3D11_TEXTURE_ADDRESS_WRAP,			// AddressW
		0.0f,								// MipLODBias
		16,									// MaxAnisotropy
		D3D11_COMPARISON_NEVER,				// ComapirsonFunc
		{ 1.0f, 1.0f, 1.0f, 1.0f },			// BorderColor
		-FLT_MAX,							// MinLOD
		FLT_MAX,							// MaxLOD
	};

	m_dxdevice->CreateSamplerState(&samplerdesc, &m_samplerStateStandard);


	


	m_cube_texture = Texture();

	//cube map skybox
	SetSkyboxCubeMap("assets/cubemaps/astrocosm_01/astrocosm_reflection_01_");
	//"assets/cubemaps/astrocosm_01/astrocosm_reflection_01_"
	//"assets/cubemaps/Skybox/Skybox-"
	//"assets/cubemaps/debug_cubemap/debug_"
	//"assets/cubemaps/brightday/"
}


void OurTestScene::SetSkyboxCubeMap(const std::string& file_path) {


	std::string cube_filenams_string[6] = {

		(file_path + "posx.png"),
		(file_path + "negx.png"),

		(file_path + "negy.png"), // dirty bug fix, swapped pos and neg y
		(file_path + "posy.png"),

		(file_path + "posz.png"),
		(file_path + "negz.png")
	};

	
	const char* cube_filenames[6];
	for (int i = 0; i < 6; i++)
		cube_filenames[i] = cube_filenams_string[i].c_str();
	
	HRESULT hr = LoadCubeTextureFromFile(m_dxdevice, cube_filenames, &m_cube_texture);

	if (SUCCEEDED(hr)) std::cout << "Cubemap OK" << std::endl;
	else std::cout << "Cubemap failed to load" << std::endl;


	


}


//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	// Basic camera control
	if (input_handler.IsKeyPressed(Keys::Up) || input_handler.IsKeyPressed(Keys::W))
		m_camera->MoveInLocal({ 0.0f, 0.0f, -m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Down) || input_handler.IsKeyPressed(Keys::S))
		m_camera->MoveInLocal({ 0.0f, 0.0f, m_camera_velocity * dt });
	if (input_handler.IsKeyPressed(Keys::Right) || input_handler.IsKeyPressed(Keys::D))
		m_camera->MoveInLocal({ m_camera_velocity * dt, 0.0f, 0.0f });
	if (input_handler.IsKeyPressed(Keys::Left) || input_handler.IsKeyPressed(Keys::A))
		m_camera->MoveInLocal({ -m_camera_velocity * dt, 0.0f, 0.0f });

	if(input_handler.IsKeyPressed(Keys::Space))
		m_camera->Move({ 0.0f, m_camera_velocity * dt, 0.0f });
	if(input_handler.IsKeyPressed(Keys::LCtrl))
		m_camera->Move({ 0.0f, -m_camera_velocity * dt, 0.0f });

	if(input_handler.IsKeyPressed(Keys::Esc))
		PostQuitMessage(0);

	

	// rotateing camera control
	
	vec3f rotation = { 0,(float)input_handler.GetMouseDeltaX(),(float)input_handler.GetMouseDeltaY() };
	
	m_camera->Rotate(rotation * m_camera_sensitivity);
	m_camera->ClampCameraPitch(-m_camera_pitch_clamp, m_camera_pitch_clamp);


	m_cameraPos = m_camera->m_position.xyz1();
	m_lightPos = vec4f(3,4,60,0);

	m_skybox_transform = mat4f::translation(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z) * mat4f::scaling(600, 600, 600);

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	
		

	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	// Increment the rotation angle.
	m_angle += m_angular_velocity * dt;

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
	m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_lightCamera_buffer);
	m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_sharedMaterialBuffer);





	

	m_dxdevice_context->PSSetSamplers(0, 1, &m_samplerStateStandard);

	unsigned cube_tex_slot = 3;
	m_dxdevice_context->PSSetShaderResources(cube_tex_slot, 1, &m_cube_texture.TextureView);



	UpdateLightCameraBuffer(m_cameraPos, m_lightPos);

	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();

	// Load matrices + the Quad's transformation to the device and render it
	UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	m_quad->Render();

	m_smallCube_transform = m_quad_transform *
		mat4f::translation(0, 0, 2) *					// Small translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *		// No rotation
		mat4f::scaling(0.5f, 0.5f, 0.5f);				// scale to 50%


	// Load matrices + the secondary cubes transformation to the device and render it
	UpdateTransformationBuffer(m_smallCube_transform, m_view_matrix, m_projection_matrix);
	m_quad->Render();

	m_smallCube_transform = m_smallCube_transform *
		mat4f::translation(0, 0, 2) *					// Small translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *		// No rotation
		mat4f::scaling(0.5f, 0.5f, 0.5f);				// scale to 50%

	// Load matrices + the secondary cubes transformation to the device and render it
	UpdateTransformationBuffer(m_smallCube_transform, m_view_matrix, m_projection_matrix);
	m_quad->Render();

	//m_sponza_transform = m_smallCube_transform*  m_sponza_transform ;

	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	m_sponza->Render();

	UpdateTransformationBuffer(m_skybox_transform, m_view_matrix, m_projection_matrix);
	m_skybox->Render();
}


void OurTestScene::Release()
{
	SAFE_RELEASE(m_cube_texture.TextureView);

	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_skybox);

	SAFE_DELETE(m_camera);

	SAFE_RELEASE(m_transformation_buffer);
	SAFE_RELEASE(m_lightCamera_buffer);
	SAFE_RELEASE(m_sharedMaterialBuffer);
	SAFE_RELEASE(m_sharedMaterialBuffer);

	// + release other CBuffers
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}




void OurTestScene::InitLightCameraBuffer() {
	HRESULT hr;
	D3D11_BUFFER_DESC lightCameraBufferDesc = { 0 };
	lightCameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightCameraBufferDesc.ByteWidth = sizeof(LightCameraBuffer);
	lightCameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCameraBufferDesc.MiscFlags = 0;
	lightCameraBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCameraBufferDesc, nullptr, &m_lightCamera_buffer));
}

void OurTestScene::UpdateLightCameraBuffer(vec4f cameraPos, vec4f lightPos) {
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_lightCamera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCameraBuffer* lightCamera = (LightCameraBuffer*)resource.pData;
	lightCamera->CameraPos = cameraPos;
	lightCamera->LightPos = lightPos;
	m_dxdevice_context->Unmap(m_lightCamera_buffer, 0);
}

void OurTestScene::InitSharedMaterialBuffer() {

	HRESULT hr;
	D3D11_BUFFER_DESC materialBuffer = { 0 };
	materialBuffer.Usage = D3D11_USAGE_DYNAMIC;
	materialBuffer.ByteWidth = sizeof(MaterialBuffer);
	materialBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBuffer.MiscFlags = 0;
	materialBuffer.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBuffer, nullptr, &m_sharedMaterialBuffer));

	
}
