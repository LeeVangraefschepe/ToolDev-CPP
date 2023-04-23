#include <direct.h> // _getwcwd
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/stream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/istreamwrapper.h"

void WriteBlock(FILE* pOFile, int x, int y, int z, int id)
{
	//fwprintf_s(pOFile, L"g Group %d\n", id);
	//Maak functie en zet offset van positie
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 0.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 0.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 1.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 0.0 + x, 1.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 0.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 0.0 + y, 1.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 1.0 + y, 0.0f + z);
	fwprintf_s(pOFile, L"v %.4f %.4f %.4f\n", 1.0 + x, 1.0 + y, 1.0f + z);

	//Pas de vertex nummers aan naar positie
	//OF lijn beginnen met "g Cube0" -> indices krijgen reset

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
	FILE* pOFile = nullptr;
	_wfopen_s(&pOFile, L"scene.obj", L"w+,ccs=UTF-8");
	if (pOFile == nullptr)
	{

		return -1;
	}


	const wchar_t* text = L"#\nmtllib materials.mtl\n";
	fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOFile);

	if (std::ifstream is{"minecraftscene.json"})
	{
		using namespace rapidjson;
		IStreamWrapper isw{ is };
		Document sceneDoc{};
		sceneDoc.ParseStream(isw);

		if (sceneDoc.IsArray())
		{
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, 1.0f);
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 0.0f, -1.0f);
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, 1.0f, 0.0f);
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0f, -1.0f, 0.0f);
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 1.0f, 0.0f, 0.0f);
			fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", -1.0f, 0.0f, 0.0f);

			int blockId{};
			for (auto layerIt = sceneDoc.Begin(); layerIt != sceneDoc.End(); ++layerIt)
			{
				const Value& block = *layerIt;
				if (block.HasMember("layer") && block.HasMember("opaque") && block.HasMember("positions"))
				{
					const Value& layer = block["layer"];
					//usemtl[material name]
					std::wstringstream material{};
					material << L"usemtl " << layer.GetString() << L"\n";
					auto materialStr = material.str();
					fwrite(materialStr.c_str(), materialStr.size() * sizeof(wchar_t), 1, pOFile);
					const Value& positions = block["positions"];
					std::cout << layer.GetString() << "\n";
					for (auto blockIt = positions.Begin(); blockIt != positions.End(); ++blockIt)
					{
						const Value& position = *blockIt;
						if (position.Size() != 3) {continue;}
						auto x = position[0].GetInt();
						auto z = position[1].GetInt();
						auto y = position[2].GetInt();
						WriteBlock(pOFile, x, y, z, blockId);
						++blockId;
					}
				}
			}
			std::cout << "Writen block id: " << blockId << "\n";
		}
	}
	
	fclose(pOFile);
	return 0;
}