#include "jGlobalStruct.h"
#include "jLog.h"
#include "jUtils.h"

unsigned int MyRes_CreateLayout::GetStride(int slotIndex)
{
	unsigned int stride = 0;
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	for (int i = 0; i < head.reserve1; ++i)
	{
		if (pLayout[i].InputSlot != slotIndex)
			continue;

		if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT)
			stride += sizeof(float) * 3;
		else if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT)
			stride += sizeof(char) * 4;
		else if (pLayout[i].Format == DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
			stride += sizeof(char) * 4;
		else
			_warn();
	}
	return stride;
}

unsigned int MyRes_CreateLayout::GetTextureOffset(int _index)
{
	D3D11_INPUT_ELEMENT_DESC *pLayout = (D3D11_INPUT_ELEMENT_DESC *)data;
	string nameString = data + (sizeof(D3D11_INPUT_ELEMENT_DESC) * head.reserve1);
	vector<string> rets;
	jUtils::Split(nameString, "_", rets);

	for (int i = 0; i < rets.size(); ++i)
	{
		if (rets[i] != "TEXCOORD")
			continue;

		if (pLayout[i].SemanticIndex != _index)
			continue;

		return pLayout[i].AlignedByteOffset;
	}
	return 0;
}
