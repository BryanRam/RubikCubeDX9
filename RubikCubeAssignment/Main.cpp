//-----------------------------------------------------------------------------
// Basic Graphics Drawing Using Direct 3D
// Draw the a filled circle using a TRIANGLEFAN primitive.

#define D3D_DEBUG_INFO

//-----------------------------------------------------------------------------
// Include these files
#include <Windows.h>	// Windows library (for window functions, menus, dialog boxes, etc)
#include <d3dx9.h>		// Direct 3D library (for all Direct 3D funtions).
#include <math.h>
#include "Cubie.h"


//-----------------------------------------------------------------------------
// Global variables

LPDIRECT3D9             g_pD3D           = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL; // The rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer  = NULL; // Buffer to hold vertices for the rectangle
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer2 = NULL; // Buffer to hold vertices for the rectangle
LPDIRECT3DVERTEXBUFFER9 g_cVertexBuffer = NULL; // Buffer to hold vertices for the rectangle
LPDIRECT3DINDEXBUFFER9 i_buffer;
FLOAT g_CameraZ = -25.0f;

float yIndex = 0.0f;
float xIndex = 0.0f;
float yIndex2 = 0.0f;
float xIndex2 = 0.0f;

float xIndexHRow1 = 0.0f;
float yIndexVRow1 = 0.0f;

/*
Define Rubik's Cube block
*/
CUBIE cubie(Black);

static float g_RotationAngle = 0.0f;
static float g_RotationAngleH2 = 0.0f;
static float g_RotationAngleH3 = 0.0f;

static float g_RotationAngleV1 = 0.0f;
static float g_RotationAngleV2 = 0.0f;
static float g_RotationAngleV3 = 0.0f;

static float g_RotationAngleZ1 = 0.0f;
static float g_RotationAngleZ2 = 0.0f;
static float g_RotationAngleZ3 = 0.0f;

static int count = 0;
static int countH2 = 0;
static int countH3 = 0;

static int countV1 = 0;
static int countV2 = 0;
static int countV3 = 0;

static int countZ1 = 0;
static int countZ2 = 0;
static int countZ3 = 0;

static int canRotateX = 0;
static int canRotateY = 0;
static int canRotateZ = 0;

static int vRowPos1[27];
static int vRowPosTest[27];
static int vRowTemp[3][3];
static int vRowTempTest[3][3];
static int vRowL[9];

static int hRowTop[3][3];
static int hRowTemp[3][3];
static bool isY[9];
static int hRowTopTest[9];
static int hRowTTTemp[3][3];
static int hRowMidTest[9];
static int hRowMTTemp[3][3];
static int hRowBotTest[9];
static int hRowBTTemp[3][3];

static int hRowMid[3][3];
static int hRowTemp2[3][3];

static int hRowBottom[3][3];
static int hRowTemp3[3][3];

bool isRotating = false;
bool isRotatingH2 = false;
bool isRotatingH3 = false;

bool isRotatingV1 = false;
bool isRotatingV2 = false;
bool isRotatingV3 = false;

bool isRotatingZ1 = false;
bool isRotatingZ2 = false;
bool isRotatingZ3 = false;

bool cw, ccw = false;



// The structure of a vertex in our vertex buffer...
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Initialise Direct 3D.
// Requires a handle to the window in which the graphics will be drawn.

HRESULT SetupD3D(HWND hWnd)
{
    // Create the D3D object, return failure if this can't be done.
    if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if (FAILED(g_pD3D -> CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice)))
    {
        return E_FAIL;
    }

	// Turn on the Z buffer
	g_pd3dDevice -> SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn off the lighting, as we're using our own vertex colours.
	//g_pd3dDevice -> SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	cubie.initialise(g_pd3dDevice);
	

    return S_OK;
}

//-----------------------------------------------------------------------------
// Release (delete) all the resources used by this program.
// Only release things if they are valid (i.e. have a valid pointer).
// If not, the program will crash at this point.

void CleanUp()
{
    if (g_pVertexBuffer != NULL) g_pVertexBuffer -> Release();
	if (g_pVertexBuffer2 != NULL) g_pVertexBuffer2->Release();
	if (g_cVertexBuffer != NULL) g_cVertexBuffer->Release();
    if (g_pd3dDevice != NULL)	 g_pd3dDevice -> Release();
    if (g_pD3D != NULL)			 g_pD3D -> Release();
}

//-----------------------------------------------------------------------------
// Set up the scene geometry.

HRESULT SetupGeometry()
{
	if (
		cubie.SetupCubie() == S_OK 
		)
	{
			return S_OK;
		
	}
	return E_FAIL;
}



//-----------------------------------------------------------------------------
// Set up the view - the view and projection matrices.

void SetupViewMatrices()
{
	// Set up the view matrix.
	// This defines which way the 'camera' will look at, and which way is up.
    D3DXVECTOR3 vCamera(0.0f, 0.0f, g_CameraZ);
    D3DXVECTOR3 vLookat(0.0f, 0.0f, g_CameraZ + 10);
    D3DXVECTOR3 vUpVector(0.0f, 1.0f, 0.0f);
    D3DXMATRIX matView, matRotate, toCenter, fromCenter, 
				yawMatrix, pitchMatrix, rollMatrix;
	D3DXVECTOR3 rightVector;

	// Setup matrices to rotate around axes
	D3DXMatrixRotationAxis(&yawMatrix, &vUpVector, xIndex2);
	D3DXMatrixRotationAxis(&pitchMatrix, &rightVector, yIndex2);
	D3DXMatrixRotationAxis(&rollMatrix, &vLookat, 0.0f);

	//move lookat vector
	D3DXVec3TransformCoord(&vLookat, &vLookat, &pitchMatrix);
	D3DXVec3TransformCoord(&vLookat, &vLookat, &yawMatrix);
	
	D3DXVec3Normalize(&vLookat, &vLookat);

	//use lookat vector to define right vector
	D3DXVec3Cross(&rightVector, &vUpVector, &vLookat);
	D3DXVec3Normalize(&rightVector, &rightVector);

	D3DXVec3Cross(&vUpVector, &vLookat, &rightVector);
	
	D3DXMatrixRotationYawPitchRoll(&matRotate, xIndex2, yIndex2, 0);
	

	D3DXMatrixIdentity(&matView);


    D3DXMatrixLookAtLH( &matView, &vCamera, &vLookat, &vUpVector);
    g_pd3dDevice -> SetTransform(D3DTS_VIEW, &matView);

	//http://www.chadvernon.com/blog/resources/directx9/moving-around-a-3d-world/
	//http://astronomy.swin.edu.au/~pbourke/geometry/rotate/

	// Set up the projection matrix.
	// This transforms 2D geometry into a 3D space.
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 600.0f);
    g_pd3dDevice -> SetTransform(D3DTS_PROJECTION, &matProj);
}


//-----------------------------------------------------------------------------
// Define the light for the scene.

void SetupLightsAndMaterials()
{
	// Define a material.
	// Reflects only diffuse colour.
	D3DMATERIAL9 Mtl;
	ZeroMemory(&Mtl, sizeof(D3DMATERIAL9));
	Mtl.Diffuse.r = 1.0f;
	Mtl.Diffuse.g = 1.0f;
	Mtl.Diffuse.b = 1.0f;
	Mtl.Diffuse.a = 1.0f;
	g_pd3dDevice->SetMaterial(&Mtl);

	// Define a light.
	// Possesses only a diffuse colour.
	D3DLIGHT9 Light1;
	ZeroMemory(&Light1, sizeof(D3DLIGHT9));
	Light1.Type = D3DLIGHT_POINT;

	Light1.Diffuse.r = 1.2f;
	Light1.Diffuse.g = 1.2f;
	Light1.Diffuse.b = 1.2f;

	Light1.Position.x = 0.0f;
	Light1.Position.y = 50.0f;
	Light1.Position.z = -50.0f;

	Light1.Attenuation0 = 1.0f;
	Light1.Attenuation1 = 0.0f;
	Light1.Attenuation2 = 0.0f;

	Light1.Range = 200.0f;

	// Select and enable the light.
	g_pd3dDevice->SetLight(0, &Light1);
	g_pd3dDevice->LightEnable(0, TRUE);
}

//-----------------------------------------------------------------------------
// Render the scene.

void Render()
{
	D3DXMATRIX matRotateY, sideMat, matRotateH, matRotateV, matRotateH2, matRotateH3;    // a matrix to store the rotation information
							  // Construct various matrices to move and expand the triangle the rectangle.
	D3DXMATRIX WorldMat, WorldMat2, TranslateMat, TranslateMat2, TranslateMat3;


	
	static float index = 0.0f;
												 

	// tell Direct3D about our matrix
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matRotateY);


    // Clear the backbuffer to a dark blue colour.
    g_pd3dDevice -> Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0);

    // Begin the scene...
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Define the viewpoint.
		SetupViewMatrices();

		xIndexHRow1 = g_RotationAngle;
		

		D3DXMatrixRotationYawPitchRoll(&matRotateY, xIndex, yIndex, 0);
		
		D3DXMatrixTranslation(&TranslateMat, 0.0f, 0.0f, 5.0f);
		D3DXMatrixIdentity(&WorldMat);								// Set WorldMat to identity matrix

		
		cubie.setY(g_RotationAngleV1);
		cubie.setY2(g_RotationAngleV2);
		cubie.setY3(g_RotationAngleV3);

		cubie.setZ(g_RotationAngleZ1);
		cubie.setZ2(g_RotationAngleZ2);
		cubie.setZ3(g_RotationAngleZ3);

		cubie.SetX1(xIndexHRow1);
		cubie.SetX2(g_RotationAngleH2);
		cubie.SetX3(g_RotationAngleH3);
		
		
		cubie.updateTheBillboard(canRotateY, canRotateX, canRotateZ);
		
		cubie.drawTheUI();
		int zVal = 0;
		for (int i = 0; i < 27; i++)
		{
			if (i < 3)
				zVal = 0;
			else if (i % 3 == 0)
				++zVal;
			if (zVal > 2)
				zVal = 0;
			cubie.render(matRotateY, 
				WorldMat, TranslateMat, 
				zVal, i);
		}
		
		

		

        // End the scene.
        g_pd3dDevice -> EndScene();

		/*Check each section for a rotation event.
		If a rotation occurs, increase/decrease the rotation angle value, which will guide the animation for that rotation
		*/

		if (isRotating)	
		{
			if (cw) //If rotation event is clockwise
			{
				if (g_RotationAngle >= (D3DX_PI / 2 * (count + 1))) //Stop animation when rotation angle is greater than or equal to 90 degrees
				{
					isRotating = !isRotating;
					cw = !cw;
					

					g_RotationAngle = 0; //reset angle, guaranteeing that the rotating segment always stops in place
					
					
					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							hRowTop[i][j] = hRowTemp[3 - j - 1][i]; //rotate clockwise 90 degrees
							hRowTopTest[(i * 3) + (j)] = hRowTTTemp[j][3 - i - 1]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							
							if (i == 0)
								vRowPos1[j] = hRowTop[i][j];
							else
								vRowPos1[(i * 3) + j] = hRowTop[i][j];
						}
					}

					for (int i = 0; i < 3; i++)
					{
						vRowL[i] = hRowTopTest[(i * 3)];
						vRowTempTest[0][i] = hRowTopTest[(i * 3)];
					}
					for (int i = 0; i < 9; i++)
					{
						vRowPosTest[i] = hRowTopTest[i];
						
					}
					
					cubie.changeTexturesHCW(hRowTopTest, 0);
				}
				else
				{
					g_RotationAngle += 0.035f;
				}
			}
			if (ccw) //If rotation event is counter-clockwise
			{
				if (g_RotationAngle <= (D3DX_PI / 2 * (count-1))) 
				{
					isRotating = !isRotating;
					ccw = !ccw;
					
					g_RotationAngle = 0;
					
					for (int i = 0; i<3; ++i) {
						for (int j = 0; j<3; ++j) {
							hRowTop[i][j] = hRowTemp[j][3 - i - 1]; //rotate counter-clockwise 90 degrees
							hRowTopTest[(i * 3) + (j)] = hRowTTTemp[j][3 - i - 1]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							if (i == 0)
								vRowPos1[j] = hRowTop[i][j];
							else
								vRowPos1[(i * 3) + j] = hRowTop[i][j];
						}
					}
					
					cubie.changeTexturesHCCW(hRowTopTest, 0);
				}
				else
				{
					g_RotationAngle -= 0.035f;
				}
			}
		}

		else if (isRotatingH2)
		{
			if (cw)
			{
				if (g_RotationAngleH2 >= (D3DX_PI / 2 * (countH2+1)))
				{
					cw = !cw;
					isRotatingH2 = !isRotatingH2;
					
					g_RotationAngleH2 = 0;

					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							hRowMid[i][j] = hRowTemp2[3 - j - 1][i]; //rotate clockwise 90 degrees
							hRowMidTest[(i * 3) + (j)] = hRowMTTemp[3 - j - 1][i]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 3; i < 6; ++i) {
						for (int j = 0; j < 3; ++j) {
							vRowPos1[(i * 3) + j] = hRowMid[i % 3][j];
						}
					}
					
					cubie.changeTexturesHCW(hRowMidTest, 1);
				}
				else
				{
					g_RotationAngleH2 += 0.035f;
				}
			}
			if (ccw)
			{
				if (g_RotationAngleH2 <= (D3DX_PI / 2 * (countH2-1)))
				{
					isRotatingH2 = !isRotatingH2;
					ccw = !ccw;
					
					g_RotationAngleH2 = 0;

					for (int i = 0; i<3; ++i) {
						for (int j = 0; j<3; ++j) {
							hRowMid[i][j] = hRowTemp2[j][3 - i - 1]; //rotate counter-clockwise 90 degrees
							hRowMidTest[(i * 3) + (j)] = hRowMTTemp[j][3 - i - 1]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 3; i < 6; ++i) {
						for (int j = 0; j < 3; ++j) {
							/*if (i == 3)
								vRowPos1[i*3] = hRowMid[(i / 3) - 1][j];
							else*/
								vRowPos1[(i * 3) + j] = hRowMid[(i % 3)][j];
						}
					}
					
					cubie.changeTexturesHCCW(hRowMidTest, 1);
				}
				else
				{
					g_RotationAngleH2 -= 0.035f;
				}
			}
		}

		else if (isRotatingH3)
		{
			if (cw)
			{
				if (g_RotationAngleH3 >= (D3DX_PI / 2 * (countH3+1)))
				{
					isRotatingH3 = !isRotatingH3;
					cw = !cw;
					
					g_RotationAngleH3 = 0;
					
					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							hRowBottom[i][j] = hRowTemp3[3 - j - 1][i]; //rotate clockwise 90 degrees
							hRowBotTest[(i * 3) + (j)] = hRowBTTemp[3 - j - 1][i]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 6; i < 9; ++i) {
						for (int j = 0; j < 3; ++j) {
							vRowPos1[(i * 3) + j] = hRowBottom[i % 3][j];
						}
					}
					
					cubie.changeTexturesHCW(hRowBotTest, 2);
				}
				else
				{
					g_RotationAngleH3 += 0.035f;
				}
			}
			if(ccw)
			{
				if (g_RotationAngleH3 <= (D3DX_PI / 2 * (countH3 - 1)))
				{
					isRotatingH3 = !isRotatingH3;
					ccw = !ccw;
					
					g_RotationAngleH3 = 0;

					for (int i = 0; i<3; ++i) {
						for (int j = 0; j<3; ++j) {
							hRowBottom[i][j] = hRowTemp3[j][3 - i - 1]; //rotate counter-clockwise 90 degrees
							hRowBotTest[(i * 3) + (j)] = hRowBTTemp[j][3 - i - 1]; //rotate clockwise 90 degrees
						}
					}

					for (int i = 6; i < 9; ++i) {
						for (int j = 0; j < 3; ++j) {
							
							vRowPos1[(i * 3) + j] = hRowBottom[(i % 3)][j];
						}
					}
					
					cubie.changeTexturesHCCW(hRowBotTest, 2);
				}
				else
				{
					g_RotationAngleH3 -= 0.035f;
				}

			}
			
		}

		else if (isRotatingV1)
		{
			if (cw)
			{
				if (g_RotationAngleV1 <= (D3DX_PI / 2 * (countV1 - 1)))
				{
					isRotatingV1 = !isRotatingV1;
					

					g_RotationAngleV1 = 0;
					cubie.changeTexturesVCW(0);
				}
				else
				{
					g_RotationAngleV1 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleV1 >= (D3DX_PI / 2 * (countV1 + 1)))
				{
					isRotatingV1 = !isRotatingV1;
					

					g_RotationAngleV1 = 0;

					for (int i = 0; i < 3; ++i) {

						for (int j = 0; j < 3; ++j) {
							vRowPosTest[(i * 9) + (j * 3)] = vRowTempTest[3 - j - 1][i];
							vRowPos1[(i * 9) + (j * 3)] = vRowTemp[3 - j - 1][i]; //rotate counter-clockwise 90 degrees
						}
					}

					for (int i = 0; i < 9; i++)
					{
						//if(i>2)
						vRowL[i] = vRowPosTest[i * 3];

						if (i == 0 && i < 3)
						{
							hRowTopTest[i] = vRowPosTest[i];
							

						}
						else if (i < 3)
						{
							hRowTopTest[i * 3] = vRowPosTest[i * 3];
							
						}
					}
					
					cubie.changeTexturesVCCW(vRowL, 0);
					
				}
				else
				{
					g_RotationAngleV1 += 0.035f;
				}
			}
		}


		else if (isRotatingV2)
		{
			if (cw)
			{
				if (g_RotationAngleV2 <= (D3DX_PI / 2 * (countV2 - 1)))
				{
					isRotatingV2 = !isRotatingV2;
					

					g_RotationAngleV2 = 0;
					cubie.changeTexturesVCW(1);
				}
				else
				{
					g_RotationAngleV2 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleV2 >= (D3DX_PI / 2 * (countV2 + 1)))
				{
					isRotatingV2 = !isRotatingV2;
					

					g_RotationAngleV2 = 0;


					cubie.changeTexturesVCCW(vRowL, 1);

				}
				else
				{
					g_RotationAngleV2 += 0.035f;
				}
			}
		}

		else if (isRotatingV3) 
		{
			if (cw)
			{
				if (g_RotationAngleV3 <= (D3DX_PI / 2 * (countV3 - 1)))
				{
					isRotatingV3 = !isRotatingV3;
					

					g_RotationAngleV3 = 0;
					cubie.changeTexturesVCW(2);
				}
				else
				{
					g_RotationAngleV3 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleV3 >= (D3DX_PI / 2 * (countV3 + 1)))
				{
					isRotatingV3 = !isRotatingV3;
					

					g_RotationAngleV3 = 0;


					cubie.changeTexturesVCCW(vRowL, 2);

				}
				else
				{
					g_RotationAngleV3 += 0.035f;
				}
			}
		}

		else if (isRotatingZ1)
		{
			if (cw)
			{
				if (g_RotationAngleZ1 <= (D3DX_PI / 2 * (countZ1 - 1)))
				{
					isRotatingZ1 = !isRotatingZ1;
					

					g_RotationAngleZ1 = 0;
					cubie.changeTexturesZCW(0);
				}
				else
				{
					g_RotationAngleZ1 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleZ1 >= (D3DX_PI / 2 * (countZ1 + 1)))
				{
					isRotatingZ1 = !isRotatingZ1;
					

					g_RotationAngleZ1 = 0;

				
					cubie.changeTexturesZCCW(0);
					
				}
				else
				{
					g_RotationAngleZ1 += 0.035f;
				}
			}
		}

		else if (isRotatingZ2)
		{
			if (cw)
			{
				if (g_RotationAngleZ2 <= (D3DX_PI / 2 * (countZ2 - 1)))
				{
					isRotatingZ2 = !isRotatingZ2;


					g_RotationAngleZ2 = 0;
					cubie.changeTexturesZCW(1);
				}
				else
				{
					g_RotationAngleZ2 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleZ2 >= (D3DX_PI / 2 * (countZ2 + 1)))
				{
					isRotatingZ2 = !isRotatingZ2;


					g_RotationAngleZ2 = 0;


					cubie.changeTexturesZCCW(1);

				}
				else
				{
					g_RotationAngleZ2 += 0.035f;
				}
			}
		}

		else if (isRotatingZ3)
		{
			if (cw)
			{
				if (g_RotationAngleZ3 <= (D3DX_PI / 2 * (countZ3 - 1)))
				{
					isRotatingZ3 = !isRotatingZ3;


					g_RotationAngleZ3 = 0;
					cubie.changeTexturesZCW(2);
				}
				else
				{
					g_RotationAngleZ3 -= 0.035f;
				}
			}

			if (ccw)
			{
				if (g_RotationAngleZ3 >= (D3DX_PI / 2 * (countZ3 + 1)))
				{
					isRotatingZ3 = !isRotatingZ3;


					g_RotationAngleZ3 = 0;


					cubie.changeTexturesZCCW(2);

				}
				else
				{
					g_RotationAngleZ3 += 0.035f;
				}
			}
		}
    }



    // Present the backbuffer to the display.
    g_pd3dDevice -> Present(NULL, NULL, NULL, NULL);
}




//-----------------------------------------------------------------------------
// The window's message handling function.

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
		{
            PostQuitMessage(0);
            return 0;
		}
		/*
		WASD keys to rotate the model
		Keys R and F to zoom in and out
		*/

		case WM_CHAR:
			switch (wParam)
			{
				//Whole Cube Movements
			case 'w':
				yIndex += 0.1f;
				return 0;
				break;

			case 's':
				yIndex -= 0.1f;
				return 0;
				break;

			case 'a':
				xIndex -= 0.1f;
				return 0;
				break;

			case 'd':
				xIndex += 0.1f;
				return 0;
				break;

				//Zooming
			case 'r':
				g_CameraZ += 10.0f;
				return 0;
				break;

			case 'f':
				g_CameraZ -= 10.0f;
				return 0;
				break;

				//Horizontal Rotations
			case 'z':
				if (canRotateX == 0)
				{
					isRotating = !isRotating;
					cw = true;
					ccw = false;
				}
				else if (canRotateX == 1)
				{
					isRotatingH2 = !isRotatingH2;
					cw = true;
					ccw = false;
				}
				else if (canRotateX == 2)
				{
					isRotatingH3 = !isRotatingH3;
					cw = true;
					ccw = false;
				}
				return 0;
				break;

			case 'x':
				if (canRotateX == 0)
				{
					isRotating = !isRotating;
					ccw = true;
					cw = false;
				}
				else if (canRotateX == 1)
				{
					isRotatingH2 = !isRotatingH2;
					ccw = true;
					cw = false;
				}
				else if (canRotateX == 2)
				{
					isRotatingH3 = !isRotatingH3;
					ccw = true;
					cw = false;
				}
				return 0;
				break;

				//Vertical Rotations based on 9 Blocks in a single column, the vCol
			case 'c':
				if (canRotateY == 0)
				{
					isRotatingV1 = !isRotatingV1;
					cw = true;
					ccw = false;
				}
				else if (canRotateY == 1)
				{
					isRotatingV2 = !isRotatingV2;
					cw = true;
					ccw = false;
				}
				else if (canRotateY == 2)
				{
					isRotatingV3 = !isRotatingV3;
					cw = true;
					ccw = false;
				}
				return 0;
				break;

			case 'v':
				if (canRotateY == 0)
				{
					isRotatingV1 = !isRotatingV1;
					ccw = true;
					cw = false;
				}
				else if (canRotateY == 1)
				{
					isRotatingV2 = !isRotatingV2;
					ccw = true;
					cw = false;
				}
				else if (canRotateY == 2)
				{
					isRotatingV3 = !isRotatingV3;
					ccw = true;
					cw = false;
				}
				return 0;
				break;

				//Vertical Rotations based on 9 Blocks in a single face, the ZCol
			case 'b':
				if (canRotateZ == 0)
				{
					isRotatingZ1 = !isRotatingZ1;
					cw = true;
					ccw = false;
				}
				else if (canRotateZ == 1)
				{
					isRotatingZ2 = !isRotatingZ2;
					cw = true;
					ccw = false;
				}
				else if (canRotateZ == 2)
				{
					isRotatingZ3 = !isRotatingZ3;
					cw = true;
					ccw = false;
				}
				return 0;
				break;
			
			case 'n':
				if (canRotateZ == 0)
				{
					isRotatingZ1 = !isRotatingZ1;
					ccw = true;
					cw = false;
				}
				else if (canRotateZ == 1)
				{
					isRotatingZ2 = !isRotatingZ2;
					ccw = true;
					cw = false;
				}
				else if (canRotateZ == 2)
				{
					isRotatingZ3 = !isRotatingZ3;
					ccw = true;
					cw = false;
				}
				return 0;
				break;

			

			//=============================================
			}

		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_LEFT:
					// Process the LEFT ARROW key. 
					// Process the UP ARROW key.
					if (canRotateY == 0)
						canRotateY = 2;
					else
						--canRotateY;

					break;

				case VK_RIGHT:
					// Process the RIGHT ARROW key. 
					if (canRotateY == 2)
						canRotateY = 0;
					else
						++canRotateY;
					break;

				case VK_UP:
					// Process the UP ARROW key.
					if (canRotateX == 0)
						canRotateX = 2;
					else
						--canRotateX;
					 

					break;

				case VK_DOWN:
					// Process the DOWN ARROW key. 
					if (canRotateX == 2)
						canRotateX = 0;
					else
						++canRotateX;

					break;

				case VK_HOME:
					// Process the HOME key. 
					if (canRotateZ == 2)
						canRotateZ = 0;
					else
						++canRotateZ;

					break;

				case VK_END:
					// Process the END key.
					if (canRotateZ == 0)
						canRotateZ = 2;
					else
						--canRotateZ;

					break;


			}
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// WinMain() - The application's entry point.
// This sort of procedure is mostly standard, and could be used in most
// DirectX applications.

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    // Register the window class
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                     GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                     "Basic D3D Example", NULL};
    RegisterClassEx(&wc);

    // Create the application's window
    HWND hWnd = CreateWindow( "Basic D3D Example", "Rubik's Cube Assignment",
                              WS_OVERLAPPEDWINDOW, 150, 150, 1200, 800,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (SUCCEEDED(SetupD3D(hWnd)))
    {
        // Create the scene geometry
        if (SUCCEEDED(SetupGeometry()))
        {
			cubie.LoadTextures();
			cubie.SetupTextureCoords();
			for (int i = 0; i < 27; i++)
			{
				vRowPosTest[i] = i;
				
				if (i < 9)
				{
					hRowTopTest[i] = i;
					isY[i] = true;
				}
				else if (i > 8 && i < 18)
					hRowMidTest[i - 9] = i;
				else if (i > 17 && i < 27)
					hRowBotTest[i - 18] = i;
				if (i % 3 == 0)
				{
					vRowPos1[i] = 3;
					vRowL[i/3] = i;
				}
				else
					vRowPos1[i] = 1;
			}

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					vRowTemp[i][j] = vRowPos1[(i * 9) + (j * 3)];
					vRowTempTest[i][j] = vRowPosTest[(i * 9) + (j * 3)];
				}
			}

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					hRowTTTemp[i][j] = hRowTopTest[(i * 3) + (j)];
					hRowMTTemp[i][j] = hRowMidTest[(i * 3) + (j)];
					hRowBTTemp[i][j] = hRowBotTest[(i * 3) + (j)];

					if (j == 0)
					{
						hRowTop[i][j] = hRowMid[i][j] = hRowBottom[i][j] = 3;
						hRowTemp[i][j] = hRowTemp2[i][j] = hRowTemp3[i][j] = hRowTop[i][j];

					}
					else
					{
						hRowTop[i][j] = hRowMid[i][j] = hRowBottom[i][j] = 1;
						hRowTemp[i][j] = hRowTemp2[i][j] = hRowTemp3[i][j] = hRowTop[i][j];
					}
				}
			}

            // Show the window
            ShowWindow(hWnd, SW_SHOWDEFAULT);
            UpdateWindow(hWnd);
			
			//Set up the light
			SetupLightsAndMaterials();

            // Enter the message loop
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while (msg.message != WM_QUIT)
            {
                if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                    Render();
            }
        }
		CleanUp();
    }

    UnregisterClass("Basic D3D Example", wc.hInstance);
    return 0;
}
