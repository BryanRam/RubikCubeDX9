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
#define CUBE_SIDES 6

#pragma once


int circleSides = 24;
float cubie_dimension = 1.705f;


//// LEGO brick properties
//const float LEGO_PITCH = 8.0f; // 8mm between studs
//const float LEGO_HALF_PITCH = 4.0f; // 8mm between studs
//const float LEGO_HEIGHT2 = 3.2f; 
//const float LEGO_HEIGHT = 9.6f; // height of a classic brick
//const float LEGO_STUD_HEIGHT = 1.7f; // height of a classic brick
//const float LEGO_STUD_WIDTH = 8.0f; // width a single stud occupies
//const float LEGO_STUD_GAP = 3.0f;
//const float LEGO_STUD_RADIUS = 2.5f;
//const float LEGO_GAP = 0.1f; // clearance gap between bricks

class CUBIE_PANEL
{
	public:
		LPDIRECT3DDEVICE9 render_target_;
		//float length = 1.833; //1.833 cm = 0.722 in
		float dimension = 1.405; //1.905 cm = 0.75 in
		float panel_height = dimension / 5; //0.281 * 2 = 0.562
		DWORD panel_colour;
		float hCubeHeight = 0.4025f, hCubeWidth = 0.7025f;
		float vCubeHeight = 0.7025f, vCubeWidth = 0.5025f;
		D3DXMATRIX rotMat;

		CUBIE_PANEL(DWORD pcolour) : panel_colour(pcolour) 
		{
			D3DXMatrixRotationYawPitchRoll(&rotMat, 0, 0, 0); //rotate 90 degrees
		}

		~CUBIE_PANEL()
		{

			// Destructor - release the points buffer.
			SAFE_RELEASE(g_topVertexBuffer);
			SAFE_RELEASE(g_baseVertexBuffer);
			SAFE_RELEASE(g_panelVertexBuffer);
			SAFE_RELEASE(g_panelVertexBuffer2);
			SAFE_RELEASE(g_studLineVertexBuffer);
			SAFE_RELEASE(g_lineVertexBuffer);
			SAFE_RELEASE(i_buffer);
		}
		LPDIRECT3DVERTEXBUFFER9 g_topVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_baseVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_panelVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_panelVertexBuffer2 = NULL; // Buffer to hold vertices for the rectangle
		LPDIRECT3DVERTEXBUFFER9 g_studLineVertexBuffer = NULL;
		LPDIRECT3DVERTEXBUFFER9 g_lineVertexBuffer = NULL;
		LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;

		HRESULT initialise(LPDIRECT3DDEVICE9 device)
		{
			// Store the render target for later use...
			render_target_ = device;

			return S_OK;
		}

		void render(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, float hOffset, float val = 0.0f)
		{	
			renderVCube(matRotateY, WorldMat, hOffset, val);
			renderHCube(matRotateY, WorldMat, hOffset, val);
			renderREdge(matRotateY, WorldMat, hOffset, 1, 1, val);
			renderREdge(matRotateY, WorldMat, hOffset, -1, 1, val);
			renderREdge(matRotateY, WorldMat, hOffset, 1, -1, val);
			renderREdge(matRotateY, WorldMat, hOffset, -1, -1, val);

		}

		void renderVCube(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, float hOffset, float val = 0.0f )
		{
			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, hOffset, 0.0f,  /*0.65f*/0.0f);

			
			

			/*D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);

			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			D3DXMatrixIdentity(&WorldMat);*/
			D3DXMATRIX rotateMatX;
			D3DXMatrixRotationYawPitchRoll(&rotateMatX, val, 0, 0); //rotate 90 degrees

			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMatX);
			D3DXMatrixTranslation(&TranslateMat3, /*-(cubie_dimension/1.3)*/0.0f, 0.0f, 0.0f);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// select the vertex and index buffers to use
			render_target_->SetStreamSource(0, g_panelVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);
			render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

			D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			/*render_target_->SetStreamSource(0, g_lineVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->DrawPrimitive(D3DPT_LINESTRIP, 0, 6);*/
		}

		void renderHCube(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, float hOffset, float val = 0.0f)
		{
			D3DXMATRIX rotateMatX;
			D3DXMatrixRotationYawPitchRoll(&rotateMatX, 0, val, 0); //rotate 90 degrees

			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, hOffset, 0.0f,  /*0.65f*/0.0f);

			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);

			D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMatX);
			
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			// select the vertex and index buffers to use
			render_target_->SetStreamSource(0, g_panelVertexBuffer2, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);
			render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

			D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
		}

		void renderREdge(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, float hOffset, int posx = 1, int posy=1, float val = 0.0f)
		{
			int j = 0;
			
			D3DXMATRIX rotateMatX;
			D3DXMatrixRotationYawPitchRoll(&rotateMatX, 0, val, 0); //rotate 90 degrees

			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, hOffset, 0.0f,  /*0.65f*/0.0f);

			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);

			D3DXMatrixMultiply(&WorldMat, &WorldMat, &rotateMatX);

			D3DXMATRIX TranslateMat4;
			D3DXMatrixTranslation(&TranslateMat4, (vCubeWidth-0.048f)*posx, (vCubeHeight-0.25f)*posy, 0.0f);
			
			
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat4);

				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
				
				//D3DXMatrixTranslation(&TranslateMat2, LEGO_HALF_PITCH, LEGO_PITCH, 0.0f);

				
				D3DXMatrixMultiply(&WorldMat, &WorldMat, &matRotateY);
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

				render_target_->SetStreamSource(0, g_baseVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, (circleSides * 2)); // using a D3DPT_TRIANGLELIST primitive

				render_target_->SetStreamSource(0, g_topVertexBuffer, 0, sizeof(CUSTOMVERTEX));
				render_target_->SetFVF(D3DFVF_CUSTOMVERTEX);
				render_target_->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, (circleSides * 2) + 3); // using a D3DPT_TRIANGLEFAN primitive

																					  //render_target_->DrawPrimitive(D3DPT_LINESTRIP, ); //outline for the bricks

				D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
				render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
			
		}
		

		HRESULT Setup()
		{
			if (SetupPanel() == S_OK)
			{
				return S_OK;
			}

			return E_FAIL;
		}

		HRESULT SetupPanel()
		{
			if(
				SetupVCube() == S_OK &&
				SetupHCube() == S_OK &&
				SetupRoundedEdges() == S_OK
			  )
			return S_OK;

			return E_FAIL;
		}

		//Sets up Vertical Cube
		HRESULT SetupVCube()
		{
			int cubeVertices = (CUBE_SIDES * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);

			// Now get Direct3D to create the vertex buffer.
			// The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
			// it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_panelVertexBuffer, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_panelVertexBuffer->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			CUSTOMVERTEX vertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				{ -vCubeWidth, vCubeHeight, -panel_height, panel_colour, },    // vertex 0 
				{ vCubeWidth, vCubeHeight, -panel_height, panel_colour, },     // vertex 1
				{ -vCubeWidth, -vCubeHeight, -panel_height, panel_colour, },   // 2
				{ vCubeWidth, -vCubeHeight, -panel_height, panel_colour, },  // 3

				/*Front Half	Top view
				 4	5		0	1
				 6	7		4	5
				*/

				/*Viewed from Left Side
				 Right Side	Left Side
				 1	5			0	4
				 3	7			2	6
				*/

				/*Viewed from Right Side
				 Right Side	Left Side
				 5	1			4	0
				 7	3			6	2
				*/

				{ -vCubeWidth, vCubeHeight, panel_height, panel_colour, },     // 4
				{ vCubeWidth, vCubeHeight, panel_height, panel_colour, }, //5
				{ -vCubeWidth, -vCubeHeight, panel_height, panel_colour, }, //6
				{ vCubeWidth, -vCubeHeight, panel_height, panel_colour, }, //7
			};



			memcpy(cVertices, vertices, sizeof(vertices));

			g_panelVertexBuffer->Unlock();

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

		//Sets up Horizontal Cube
		HRESULT SetupHCube()
		{
			int cubeVertices = (CUBE_SIDES * 3);
			int cubeBufferSize = cubeVertices * sizeof(CUSTOMVERTEX);

			// Now get Direct3D to create the vertex buffer.
			// The vertex buffer for the rectangle doesn't exist at this point, but the pointer to
			// it does (g_topVertexBuffer)
			if (FAILED(render_target_->CreateVertexBuffer(cubeBufferSize, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_panelVertexBuffer2, NULL)))
			{
				return E_FAIL; // if the vertex buffer culd not be created.
			}

			// Fill the buffer with appropriate vertices to describe the circle.

			// Create a pointer to the first vertex in the buffer
			// Also lock it, so nothing else can touch it while the values are being inserted.
			VOID* cVertices;
			if (FAILED(g_panelVertexBuffer2->Lock(0, 0, (void**)&cVertices, 0)))
			{
				return E_FAIL;  // if the pointer to the vertex buffer could not be established.
			}

			CUSTOMVERTEX vertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				{ -hCubeWidth, hCubeHeight, -panel_height, panel_colour, },    // vertex 0 
				{ hCubeWidth, hCubeHeight, -panel_height, panel_colour, },     // vertex 1
				{ -hCubeWidth, -hCubeHeight, -panel_height, panel_colour, },   // 2
				{ hCubeWidth, -hCubeHeight, -panel_height, panel_colour, },  // 3

				/*Front Half	Top view
				4	5		0	1
				6	7		4	5
				*/

				/*Viewed from Left Side
				Right Side	Left Side
				1	5			0	4
				3	7			2	6
				*/

				/*Viewed from Right Side
				Right Side	Left Side
				5	1			4	0
				7	3			6	2
				*/

				{ -hCubeWidth, hCubeHeight, panel_height, panel_colour, },     // 4
				{ hCubeWidth, hCubeHeight, panel_height, panel_colour, }, //5
				{ -hCubeWidth, -hCubeHeight, panel_height, panel_colour, }, //6
				{ hCubeWidth, -hCubeHeight, panel_height, panel_colour, }, //7
			};


			
			memcpy(cVertices, vertices, sizeof(vertices));
			
			g_panelVertexBuffer2->Unlock();

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

		HRESULT SetupRoundedEdges()
		{
			// Calculate the number of vertices required, and the size of the buffer to hold them.
			//int BufferSize = (Sides + 2) * sizeof(CUSTOMVERTEX);
			
			int BufferSize = ((circleSides * 2) + 2 + 3) * sizeof(CUSTOMVERTEX);
			int rectVertices = (circleSides * 6);
			int rectBufferSize = rectVertices * sizeof(CUSTOMVERTEX);
			int cubeVertices = (CUBE_SIDES * 3);
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
			Angle = Angle / (float) circleSides;
			float Radius = 0.250625;


			topVertices[0].x = 0;
			topVertices[0].y = 0;
			topVertices[0].z = -panel_height;
			topVertices[0].colour = panel_colour;

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
			for (int i = 1; i < (circleSides + 2); ++i)
			{
				float a = Angle*( - i);

				float x = Radius * (cos(a));
				float y = Radius * (sin(a));

				topVertices[i].x = x;
				topVertices[i].y = y;
				topVertices[i].z = -panel_height;
				topVertices[i].colour = panel_colour;

				topLineVertices[i - 1].x = x;
				topLineVertices[i - 1].y = y;
				topLineVertices[i - 1].z = -1.6f;
				topLineVertices[i - 1].colour = Black;
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
			float height = 1.6f;
			while (i < (rectVertices))
			{
				//
				//if(j > 12)
				//{ 
				baseVertices[i].x = topVertices[j].x;
				baseVertices[i].y = topVertices[j].y;
				baseVertices[i].z = panel_height;
				baseVertices[i].colour = panel_colour;


				baseVertices[i + 1].x = topVertices[j + 1].x;
				baseVertices[i + 1].y = topVertices[j + 1].y;
				baseVertices[i + 1].z = topVertices[j + 1].z;
				baseVertices[i + 1].colour = panel_colour;

				//pV 1 6
				baseVertices[i + 2].x = topVertices[j].x;
				baseVertices[i + 2].y = topVertices[j].y;
				baseVertices[i + 2].z = topVertices[j].z;
				baseVertices[i + 2].colour = panel_colour;




				baseVertices[i + 3].x = topVertices[j + 1].x;
				baseVertices[i + 3].y = topVertices[j + 1].y;
				baseVertices[i + 3].z = panel_height;
				baseVertices[i + 3].colour = panel_colour;

				baseVertices[i + 4].x = topVertices[j + 1].x;
				baseVertices[i + 4].y = topVertices[j + 1].y;
				baseVertices[i + 4].z = topVertices[j + 1].z;
				baseVertices[i + 4].colour = panel_colour;

				baseVertices[i + 5].x = topVertices[j].x;
				baseVertices[i + 5].y = topVertices[j].y;
				baseVertices[i + 5].z = panel_height;
				baseVertices[i + 5].colour = panel_colour;


				i = i + 6;
				j += 1;
			}



			g_baseVertexBuffer->Unlock();

			return S_OK;
		}
		

	private:
		D3DXMATRIX MakeTranslateMatrix(int countx = 0, int county = 0, int countz = 0, float x = 0, float y = 0, float z = 0)
		{
			D3DXMATRIX TranslateMat;

			D3DXMatrixTranslation(&TranslateMat, x * countx, y * county, z * countz);

			return TranslateMat;
		}
		
		

		
};