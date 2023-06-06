#include "TableModel.h"

#include <codecvt>
#include <locale>

TableModel::TableModel(const std::vector<Block>& blocks)
{
	m_Vectors.reserve(blocks.size());
	for (const auto& block : blocks)
	{
		//Create converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string name = converter.to_bytes(block.data->name);

		m_Vectors.push_back(NamedVector3{ name, static_cast<float>(block.position.x), static_cast<float>(block.position.y), static_cast<float>(block.position.z) });
	}
}

int TableModel::getNumRows()
{
	return static_cast<int>(m_Vectors.size());
}

void TableModel::paintRowBackground(Graphics& g, int /*rowNumber*/, int width, int height, bool rowIsSelected)
{
	if (rowIsSelected)
	{
		g.setColour(Colour{210,210,210});
		g.fillRoundedRectangle(0, 0, static_cast<float>(width), static_cast<float>(height), 3);
		g.drawRoundedRectangle(0, 0, static_cast<float>(width), static_cast<float>(height), 3, 2);
	}else
	{
		g.setColour(Colour{ 255,128,64 });
		g.drawRoundedRectangle(0, 0, static_cast<float>(width), static_cast<float>(height), 3, 2);
	}
}

void TableModel::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	if (rowNumber < m_Vectors.size())
	{
		if (rowIsSelected)
		{
			g.setColour(Colour{ 64,64,64 });
		}
		else
		{
			g.setColour(Colour{ 243,243,243 });
		}
		NamedVector3& vector = m_Vectors[rowNumber];
		switch (columnId)
		{

		case 1: g.drawText(vector.name, 40, 0, width-10, height, Justification::centredLeft);
			break;
		case 2: g.drawText(String{ vector.x }, 0, 0, width, height, Justification::centredLeft);
			break;
		case 3: g.drawText(String{ vector.y }, 0, 0, width, height, Justification::centredLeft);
			break;
		case 4: g.drawText(String{ vector.z }, 0, 0, width, height, Justification::centredLeft);
			break;
		}
	}
}
