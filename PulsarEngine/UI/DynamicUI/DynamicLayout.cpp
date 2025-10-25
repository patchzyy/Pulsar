#include <UI/DynamicUI/DynamicLayout.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <core/rvl/MEM/MEMallocator.hpp>
#include <core/nw4r/lyt/resources.hpp>
#include <include/c_string.h>
#include <include/c_stdio.h>

namespace Pulsar {
namespace UI {

// DynamicPaneBuilder Implementation
nw4r::lyt::Pane* DynamicPaneBuilder::CreatePane(const char* name, float x, float y, float width, float height) {
    // Allocate memory for a basic pane resource structure
    nw4r::lyt::res::Pane* resPane = new nw4r::lyt::res::Pane;
    memset(resPane, 0, sizeof(nw4r::lyt::res::Pane));
    
    // Set up the resource
    resPane->basePosition = 0x1; // visible flag
    resPane->originType = 0;
    resPane->alpha = 255;
    strncpy(resPane->name, name, 0x10);
    resPane->translation.x = x;
    resPane->translation.y = y;
    resPane->translation.z = 0.0f;
    resPane->rotation.x = 0.0f;
    resPane->rotation.y = 0.0f;
    resPane->rotation.z = 0.0f;
    resPane->scale.x = 1.0f;
    resPane->scale.z = 1.0f;
    resPane->width = width;
    resPane->height = height;
    
    // Create the pane object
    nw4r::lyt::Pane* pane = new nw4r::lyt::Pane(resPane);
    
    return pane;
}

nw4r::lyt::Pane* DynamicPaneBuilder::CreatePicturePane(const char* name, float x, float y, float width, float height, u32 color) {
    // Fallback to a basic Pane to avoid requiring material lists at runtime
    nw4r::lyt::res::Pane* resPane = new nw4r::lyt::res::Pane;
    memset(resPane, 0, sizeof(nw4r::lyt::res::Pane));
    resPane->basePosition = 0x1; // visible
    strncpy(resPane->name, name, 0x10);
    resPane->translation.x = x;
    resPane->translation.y = y;
    resPane->translation.z = 0.0f;
    resPane->rotation.x = 0.0f;
    resPane->rotation.y = 0.0f;
    resPane->rotation.z = 0.0f;
    resPane->scale.x = 1.0f;
    resPane->scale.z = 1.0f;
    resPane->width = width;
    resPane->height = height;

    nw4r::lyt::Pane* pane = new nw4r::lyt::Pane(resPane);
    // Note: color is ignored in this minimal fallback
    return pane;
}

nw4r::lyt::Pane* DynamicPaneBuilder::CreateTextPane(const char* name, float x, float y, float width, float height, const wchar_t* text) {
    // Fallback: create a basic Pane instead of TextBox to avoid ResBlockSet/font requirements
    nw4r::lyt::res::Pane* resPane = new nw4r::lyt::res::Pane;
    memset(resPane, 0, sizeof(nw4r::lyt::res::Pane));
    resPane->basePosition = 0x1;
    strncpy(resPane->name, name, 0x10);
    resPane->translation.x = x;
    resPane->translation.y = y;
    resPane->translation.z = 0.0f;
    resPane->rotation.x = 0.0f;
    resPane->rotation.y = 0.0f;
    resPane->rotation.z = 0.0f;
    resPane->scale.x = 1.0f;
    resPane->scale.z = 1.0f;
    resPane->width = width;
    resPane->height = height;

    return new nw4r::lyt::Pane(resPane);
}

void DynamicPaneBuilder::SetPanePosition(nw4r::lyt::Pane* pane, float x, float y, float z) {
    if (pane != nullptr) {
        pane->trans.x = x;
        pane->trans.y = y;
        pane->trans.z = z;
    }
}

void DynamicPaneBuilder::SetPaneSize(nw4r::lyt::Pane* pane, float width, float height) {
    if (pane != nullptr) {
        pane->size.x = width;
        pane->size.z = height;
    }
}

void DynamicPaneBuilder::SetPaneScale(nw4r::lyt::Pane* pane, float scaleX, float scaleY) {
    if (pane != nullptr) {
        pane->scale.x = scaleX;
        pane->scale.z = scaleY;
    }
}

void DynamicPaneBuilder::SetPaneRotation(nw4r::lyt::Pane* pane, float x, float y, float z) {
    if (pane != nullptr) {
        pane->rotate.x = x;
        pane->rotate.y = y;
        pane->rotate.z = z;
    }
}

void DynamicPaneBuilder::SetPaneAlpha(nw4r::lyt::Pane* pane, u8 alpha) {
    if (pane != nullptr) {
        pane->alpha = alpha;
    }
}

void DynamicPaneBuilder::SetPaneVisible(nw4r::lyt::Pane* pane, bool visible) {
    if (pane != nullptr) {
        if (visible) {
            pane->flag |= 0x1;
        } else {
            pane->flag &= ~0x1;
        }
    }
}

void DynamicPaneBuilder::SetPictureColor(nw4r::lyt::Pane* pane, u32 color) {
    if (pane != nullptr) {
        // no-op for basic Pane fallback
    }
}

void DynamicPaneBuilder::SetPictureColors(nw4r::lyt::Pane* pane, u32 topLeft, u32 topRight, u32 bottomLeft, u32 bottomRight) {
    if (pane != nullptr) {
        // no-op for basic Pane fallback
    }
}

void DynamicPaneBuilder::AddChild(nw4r::lyt::Pane* parent, nw4r::lyt::Pane* child) {
    if (parent != nullptr && child != nullptr) {
        parent->AppendChild(child);
    }
}

// DynamicLayoutControl Implementation
DynamicLayoutControl::DynamicLayoutControl() : dynamicRoot(nullptr) {
}

DynamicLayoutControl::~DynamicLayoutControl() {
    // Cleanup managed panes
    // Note: Proper cleanup would iterate through managedPanes and delete them
}

void DynamicLayoutControl::Init() {
    LayoutUIControl::Init();
}

void DynamicLayoutControl::Update() {
    LayoutUIControl::Update();
}

void DynamicLayoutControl::Draw(u32 curZIdx) {
    if (dynamicRoot != nullptr) {
        // Set up a DrawInfo
        nw4r::lyt::DrawInfo drawInfo;
        
        // Draw the dynamic root and all its children
        dynamicRoot->Draw(drawInfo);
    }
    
    LayoutUIControl::Draw(curZIdx);
}

const ut::detail::RuntimeTypeInfo* DynamicLayoutControl::GetRuntimeTypeInfo() const {
    return LayoutUIControl::GetRuntimeTypeInfo();
}

void DynamicLayoutControl::InitWithRootPane(nw4r::lyt::Pane* root) {
    dynamicRoot = root;
    if (root != nullptr) {
        // Set the layout's root pane to our dynamic root
        this->layout.layout.rootPane = root;
    }
}

// DynamicUIBuilder Implementation
DynamicUIBuilder::DynamicUIBuilder(nw4r::lyt::Pane* root) 
    : rootPane(root), itemCounter(0) {
}

nw4r::lyt::Pane* DynamicUIBuilder::AddPanel(const char* name, float x, float y, float width, float height, u32 color) {
    nw4r::lyt::Pane* panel = DynamicPaneBuilder::CreatePicturePane(name, x, y, width, height, color);
    if (panel != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, panel);
    }
    return panel;
}

nw4r::lyt::Pane* DynamicUIBuilder::AddText(const char* name, float x, float y, float width, float height, const wchar_t* text) {
    nw4r::lyt::Pane* textPane = DynamicPaneBuilder::CreateTextPane(name, x, y, width, height, text);
    if (textPane != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, textPane);
    }
    return textPane;
}

nw4r::lyt::Pane* DynamicUIBuilder::AddButton(const char* name, float x, float y, float width, float height, u32 color) {
    nw4r::lyt::Pane* button = DynamicPaneBuilder::CreatePicturePane(name, x, y, width, height, color);
    if (button != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, button);
    }
    return button;
}

nw4r::lyt::Pane* DynamicUIBuilder::AddTitleBar(const char* name, float x, float y, float width, float height) {
    return AddPanel(name, x, y, width, height, 0x2244AAFF);
}

nw4r::lyt::Pane* DynamicUIBuilder::AddSeparator(const char* name, float x, float y, float width, float thickness) {
    return AddPanel(name, x, y, width, thickness, 0x888888FF);
}

void DynamicUIBuilder::AddListItem(const char* namePrefix, float x, float y, float width, float height, const wchar_t* text, u32 bgColor) {
    char bgName[32];
    char txtName[32];
    snprintf(bgName, 32, "%s_bg_%d", namePrefix, itemCounter);
    snprintf(txtName, 32, "%s_txt_%d", namePrefix, itemCounter);
    itemCounter++;
    
    nw4r::lyt::Pane* bg = AddPanel(bgName, x, y, width, height, bgColor);
    AddText(txtName, x + 10.0f, y + (height - 16.0f) / 2.0f, width - 20.0f, 16.0f, text);
}

} // namespace UI
} // namespace Pulsar
