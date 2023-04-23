#include <direct.h> // _getwcwd
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>
#include <vector>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/istreamwrapper.h"

struct Vector3
{
	int x{}, y{}, z{};
};

struct BlockData
{
	std::wstring name{};
	bool isOpaque{};
};

struct Block
{
	BlockData data; //Should be pointer later
	Vector3 position;
};

std::vector<Block> g_Blocks{};

int HandleArgs(int argc, wchar_t* argv[], wchar_t* envp[], std::wstring& inputFile, std::wstring& outputFile);
void PrintHelp();

void WriteBlock(FILE* pOFile, const Block& block, int id)
{
	auto x = block.position.x;
	auto y = block.position.y;
	auto z = block.position.z;

	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 0.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 0.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 1.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 1.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 0.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 0.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 1.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 1.0 + y, 1.0f + z);


	const int offset = id * 8;

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 7 + offset, 2, 5 + offset, 2);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 2, 3 + offset, 2, 7 + offset, 2);

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 6, 4 + offset, 6, 3 + offset, 6);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 6, 2 + offset, 6, 4 + offset, 6);

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 8 + offset, 3, 7 + offset, 3);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 3 + offset, 3, 4 + offset, 3, 8 + offset, 3);

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 7 + offset, 5, 8 + offset, 5);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 5 + offset, 5, 8 + offset, 5, 6 + offset, 5);

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 5 + offset, 4, 6 + offset, 4);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 1 + offset, 4, 6 + offset, 4, 2 + offset, 4);

	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 6 + offset, 1, 8 + offset, 1);
	fwprintf_s(pOFile, L"f %d//%d %d//%d %d//%d\n", 2 + offset, 1, 8 + offset, 1, 4 + offset, 1);
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	std::wstring inputFile{};
	std::wstring outputFile{};
	HandleArgs(argc, argv, envp, inputFile, outputFile);

	std::wcout << "Input file: " << inputFile << "\n";
	std::wcout << "Output file: " << outputFile << "\n";

	FILE* pOFile = nullptr;
	_wfopen_s(&pOFile, outputFile.c_str(), L"w+,ccs=UTF-8");
	if (pOFile == nullptr)
	{
		return -1;
	}


	const wchar_t* text = L"#\nmtllib materials.mtl\n";
	fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOFile);

	if (std::ifstream is{inputFile.c_str()})
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
					BlockData blockData{};

					//Get name/layer
					const Value& layer = blockJson["layer"];
					std::wstringstream layerss{};
					layerss << layer.GetString();
					blockData.name = layerss.str();

					//Get opaque
					const Value& opaque = blockJson["opaque"];
					blockData.isOpaque = opaque.GetBool();

					//Get the array of block positions
					const Value& positions = blockJson["positions"];
					for (auto blockIt = positions.Begin(); blockIt != positions.End(); ++blockIt)
					{
						//Create block
						Block block{};
						block.data = blockData;

						//Check valid array data
						const Value& positionJson = *blockIt;
						if (positionJson.Size() != 3) {continue;}

						//Read to vector
						block.position.x = positionJson[0].GetInt();
						block.position.y = positionJson[2].GetInt(); //Reading z as y
						block.position.z = positionJson[1].GetInt(); //Reading y as z
						g_Blocks.push_back(block);
					}
				}
			}
			std::cout << "Readen blocks: " << g_Blocks.size() << "\n";
		}
	}

	//Write the normals for every block one time
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, 1.0f);
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, -1.0f);
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 1.0f, 0.0f);
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, -1.0f, 0.0f);
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 1.0f, 0.0f, 0.0f);
	fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", -1.0f, 0.0f, 0.0f);

	const int size = static_cast<int>(g_Blocks.size());
	auto prevData = BlockData{};
	for (int i{}; i < size; ++i)
	{
		auto& block = g_Blocks[i];

		//Write material if datatype changed
		if (prevData.name != block.data.name)
		{
			std::wstringstream material{};
			material << L"usemtl " << block.data.name << L"\n";
			auto materialStr = material.str();
			fwrite(materialStr.c_str(), materialStr.size() * sizeof(wchar_t), 1, pOFile);
		}

		//Write block
		WriteBlock(pOFile, block, i);
	}
	
	fclose(pOFile);
	return 0;
}

int HandleArgs(int argc, wchar_t* argv[], wchar_t* envp[], std::wstring& inputFile, std::wstring& outputFile)
{
	if (argc != 3 && argc != 5)
	{
		PrintHelp();
		return -1;
	}

	for (int i{ 1 }; i < argc; i += 2)
	{
		std::wstring identifierStr{ argv[i] };
		if (identifierStr.size() != 2)
		{
			wprintf_s(L"Identifier not found, your input was %s\n", identifierStr.c_str());
			PrintHelp();
			return -1;
		}

		const wchar_t identifier{ identifierStr[1] };
		switch (identifier)
		{
		case 'i':
			inputFile = argv[i + 1];
			break;
		case 'o':
			outputFile = argv[i + 1];
			break;
		default:
			{
			wprintf_s(L"Identifier %c does not exist\n", identifier);
			PrintHelp();
			return -1;
			}
		}
	}

	if (outputFile.empty())
	{
		outputFile = inputFile.substr(0,inputFile.length()-5) + L".obj";
	}

	return 0;
}

void PrintHelp()
{
	
}