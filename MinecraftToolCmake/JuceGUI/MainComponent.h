#pragma once

#include <JuceHeader.h>

#include "Block.h"
#include "SceneParser.h"
#include "TableModel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void SelectFile();
    void SelectFolder();
    void LoadFile();
    void SaveFile() const;
private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton m_setSaveButton;
    juce::TextButton m_setSourceButton;
    std::unique_ptr<juce::FileChooser> myChooser;

    juce::TextButton m_loadButton;
    juce::TextButton m_saveButton;
    SceneParser* m_sceneParser{nullptr};

    juce::TableListBox m_DataTable;
    TableModel* m_pTableModel;
    juce::TextEditor m_fileSaveName;
    juce::TextEditor m_fileSourceName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};