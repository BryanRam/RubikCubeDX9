#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <stdlib.h>		// Included for the random number generator routines.
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <vector>
#include <algorithm>
#include <functional>

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	float x, y, z;      // X, Y, Z position of the vertex.
	DWORD colour;       // The vertex color
};

DWORD Black = 0x00000000;
DWORD Blue = 0x000000ff;
DWORD White = 0x00ffffff;
DWORD Yellow = 0x00ffff00;
DWORD Green = 0x008000;
DWORD Orange = 0x00ffa500;
DWORD Red = 0xff0000;

// The structure of a vertex in our vertex buffer...
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define SAFE_RELEASE(p)      {if(p) {(p)->Release(); (p)=NULL;}}

#pragma once





// LEGO brick properties
const float LEGO_PITCH = 8.0f; // 8mm between studs
const float LEGO_HALF_PITCH = 4.0f; // 8mm between studs
const float LEGO_HEIGHT2 = 3.2f; 
const float LEGO_HEIGHT = 9.6f; // height of a classic brick
const float LEGO_STUD_HEIGHT = 1.7f; // height of a classic brick
const float LEGO_STUD_WIDTH = 8.0f; // width a single stud occupies
const float LEGO_STUD_GAP = 3.0f;
const float LEGO_STUD_RADIUS = 2.5f;
const float LEGO_GAP = 0.1f; // clearance gap between bricks

class CUBIE
{
	public:

		float dimension = 1.905f;
		DWORD colour = Black;


		LPDIRECT3DDEVICE9 render_target_;
		int studs;
		int columns, rows;
		int Sides = 24;  // The number of sides used to contruct the circle. More sides = smoother circle.
		int cubeSides = 6;
		float brick_height, brick_length, brick_width;
		float stud_height, stud_radius;
		DWORD brick_colour;

		CUBIE(DWORD cube_colour) :
			colour(cube_colour)
		{}

		~CUBIE()
		{
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

		void render(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,  
			D3DXMATRIX TranslateMat2, int count = 1)
		{	
			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, 0.0f, -24.0f, 0.0f);

			RenderStuds(matRotateY, WorldMat, TranslateMat2, count);
			//D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			RenderCube2X4(matRotateY, WorldMat, TranslateMat, count);
			
		}

		void renderWithTranslate(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,
			D3DXMATRIX TranslateMat2, int countx, int county, int countz, float x = 0, float y = 0, float z = 0)
		{
			RenderStudsWithTranslate(matRotateY, WorldMat, TranslateMat2, MakeTranslateMatrix(countx, county, countz, x, y, z));

			RenderCubeWithTranslate(matRotateY, WorldMat, TranslateMat, MakeTranslateMatrix(countx, county, countz, x, y, z));
		}

		

		HRESULT Setup()
		{
			if (SetupCylinder() == S_OK)
			{
				if (SetupCubeBase() == S_OK)
					return S_OK;
				else
					return E_FAIL;
			}

			return E_FAIL;
		}

		HRESULT SetupCylinder()
		{
			// Calculate the number of vertices required, and the size of the buffer to hold them.
			//int BufferSize = (Sides + 2) * sizeof(CUSTOMVERTEX);

			int BufferSize = ((Sides * 2) + 2 + 3) * sizeof(CUSTOMVERTEX);
			int rectVertices = (Sides * 6);
			int rectBufferSize = rectVertices * sizeof(CUSTOMVERTEX);
			int cubeVertices = (cubeSides * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);

			// Now get Direct3D to create the vertex buffer.
			// The vertex buffer for the circle doesn't exist at this point, but the pointer to
			// it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(BufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_topVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			CUSTOMVERTEX* topVertices;
			if (FAILED(g_topVertexBuffer->Lock(0, 0, (void**)&topVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			// Fill the vertex buffers with data...
			// Must do this in reverse order so the points are specified are clockwise.

			float Angle = (2 * D3DX_PI);
			Angle = Angle / (float)Sides;
			float Radius = LEGO_STUD_RADIUS * 1;
			

			topVertices[0].x = 0;
			topVertices[0].y = 0;
			topVertices[0].z = 0;
			topVertices[0].colour = brick_colour;

			// Now get Direct3D to create the vertex buffer.
			// The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
			// it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(BufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_studLineVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			CUSTOMVERTEX* topLineVertices;
			if (FAILED(g_studLineVertexBuffer->Lock(0, 0, (void**)&topLineVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			// Initialise the vertices of the circle.
			for (int i = 1; i < (Sides + 2); ++i)
			{
				float a = Angle*(Sides - i);

				float x = Radius * (cos(a));
				float y = Radius * (sin(a));

				topVertices[i].x = x;
				topVertices[i].y = y;
				topVertices[i].z = 0;
				topVertices[i].colour = brick_colour;

				topLineVertices[i-1].x = x;
				topLineVertices[i-1].y = y;
				topLineVertices[i-1].z = 0;
				topLineVertices[i-1].colour = Black;
			}

			// Unlock the vertex buffer...
			g_topVertexBuffer->Unlock();

			g_studLineVertexBuffer->Unlock();



			// Now get Direct3D to create the vertex buffer.
			// The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
			// it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(rectBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_baseVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			CUSTOMVERTEX* baseVertices;
			if (FAILED(g_baseVertexBuffer->Lock(0, 0, (void**)&baseVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			int i = 0;
			int j = 1;
			while (i < (rectVertices))
			{
				//
				//if(j > 12)
				//{ 
				baseVertices[i].x = topVertices[j].x;
				baseVertices[i].y = topVertices[j].y;
				baseVertices[i].z = LEGO_STUD_HEIGHT;
				baseVertices[i].colour = brick_colour;


				baseVertices[i + 1].x = topVertices[j + 1].x;
				baseVertices[i + 1].y = topVertices[j + 1].y;
				baseVertices[i + 1].z = topVertices[j + 1].z;
				baseVertices[i + 1].colour = brick_colour;

				//pV 1 6
				baseVertices[i + 2].x = topVertices[j].x;
				baseVertices[i + 2].y = topVertices[j].y;
				baseVertices[i + 2].z = topVertices[j].z;
				baseVertices[i + 2].colour = brick_colour;




				baseVertices[i + 3].x = topVertices[j + 1].x;
				baseVertices[i + 3].y = topVertices[j + 1].y;
				baseVertices[i + 3].z = LEGO_STUD_HEIGHT;
				baseVertices[i + 3].colour = brick_colour;

				baseVertices[i + 4].x = topVertices[j + 1].x;
				baseVertices[i + 4].y = topVertices[j + 1].y;
				baseVertices[i + 4].z = topVertices[j + 1].z;
				baseVertices[i + 4].colour = brick_colour;

				baseVertices[i + 5].x = topVertices[j].x;
				baseVertices[i + 5].y = topVertices[j].y;
				baseVertices[i + 5].z = LEGO_STUD_HEIGHT;
				baseVertices[i + 5].colour = brick_colour;


				i = i + 6;
				j += 1;
			}



			g_baseVertexBuffer->Unlock();
		}

		HRESULT SetupCube2X4()
		{
			int cubeVertices = (cubeSides * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);
			DWORD CircleColour = 0x00ff0000; // (red)

											 // Now get Direct3D to create the vertex buffer.
											 // The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
											 // it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_cubeVertexBuffer->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			// create the vertices using the CUSTOMVERTEX struct
			/*CUSTOMVERTEX vertices[] =
			{
			{ -3.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
			{ 3.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
			{ -3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
			{ 3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },
			};
			*/



			CUSTOMVERTEX vertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				{ -8.0f, /*4.8f*/12.8f, -1.6f /*-12.8f*/, CircleColour /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0 
				{ 8.0f, /*4.8f*/12.8f, -1.6f /*-12.8f*/, CircleColour/*D3DCOLOR_XRGB(0, 255, 0)*/, },     // vertex 1
				{ -8.0f, /*-4.8f*/-12.8f, -1.6f /*-12.8f*/, CircleColour/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				{ 8.0f, /*-4.8f*/ -12.8f, -1.6f /*-12.8f*/, CircleColour/*D3DCOLOR_XRGB(0, 255, 255)*/, },  // 3

																											/*Front Half	Top view
																											4	5		0	1
																											6	7		4	5
																											*/
				{ -8.0f, /*4.8f*/12.8f, 1.6f /*12.8f*/, CircleColour/*D3DCOLOR_XRGB(0, 0, 255)*/, },     // ...
				{ 8.0f, /*4.8f*/12.8f, 1.6f /*12.8f*/, CircleColour/*D3DCOLOR_XRGB(255, 0, 0)*/, },
				{ -8.0f, /*-4.8f*/-12.8f, 1.6f /*12.8f*/, CircleColour/*D3DCOLOR_XRGB(0, 255, 0)*/, },
				{ 8.0f, /*-4.8f*/-12.8f, 1.6f /*12.8f*/, CircleColour/*D3DCOLOR_XRGB(0, 255, 255)*/, },
			};


			//VOID* pVoid;    // a void pointer

			// lock v_buffer and load the vertices into it
			//v_buffer->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(cVertices, vertices, sizeof(vertices));
			//v_buffer->Unlock();




			g_cubeVertexBuffer->Unlock();

			// create the indices using an int array
			short indices[] =
			{
				2, 3, 6,
				6, 3, 7,

				0, 2, 4,
				4, 2, 6,

				5, 1, 4,
				4, 1, 0,

				7, 3, 5,
				5, 3, 1,

				0, 1, 2,
				2, 1, 3,

				7, 5, 6,
				6, 5, 4,

				/*
				0, 1, 2,    // side 1
				2, 1, 3,

				4, 0, 6,    // side 2
				6, 0, 2,

				7, 5, 6,    // side 3
				6, 5, 4,

				3, 1, 7,    // side 4
				7, 1, 5,

				4, 5, 0,    // side 5
				0, 5, 1,

				3, 7, 2,    // side 6
				2, 7, 6,
				*/
			};


			render_target_->CreateIndexBuffer(36 * sizeof(short),    // 3 per triangle, 12 triangles
				0,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&i_buffer,
				NULL);


			// lock i_buffer and load the indices into it
			i_buffer->Lock(0, 0, (void**)&cVertices, 0);
			memcpy(cVertices, indices, sizeof(indices));
			i_buffer->Unlock();


			return S_OK;
		}

		HRESULT SetupCubie()
		{
			int cubeVertices = cubeSides * 3;
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);

			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_cubeVertexBuffer->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}
		}

		HRESULT SetupCubeBase()
		{
			int cubeVertices = (cubeSides * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);
			//DWORD ShaftColour = 0x00ff0000; // (red)
			

											 // Now get Direct3D to create the vertex buffer.
											 // The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
											 // it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_cubeVertexBuffer->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			

			// create the vertices using the CUSTOMVERTEX struct
			
			CUSTOMVERTEX vertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				{ (-LEGO_STUD_WIDTH/2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows-1)) + 2.8)/2, 0 /*-12.8f*/, brick_colour /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0 
				{ (LEGO_STUD_WIDTH/2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows-1)) + 2.8) / 2, 0 /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 0)*/, },     // vertex 1
				{ (-LEGO_STUD_WIDTH/2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows-1)) + 2.8) / 2, 0 /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				{ (LEGO_STUD_WIDTH/2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows-1)) + 2.8) / 2, 0 /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 255)*/, },  // 3

																											/*Front Half	Top view
																											4	5		0	1
																											6	7		4	5
																											*/
				{ (-LEGO_STUD_WIDTH/2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 0, 255)*/, },     // ...
				{ (LEGO_STUD_WIDTH/2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(255, 0, 0)*/, },
				{ (-LEGO_STUD_WIDTH/2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 0)*/, },
				{ (LEGO_STUD_WIDTH/2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 255)*/, },
			};

			


			//VOID* pVoid;    // a void pointer

			// lock v_buffer and load the vertices into it
			//v_buffer->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(cVertices, vertices, sizeof(vertices));
			//v_buffer->Unlock();




			g_cubeVertexBuffer->Unlock();

			// create the indices using an int array
			short indices[] =
			{
				2, 3, 6,
				6, 3, 7,

				0, 2, 4,
				4, 2, 6,

				5, 1, 4,
				4, 1, 0,

				7, 3, 5,
				5, 3, 1,

				0, 1, 2,
				2, 1, 3,

				7, 5, 6,
				6, 5, 4,

				/*
				0, 1, 2,    // side 1
				2, 1, 3,

				4, 0, 6,    // side 2
				6, 0, 2,

				7, 5, 6,    // side 3
				6, 5, 4,

				3, 1, 7,    // side 4
				7, 1, 5,

				4, 5, 0,    // side 5
				0, 5, 1,

				3, 7, 2,    // side 6
				2, 7, 6,
				*/
			};


			render_target_->CreateIndexBuffer(36 * sizeof(short),    // 3 per triangle, 12 triangles
				0,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&i_buffer,
				NULL);


			// lock i_buffer and load the indices into it
			i_buffer->Lock(0, 0, (void**)&cVertices, 0);
			memcpy(cVertices, indices, sizeof(indices));
			i_buffer->Unlock();

			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_lineVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			VOID* lVertices;
			if (FAILED(g_lineVertexBuffer->Lock(0, 0, (void**)&lVertices, 0)))
			{
				return E_FAIL;
			}

			CUSTOMVERTEX lineVertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				//0
				{ (-LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0 
				//1
				{ (LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(0, 255, 0)*/, },     // vertex 1
				//{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, -1.6f /*-12.8f*/, ShaftColour/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				//{ (LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(0, 255, 255)*/, },  // 3
				//{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2

				/*Front Half	Top view
					4	5		0	1
					6	7		4	5
				*/
				///*
				//5
				{ (LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
				//4
				{ (-LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },     // ...
				//0
				{ (-LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0 
				//2
				{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				//6
				{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
				//4
				{ (-LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },     // ...
				//5
				{ (LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
				//7
				{ (LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
				//3
				{ (LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0, Black, },
				//1
				{ (LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(0, 255, 0)*/, },
				//3
				{ (LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0, Black, },
				//2
				{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, 0 /*-12.8f*/, Black/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2

			//{ (LEGO_STUD_WIDTH / 2)*columns, (((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
			//{ (-LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
			//{ (LEGO_STUD_WIDTH / 2)*columns, -(((LEGO_STUD_RADIUS * 2) * rows) + (LEGO_STUD_GAP * (rows - 1)) + 2.8) / 2, brick_height, Black, },
			//*/
			};

			memcpy(lVertices, lineVertices, sizeof(lineVertices));

			g_lineVertexBuffer->Unlock();

			return S_OK;
		}

		HRESULT SetupCube()
		{
			int cubeVertices = (cubeSides * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);
			DWORD ShaftColour = 0x00ff0000; // (red)

											 // Now get Direct3D to create the vertex buffer.
											 // The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
											 // it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_cubeVertexBuffer->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			// create the vertices using the CUSTOMVERTEX struct
			/*CUSTOMVERTEX vertices[] =
			{
			{ -3.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
			{ 3.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
			{ -3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
			{ 3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 255), },
			};
			*/



			CUSTOMVERTEX vertices[] =
			{
				{ -4.0f, 4.0f, -4.0f, ShaftColour /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0
				{ 4.0f, 4.0f, -4.0f, ShaftColour/*D3DCOLOR_XRGB(0, 255, 0)*/, },     // vertex 1
				{ -4.0f, -4.0f, -4.0f, ShaftColour/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				{ 4.0f, -4.0f, -4.0f, ShaftColour/*D3DCOLOR_XRGB(0, 255, 255)*/, },  // 3
				{ -4.0f, 4.0f, 4.0f, ShaftColour/*D3DCOLOR_XRGB(0, 0, 255)*/, },     // ...
				{ 4.0f, 4.0f, 4.0f, ShaftColour/*D3DCOLOR_XRGB(255, 0, 0)*/, },
				{ -4.0f, -4.0f, 4.0f, ShaftColour/*D3DCOLOR_XRGB(0, 255, 0)*/, },
				{ 4.0f, -4.0f, 4.0f, ShaftColour/*D3DCOLOR_XRGB(0, 255, 255)*/, },
			};


			//VOID* pVoid;    // a void pointer

			// lock v_buffer and load the vertices into it
			//v_buffer->Lock(0, 0, (void**)&pVoid, 0);
			memcpy(cVertices, vertices, sizeof(vertices));
			//v_buffer->Unlock();




			g_cubeVertexBuffer->Unlock();

			// create the indices using an int array
			short indices[] =
			{
				0, 1, 2,    // side 1
				2, 1, 3,
				4, 0, 6,    // side 2
				6, 0, 2,
				7, 5, 6,    // side 3
				6, 5, 4,
				3, 1, 7,    // side 4
				7, 1, 5,
				4, 5, 0,    // side 5
				0, 5, 1,
				3, 7, 2,    // side 6
				2, 7, 6,
			};


			render_target_->CreateIndexBuffer(36 * sizeof(short),    // 3 per triangle, 12 triangles
				0,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&i_buffer,
				NULL);


			// lock i_buffer and load the indices into it
			i_buffer->Lock(0, 0, (void**)&cVertices, 0);
			memcpy(cVertices, indices, sizeof(indices));
			i_buffer->Unlock();

			return S_OK;
		}

	private:
		D3DXMATRIX MakeTranslateMatrix(int countx = 0, int county = 0, int countz = 0, float x = 0, float y = 0, float z = 0)
		{
			D3DXMATRIX TranslateMat;

			D3DXMatrixTranslation(&TranslateMat, x * countx, y * county, z * countz);

			return TranslateMat;
		}
		void RenderStuds(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat2, int count = 1)
		{
			int j = 0;
			float pos[] = { LEGO_PITCH, 0.0f, -LEGO_PITCH, (-LEGO_PITCH * 2) };
			float col = LEGO_HALF_PITCH * (rows-1);
			float row = LEGO_HALF_PITCH;
			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, 0.0f, 0.0f , brick_height * (count - 1));
			D3DXMATRIX rotateMat;
			D3DXMatrixRotationYawPitchRoll(&rotateMat, 0, 70.0f, 0);
			for (int i = 1; i <= studs; i++)
			{
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);

				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
				if (i == 1)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
				}
				else if (i % columns == 0)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
					++j;
					col -= LEGO_PITCH;
					row = LEGO_HALF_PITCH + LEGO_PITCH;
				}
				else if (i % columns != 0)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
				}

				row -= LEGO_PITCH;
				//D3DXMatrixTranslation(&TranslateMat2, LEGO_HALF_PITCH, LEGO_PITCH, 0.0f);

				D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat2);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMat);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

				render_target_->SetStreamSource(0, g_baseVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (Sides * 2)); // using a D3DPT_TRIANGLELIST primitive

				render_target_->SetStreamSource(0, g_topVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, (Sides * 2) + 3); // using a D3DPT_TRIANGLEFAN primitive

																					  //render_target_->DrawPrimitive(D3DPT_LINESTRIP, ); //outline for the bricks

				D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			}
		}

		void RenderCube2X4(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat, int count = 1)
		{
			// Construct various matrices to move and expand the triangle the rectangle.
			D3DXMATRIX matRotateY2;

			D3DXMatrixRotationYawPitchRoll(&matRotateY2, 0, 90, 0/*index*/);

			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, 0.0f, 0.0f, brick_height * (count - 1));
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			D3DXMATRIX rotateMat;
			D3DXMatrixRotationYawPitchRoll(&rotateMat, 0, 70.0f, 0);

			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, -LEGO_HALF_PITCH * (columns-2), 0.0f, /*brick_height-1*/ LEGO_STUD_HEIGHT);
			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY2);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// Reset the world to its original 'shape'.
			D3DXMatrixIdentity(&WorldMat);

			// select the vertex and index buffers to use
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

			render_target_->SetStreamSource(0, g_lineVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->DrawPrimitive(D3DPT_LINESTRIP, 0, 6);

		}

		void RenderCube(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat)
		{
			// Construct various matrices to move and expand the triangle the rectangle.
			D3DXMATRIX matRotateY2;

			D3DXMatrixRotationYawPitchRoll(&matRotateY2, 0, 90, 0/*index*/);

			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, 0.0f, 0.0f, LEGO_HALF_PITCH - 1);
			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY2);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// Reset the world to its original 'shape'.
			D3DXMatrixIdentity(&WorldMat);

			// select the vertex and index buffers to use
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
			render_target_->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 8, 0, 12);

		}

		void RenderStudsWithTranslate(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat2, D3DXMATRIX TranslateMat3)
		{
			int j = 0;
			float pos[] = { LEGO_PITCH, 0.0f, -LEGO_PITCH, (-LEGO_PITCH * 2) };
			float col = LEGO_HALF_PITCH * (rows - 1);
			float row = LEGO_HALF_PITCH;

			D3DXMATRIX TranslateMat4;
			D3DXMatrixTranslation(&TranslateMat4, 0.0f, 0.0f, LEGO_STUD_HEIGHT);

			D3DXMATRIX rotateMat;
			D3DXMatrixRotationYawPitchRoll(&rotateMat, 0, 70.0f, 0);
			for (int i = 1; i <= studs; i++)
			{
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);

				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
				if (i == 1)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
				}
				else if (i % columns == 0)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
					++j;
					col -= LEGO_PITCH;
					row = LEGO_HALF_PITCH + LEGO_PITCH;
				}
				else if (i % columns != 0)
				{
					D3DXMatrixTranslation(&TranslateMat2, row, col, 0.0f);
				}

				row -= LEGO_PITCH;
				//D3DXMatrixTranslation(&TranslateMat2, LEGO_HALF_PITCH, LEGO_PITCH, 0.0f);

				D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat2);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMat);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

				render_target_->SetStreamSource(0, g_baseVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (Sides * 2)); // using a D3DPT_TRIANGLELIST primitive


				render_target_->SetStreamSource(0, g_topVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, (Sides * 2) + 3); // using a D3DPT_TRIANGLEFAN primitive
				render_target_->SetStreamSource(0, g_studLineVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->DrawPrimitive(D3DPT_LINESTRIP, 0, Sides );

				/*D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat4);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMat);
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
				render_target_->DrawPrimitive(D3DPT_LINESTRIP, 0, Sides);*/

				D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

				
			}
		}

		void RenderCubeWithTranslate(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat, 
			D3DXMATRIX TranslateMat3)
		{
			// Construct various matrices to move and expand the triangle the rectangle.
			D3DXMATRIX matRotateY2;

			D3DXMatrixRotationYawPitchRoll(&matRotateY2, 0, 90, 0/*index*/);

			//D3DXMATRIX TranslateMat3;
			
			//D3DXMatrixTranslation(&TranslateMat3, x * count, y * count, z * count);
			/*
			if (z > 0)
				D3DXMatrixTranslation(&TranslateMat3, 0.0f, 0.0f, z * (count - 1));
			else if (y > 0)
				D3DXMatrixTranslation(&TranslateMat3, 0.0f, y * (count - 1), 0.0f);
			else if (x > 0)
				D3DXMatrixTranslation(&TranslateMat3, x * (count - 1), 0.0f, 0.0f);
			*/
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			D3DXMATRIX rotateMat;
			D3DXMatrixRotationYawPitchRoll(&rotateMat, 0, 70.0f, 0);

			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, -LEGO_HALF_PITCH * (columns - 2), 0.0f, /*brick_height-1*/ LEGO_STUD_HEIGHT);
			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY2);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// Reset the world to its original 'shape'.
			D3DXMatrixIdentity(&WorldMat);

			// select the vertex and index buffers to use
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

			render_target_->SetStreamSource(0, g_lineVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->DrawPrimitive(D3DPT_LINESTRIP, 0, 13);
		}
};