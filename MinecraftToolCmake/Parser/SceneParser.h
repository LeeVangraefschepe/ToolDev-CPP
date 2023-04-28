#pragma once
#include <sstream>
#include <vector>
#include "Block.h"

class SceneParser final
{
public:
	SceneParser(const std::wstring& path);
	SceneParser(const SceneParser& other) = delete;
	SceneParser(SceneParser&& other) = delete;
	SceneParser& operator=(const SceneParser& other) = delete;
	SceneParser& operator=(SceneParser&& other) = delete;

	bool SaveObj(const std::wstring& path) const;
	std::vector<Block>& GetBlocks() { return m_Blocks; }

private:
	std::vector<Block> m_Blocks{};

	void WriteBlock(FILE* pOFile, const Block& block, int id) const;
};