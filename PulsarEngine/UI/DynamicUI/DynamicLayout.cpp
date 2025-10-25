#include <UI/DynamicUI/DynamicLayout.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <core/rvl/MEM/MEMallocator.hpp>
#include <cstring>
#include <cstdio>

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
    resPane->scale.y = 1.0f;
    resPane->width = width;
    resPane->height = height;
    
    // Create the pane object
    nw4r::lyt::Pane* pane = new nw4r::lyt::Pane(resPane);
    
    return pane;
}

nw4r::lyt::Picture* DynamicPaneBuilder::CreatePicturePane(const char* name, float x, float y, float width, float height, u32 color) {
    // Allocate memory for a picture pane resource structure
    nw4r::lyt::res::Picture* resPic = new nw4r::lyt::res::Picture;
    memset(resPic, 0, sizeof(nw4r::lyt::res::Picture));
    
    // Set up the pane base
    resPic->basePosition = 0x1; // visible
    resPic->originType = 0;
    resPic->alpha = 255;
    strncpy(resPic->name, name, 0x10);
    resPic->translation.x = x;
    resPic->translation.y = y;
    resPic->translation.z = 0.0f;
    resPic->rotation.x = 0.0f;
    resPic->rotation.y = 0.0f;
    resPic->rotation.z = 0.0f;
    resPic->scale.x = 1.0f;
    resPic->scale.y = 1.0f;
    resPic->width = width;
    resPic->height = height;
    
    // Set vertex colors
    resPic->vertexColours[0] = color;
    resPic->vertexColours[1] = color;
    resPic->vertexColours[2] = color;
    resPic->vertexColours[3] = color;
    resPic->materialId = 0;
    resPic->texCoordNum = 0;
    
    // Create an empty ResBlockSet
    nw4r::lyt::ResBlockSet blockSet;
    memset(&blockSet, 0, sizeof(blockSet));
    
    // Create the picture object using proper casting
    nw4r::lyt::Picture* pic = new nw4r::lyt::Picture(reinterpret_cast<nw4r::lyt::res::TextBox*>(resPic), blockSet);
    
    // Set the vertex colors directly on the created object
    pic->vertexColours[0] = nw4r::ut::Color(color);
    pic->vertexColours[1] = nw4r::ut::Color(color);
    pic->vertexColours[2] = nw4r::ut::Color(color);
    pic->vertexColours[3] = nw4r::ut::Color(color);
    
    return pic;
}

nw4r::lyt::TextBox* DynamicPaneBuilder::CreateTextPane(const char* name, float x, float y, float width, float height, const wchar_t* text) {
    // Allocate memory for a textbox resource structure
    nw4r::lyt::res::TextBox* resTxt = new nw4r::lyt::res::TextBox;
    memset(resTxt, 0, sizeof(nw4r::lyt::res::TextBox));
    
    // Set up the pane base
    resTxt->basePosition = 0x1;
    resTxt->originType = 0;
    resTxt->alpha = 255;
    strncpy(resTxt->name, name, 0x10);
    resTxt->translation.x = x;
    resTxt->translation.y = y;
    resTxt->translation.z = 0.0f;
    resTxt->rotation.x = 0.0f;
    resTxt->rotation.y = 0.0f;
    resTxt->rotation.z = 0.0f;
    resTxt->scale.x = 1.0f;
    resTxt->scale.y = 1.0f;
    resTxt->width = width;
    resTxt->height = height;
    
    // Set text properties
    resTxt->stringSize = 0;
    resTxt->maxStringSize = 256;
    resTxt->materialId = 0;
    resTxt->fontId = 0;
    resTxt->textPosition = 0;
    resTxt->alignment = 0;
    resTxt->topColour = 0xFFFFFFFF;
    resTxt->bottomColour = 0xFFFFFFFF;
    resTxt->fontSize.x = 16.0f;
    resTxt->fontSize.y = 16.0f;
    resTxt->characterSpace = 0.0f;
    resTxt->lineSpace = 0.0f;
    
    // Create an empty ResBlockSet
    nw4r::lyt::ResBlockSet blockSet;
    memset(&blockSet, 0, sizeof(blockSet));
    
    // Create the textbox object
    nw4r::lyt::TextBox* textBox = new nw4r::lyt::TextBox(resTxt, blockSet);
    
    // Allocate string buffer
    textBox->AllocStringBuffer(256);
    
    // Set text if provided
    if (text != nullptr) {
        textBox->SetString(text, 0);
    }
    
    return textBox;
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
        pane->size.y = height;
    }
}

void DynamicPaneBuilder::SetPaneScale(nw4r::lyt::Pane* pane, float scaleX, float scaleY) {
    if (pane != nullptr) {
        pane->scale.x = scaleX;
        pane->scale.y = scaleY;
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

void DynamicPaneBuilder::SetPictureColor(nw4r::lyt::Picture* pane, u32 color) {
    if (pane != nullptr) {
        nw4r::ut::Color c(color);
        pane->vertexColours[0] = c;
        pane->vertexColours[1] = c;
        pane->vertexColours[2] = c;
        pane->vertexColours[3] = c;
    }
}

void DynamicPaneBuilder::SetPictureColors(nw4r::lyt::Picture* pane, u32 topLeft, u32 topRight, u32 bottomLeft, u32 bottomRight) {
    if (pane != nullptr) {
        pane->vertexColours[0] = nw4r::ut::Color(topLeft);
        pane->vertexColours[1] = nw4r::ut::Color(topRight);
        pane->vertexColours[2] = nw4r::ut::Color(bottomLeft);
        pane->vertexColours[3] = nw4r::ut::Color(bottomRight);
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

nw4r::lyt::Picture* DynamicUIBuilder::AddPanel(const char* name, float x, float y, float width, float height, u32 color) {
    nw4r::lyt::Picture* panel = DynamicPaneBuilder::CreatePicturePane(name, x, y, width, height, color);
    if (panel != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, panel);
    }
    return panel;
}

nw4r::lyt::TextBox* DynamicUIBuilder::AddText(const char* name, float x, float y, float width, float height, const wchar_t* text) {
    nw4r::lyt::TextBox* textBox = DynamicPaneBuilder::CreateTextPane(name, x, y, width, height, text);
    if (textBox != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, textBox);
    }
    return textBox;
}

nw4r::lyt::Picture* DynamicUIBuilder::AddButton(const char* name, float x, float y, float width, float height, u32 color) {
    nw4r::lyt::Picture* button = DynamicPaneBuilder::CreatePicturePane(name, x, y, width, height, color);
    if (button != nullptr && rootPane != nullptr) {
        DynamicPaneBuilder::AddChild(rootPane, button);
    }
    return button;
}

nw4r::lyt::Picture* DynamicUIBuilder::AddTitleBar(const char* name, float x, float y, float width, float height) {
    return AddPanel(name, x, y, width, height, 0x2244AAFF);
}

nw4r::lyt::Picture* DynamicUIBuilder::AddSeparator(const char* name, float x, float y, float width, float thickness) {
    return AddPanel(name, x, y, width, thickness, 0x888888FF);
}

void DynamicUIBuilder::AddListItem(const char* namePrefix, float x, float y, float width, float height, const wchar_t* text, u32 bgColor) {
    char bgName[32];
    char txtName[32];
    snprintf(bgName, 32, "%s_bg_%d", namePrefix, itemCounter);
    snprintf(txtName, 32, "%s_txt_%d", namePrefix, itemCounter);
    itemCounter++;
    
    nw4r::lyt::Picture* bg = AddPanel(bgName, x, y, width, height, bgColor);
    AddText(txtName, x + 10.0f, y + (height - 16.0f) / 2.0f, width - 20.0f, 16.0f, text);
}

} // namespace UI
} // namespace Pulsar
