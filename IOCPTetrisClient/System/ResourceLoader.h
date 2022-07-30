#pragma once

#include "pch.h"

#include <unordered_map>

using namespace DirectX;
using namespace Microsoft::WRL;

class ResourceLoader
{
public:
	ResourceLoader(ID3D11Device1* _device);
	~ResourceLoader();
	void Init();

	void LoadResource(IOCPTetris::ResourceID resourceID, const wchar_t* path, int width, int height, RECT rect);

	ResourceObject* GetResource(IOCPTetris::ResourceID resourceID);

	void Reset();

private:
	std::unordered_map<IOCPTetris::ResourceID, ResourceObject*> resourceMap;
	ID3D11Device1* device;
};
