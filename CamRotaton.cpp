bool CameraNode::Initialise(D3DXVECTOR3 pos, float yawDeg, float pitchDeg, float rollDeg)
{
	position = pos;
	//convert degrees into radians
	yawRad = D3DXToRadian(yawDeg);
	pitchRad = D3DXToRadian(pitchDeg);
	rollRad = D3DXToRadian(rollDeg);
	
	//define direction vectors
	upVector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	rightVector = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	lookAtVector = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	
	// Setup matrices to rotate around axes
	D3DXMatrixRotationAxis(&yawMatrix, &upVector, yawRad);
	D3DXMatrixRotationAxis(&pitchMatrix, &rightVector, pitchRad);
	D3DXMatrixRotationAxis(&rollMatrix, &lookAtVector, rollRad);
	
	//move lookat vector
	D3DXVec3TransformCoord(&lookAtVector, &lookAtVector, &yawMatrix);
	D3DXVec3TransformCoord(&lookAtVector, &lookAtVector, &pitchMatrix);
	D3DXVec3Normalize(&lookAtVector, &lookAtVector);
	
	//use lookat vector to define right vector
	D3DXVec3Cross(&rightVector, &upVector, &lookAtVector);
	D3DXVec3Normalize(&rightVector, &rightVector);
	
	//use vectors to define up vector
	D3DXVec3Cross(&upVector, &lookAtVector, &rightVector);
	
	// SET UP VIEW MATRIX
	D3DXMatrixIdentity(&viewMatrix);
	
	viewMatrix._11 = rightVector.x;
	viewMatrix._21 = rightVector.y;
	viewMatrix._31 = rightVector.z;
	
	viewMatrix._12 = upVector.x;
	viewMatrix._22 = upVector.y;
	viewMatrix._32 = upVector.z;
	
	viewMatrix._13 = lookAtVector.x;
	viewMatrix._23 = lookAtVector.y;
	viewMatrix._33 = lookAtVector.z;
	
	viewMatrix._41 = -D3DXVec3Dot(&position, &rightVector);
	viewMatrix._42 = -D3DXVec3Dot(&position, &upVector);
	viewMatrix._43 = -D3DXVec3Dot(&position, &lookAtVector);
	
	return true;
	
}
