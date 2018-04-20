#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <stdlib.h>		// Included for the random number generator routines.
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <vector>
#include <algorithm>
#include <functional>
//#include "Panel.h"


// A structure for our custom vertex type
//struct CUSTOMVERTEX
//{
//	float x, y, z;      // X, Y, Z position of the vertex.
//	DWORD colour;       // The vertex color
//};
//
DWORD Black = 0x00000000;
DWORD Blue = 0x000000ff;
DWORD White = 0x00ffffff;
DWORD Yellow = 0x00ffff00;
DWORD Green = 0x008000;
DWORD Orange = 0x00ffa500;
DWORD Red = 0xff0000;

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;	// Position
	D3DXVECTOR3 normal;		// Vertex normal
	float u, v;				// Texture co-ordinates.
	DWORD colour;
};


// The structure of a vertex in our vertex buffer...
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define SAFE_RELEASE(p)      {if(p) {(p)->Release(); (p)=NULL;}}

#pragma once

std::vector<float> transXCoordsToCenter(14, 0.0f);

std::vector<float> transXCoordsFromCenter;

std::vector<float> transYCoordsToCenter;
std::vector<float> transYCoordsFromCenter;


//-----------------------------------------------------------------------------
// Global variables
const int Textures = 7;
LPDIRECT3DTEXTURE9		g_pTextures[Textures];   // Array of pointers for the textures.



struct transCoords
{
	float toX, toY, toZ;
	float fromX, fromY, fromZ;
};


class CUBIE
{
	public:

		float dimension = 1.705f;
		float cubie_side = dimension/2;
		DWORD colour = Black;
		


		LPDIRECT3DDEVICE9 render_target_;
		int studs;
		int columns, rows;
		int Sides = 24;  // The number of sides used to contruct the circle. More sides = smoother circle.
		int cubeSides = 6;
		float brick_height, brick_length, brick_width;
		float stud_height, stud_radius;
		DWORD brick_colour;
		std::vector<float> transXCoordsToCenter;
		float toArr[9] = { dimension, 0.0f, -(dimension), 
						    dimension, 0.0f, -(dimension), 
						  dimension, 0.0f, -dimension
						};
		float toArr2[9] = { dimension, dimension, dimension,
						 	 0.0f, 0.0f, 0.0f,
						 -(dimension), -(dimension), -(dimension)
						 };

		float fromArr[9] = {-(dimension), 0, dimension,
							-(dimension), 0.0f, dimension, 
							-(dimension), 0.0f, dimension
							};

		float fromArr2[9] = {-(dimension), -(dimension), -(dimension),
							0.0f, 0.0f, 0.0f,
							dimension, dimension, dimension
							};

		//float toArrV[9] = {};
		float mul = 1.0f;
		float toArrV2[9] = {(dimension * mul), 
			0, 0,
							 0.0f,
			0, 0,
							 -(dimension * mul),
			0, 0 };

		float fromArrV2[9] = { -(dimension * mul/2),
			0,  0,
							  0.0f,
			0, 0,
							  (dimension * mul/2),
			0, 0 };
		int n = 1;
		transCoords vCoords[27] = { //0
									{0, (-dimension * n), (dimension * n)-0.65f, 
									 0, (dimension * n), (-dimension * n)+0.65f},

									 //1
								   {0},

								   //2
								   {0},

								   //3
								   {0, (-dimension * n), 0-0.65f, 
									0, (dimension * n), 0+0.65f},

									//4
								   { 0 },

								   //5
								   { 0 },

								   //6
								   {0, (-dimension * n), -(dimension * n) - 0.65f,
									0, (dimension * n), (dimension * n)+ 0.65f },

									//7
								   { 0 },

								   //8
								   { 0 },

								   //9
								   {0, dimension, (dimension * 2) - 0.65f,
									0, -dimension, (-dimension * 2) + 0.65f },

									//10
								   { 0 },

								   //11
								   { 0 },

								   //12
								   { 0, dimension, 0 - 0.65f,
								     0, -dimension, 0 + 0.65f },

								   //13
								   { 0 },

								   //14
								   { 0 },

								   //15
								   {0, dimension, -(dimension * 2) - 0.65f,
									0, -dimension, (dimension * 2) + 0.65f },

									//16
								   { 0 },

								   //17
								   { 0 },

								   //18
								   {0, dimension, dimension, 
									0, -dimension, -dimension},

									//19
								   { 0 },

								   //20
								   { 0 },

								   //21
								   {0, dimension, 0, 
									0, -dimension, 0 },

									//22
								   { 0 },

								   //23
								   { 0 },

								   //24
								   {0, dimension, -dimension, 
									0, -dimension, dimension},

									//25
								   { 0 },

								   //26
								   { 0 },

		                          };
		//transXCoordsToCenter.insert(transXCoordsToCenter.begin(), {1, -(dimension)+1, -(dimension*2)+1, 0,0,0,0,0,0, 1, 0, -1, 1, 0, -1});

		 

		CUBIE(DWORD cube_colour) :
			colour(cube_colour)
		{}

		~CUBIE()
		{
			// Delete the textures.
			for (int i(0); i < Textures; ++i)
			{
				if (g_pTextures[i] != NULL) g_pTextures[i]->Release();
			}
			// Destructor - release the points buffer.
			SAFE_RELEASE(g_topVertexBuffer);
			SAFE_RELEASE(g_baseVertexBuffer);
			SAFE_RELEASE(g_cubeVertexBuffer);
			SAFE_RELEASE(g_studLineVertexBuffer);
			SAFE_RELEASE(g_lineVertexBuffer);
			SAFE_RELEASE(i_buffer);
		}

		LPDIRECT3DVERTEXBUFFER9 g_topVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_baseVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_cubeVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_studLineVertexBuffer = NULL;
		LPDIRECT3DVERTEXBUFFER9 g_lineVertexBuffer = NULL;
		LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;

		HRESULT initialise(LPDIRECT3DDEVICE9 device)
		{
			// Store the render target for later use...
			render_target_ = device;
			return S_OK;
		}

		//-----------------------------------------------------------------------------
		// Load and initialise the textures.

		void LoadTextures()
		{
			/*D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[0]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[1]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[2]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[3]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[4]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[5]);*/

			D3DXCreateTextureFromFile(render_target_, "Blue_def.png", &g_pTextures[0]);
			D3DXCreateTextureFromFile(render_target_, "Green_def.png", &g_pTextures[1]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_orange.png", &g_pTextures[2]);
			D3DXCreateTextureFromFile(render_target_, "Red_def.png", &g_pTextures[3]);
			D3DXCreateTextureFromFile(render_target_, "White_def.png", &g_pTextures[4]);
			D3DXCreateTextureFromFile(render_target_, "Yellow_def.png", &g_pTextures[5]);

			/*D3DXCreateTextureFromFile(render_target_, "Rubik�s_blue.png", &g_pTextures[0]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_green.png", &g_pTextures[1]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_orange.png", &g_pTextures[2]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_red.png", &g_pTextures[3]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_white.png", &g_pTextures[4]);
			D3DXCreateTextureFromFile(render_target_, "Rubik�s_yellow.png", &g_pTextures[5]);*/

			/*D3DXCreateTextureFromFile(render_target_, "Rubik_green2.png", &g_pTextures[0]);
			D3DXCreateTextureFromFile(render_target_, "CloudTexture.png", &g_pTextures[1]);
			D3DXCreateTextureFromFile(render_target_, "DXTexture.png", &g_pTextures[2]);
			D3DXCreateTextureFromFile(render_target_, "TimesSquareTexture.png", &g_pTextures[3]);
			D3DXCreateTextureFromFile(render_target_, "Pattern1Texture.png", &g_pTextures[4]);
			D3DXCreateTextureFromFile(render_target_, "TilesTexture.png", &g_pTextures[5]);*/

			D3DXCreateTextureFromFile(render_target_, "Black_def.png", &g_pTextures[6]);
		}

		void render(D3DXMATRIX matRotateY, 
			D3DXMATRIX matRotateH, D3DXMATRIX matRotateH2, D3DXMATRIX matRotateH3, 
			D3DXMATRIX matRotateV, 
			D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,
			D3DXMATRIX TranslateMat2, int zVal, int count = 1)
		{	
			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, 0.0f, -24.0f, 0.0f);

			RenderCubie(matRotateY, 
				matRotateH, matRotateH2, matRotateH3, 
				matRotateV, WorldMat, TranslateMat, 
				zVal, count);

			//RenderStuds(matRotateY, WorldMat, TranslateMat2, count);
			//D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			//RenderCube2X4(matRotateY, WorldMat, TranslateMat, count);
			
		}

		HRESULT SetupCubie2()
		{
			// Calculate the number of vertices required, and the size of the buffer to hold them.
			int Vertices = 2 * 3 * 6;	// Six vertices for each side, six sides.
			int BufferSize = Vertices * sizeof(CUSTOMVERTEX);

			// Create the vertex buffer.
			if (FAILED(render_target_->CreateVertexBuffer(BufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer could not be created.
			}

			// Fill the buffer with appropriate vertices to describe the cube...

			// Create a pointer to the first vertex in the buffer.
			CUSTOMVERTEX *pVertices;
			if (FAILED(g_cubeVertexBuffer->Lock(0, 0, (void**)&pVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			// Fill the vertex buffers with data...

			DWORD FaceColour = 0x00ff00000;

			/*for (int i = 0; i < 36; i++)
			{
				pVertices[i].colour = Black;
			}*/

			/**/
			// Side 1 - Front face
			pVertices[0].position.x = -cubie_side;	// Vertex co-ordinate.
			pVertices[0].position.y = -cubie_side;
			pVertices[0].position.z = -cubie_side;
			pVertices[0].normal.x = 0;		// Vertex normal.
			pVertices[0].normal.y = 0;
			pVertices[0].normal.z = -1;
			pVertices[0].colour = Black;
			pVertices[0].u = 0;
			pVertices[0].v = 0;

			pVertices[1].position.x = -cubie_side;
			pVertices[1].position.y = cubie_side;
			pVertices[1].position.z = -cubie_side;
			pVertices[1].normal.x = 0;
			pVertices[1].normal.y = 0;
			pVertices[1].normal.z = -1;
			pVertices[1].colour = Black;
			pVertices[1].u = 0;
			pVertices[1].v = 1;

			pVertices[2].position.x = cubie_side;
			pVertices[2].position.y = cubie_side;
			pVertices[2].position.z = -cubie_side;
			pVertices[2].normal.x = 0;
			pVertices[2].normal.y = 0;
			pVertices[2].normal.z = -1;
			pVertices[2].colour = Black;
			pVertices[2].u = 1;
			pVertices[2].v = 1;

			pVertices[3].position.x = cubie_side;
			pVertices[3].position.y = -cubie_side;
			pVertices[3].position.z = -cubie_side;
			pVertices[3].normal.x = 0;
			pVertices[3].normal.y = 0;
			pVertices[3].normal.z = -1;
			pVertices[3].colour = Black;
			pVertices[3].u = 1;
			pVertices[3].v = 0;

			pVertices[4].position.x = -cubie_side;
			pVertices[4].position.y = -cubie_side;
			pVertices[4].position.z = -cubie_side;
			pVertices[4].normal.x = 0;
			pVertices[4].normal.y = 0;
			pVertices[4].normal.z = -1;
			pVertices[4].colour = Black;
			pVertices[4].u = 0;
			pVertices[4].v = 0;

			pVertices[5].position.x = cubie_side;
			pVertices[5].position.y = cubie_side;
			pVertices[5].position.z = -cubie_side;
			pVertices[5].normal.x = 0;
			pVertices[5].normal.y = 0;
			pVertices[5].normal.z = -1;
			pVertices[5].colour = Black;
			pVertices[5].u = 1;
			pVertices[5].v = 1;

			// Side 2 - Right face
			pVertices[6].position.x = cubie_side;
			pVertices[6].position.y = -cubie_side;
			pVertices[6].position.z = -cubie_side;
			pVertices[6].normal.x = 1;
			pVertices[6].normal.y = 0;
			pVertices[6].normal.z = 0;
			pVertices[6].colour = Black;
			pVertices[6].u = 0;
			pVertices[6].v = 0;

			pVertices[7].position.x = cubie_side;
			pVertices[7].position.y = cubie_side;
			pVertices[7].position.z = -cubie_side;
			pVertices[7].normal.x = 1;
			pVertices[7].normal.y = 0;
			pVertices[7].normal.z = 0;
			pVertices[7].colour = Black;
			pVertices[7].u = 0;
			pVertices[7].v = 1;

			pVertices[8].position.x = cubie_side;
			pVertices[8].position.y = cubie_side;
			pVertices[8].position.z = cubie_side;
			pVertices[8].normal.x = 1;
			pVertices[8].normal.y = 0;
			pVertices[8].normal.z = 0;
			pVertices[8].colour = Black;
			pVertices[8].u = 1;
			pVertices[8].v = 1;

			pVertices[9].position.x = cubie_side;
			pVertices[9].position.y = -cubie_side;
			pVertices[9].position.z = cubie_side;
			pVertices[9].normal.x = 1;
			pVertices[9].normal.y = 0;
			pVertices[9].normal.z = 0;
			pVertices[9].colour = Black;
			pVertices[9].u = 1;
			pVertices[9].v = 0;

			pVertices[10].position.x = cubie_side;
			pVertices[10].position.y = -cubie_side;
			pVertices[10].position.z = -cubie_side;
			pVertices[10].normal.x = 1;
			pVertices[10].normal.y = 0;
			pVertices[10].normal.z = 0;
			pVertices[10].colour = Black;
			pVertices[10].u = 0;
			pVertices[10].v = 0;

			pVertices[11].position.x = cubie_side;
			pVertices[11].position.y = cubie_side;
			pVertices[11].position.z = cubie_side;
			pVertices[11].normal.x = 1;
			pVertices[11].normal.y = 0;
			pVertices[11].normal.z = 0;
			pVertices[11].colour = Black;
			pVertices[11].u = 1;
			pVertices[11].v = 1;

			// Side 3 - Rear face
			pVertices[12].position.x = cubie_side;
			pVertices[12].position.y = -cubie_side;
			pVertices[12].position.z = cubie_side;
			pVertices[12].normal.x = 0;
			pVertices[12].normal.y = 0;
			pVertices[12].normal.z = 1;
			pVertices[12].colour = Black;
			pVertices[12].u = 0;
			pVertices[12].v = 0;

			pVertices[13].position.x = cubie_side;
			pVertices[13].position.y = cubie_side;
			pVertices[13].position.z = cubie_side;
			pVertices[13].normal.x = 0;
			pVertices[13].normal.y = 0;
			pVertices[13].normal.z = 1;
			pVertices[13].colour = Black;
			pVertices[13].u = 0;
			pVertices[13].v = 1;

			pVertices[14].position.x = -cubie_side;
			pVertices[14].position.y = cubie_side;
			pVertices[14].position.z = cubie_side;
			pVertices[14].normal.x = 0;
			pVertices[14].normal.y = 0;
			pVertices[14].normal.z = 1;
			pVertices[14].colour = Black;
			pVertices[14].u = 1;
			pVertices[14].v = 1;

			pVertices[15].position.x = -cubie_side;
			pVertices[15].position.y = -cubie_side;
			pVertices[15].position.z = cubie_side;
			pVertices[15].normal.x = 0;
			pVertices[15].normal.y = 0;
			pVertices[15].normal.z = 1;
			pVertices[15].colour = Black;
			pVertices[15].u = 1;
			pVertices[15].v = 0;

			pVertices[16].position.x = cubie_side;
			pVertices[16].position.y = -cubie_side;
			pVertices[16].position.z = cubie_side;
			pVertices[16].normal.x = 0;
			pVertices[16].normal.y = 0;
			pVertices[16].normal.z = 1;
			pVertices[16].colour = Black;
			pVertices[16].u = 0;
			pVertices[16].v = 0;

			pVertices[17].position.x = -cubie_side;
			pVertices[17].position.y = cubie_side;
			pVertices[17].position.z = cubie_side;
			pVertices[17].normal.x = 0;
			pVertices[17].normal.y = 0;
			pVertices[17].normal.z = 1;
			pVertices[17].colour = Black;
			pVertices[17].u = 1;
			pVertices[17].v = 1;

			// Side 4 - Left face
			pVertices[18].position.x = -cubie_side;
			pVertices[18].position.y = -cubie_side;
			pVertices[18].position.z = cubie_side;
			pVertices[18].normal.x = -1;
			pVertices[18].normal.y = 0;
			pVertices[18].normal.z = 0;
			pVertices[18].colour = Black;
			pVertices[18].u = 0;
			pVertices[18].v = 0;

			pVertices[19].position.x = -cubie_side;
			pVertices[19].position.y = cubie_side;
			pVertices[19].position.z = cubie_side;
			pVertices[19].normal.x = -1;
			pVertices[19].normal.y = 0;
			pVertices[19].normal.z = 0;
			pVertices[19].colour = Black;
			pVertices[19].u = 0;
			pVertices[19].v = 1;

			pVertices[20].position.x = -cubie_side;
			pVertices[20].position.y = cubie_side;
			pVertices[20].position.z = -cubie_side;
			pVertices[20].normal.x = -1;
			pVertices[20].normal.y = 0;
			pVertices[20].normal.z = 0;
			pVertices[20].colour = Black;
			pVertices[20].u = 1;
			pVertices[20].v = 1;

			pVertices[21].position.x = -cubie_side;
			pVertices[21].position.y = -cubie_side;
			pVertices[21].position.z = -cubie_side;
			pVertices[21].normal.x = -1;
			pVertices[21].normal.y = 0;
			pVertices[21].normal.z = 0;
			pVertices[21].colour = Black;
			pVertices[21].u = 1;
			pVertices[21].v = 0;

			pVertices[22].position.x = -cubie_side;
			pVertices[22].position.y = -cubie_side;
			pVertices[22].position.z = cubie_side;
			pVertices[22].normal.x = -1;
			pVertices[22].normal.y = 0;
			pVertices[22].normal.z = 0;
			pVertices[22].colour = Black;
			pVertices[22].u = 0;
			pVertices[22].v = 0;

			pVertices[23].position.x = -cubie_side;
			pVertices[23].position.y = cubie_side;
			pVertices[23].position.z = -cubie_side;
			pVertices[23].normal.x = -1;
			pVertices[23].normal.y = 0;
			pVertices[23].normal.z = 0;
			pVertices[23].colour = Black;
			pVertices[23].u = 1;
			pVertices[23].v = 1;

			// Side 5 - Top face

			pVertices[24].position.x = -cubie_side;
			pVertices[24].position.y = cubie_side;
			pVertices[24].position.z = -cubie_side;
			pVertices[24].normal.x = 0;
			pVertices[24].normal.y = 1;
			pVertices[24].normal.z = 0;
			pVertices[24].colour = Black;
			pVertices[24].u = 0;
			pVertices[24].v = 0;

			pVertices[25].position.x = -cubie_side;
			pVertices[25].position.y = cubie_side;
			pVertices[25].position.z = cubie_side;
			pVertices[25].normal.x = 0;
			pVertices[25].normal.y = 1;
			pVertices[25].normal.z = 0;
			pVertices[25].colour = Black;
			pVertices[25].u = 0;
			pVertices[25].v = 1;

			pVertices[26].position.x = cubie_side;
			pVertices[26].position.y = cubie_side;
			pVertices[26].position.z = -cubie_side;
			pVertices[26].normal.x = 0;
			pVertices[26].normal.y = 1;
			pVertices[26].normal.z = 0;
			pVertices[26].colour = Black;
			pVertices[26].u = 1;
			pVertices[26].v = 0;

			pVertices[27].position.x = cubie_side;
			pVertices[27].position.y = cubie_side;
			pVertices[27].position.z = -cubie_side;
			pVertices[27].normal.x = 0;
			pVertices[27].normal.y = 1;
			pVertices[27].normal.z = 0;
			pVertices[27].colour = Black;
			pVertices[27].u = 1;
			pVertices[27].v = 0;

			pVertices[28].position.x = -cubie_side;
			pVertices[28].position.y = cubie_side;
			pVertices[28].position.z = cubie_side;
			pVertices[28].normal.x = 0;
			pVertices[28].normal.y = 1;
			pVertices[28].normal.z = 0;
			pVertices[28].colour = Black;
			pVertices[28].u = 0;
			pVertices[28].v = 1;

			pVertices[29].position.x = cubie_side;
			pVertices[29].position.y = cubie_side;
			pVertices[29].position.z = cubie_side;
			pVertices[29].normal.x = 0;
			pVertices[29].normal.y = 1;
			pVertices[29].normal.z = 0;
			pVertices[29].colour = Black;
			pVertices[29].u = 1;
			pVertices[29].v = 1;

			// Side 6 - Bottom face

			pVertices[30].position.x = -cubie_side;
			pVertices[30].position.y = -cubie_side;
			pVertices[30].position.z = -cubie_side;
			pVertices[30].normal.x = 0;
			pVertices[30].normal.y = -1;
			pVertices[30].normal.z = 0;
			pVertices[30].colour = Black;
			pVertices[30].u = 0;
			pVertices[30].v = 0;

			pVertices[31].position.x = cubie_side;
			pVertices[31].position.y = -cubie_side;
			pVertices[31].position.z = -cubie_side;
			pVertices[31].normal.x = 0;
			pVertices[31].normal.y = -1;
			pVertices[31].normal.z = 0;
			pVertices[31].colour = Black;
			pVertices[31].u = 1;
			pVertices[31].v = 0;

			pVertices[32].position.x = -cubie_side;
			pVertices[32].position.y = -cubie_side;
			pVertices[32].position.z = cubie_side;
			pVertices[32].normal.x = 0;
			pVertices[32].normal.y = -1;
			pVertices[32].normal.z = 0;
			pVertices[32].colour = Black;
			pVertices[32].u = 0;
			pVertices[32].v = 1;

			pVertices[33].position.x = cubie_side;
			pVertices[33].position.y = -cubie_side;
			pVertices[33].position.z = -cubie_side;
			pVertices[33].normal.x = 0;
			pVertices[33].normal.y = -1;
			pVertices[33].normal.z = 0;
			pVertices[33].colour = Black;
			pVertices[33].u = 1;
			pVertices[33].v = 0;

			pVertices[34].position.x = cubie_side;
			pVertices[34].position.y = -cubie_side;
			pVertices[34].position.z = cubie_side;
			pVertices[34].normal.x = 0;
			pVertices[34].normal.y = -1;
			pVertices[34].normal.z = 0;
			pVertices[34].colour = Black;
			pVertices[34].u = 1;
			pVertices[34].v = 1;

			pVertices[35].position.x = -cubie_side;
			pVertices[35].position.y = -cubie_side;
			pVertices[35].position.z = cubie_side;
			pVertices[35].normal.x = 0;
			pVertices[35].normal.y = -1;
			pVertices[35].normal.z = 0;
			pVertices[35].colour = Black;
			pVertices[35].u = 0;
			pVertices[35].v = 1;
			//*/

			// Unlock the vertex buffer...
			g_cubeVertexBuffer->Unlock();

			//panelRed->Setup();

			return S_OK;
		}
				
		

		

		
	private:
		
		
		void RenderCubie(D3DXMATRIX matRotateY, 
			D3DXMATRIX matRotateH, D3DXMATRIX matRotateH2, D3DXMATRIX matRotateH3, 
			D3DXMATRIX matRotateV, 
			D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat, 
			int zVal, int count = 1)
		{
			int k = count / 9;
			int j;

			int i = count % 3;

			int l = count % 9;


			D3DXMATRIX tMat, tMat2, tMat3, tMat4, WorldMat2, WorldMat3, WorldMat4, emptyMat, testMat;
			D3DXMATRIX TransformMatrix, TranslateMatEx;

			float RotateAngle = 0.0f;

			D3DXQUATERNION Rotation, ScalingRotation;
			D3DXMatrixIdentity(&WorldMat2);
			D3DXMatrixIdentity(&WorldMat3);
			D3DXMatrixIdentity(&testMat);
			D3DXQuaternionRotationMatrix(&Rotation, &matRotateH);
			D3DXQuaternionRotationYawPitchRoll(&Rotation, 0.0f, 0.0f, RotateAngle);
			D3DXQuaternionRotationYawPitchRoll(&ScalingRotation, 0.0f, 0.0f, 0.0f);

			D3DXVECTOR3 ScalingCentre(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 RotationCentre(1.0f, 0.0f, 0.65f * dimension);
			D3DXVECTOR3 Translate(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 Scaling(1.0f, 1.0f, 1.0f);

			D3DXMatrixTransformation(&TransformMatrix, &ScalingCentre, &ScalingRotation, &Scaling, &RotationCentre, &Rotation, &Translate);

			D3DXMatrixTranslation(&TranslateMatEx, dimension, 0.0f, 0.65f + dimension);

			//D3DXMatrixTranslation(&tMat3, 0.0f, 0.0f, 0.65f + toArrV2[count]);
			//D3DXMatrixTranslation(&tMat4, 0.0f, 0.0f, 0.65f + fromArrV2[count]);
			//D3DXMatrixTranslation(&tMat3, 0, (-dimension * n), (dimension * n) - 0.65f);
			D3DXMatrixTranslation(&tMat3, 0.0f, vCoords[count].toY, 0.65 + vCoords[count].toZ);
			
			D3DXMatrixTranslation(&WorldMat4, 0.0f, vCoords[count].toY, 0.65 + vCoords[count].toZ);
			D3DXMatrixIdentity(&WorldMat4);
			
			D3DXMatrixTranslation(&tMat4, 0.0f, vCoords[count].fromY, 0.65f + vCoords[count].fromZ);
			D3DXMatrixMultiply(&WorldMat4, &WorldMat4, &tMat4);
			D3DXMatrixIdentity(&WorldMat4);

			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, dimension * (i), -(dimension * (k)), 0.65f + (dimension * zVal));
			D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -0.65f);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMatEx);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &TranslateMat);
			//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);

			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
			//D3DXMatrixIdentity(&WorldMat);

			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &tMat);

			/*if (count % 3 == 0)
			{
				D3DXMatrixTranslation(&tMat, toArr[count], 0.0f, toArr2[count]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

				D3DXMatrixIdentity(&WorldMat2);

				D3DXMatrixTranslation(&tMat2, fromArr[count], 0.0f, fromArr2[count]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateH);


				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
				D3DXMatrixIdentity(&WorldMat3);

				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);



				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);
			}

			else*/ if ((count < 9))
			{
				D3DXMatrixTranslation(&tMat, toArr[count], 0.0f, toArr2[count]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

				D3DXMatrixIdentity(&WorldMat2);

				D3DXMatrixTranslation(&tMat2, fromArr[count], 0.0f, fromArr2[count]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);

				/*Middle hRotation ((count < 18) && (count > 8))
				D3DXMatrixTranslation(&tMat, toArr[count - 9], -(dimension), toArr2[count - 9]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

				D3DXMatrixIdentity(&WorldMat2);

				D3DXMatrixTranslation(&tMat2, fromArr[count - 9], -(dimension), fromArr2[count - 9]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);*/

				/*Bottom hRotation Buggy ((count < 27) && (count > 17))
				D3DXMatrixTranslation(&tMat, toArr[count - 18], -(dimension) * 2, toArr2[count - 18]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

				D3DXMatrixIdentity(&WorldMat2);

				D3DXMatrixTranslation(&tMat2, fromArr[count - 18], -(dimension) * 2, fromArr2[count - 18]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);*/

				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateH);

				if ((count) % 3 == 0)
				{
					
					render_target_->SetTransform(D3DTS_WORLD, &WorldMat3);
					D3DXMatrixIdentity(&WorldMat3);
					//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
					
					//D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -(0.65f * dimension));
					//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
					//D3DXMatrixMultiply(&testMat, &testMat, &tMat3);
					////D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &testMat);
					D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
					//
					////D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
					D3DXMatrixIdentity(&WorldMat3);
					//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
					render_target_->SetTransform(D3DTS_WORLD, &WorldMat3);
					D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
					////D3DXMatrixIdentity(&WorldMat3);
					//D3DXMatrixMultiply(&WorldMat4, &WorldMat4, &WorldMat3);
					//D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -0.65f);
					//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
					
					D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);
				}

				/*D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
				D3DXMatrixIdentity(&WorldMat3);

				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);*/

				//D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);
				//D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat4);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);


			}
			

			///*Middle hRotation
			else if(/*(count % 3 != 0) && */(count < 18) && (count > 8))
			{
			D3DXMatrixTranslation(&tMat, toArr[count - 9], -(dimension), toArr2[count - 9]);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

			D3DXMatrixIdentity(&WorldMat2);

			D3DXMatrixTranslation(&tMat2, fromArr[count - 9], -(dimension), fromArr2[count - 9]);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);
			
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateH2);

			if ((count) % 3 == 0)
			{
				//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
				//D3DXMatrixIdentity(&WorldMat3);
				////D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -(0.65f * dimension));
				////D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
				////D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				//D3DXMatrixIdentity(&WorldMat3);
				//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
				//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				////D3DXMatrixIdentity(&WorldMat3);
				////D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);
				////D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -0.65f);
				////D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				//D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3); 
			}
			/*D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
			D3DXMatrixIdentity(&WorldMat3);

			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);*/

			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);
			//*/
		    }

			/*D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
			D3DXMatrixIdentity(&WorldMat3);

			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);

			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);*/

			//D3DXMatrixTranslation(&tMat2, fromArr[count], 0.0f, fromArr2[count]);
			else
			{
				D3DXMatrixTranslation(&tMat, toArr[count - 18], -(dimension*2), toArr2[count - 18]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);

				D3DXMatrixIdentity(&WorldMat2);

				D3DXMatrixTranslation(&tMat2, fromArr[count - 18], -(dimension*2), fromArr2[count - 18]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);

				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateH3);

				//if ((count) % 3 == 0)
				//{
				//	D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &WorldMat2);
				//	//D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -(0.65f * dimension));
				//	//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				//	//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
				//	D3DXMatrixIdentity(&WorldMat3);
				//	//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
				//	D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				//	//D3DXMatrixIdentity(&WorldMat3);
				//	//D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);
				//	//D3DXMatrixTranslation(&emptyMat, 0.0f, 0.0f, -0.65f);
				//	//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				//	D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat3);
				//}
				

				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);
			}
			
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat2);
			D3DXMatrixIdentity(&WorldMat2);
			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &WorldMat2);
			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			
			/*
			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY2);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// Reset the world to its original 'shape'.
			D3DXMatrixIdentity(&WorldMat);
			*/

			// select the vertex and index buffers to use
			//render_target_->SetTexture(0, g_pTextures[0]);
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
			render_target_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			render_target_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			render_target_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			
			
			//render_target_->SetIndices(i_buffer);

			// Render each face in turn, but select a different texture for each one.
			for (int i = 0; i < 6; i++)
			{
				// Select the texture.
				if ((count % 3 == 0 && i == 1) || (count % 3 == 2 && i == 3))
				{
					render_target_->SetTexture(0, g_pTextures[6]);
					render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
				}
				else if ( (k == 0 && i == 5))
				{
					render_target_->SetTexture(0, g_pTextures[6]);
					render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
					//render_target_->SetTexture(0, g_pTextures[i]);
				//	render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
					
				}
				else  if ((k == 1 && i == 4) || (k == 1 && i == 5))
				{
					render_target_->SetTexture(0, g_pTextures[6]);
					render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
				}
				else if (k == 2 && i == 4)
				{
					render_target_->SetTexture(0, g_pTextures[6]);
					render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
				}
				else
				{
					render_target_->SetTexture(0, g_pTextures[i]);





					//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
					// Render the contents of the vertex buffer.
					render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
				}
			}

			// draw the cube
			//render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
			/*
			if(i == 0)
			panelRed->render(matRotateH, WorldMat2, i, -(dimension * (k)));
			else
				panelRed->render(matRotateH, WorldMat2, i * ((panelRed->hCubeWidth*2) + (dimension- panelRed->hCubeWidth * 2)), -(dimension * (k)));
			*/
			//render_target_->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 8, 0, 12);
		}

		

		
};