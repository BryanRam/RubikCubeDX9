#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <stdlib.h>		// Included for the random number generator routines.
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <vector>
#include <algorithm>
#include <functional>
#include "Panel.h"


// A structure for our custom vertex type
//struct CUSTOMVERTEX
//{
//	float x, y, z;      // X, Y, Z position of the vertex.
//	DWORD colour;       // The vertex color
//};
//
//DWORD Black = 0x00000000;
//DWORD Blue = 0x000000ff;
//DWORD White = 0x00ffffff;
//DWORD Yellow = 0x00ffff00;
//DWORD Green = 0x008000;
//DWORD Orange = 0x00ffa500;
//DWORD Red = 0xff0000;

// The structure of a vertex in our vertex buffer...
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define SAFE_RELEASE(p)      {if(p) {(p)->Release(); (p)=NULL;}}

#pragma once

std::vector<float> transXCoordsToCenter(14, 0.0f);

std::vector<float> transXCoordsFromCenter;

std::vector<float> transYCoordsToCenter;
std::vector<float> transYCoordsFromCenter;



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

		float dimension = 1.705f;
		float cubie_side = dimension/2;
		DWORD colour = Black;
		CUBIE_PANEL * panel = new CUBIE_PANEL(Red);


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
		int mul = 1;
		float toArrV2[9] = {(dimension * mul), 
			0, 0,
							 0.0f,
			0, 0,
							 -(dimension * mul),
			0, 0 };

		float fromArrV2[9] = { -(dimension * mul),
			0,  0,
							  0.0f,
			0, 0,
							  (dimension * mul),
			0, 0 };

		//transXCoordsToCenter.insert(transXCoordsToCenter.begin(), {1, -(dimension)+1, -(dimension*2)+1, 0,0,0,0,0,0, 1, 0, -1, 1, 0, -1});

		 

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
			panel->initialise(device);
			return S_OK;
		}

		void render(D3DXMATRIX matRotateY, D3DXMATRIX matRotateH, D3DXMATRIX matRotateV, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,
			D3DXMATRIX TranslateMat2, CUBIE_PANEL panels, int zVal, int count = 1)
		{	
			D3DXMATRIX TranslateMat3;
			D3DXMatrixTranslation(&TranslateMat3, 0.0f, -24.0f, 0.0f);

			RenderCubie(matRotateY, matRotateH, matRotateV, WorldMat, TranslateMat, panel, zVal, count);

			//RenderStuds(matRotateY, WorldMat, TranslateMat2, count);
			//D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			//RenderCube2X4(matRotateY, WorldMat, TranslateMat, count);
			
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

			// create the vertices using the CUSTOMVERTEX struct

			CUSTOMVERTEX vertices[] =
			{
				/*Back Half   Bottom view  Heights: 3.2(1.6), 9.6(4.8)
				0	1		2	3
				2	3		6	7
				*/
				{ -cubie_side, cubie_side, -cubie_side /*-12.8f*/, brick_colour /*D3DCOLOR_XRGB(0, 0, 255)*/, },    // vertex 0 
				{ cubie_side, cubie_side, -cubie_side /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 0)*/, },     // vertex 1
				{ -cubie_side, -cubie_side, -cubie_side /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(255, 0, 0)*/, },   // 2
				{ cubie_side, -cubie_side, -cubie_side /*-12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 255)*/, },  // 3

																																															/*Front Half	Top view
																																															4	5		0	1
																																															6	7		4	5
																																															*/
				{ -cubie_side, cubie_side, cubie_side /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 0, 255)*/, },     // ...
				{ cubie_side, cubie_side, cubie_side /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(255, 0, 0)*/, },
				{ -cubie_side, -cubie_side, cubie_side /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 0)*/, },
				{ cubie_side, -cubie_side, cubie_side /*12.8f*/, brick_colour/*D3DCOLOR_XRGB(0, 255, 255)*/, },
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

			panel->Setup();

			return S_OK;
		}

		

		
	private:
		
		
		void RenderCubie(D3DXMATRIX matRotateY, D3DXMATRIX matRotateH,D3DXMATRIX matRotateV, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat, CUBIE_PANEL* panel, int zVal, int count = 1)
		{
			int k = count / 9;
			int j;

			int i = count % 3;
			
			int l = count % 9;
			

			D3DXMATRIX tMat, tMat2, tMat3, tMat4, WorldMat2, WorldMat3, emptyMat;
			D3DXMATRIX TransformMatrix, TranslateMatEx;

			float RotateAngle = 0.0f;

			D3DXQUATERNION Rotation, ScalingRotation;
			D3DXMatrixIdentity(&WorldMat2);
			D3DXMatrixIdentity(&WorldMat3);
			D3DXQuaternionRotationMatrix(&Rotation, &matRotateH);
			D3DXQuaternionRotationYawPitchRoll(&Rotation, 0.0f, 0.0f, RotateAngle);
			D3DXQuaternionRotationYawPitchRoll(&ScalingRotation, 0.0f, 0.0f, 0.0f);

			D3DXVECTOR3 ScalingCentre(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 RotationCentre(1.0f, 0.0f, 0.65f * dimension);
			D3DXVECTOR3 Translate(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 Scaling(1.0f, 1.0f, 1.0f);

			D3DXMatrixTransformation(&TransformMatrix, &ScalingCentre, &ScalingRotation, &Scaling, &RotationCentre, &Rotation, &Translate);
			
			D3DXMatrixTranslation(&TranslateMatEx, dimension, 0.0f, 0.65f + dimension);
			D3DXMatrixTranslation(&tMat, toArr[count], 0.0f, toArr2[count]);
			D3DXMatrixTranslation(&tMat3, 0.0f, 0.0f, 0.65f + toArrV2[count]);
			D3DXMatrixTranslation(&tMat4, 0.0f, 0.0f, 0.65f + fromArrV2[count]);
			//Move forward 10 units and apply world rotation
			D3DXMatrixTranslation(&TranslateMat, dimension * (i), -(dimension * (k)), 0.65f + (dimension * zVal));
			D3DXMatrixTranslation(&emptyMat, dimension * (i), -(dimension * (k)), 0.65f - (dimension * zVal));
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMatEx);
			D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &TranslateMat);
			D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &TranslateMat);
			
			//render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
			//D3DXMatrixIdentity(&WorldMat);

			//D3DXMatrixMultiply(&WorldMat, &WorldMat, &tMat);
			

			if (count < 9)
			{
				//if (count % 3 == 0)
				//{
				//	D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat3);
				//	D3DXMatrixIdentity(&WorldMat3);
				//	
				//	//D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &tMat4);
				//	D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &matRotateV);
				//	D3DXMatrixMultiply(&WorldMat3, &WorldMat3, &emptyMat);
				//	D3DXMatrixMultiply(&WorldMat, &WorldMat, &WorldMat3);
				//}
				
				
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat);
				D3DXMatrixIdentity(&WorldMat2);
				D3DXMatrixTranslation(&tMat2, fromArr[count], 0.0f, fromArr2[count]);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &tMat2);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &matRotateH);
				D3DXMatrixMultiply(&WorldMat2, &WorldMat2, &WorldMat);
				
				
			}

			//D3DXMatrixTranslation(&tMat2, fromArr[count], 0.0f, fromArr2[count]);
			
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
			render_target_->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
			render_target_->SetIndices(i_buffer);

			// draw the cube
			render_target_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
			if(i == 0)
			panel->render(matRotateY, WorldMat2, i * (dimension - (panel->hCubeWidth)));
			else
				panel->render(matRotateY, WorldMat2, i * (dimension-(panel->hCubeWidth)));
			//render_target_->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, 8, 0, 12);
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