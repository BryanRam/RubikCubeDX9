/*
-----------------------------------------
* Game hacking QTS ( Quickie Tip Series ) 
* no. 17 - Primitive rounded rectangle drawing function
-----------------------------------------
* Author: SEGnosis 	- GHAnon.net
* Thanks to:
* bitterbanana		- No known site
* Drunken Cheetah 	- No known site
* fatboy88 		- No known site
* Geek4Ever 		- No known site
* learn_more 		- www.uc-forum.com
* Novocaine 		- http://ilsken.net/blog/?page_id=64
* Philly0494 		- No known site
* Roverturbo 		- www.uc-forum.com
* SilentKarma 		- www.halocoders.com - offline
* Strife 		- www.uc-forum.com
* Wieter20 		- No known site
*/

//----------------------------------//
void Rounded( int x, int y, int w, int h, int iSmooth, D3DCOLOR Color )
{
	POINT pt[ 4 ];

	// Get all corners
	pt[ 0 ].x = x + ( w - iSmooth );
	pt[ 0 ].y = y + ( h - iSmooth );

	pt[ 1 ].x = x + iSmooth;
	pt[ 1 ].y = y + ( h - iSmooth );

	pt[ 2 ].x = x + iSmooth;
	pt[ 2 ].y = y + iSmooth;

	pt[ 3 ].x = x + w - iSmooth;
	pt[ 3 ].y = y + iSmooth;


	// Draw cross
	CDraw.Rect( x, y + iSmooth, w, h - iSmooth * 2, Color );

	CDraw.Rect( x + iSmooth, y, w - iSmooth * 2, h, Color );


	float fDegree = 0;

	for( int i = 0; i < 4; i++ )
	{
		for( float k = fDegree; k < fDegree + PI_SQUARED/4; k += D3DXToRadian( 1 ) )
		{
			// Draw quarter circles on every corner
			CDraw.Line( pt[ i ].x, pt[ i ].y, 
				pt[ i ].x + ( cosf( k ) * iSmooth ),
				pt[ i ].y + ( sinf( k ) * iSmooth ), 
				3, Color ); // 3 is with line width
		}

		fDegree += PI_SQUARED/4; // quarter circle offset
	}

}
//----------------------------------//