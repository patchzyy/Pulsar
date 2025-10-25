#ifndef _PUL_DYNAMIC_LAYOUT_
#define _PUL_DYNAMIC_LAYOUT_

#include <kamek.hpp>
#include <core/nw4r/lyt/Pane.hpp>
#include <core/nw4r/lyt/Picture.hpp>
#include <core/nw4r/lyt/TextBox.hpp>
#include <core/nw4r/lyt/Material.hpp>
#include <core/nw4r/ut/Color.hpp>
#include <MarioKartWii/UI/Layout/Layout.hpp>
#include <MarioKartWii/UI/Ctrl/UIControl.hpp>

/*
DynamicLayout - A system for creating UI layouts entirely from code without needing brlyt/brctr files.
This allows for fully dynamic UI generation at runtime.
*/

namespace Pulsar {
namespace UI {

// Helper class to create panes programmatically
class DynamicPaneBuilder {
public:
    // Create a basic pane with position and size
    static nw4r::lyt::Pane* CreatePane(const char* name, float x, float y, float width, float height);
    
    // Create a picture pane (colored rectangle or textured)
    static nw4r::lyt::Picture* CreatePicturePane(const char* name, float x, float y, float width, float height, u32 color = 0xFFFFFFFF);
    
    // Create a text box pane
    static nw4r::lyt::TextBox* CreateTextPane(const char* name, float x, float y, float width, float height, const wchar_t* text = nullptr);
    
    // Helper to set pane properties
    static void SetPanePosition(nw4r::lyt::Pane* pane, float x, float y, float z = 0.0f);
    static void SetPaneSize(nw4r::lyt::Pane* pane, float width, float height);
    static void SetPaneScale(nw4r::lyt::Pane* pane, float scaleX, float scaleY);
    static void SetPaneRotation(nw4r::lyt::Pane* pane, float x, float y, float z);
    static void SetPaneAlpha(nw4r::lyt::Pane* pane, u8 alpha);
    static void SetPaneVisible(nw4r::lyt::Pane* pane, bool visible);
    
    // Helper to set picture pane colors
    static void SetPictureColor(nw4r::lyt::Picture* pane, u32 color);
    static void SetPictureColors(nw4r::lyt::Picture* pane, u32 topLeft, u32 topRight, u32 bottomLeft, u32 bottomRight);
    
    // Helper to add child panes
    static void AddChild(nw4r::lyt::Pane* parent, nw4r::lyt::Pane* child);
};

// A control that manages dynamically created UI
class DynamicLayoutControl : public LayoutUIControl {
public:
    DynamicLayoutControl();
    ~DynamicLayoutControl() override;
    
    void Init() override;
    void Update() override;
    void Draw(u32 curZIdx) override;
    const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const override;
    
    // Initialize with a root pane - this becomes the root of the dynamic layout
    void InitWithRootPane(nw4r::lyt::Pane* root);
    
    // Get the root pane for adding children
    nw4r::lyt::Pane* GetRootPane() const { return dynamicRoot; }
    
protected:
    nw4r::lyt::Pane* dynamicRoot;
};

// Builder class for creating common UI elements easily
class DynamicUIBuilder {
public:
    DynamicUIBuilder(nw4r::lyt::Pane* root);
    
    // Build a panel (colored rectangle)
    nw4r::lyt::Picture* AddPanel(const char* name, float x, float y, float width, float height, u32 color = 0x333333FF);
    
    // Build a text label
    nw4r::lyt::TextBox* AddText(const char* name, float x, float y, float width, float height, const wchar_t* text);
    
    // Build a button background (can be styled differently)
    nw4r::lyt::Picture* AddButton(const char* name, float x, float y, float width, float height, u32 color = 0x4444AAFF);
    
    // Build a title bar
    nw4r::lyt::Picture* AddTitleBar(const char* name, float x, float y, float width, float height);
    
    // Build a separator line
    nw4r::lyt::Picture* AddSeparator(const char* name, float x, float y, float width, float thickness = 2.0f);
    
    // Add a list item (panel + text)
    void AddListItem(const char* namePrefix, float x, float y, float width, float height, const wchar_t* text, u32 bgColor = 0x444444FF);
    
private:
    nw4r::lyt::Pane* rootPane;
    u32 itemCounter;
};

} // namespace UI
} // namespace Pulsar

#endif
