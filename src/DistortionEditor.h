#pragma once
/*
 * AUTO GENERATED,
 * NOT A GOOD IDEA TO CHANGE STUFF HERE
 * Keep the file readonly
 */

#include "DistortionProcessor.h"

#include "UiElements.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
  public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor(&p)
        , processorRef(p)
        , valueTreeState(vts)
        , backgroundApp(juce::Colour(Constants::Colors::bg_App))
    {
        setLookAndFeel(&m_laf);
        initWidgets();
        setResizable(true, true);
        setResizeLimits(Constants::InitJuce::WindowWidth, Constants::InitJuce::WindowHeight, 4000, 3000);
        setSize(Constants::InitJuce::WindowWidth, Constants::InitJuce::WindowHeight);
        startTimerHz(Constants::InitJuce::TimerHertz);
    }

    ~AudioPluginAudioProcessorEditor() override
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(backgroundApp);
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
    void resized() override
    {
        auto area = getLocalBounds().reduced(static_cast<int>(Constants::Margins::big));

        // auto generated
        // const juce::FlexItem::Margin knobMargin = juce::FlexItem::Margin(Constants::Margins::small);
        const juce::FlexItem::Margin knobMarginSmall = juce::FlexItem::Margin(Constants::Margins::medium);

        std::vector<juce::Rectangle<int>> areas(3);
        const auto colWidth = area.getWidth() / 16;
        const auto rowHeight = area.getHeight() / 5;
        areas[0] = area.removeFromLeft(colWidth * 2).reduced(Constants::Margins::small);
        areas[1] = area.removeFromTop(rowHeight * 1).reduced(Constants::Margins::small);
        areas[2] = area.reduced(Constants::Margins::small);

        {
            juce::FlexBox box;
            box.flexWrap = juce::FlexBox::Wrap::noWrap;
            box.flexDirection = juce::FlexBox::Direction::column;
            box.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
            box.items.add(juce::FlexItem(levelDial).withFlex(1).withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(levelGauge).withHeight(200).withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(cpuGauge).withHeight(200).withMargin(knobMarginSmall));
            box.performLayout(areas[0].toFloat());
        }
        {
            juce::FlexBox box;
            box.flexWrap = juce::FlexBox::Wrap::noWrap;
            box.flexDirection = juce::FlexBox::Direction::row;
            box.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
            box.items.add(juce::FlexItem(typeDrop)
                              .withWidth(Constants::Text::labelWidth)
                              .withHeight(Constants::Text::labelHeight)
                              .withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(preboostLowDial).withFlex(1).withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(preboostHighDial).withFlex(1).withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(crossOverDial).withFlex(1).withMargin(knobMarginSmall));
            box.items.add(juce::FlexItem(cutDial).withFlex(1).withMargin(knobMarginSmall));
            box.performLayout(areas[1].toFloat());
        }
        {
            juce::FlexBox box;
            box.flexWrap = juce::FlexBox::Wrap::noWrap;
            box.flexDirection = juce::FlexBox::Direction::row;
            box.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
            box.items.add(juce::FlexItem(spectrogramGauge).withFlex(1).withMargin(knobMarginSmall));
            box.performLayout(areas[2].toFloat());
        }
    }
#pragma GCC diagnostic pop

    void timerCallback() override
    {
        cpuGauge.update(processorRef.getCpuLoad());
        levelGauge.update(processorRef.getInputDbLoad(), processorRef.getOutputDbLoad());
        spectrogramGauge.update(processorRef.getSpectrogram());
    }

    void initWidgets()
    {
        addAndMakeVisible(levelDial);
        levelDial.reset(valueTreeState, "level");
        levelDial.setLabelText(juce::String::fromUTF8("Level"));
        addAndMakeVisible(cpuGauge);
        cpuGauge.setLabelText(juce::String::fromUTF8("CPU"));
        addAndMakeVisible(levelGauge);
        levelGauge.setLabelText(juce::String::fromUTF8("Level"));
        addAndMakeVisible(typeDrop);
        typeDrop.addItemList(valueTreeState.getParameter("type")->getAllValueStrings(), 1);
        typeDropAttachment =
            std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(valueTreeState, "type", typeDrop);
        addAndMakeVisible(preboostLowDial);
        preboostLowDial.reset(valueTreeState, "preboostLow");
        preboostLowDial.setLabelText(juce::String::fromUTF8("Preboost Low"));
        addAndMakeVisible(preboostHighDial);
        preboostHighDial.reset(valueTreeState, "preboostHigh");
        preboostHighDial.setLabelText(juce::String::fromUTF8("Preboost High"));
        addAndMakeVisible(crossOverDial);
        crossOverDial.reset(valueTreeState, "crossOver");
        crossOverDial.setLabelText(juce::String::fromUTF8("Cross Over"));
        addAndMakeVisible(cutDial);
        cutDial.reset(valueTreeState, "cut");
        cutDial.setLabelText(juce::String::fromUTF8("Cut"));
        addAndMakeVisible(spectrogramGauge);
        spectrogramGauge.setLabelText(juce::String::fromUTF8("Spectrogram"));
    }

  private:
    AudioPluginAudioProcessor& processorRef;
    juce::AudioProcessorValueTreeState& valueTreeState;
    GuiLookAndFeel m_laf;
    juce::Colour backgroundApp;

    CustomRotaryDial levelDial{this};
    CpuGauge cpuGauge{};
    Gauge levelGauge{};
    juce::ComboBox typeDrop{};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeDropAttachment;
    CustomRotaryDial preboostLowDial{this};
    CustomRotaryDial preboostHighDial{this};
    CustomRotaryDial crossOverDial{this};
    CustomRotaryDial cutDial{this};
    SpectrogramDisplay spectrogramGauge{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
