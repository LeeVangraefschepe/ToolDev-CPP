#include <fstream>
#include "SceneParser.h"

#include <RapidJson/document.h>
#include <RapidJson/filereadstream.h>
#include <RapidJson/istreamwrapper.h>

SceneParser::SceneParser(const std::wstring& path)
{
	if (std::ifstream is{ path.c_str() })
	{
		using namespace rapidjson;
		IStreamWrapper isw{ is };
		Document sceneDoc{};
		sceneDoc.ParseStream(isw);

		if (sceneDoc.IsArray())
		{
			for (auto layerIt = sceneDoc.Begin(); layerIt != sceneDoc.End(); ++layerIt)
			{
				const Value& blockJson = *layerIt;
				if (blockJson.HasMember("layer") && blockJson.HasMember("opaque") && blockJson.HasMember("positions"))
				{

					//Get name/layer
					const Value& layer = blockJson["layer"];
					std::wstringstream layerss{};
					layerss << layer.GetString();

					//Get opaque
					const Value& opaque = blockJson["opaque"];

					//Create blockdata
					auto blockData = std::make_shared<BlockData>(layerss.str(), opaque.GetBool());

					//Get the array of block positions
					const Value& positions = blockJson["positions"];
					for (auto blockIt = positions.Begin(); blockIt != positions.End(); ++blockIt)
					{
						//Create block
						Block block{};
						block.data = blockData;

						//Check valid array data
						const Value& positionJson = *blockIt;
						if (positionJson.Size() != 3) { continue; }

						//Read to vector
						block.position.x = positionJson[0].GetInt();
						block.position.y = positionJson[2].GetInt(); //Reading z as y
						block.position.z = positionJson[1].GetInt(); //Reading y as z
						m_Blocks.push_back(block);
					}
				}
			}
		}
	}
}

bool SceneParser::SaveObj(const std::wstring& path) const
{
	//Create output file and open it
	FILE* pOFile = nullptr;
	_wfopen_s(&pOFile, path.c_str(), L"w+,ccs=UTF-8");
	if (pOFile == nullptr)
	{
		return false;
	}

	//Write beginning
	const wchar_t* text = L"#This tool is made by Lee Vangraefschepe\nmtllib materials.mtl\n";
	fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOFile);

	//Write the normals for every block one time
	fwprintf_s(pOFile, L"vn %d %d %d\n", 0, 0, 1);
	fwprintf_s(pOFile, L"vn %d %d %d\n", 0, 0, -1);
	fwprintf_s(pOFile, L"vn %d %d %d\n", 0, 1, 0);
	fwprintf_s(pOFile, L"vn %d %d %d\n", 0, -1, 0);
	fwprintf_s(pOFile, L"vn %d %d %d\n", 1, 0, 0);
	fwprintf_s(pOFile, L"vn %d %d %d\n", -1, 0, 0);

	const int size = static_cast<int>(m_Blocks.size());
	std::shared_ptr<BlockData> prevData = nullptr;
	for (int i{}; i < size; ++i)
	{
		auto& block = m_Blocks[i];

		//Write material if datatype changed
		if (prevData != block.data)
		{
			std::wstringstream material{};
			material << L"usemtl " << block.data->name << L"\n";
			auto materialStr = material.str();
			fwrite(materialStr.c_str(), materialStr.size() * sizeof(wchar_t), 1, pOFile);
			prevData = block.data;
		}

		//Write block
		WriteBlock(pOFile, block, i);
	}
	fclose(pOFile);
}

void SceneParser::WriteBlock(FILE* pOFile, const Block& block, int id) const
{
	const auto x = block.position.x;
	const auto y = block.position.y;
	const auto z = block.position.z;
	const bool isOpaque = block.data->isOpaque;

	fwprintf_s(pOFile, L"v %d %d %d\n", 0 + x, 0 + y, 0 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 0 + x, 0 + y, 1 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 0 + x, 1 + y, 0 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 0 + x, 1 + y, 1 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 1 + x, 0 + y, 0 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 1 + x, 0 + y, 1 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 1 + x, 1 + y, 0 + z);
	fwprintf_s(pOFile, L"v %d %d %d\n", 1 + x, 1 + y, 1 + z);

	const int offset = id * 8;

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x && otherBlock.position.y == y && otherBlock.position.z == z - 1 && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 2, offset + 7, 2, offset + 5, 2);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 2, offset + 3, 2, offset + 7, 2);
	}

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x - 1 && otherBlock.position.y == y && otherBlock.position.z == z && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 6, offset + 4, 6, offset + 3, 6);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 6, offset + 2, 6, offset + 4, 6);
	}

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x && otherBlock.position.y == y + 1 && otherBlock.position.z == z && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 3, 3, offset + 8, 3, offset + 7, 3);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 3, 3, offset + 4, 3, offset + 8, 3);
	}

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x + 1 && otherBlock.position.y == y && otherBlock.position.z == z && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 5, 5, offset + 7, 5, offset + 8, 5);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 5, 5, offset + 8, 5, offset + 6, 5);
	}

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x && otherBlock.position.y == y - 1 && otherBlock.position.z == z && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 4, offset + 5, 4, offset + 6, 4);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 1, 4, offset + 6, 4, offset + 2, 4);
	}

	if (!isOpaque || std::find_if(begin(m_Blocks), end(m_Blocks),
		[&](const Block& otherBlock) { return otherBlock.position.x == x && otherBlock.position.y == y && otherBlock.position.z == z + 1 && otherBlock.data->isOpaque; })
		== end(m_Blocks))
	{
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 2, 1, offset + 6, 1, offset + 8, 1);
		fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", offset + 2, 1, offset + 8, 1, offset + 4, 1);
	}
}
