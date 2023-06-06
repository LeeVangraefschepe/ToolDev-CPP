#include <codecvt>
#include <iostream>
#include "SceneParser.h"

bool gSilent{};
int HandleArgs(int argc, wchar_t* argv[], wchar_t* envp[], std::wstring& inputFile, std::wstring& outputFile);
void PrintHelp();

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	std::wstring inputFile{};
	std::wstring outputFile{};

	if (const int error = HandleArgs(argc, argv, envp, inputFile, outputFile) != 0)
	{
		return error;
	}

	if (!gSilent)
	{
		std::wcout << L"Input: " << inputFile << "\n";
		std::wcout << L"Output: " << outputFile << "\n";
	}

	SceneParser scene{ inputFile };
	if (!gSilent) { std::cout << "Amount of blocks loaded: " << scene.GetBlocks().size() << "\n"; }

	if (scene.SaveObj(outputFile) == false)
	{
		if (!gSilent) { std::cout << "Failed to save file\n"; }
		return -1;
	}

	if (gSilent)
	{
		return 0;
	}

	//Convert wstring to UTF-8 encoded string
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	const std::string utf8_path = converter.to_bytes(outputFile);

	//Execute the file
	int result = std::system(utf8_path.c_str());

	if (result == 0)
	{
		std::cout << "File executed successfully." << std::endl;
	}
	else
	{
		std::cout << "File execution failed." << std::endl;
	}

	return 0;
}

int HandleArgs(int argc, wchar_t* argv[], wchar_t* envp[], std::wstring& inputFile, std::wstring& outputFile)
{
	for (int i{ 1 }; i < argc; i += 2)
	{
		std::wstring identifierStr{ argv[i] };
		if (identifierStr.size() != 2)
		{
			wprintf_s(L"Identifier not found, your input was %ls\n", identifierStr.c_str());
			if (!gSilent){ PrintHelp(); }
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
		case 's':
			gSilent = true;
			--i;
			break;
		default:
		{
			wprintf_s(L"Identifier %c does not exist\n", identifier);
			if (!gSilent) { PrintHelp(); }
			return -1;
		}
		}
	}

	if (outputFile.empty())
	{
		outputFile = inputFile.substr(0, inputFile.length() - 5) + L".obj";
	}

	return 0;
}

void PrintHelp()
{
	wprintf_s(L"Not the right amount of arguments.\n");
	wprintf_s(L"Possible commands:\n");
	wprintf_s(L"MinecraftJson -i (inputfile).json\n");
	wprintf_s(L"MinecraftJson -i (inputfile).json -o (outputfile).obj\n");
}