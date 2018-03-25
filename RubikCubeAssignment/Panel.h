#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <stdlib.h>		// Included for the random number generator routines.
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <vector>
#include <algorithm>
#include <functional>

#pragma once

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

class CUBIE_PANEL
{
	public:
		LPDIRECT3DDEVICE9 render_target_;
		float length = 1.833; //1.833 cm = 0.722 in
		float width = 1.905; //1.905 cm = 0.75 in
		DWORD brick_colour;

		~CUBIE_PANEL()
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
			/*
			D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			RenderStuds(matRotateY, WorldMat, TranslateMat2, 2);
			RenderCube2X4(matRotateY, WorldMat, TranslateMat, 2);
			*/
			/*
			D3DXMatrixMultiply(&WorldMat, &WorldMat, &TranslateMat3);
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			
			D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);

			RenderCube2X4(matRotateY, WorldMat, TranslateMat);
					
			D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrice
			render_target_->SetTransform(D3DTS_WORLD, &WorldMat);
			*/
		}

		void renderWithTranslate(D3DXMATRIX matRotateY, D3DXMATRIX WorldMat, D3DXMATRIX TranslateMat,
			D3DXMATRIX TranslateMat2, int countx, int county, int countz, float x = 0, float y = 0, float z = 0)
		{
			RenderStudsWithTranslate(matRotateY, WorldMat, TranslateMat2, MakeTranslateMatrix(countx, county, countz, x, y, z));

			RenderCubeWithTranslate(matRotateY, WorldMat, TranslateMat, MakeTranslateMatrix(countx, county, countz, x, y, z));
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