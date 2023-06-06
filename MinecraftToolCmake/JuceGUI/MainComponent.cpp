#include "MainComponent.h"
#include "SceneParser.h"

MainComponent::~MainComponent()
{
    delete m_pTableModel;
    delete m_sceneParser;
}
//==============================================================================
MainComponent::MainComponent()
{
    setSize(1000, 600);
    m_setSourceButton.setButtonText("Select source file destination");
    m_setSaveButton.setButtonText("Select save destination");
    m_loadButton.setButtonText("Load from source");
    m_saveButton.setButtonText("Save file");
    addAndMakeVisible(m_setSourceButton);
    addAndMakeVisible(m_setSaveButton);
    addAndMakeVisible(m_fileSaveName);
    addAndMakeVisible(m_fileSourceName);
    addAndMakeVisible(m_saveButton);
    addAndMakeVisible(m_loadButton);
    m_setSourceButton.onClick = [this]()
    {
        SelectFile();
    };
    m_setSaveButton.onClick = [this]()
    {
        SelectFolder();
    };
    m_loadButton.onClick = [this]()
    {
        LoadFile();
    };
    m_saveButton.onClick = [this]()
    {
        SaveFile();
    };
    addAndMakeVisible(m_DataTable);
    m_pTableModel = new TableModel(std::vector<Block>{});
    m_DataTable.setModel(m_pTableModel);
    m_DataTable.getHeader().addColumn("Name", 1, 100, 50, 200);
    m_DataTable.getHeader().addColumn("x", 2, 50, 20, 60);
    m_DataTable.getHeader().addColumn("y", 3, 50, 20, 60);
    m_DataTable.getHeader().addColumn("z", 4, 50, 20, 60);
}
void MainComponent::SelectFile()
{
    using namespace juce;
    myChooser = std::make_unique<FileChooser>(
        "Please select the obj file you want to load...",
        File::getSpecialLocation(File::userHomeDirectory),
        "*.json;*.txt");
    constexpr auto folderChooserFlags =
        FileBrowserComponent::openMode;
    myChooser->launchAsync(folderChooserFlags,
        [this](const FileChooser& chooser)
        {
	        const File file = chooser.getResult();

            std::stringstream ss{};
            ss << "File selected: " << file.getFullPathName() << "\n";

	        Logger::getCurrentLogger()->writeToLog(ss.str());

            m_fileSourceName.setText(file.getFullPathName().toWideCharPointer());
        }
    );
}

void MainComponent::SelectFolder()
{
    using namespace juce;
    myChooser = std::make_unique<FileChooser>(
        "Please select the folder containing the files you want to load...",
        File::getSpecialLocation(File::userHomeDirectory),
        String()
    );

    constexpr auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories;

    myChooser->launchAsync(folderChooserFlags,
        [this](const FileChooser& chooser)
        {
            const File folder = chooser.getResult();

            std::stringstream ss{};
            ss << "Folder selected: " << folder.getFullPathName() << "\n";
            Logger::getCurrentLogger()->writeToLog(ss.str());

            std::wstringstream path{};
            path << folder.getFullPathName().toWideCharPointer() << L"\\MinecraftScene.obj";

            m_fileSaveName.setText(path.str().c_str());
        }
    );
}

void MainComponent::LoadFile()
{
    if (m_fileSourceName.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "ERROR",
            "No source path set",
            "OK");
        return;
    }
    delete m_sceneParser;
    m_sceneParser = new SceneParser{ m_fileSourceName.getText().toWideCharPointer() };

    delete m_pTableModel;
    m_pTableModel = new TableModel{ m_sceneParser->GetBlocks() };
    m_DataTable.setModel(m_pTableModel);
}

void MainComponent::SaveFile() const
{
    if (!m_sceneParser)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "ERROR",
            "No blocks loaded to save",
            "OK");
        return;
    }
    if (m_fileSaveName.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "ERROR",
            "No save path set",
            "OK");
        return;
    }
    if (m_sceneParser->SaveObj(m_fileSaveName.getText().toWideCharPointer()) == false)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "ERROR",
            "File failed to save",
            "OK");
        return;
    }
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::NoIcon,
        "INFO",
        "File has been saved",
        "OK");
}

void MainComponent::resized()
{
    m_setSourceButton.setBounds(10, 10, 480, 40);
    m_setSaveButton.setBounds(510,10, 480, 40);
    m_DataTable.setBounds(0, 200, getWidth(), getHeight()-200);
    m_fileSourceName.setBounds(10, 60, 480, 25);
    m_fileSaveName.setBounds(510, 60, 480, 25);
    m_loadButton.setBounds(10, 95, 480, 40);
    m_saveButton.setBounds(510, 95, 480, 40);
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setFont(juce::Font(17.0f));
    g.setColour(juce::Colours::white);
    g.drawText("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}