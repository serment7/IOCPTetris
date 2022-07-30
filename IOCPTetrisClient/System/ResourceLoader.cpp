#include "pch.h"
#include "ResourceLoader.h"

ResourceLoader::ResourceLoader(ID3D11Device1* _device)
	:device(_device)
{

}

ResourceLoader::~ResourceLoader()
{
	for (auto resource : resourceMap)
	{
		SAFE_DELETE(resource.second);
	}
}

void ResourceLoader::Init()
{
	short resourceIndex = (short)IOCPTetris::ResourceID::NONE;
	resourceIndex += 1;

	for (; resourceIndex < (short)IOCPTetris::ResourceID::END; ++resourceIndex)
	{
		IOCPTetris::ResourceID resourceID = (IOCPTetris::ResourceID)resourceIndex;
		const auto& resourceInfo = IOCPTetris::RESOURCE_DIC.at(resourceID);
		LoadResource(resourceID, resourceInfo.path, BLOCKSIZE, BLOCKSIZE, resourceInfo.rect);;
	}
}

void ResourceLoader::LoadResource(IOCPTetris::ResourceID resourceID, const wchar_t* path, int width, int height, RECT rect)
{
	TCHAR fullPath[256];
	wsprintf(fullPath, L"%s/%s", IOCPTetris::RESOURCE_ROOT_PATH, path);

	ResourceObject* result = new ResourceObject;

	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, fullPath,
			resource.GetAddressOf(),
			result->texture.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));

	result->origin.x = 0;
	result->origin.y = 0;
	result->tile = rect;
	resourceMap[resourceID] = result;
}

ResourceObject* ResourceLoader::GetResource(IOCPTetris::ResourceID resourceID)
{
	if (resourceMap.find(resourceID) != resourceMap.end())
	{
		return resourceMap.at(resourceID);
	}
	return nullptr;
}

void ResourceLoader::Reset()
{
	for (auto resource : resourceMap)
	{
		resource.second->texture.Reset();
	}
}
