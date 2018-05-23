#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <stdlib.h>		// Included for the random number generator routines.
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>




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
LPDIRECT3DTEXTURE9		g_pTexturesCopy[Textures];   // Array of pointers for the textures.


struct transCoords
{
	float toX, toY, toZ;
	float fromX, fromY, fromZ;
};

struct faces
{
	int frontSide,	//0
		rightSide,	//1
		backSide,	//2
		leftSide,	//3
		topSide,	//4
		bottomSide; //5
};


class CUBIE
{
	public:

		float dimension = 1.705f;
		float cubie_side = dimension/2;
		DWORD colour = Black;
		


		LPDIRECT3DDEVICE9 render_target_;
		float yIndexRowV1, yIndexRowV2, yIndexRowV3;
		float zIndexRow1, zIndexRow2, zIndexRow3;
		int studs;
		int columns, rows;
		int Sides = 24;  // The number of sides used to construct the circle. More sides = smoother circle.
		int cubeSides = 6;
		float brick_height, brick_length, brick_width;
		float stud_height, stud_radius;
		DWORD brick_colour;
		std::vector<float> transXCoordsToCenter;
		std::map<int, faces> textureStore, textureStore2;
		std::map<int, faces>::iterator ts_it;
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

		transCoords vCoords[27] = { //0
									{0, -dimension, (dimension)-0.65f, 
									 0, dimension, (-dimension)+0.65f},
								   {0},
								   {0},
								   //3
								   {0, -dimension, -0.65f, 
									0, dimension, +0.65f},
								   { 0 },
								   { 0 },
								   //6
								   {0, -dimension, -(dimension * 1) - 0.65f,
									0, dimension, (dimension * 1)+ 0.65f },
								   { 0 },
								   { 0 },
								   //9
								   {0, 0, (dimension * 1) - 0.65f,
									0, 0, (-dimension * 1) + 0.65f },
								   { 0 },
								   { 0 },
								   //12
								   { 0, 0, 0 - 0.65f,
								     0, 0, 0 + 0.65f },
								   { 0 },
								   { 0 },
								   //15
								   {0, 0, -(dimension * 1) - 0.65f,
									0, 0, (dimension * 1) + 0.65f },
								   { 0 },
								   { 0 },
								   //18
								   {0, dimension, dimension - 0.65f, 
									0, -dimension, -dimension + 0.65f },
								   { 0 },
								   { 0 },
								   //21
								   {0, dimension, -0.65f,
									0, -dimension, 0.65f },
								   { 0 },
								   { 0 },
								   //24
								   {0, dimension, -dimension - 0.65f,
									0, -dimension, dimension + 0.65f },
								   { 0 },
								   { 0 },

		                          };

		int vRow1[27];
		int vRowTest[27];
		int vRowL1[9];
		int hTopRow[9];
		bool isY[9];
		int hMidRow[9];
		int hBotRow[9];
		float xTest1, xTest2, xTest3;

		//Variable for the UI
		ID3DXFont *font1, *font2,
			*font_vcol, *font_row, *font_zcol;
		RECT title_rectangle, 
			instruction_rectangle, 
			instruction_vcol_rect,
			instruction_row_rect, 
			instruction_zcol_rect;
		std::string title, instruction, 
			instruction_row, instruction_vcol, instruction_zcol;

	

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
			font1->Release();
			font2->Release();
			
			font_row->Release();
			font_vcol->Release();
			font_zcol->Release();
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
			

			D3DXCreateTextureFromFile(render_target_, "Rubik’s_blue_final.png", &g_pTextures[0]);
			D3DXCreateTextureFromFile(render_target_, "Rubik’s_green_final.png", &g_pTextures[1]);
			D3DXCreateTextureFromFile(render_target_, "Rubik’s_orange_final.png", &g_pTextures[2]);
			D3DXCreateTextureFromFile(render_target_, "Rubik’s_red_final.png", &g_pTextures[3]);
			D3DXCreateTextureFromFile(render_target_, "Rubik’s_white_final.png", &g_pTextures[4]);
			D3DXCreateTextureFromFile(render_target_, "Rubik’s_yellow_final.png", &g_pTextures[5]);

			

			D3DXCreateTextureFromFile(render_target_, "Black_def.png", &g_pTextures[6]);

			//g_pTexturesCopy = g_pTextures;
		}

		void SetupTextureCoords(int count = 27)
		{
			for (int i = 0; i < count; i++)
			{
				faces newFaces;
				textureStore.insert(std::make_pair(i, newFaces));
				for (int j = 0; j < 6; j++)
				{
					ts_it = textureStore.find(i);
					switch (j)
					{
					case 0:
						ts_it->second.frontSide = j;
						break;
					case 1:
						ts_it->second.rightSide = j;
						break;
					case 2:
						ts_it->second.backSide = j;
						break;
					case 3:
						ts_it->second.leftSide = j;
						break;
					case 4:
						ts_it->second.topSide = j;
						break;
					case 5:
						ts_it->second.bottomSide = j;
						break;
					}
				}
				textureStore2 = textureStore;
				
			}
		}

		void render(D3DXMATRIX matRotateY, 
			D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,
			int zVal, int count = 1)
		{	

			
			RenderCubie(matRotateY, 
				WorldMat, TranslateMat, 
				zVal, count);

			
		}

		HRESULT SetupCubie()
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
			initialiseUI();

			return S_OK;
		}
				
		
		void setY(float yIndex)
		{
			yIndexRowV1= yIndex;
		}

		void setY2(float yIndex)
		{
			yIndexRowV2 = yIndex;
		}

		void setY3(float yIndex)
		{
			yIndexRowV3 = yIndex;
		}

		void setZ(float zIndex)
		{
			zIndexRow1 = zIndex;
		}

		void setZ2(float zIndex)
		{
			zIndexRow2 = zIndex;
		}

		void setZ3(float zIndex)
		{
			zIndexRow3 = zIndex;
		}

		void SetX1(float xIndex)
		{
			xTest1 = xIndex;
		}
		void SetX2(float xIndex)
		{
			xTest2 = xIndex;
		}
		void SetX3(float xIndex)
		{
			xTest3 = xIndex;
		}

		void SetH1(int* hRow, bool* yesY)
		{
			for (int i = 0; i < 9; i++)
			{
				hTopRow[i] = hRow[i];
				isY[i] = yesY[i];
			}
			
		}
		
		void SetH2(int* hRow)
		{
			for (int i = 0; i < 9; i++)
			{
				hMidRow[i] = hRow[i];
			}

		}
		
		void SetH3(int* hRow)
		{
			for (int i = 0; i < 9; i++)
			{
				hBotRow[i] = hRow[i];
			}

		}

		void setVRow1(int* vRow)
		{
			for (int i = 0; i < 27; i++)
			{
				vRow1[i] = vRow[i];
			}
		}

		void setVRowTest(int* vRow)
		{
			for (int i = 0; i < 27; i++)
			{
				vRowTest[i] = vRow[i];
			}
		}

		void setVRowL(int* vRow)
		{
			for (int i = 0; i < 9; i++)
			{
				vRowL1[i] = vRow[i];
			}
		}
		
		/*Change the textures on the appropriate faces,
		 depending on what rotation was specified
		 V = 9 blocks in a single column, vertical rotation 
		 H = 9 blocks in a horizontal row
		 Z = 9 blocks that would make up a "face", also a vertical rotation
		*/

		//Clockwise, Leftmost 9 blocks in a single column
		void changeTexturesVCW(int i)
		{

			std::map<int, faces>::iterator ts_it2;

			ts_it = textureStore.find(0 + i);
			ts_it2 = textureStore2.find(6 + i);

			ts_it->second.frontSide = ts_it2->second.topSide;
			ts_it->second.topSide = ts_it2->second.backSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(3 + i);
			ts_it2 = textureStore2.find(15 + i);

			ts_it->second.topSide = ts_it2->second.backSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(6 + i);
			ts_it2 = textureStore2.find(24 + i);

			ts_it->second.topSide = ts_it2->second.backSide;
			ts_it->second.backSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(9 + i);
			ts_it2 = textureStore2.find(3 + i);

			ts_it->second.frontSide = ts_it2->second.topSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(15 + i);
			ts_it2 = textureStore2.find(21 + i);

			ts_it->second.backSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(18 + i);
			ts_it2 = textureStore2.find(0 + i);

			ts_it->second.frontSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.frontSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(21 + i);
			ts_it2 = textureStore2.find(9 + i);

			ts_it->second.bottomSide = ts_it2->second.frontSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;

			ts_it = textureStore.find(24 + i);
			ts_it2 = textureStore2.find(18 + i);

			ts_it->second.backSide = ts_it2->second.bottomSide;
			ts_it->second.bottomSide = ts_it2->second.frontSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;

			textureStore2 = textureStore;

		}

		//Counter-clockwise, Leftmost 9 blocks in a single column
		void changeTexturesVCCW(int* row, int i)
		{
			std::map<int, faces>::iterator ts_it2;
			
			ts_it = textureStore.find(0 + i);
			ts_it2 = textureStore2.find(18 + i);

			ts_it->second.frontSide = ts_it2->second.bottomSide;
			ts_it->second.topSide = ts_it2->second.frontSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(3 + i);
			ts_it2 = textureStore2.find(9 + i);

			ts_it->second.topSide = ts_it2->second.frontSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(6 + i);
			ts_it2 = textureStore2.find(0 + i);

			ts_it->second.topSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.topSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(9 + i);
			ts_it2 = textureStore2.find(21 + i);

			ts_it->second.frontSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(15 + i);
			ts_it2 = textureStore2.find(3 + i);

			ts_it->second.backSide = ts_it2->second.topSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(18 + i);
			ts_it2 = textureStore2.find(24 + i);

			ts_it->second.frontSide = ts_it2->second.bottomSide;
			ts_it->second.bottomSide = ts_it2->second.backSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;


			ts_it = textureStore.find(21 + i);
			ts_it2 = textureStore2.find(15 + i);

			ts_it->second.bottomSide = ts_it2->second.backSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;

			ts_it = textureStore.find(24 + i);
			ts_it2 = textureStore2.find(6 + i);

			ts_it->second.backSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.backSide;
			ts_it->second.leftSide = ts_it2->second.leftSide;

			textureStore2 = textureStore;
		}

		//Top row of 9 blocks, horizontal rotation, clockwise
		void changeTexturesHCW(int* row, int i)
		{
			std::map<int, faces>::iterator ts_it2;
			ts_it = textureStore.find((i*9) + 0);
			ts_it2 = textureStore2.find((i * 9) + 2);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.frontSide;
			ts_it->second.frontSide = ts_it2->second.rightSide;


			ts_it = textureStore.find((i * 9) + 1);
			ts_it2 = textureStore2.find((i * 9) + 5);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.rightSide;


			ts_it = textureStore.find((i * 9) + 2);
			ts_it2 = textureStore2.find((i * 9) + 8);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.rightSide = ts_it2->second.backSide;
			ts_it->second.frontSide = ts_it2->second.rightSide;


			ts_it = textureStore.find((i * 9) + 3);
			ts_it2 = textureStore2.find((i * 9) + 1);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.frontSide;


			ts_it = textureStore.find((i * 9) + 5);
			ts_it2 = textureStore2.find((i * 9) + 7);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.rightSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 9) + 6);
			ts_it2 = textureStore2.find((i * 9));

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.leftSide;
			ts_it->second.leftSide = ts_it2->second.frontSide;


			ts_it = textureStore.find((i * 9) + 7);
			ts_it2 = textureStore2.find((i * 9) + 3);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.leftSide;


			ts_it = textureStore.find((i * 9) + 8);
			ts_it2 = textureStore2.find((i * 9) + 6);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.leftSide;
			ts_it->second.rightSide = ts_it2->second.backSide;


			textureStore2 = textureStore;

			
		}

		//counter clockwise, top row
		void changeTexturesHCCW(int* row, int i)
		{
			std::map<int, faces>::iterator ts_it2;
			ts_it = textureStore.find((i * 9) + 0);
			ts_it2 = textureStore2.find((i * 9) + 6);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.backSide;
			ts_it->second.frontSide = ts_it2->second.leftSide;


			ts_it = textureStore.find((i * 9) + 1);
			ts_it2 = textureStore2.find((i * 9) + 3);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.leftSide;


			ts_it = textureStore.find((i * 9) + 2);
			ts_it2 = textureStore2.find((i * 9) + 0);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.rightSide = ts_it2->second.frontSide;
			ts_it->second.frontSide = ts_it2->second.leftSide;


			ts_it = textureStore.find((i * 9) + 3);
			ts_it2 = textureStore2.find((i * 9) + 7);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.leftSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 9) + 5);
			ts_it2 = textureStore2.find((i * 9) + 1);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.rightSide = ts_it2->second.frontSide;


			ts_it = textureStore.find((i * 9) + 6);
			ts_it2 = textureStore2.find((i * 9) + 8);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.rightSide;
			ts_it->second.leftSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 9) + 7);
			ts_it2 = textureStore2.find((i * 9) + 5);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.rightSide;


			ts_it = textureStore.find((i * 9) + 8);
			ts_it2 = textureStore2.find((i * 9) + 2);

			ts_it->second.topSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.bottomSide;
			ts_it->second.backSide = ts_it2->second.rightSide;
			ts_it->second.rightSide = ts_it2->second.frontSide;


			textureStore2 = textureStore;
			
		}
		
		//9 blocks of the cube that make up the front "face", rotated clockwise
		void changeTexturesZCW(int i)
		{
			std::map<int, faces>::iterator ts_it2;
			ts_it = textureStore.find((i * 3) + 0);
			ts_it2 = textureStore2.find((i * 3) + 18);

			ts_it->second.topSide = ts_it2->second.leftSide;
			ts_it->second.leftSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 1);
			ts_it2 = textureStore2.find((i * 3) + 9);


			ts_it->second.topSide = ts_it2->second.leftSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 2);
			ts_it2 = textureStore2.find((i * 3) + 0);

			ts_it->second.topSide = ts_it2->second.leftSide;
			ts_it->second.rightSide = ts_it2->second.topSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 9);
			ts_it2 = textureStore2.find((i * 3) + 19);

			ts_it->second.leftSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 11);
			ts_it2 = textureStore2.find((i * 3) + 1);

			ts_it->second.rightSide = ts_it2->second.topSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 18);
			ts_it2 = textureStore2.find((i * 3) + 20);

			ts_it->second.bottomSide = ts_it2->second.rightSide;
			ts_it->second.leftSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 19);
			ts_it2 = textureStore2.find((i * 3) + 11);

			ts_it->second.bottomSide = ts_it2->second.rightSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 20);
			ts_it2 = textureStore2.find((i * 3) + 2);

			ts_it->second.rightSide = ts_it2->second.topSide;
			ts_it->second.bottomSide = ts_it2->second.rightSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			textureStore2 = textureStore;
		}

		//Counter-clockwise
		void changeTexturesZCCW(int i)
		{
			std::map<int, faces>::iterator ts_it2;
			ts_it = textureStore.find((i * 3) + 0);
			ts_it2 = textureStore2.find((i * 3) + 2);

			ts_it->second.topSide = ts_it2->second.rightSide;
			ts_it->second.leftSide = ts_it2->second.topSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 1);
			ts_it2 = textureStore2.find((i * 3) + 11);


			ts_it->second.topSide = ts_it2->second.rightSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 2);
			ts_it2 = textureStore2.find((i * 3) + 20);

			ts_it->second.topSide = ts_it2->second.rightSide;
			ts_it->second.rightSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 9);
			ts_it2 = textureStore2.find((i * 3) + 1);

			ts_it->second.leftSide = ts_it2->second.topSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 11);
			ts_it2 = textureStore2.find((i * 3) + 19);

			ts_it->second.rightSide = ts_it2->second.bottomSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 18);
			ts_it2 = textureStore2.find((i * 3)+ 0);

			ts_it->second.bottomSide = ts_it2->second.leftSide;
			ts_it->second.leftSide = ts_it2->second.topSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 19);
			ts_it2 = textureStore2.find((i * 3) + 9);

			ts_it->second.bottomSide = ts_it2->second.leftSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			ts_it = textureStore.find((i * 3) + 20);
			ts_it2 = textureStore2.find((i * 3) + 18);

			ts_it->second.rightSide = ts_it2->second.bottomSide;
			ts_it->second.bottomSide = ts_it2->second.leftSide;
			ts_it->second.frontSide = ts_it2->second.frontSide;
			ts_it->second.backSide = ts_it2->second.backSide;


			textureStore2 = textureStore;




			
		}

		//Set text for UI strings, create appropriate fonts
		HRESULT initialiseUI()
		{
			//Set the title
			font1 = NULL;
			D3DXCreateFont(render_target_, 40, 0, FW_BOLD, 1, true, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Helvetica", &font1);

			SetRect(&title_rectangle, 40, 70, 700, 700);
			title = "THE RUBIK'S CUBE ";


			//Set the instructions
			font2 = NULL;
			D3DXCreateFont(render_target_, 17, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Helvetica", &font2);

			SetRect(&instruction_rectangle, 40, 120, 1200, 800);
			instruction = "WSAD keys to rotate the whole cube.\n";
			instruction += "Press the left and right arrows to change the vcolumns, up and down arrows to change the row, and Home and End keys to change the zcolumns\n";
			instruction += "Buttons Z,X: Rotate Row Clockwise/CounterClockwise\nButtons C,V: Rotate VCol CW/CCW\nButtons B,N: Rotate ZCol CW/CCW";

			setTheBillboard();

			return S_OK;
		}

		void setTheBillboard(int vcol = 0, int row = 0, int zcol = 0)
		{
			//Set the row

			font_row = NULL;

			D3DXCreateFont(render_target_, 17, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Helvetica", &font_row);

			SetRect(&instruction_row_rect, 40, 205, 800, 800);
			instruction_row = "Current row number: " + std::to_string(row + 1);


			

			//Set the vcol
			

			font_vcol = NULL;

			D3DXCreateFont(render_target_, 17, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Helvetica", &font_vcol);

			SetRect(&instruction_vcol_rect, 40, 220, 800, 800);

			instruction_vcol = "Current vcol number: " + std::to_string(vcol + 1);

			//Set the zcol
			font_zcol = NULL;

			D3DXCreateFont(render_target_, 17, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, "Helvetica", &font_zcol);

			SetRect(&instruction_zcol_rect, 40, 235, 800, 800);

			instruction_zcol = "Current zcol number: " + std::to_string(zcol + 1);

		}

		/*
		Provide current row and column numbers to instruction strings
		*/
		void updateTheBillboard(int vcol, int row, int zcol)
		{
			instruction_row = "Current row number: " + std::to_string(row + 1);
			instruction_vcol = "Current vcol number: " + std::to_string(vcol + 1);
			instruction_zcol = "Current zcol number: " + std::to_string(zcol + 1);
		}

		/*
		Displays UI Text to the Screen
		*/
		void drawTheUI()
		{
			font1->DrawTextA(NULL, title.c_str(), -1, &title_rectangle, DT_LEFT, D3DCOLOR_XRGB(0, 255, 0));
			font2->DrawTextA(NULL, instruction.c_str(), -1, &instruction_rectangle, DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));

			font_row->DrawTextA(NULL, instruction_row.c_str(), -1, &instruction_row_rect, DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
			font_vcol->DrawTextA(NULL, instruction_vcol.c_str(), -1, &instruction_vcol_rect, DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
			font_zcol->DrawTextA(NULL, instruction_zcol.c_str(), -1, &instruction_zcol_rect, DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
		}

	private:
		
		
		void RenderCubie(D3DXMATRIX matRotateY,
			D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat, 
			int zVal, int count = 1)
		{
			int k = count / 9;
			int j;

			int i = count % 3;

			int l = count % 9;


			
			D3DXMATRIX WorldMat2;
			D3DXMATRIX TransformMatrix, TranslateMatEx;

			float RotateAngle = 0.0f;

			D3DXQUATERNION Rotation, ScalingRotation;
			D3DXMatrixIdentity(&WorldMat2);
			
			
			D3DXQuaternionRotationYawPitchRoll(&Rotation, 0.0f, 0.0f, RotateAngle);
			D3DXQuaternionRotationYawPitchRoll(&ScalingRotation, 0.0f, 0.0f, 0.0f);

			D3DXVECTOR3 ScalingCentre(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 RotationCentre(1.0f, 0.0f, 0.65f * dimension);
			D3DXVECTOR3 Translate(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 Scaling(1.0f, 1.0f, 1.0f);
			D3DXVECTOR3 axis(0.0f, -dimension, dimension + 0.65f);

			D3DXMatrixTransformation(&TransformMatrix, &ScalingCentre, &ScalingRotation, &Scaling, &RotationCentre, &Rotation, &Translate);

			D3DXMatrixTranslation(&TranslateMatEx, dimension, 0.0f, 0.65f + dimension);

			
			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, dimension * (i), -(dimension * (k)), 0.65f + (dimension * zVal));
			
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMatEx);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &TranslateMat);
			
			
			//Focus on the top horizontal row of 9 cubes
			if (count < 9)
			{
				D3DXMATRIX matRotateHT;
				D3DXQUATERNION  qR;
				D3DXVECTOR3 rotCentre;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), xTest1);
				rotCentre = D3DXVECTOR3(dimension, 0.0f, dimension + 0.65f);

				D3DXMatrixTransformation(&matRotateHT, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateHT;
			}
			
			//Focus on the middle horizontal row of 9 cubes
			else if (count < 18 && count>8)
			{
				D3DXMATRIX matRotateHT;
				D3DXQUATERNION  qR;
				D3DXVECTOR3 rotCentre;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), xTest2);
				rotCentre = D3DXVECTOR3(dimension, 0.0f, dimension + 0.65f);

				D3DXMatrixTransformation(&matRotateHT, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateHT;
			}

			//Focus on the bottom horizontal row of 9 cubes
			else
			{
				D3DXMATRIX matRotateHT;
				D3DXQUATERNION  qR;
				D3DXVECTOR3 rotCentre;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), xTest3);
				rotCentre = D3DXVECTOR3(dimension, 0.0f, dimension + 0.65f);

				D3DXMatrixTransformation(&matRotateHT, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateHT;
			}
			
			// V Columns
			if (count % 3 == 0)
			{
				D3DXMATRIX matRotateV;
				D3DXQUATERNION  qR;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), yIndexRowV1);

				D3DXVECTOR3 rotCentre (0.0f, -dimension, dimension + 0.65f);
				D3DXMatrixTransformation(&matRotateV, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateV;
			}

			else if (count % 3 == 1)
			{
				D3DXMATRIX matRotateV2;
				D3DXQUATERNION  qR;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), yIndexRowV2);

				D3DXVECTOR3 rotCentre(dimension, -dimension, dimension + 0.65f);
				D3DXMatrixTransformation(&matRotateV2, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateV2;
			}

			else if (count % 3 == 2)
			{
				D3DXMATRIX matRotateV3;
				D3DXQUATERNION  qR;
				D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), yIndexRowV3);

				D3DXVECTOR3 rotCentre(dimension * 2, -dimension, dimension + 0.65f);
				D3DXMatrixTransformation(&matRotateV3, NULL, NULL, NULL, &rotCentre, &qR, NULL);
				WorldMat2 *= matRotateV3;
			}

			//Z Columns/Faces
			int zRow1[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};
			int zRow2[9] = {3, 4, 5, 12, 13, 14, 21, 22, 23 };
			int zRow3[9] = {6, 7, 8, 15, 16, 17, 24, 25, 26 };

			for (int i = 0; i < 9; i++)
			{
				if (count == zRow1[i])
				{
					D3DXMATRIX matRotateZ1;
					D3DXQUATERNION  qR;
					D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), zIndexRow1);

					D3DXVECTOR3 rotCentre(dimension, -dimension, 0.65f);
					D3DXMatrixTransformation(&matRotateZ1, NULL, NULL, NULL, &rotCentre, &qR, NULL);
					WorldMat2 *= matRotateZ1;
				}
				else if (count == zRow2[i])
				{
					D3DXMATRIX matRotateZ2;
					D3DXQUATERNION  qR;
					D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), zIndexRow2);

					D3DXVECTOR3 rotCentre(dimension, -dimension, dimension + 0.65f);
					D3DXMatrixTransformation(&matRotateZ2, NULL, NULL, NULL, &rotCentre, &qR, NULL);
					WorldMat2 *= matRotateZ2;
				}
				else if (count == zRow3[i])
				{
					D3DXMATRIX matRotateZ3;
					D3DXQUATERNION  qR;
					D3DXQuaternionRotationAxis(&qR, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), zIndexRow3);

					D3DXVECTOR3 rotCentre(dimension, -dimension, (dimension * 2) + 0.65f);
					D3DXMatrixTransformation(&matRotateZ3, NULL, NULL, NULL, &rotCentre, &qR, NULL);
					WorldMat2 *= matRotateZ3;
				}
			}


			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateY);
			
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat2);
			
			D3DXMatrixIdentity(&WorldMat2);

			
			

			
			

			// select the vertex and index buffers to use
			//render_target_->SetTexture(0, g_pTextures[0]);
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
			render_target_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			render_target_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			render_target_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			
			
			

			// Render each face in turn, but select a different texture for each one.
			for (int i = 0; i < 6; i++)
			{
				// Select the texture.
				
					ts_it = textureStore.find(count);
					
						switch (i)
						{
						case 0:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.frontSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.frontSide = i;
							break;
						case 1:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.rightSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.rightSide = i;
							break;
						case 2:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.backSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.backSide = i;
							break;
						case 3:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.leftSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.leftSide = i;
							break;
						case 4:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.topSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.topSide = i;
							break;
						case 5:
							render_target_->SetTexture(0, g_pTextures[ts_it->second.bottomSide]);
							render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, i * 6, 2);
							//ts_it->second.bottomSide = i;
							break;
						}

					
			}

			
			
		}

		

		
};