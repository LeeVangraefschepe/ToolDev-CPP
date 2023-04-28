#pragma once
#include <memory>
#include <string>
#include "Structs.h"

struct BlockData
{
	BlockData(const std::wstring& pName, bool pIsOpaque) : name(pName), isOpaque(pIsOpaque)
	{
	}
	const std::wstring name;
	const bool isOpaque;
};

struct Block
{
	std::shared_ptr<BlockData> data;
	Vector3 position;
};