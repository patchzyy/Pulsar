# Code-Driven UI Layout System for Pulsar

## Overview

This document describes the new code-driven UI layout system implemented for the Pulsar engine. This system allows developers to create entire UI layouts using only C++ code, eliminating the need for external BRLYT (layout) and BRLAN (animation) files.

## Motivation

Traditionally, Mario Kart Wii UI requires:
- **BRLYT files**: Define layout structure, pane positions, and materials
- **BRLAN files**: Define animations
- **BRCTR files**: Control configuration files

This approach has several drawbacks:
1. Requires specialized tools to create and edit
2. Binary format is not version-control friendly
3. Difficult to generate layouts programmatically
4. Hard to debug without visual tools
5. Creates dependencies on external asset files

The code-driven system solves these problems by allowing layouts to be defined entirely in C++.

## Architecture

### Core Components

#### 1. CodePane (`PulsarEngine/UI/CodeLayout/CodePane.hpp/cpp`)

Base abstraction for UI panes created in code.

**Class Hierarchy:**
```
CodePane (abstract base)
├── CodeBasicPane (basic rectangular pane)
├── CodeTextPane (text pane - simplified)
└── CodePicturePane (picture pane - simplified)
```

**Features:**
- Position control (x, y, z coordinates)
- Size control (width, height)
- Scale and rotation
- Alpha transparency
- Visibility toggling
- Hierarchical parent-child relationships
- Named panes for identification

**Example Usage:**
```cpp
// Create a basic colored pane
CodeBasicPane* pane = new CodeBasicPane(200.0f, 50.0f);
pane->SetPosition(0.0f, -100.0f, 0.0f);
pane->SetAlpha(255);

// Set color
nw4r::ut::Color blue = {100, 150, 255, 255};
pane->GetPane()->SetVtxColor(0, blue);
pane->GetPane()->SetVtxColor(1, blue);
pane->GetPane()->SetVtxColor(2, blue);
pane->GetPane()->SetVtxColor(3, blue);
```

#### 2. CodeLayout (`PulsarEngine/UI/CodeLayout/CodeLayout.hpp/cpp`)

Main layout manager that extends `LayoutUIControl`.

**Features:**
- Manages collection of CodePanes
- Handles drawing and updates
- Provides root pane system for hierarchy
- Integrates seamlessly with existing Page system

**Example Usage:**
```cpp
CodeLayout* layout = new CodeLayout();

// Create and set root pane
CodeBasicPane* root = new CodeBasicPane(608.0f, 456.0f);
layout->SetRootPane(root);

// Add child panes
CodeBasicPane* child = new CodeBasicPane(300.0f, 100.0f);
child->SetPosition(0.0f, -50.0f);
root->AddChild(child);

// Build and add to page
layout->BuildLayout();
this->AddControl(controlCount++, *layout, 0);
```

#### 3. CodeDrivenPage (`PulsarEngine/Settings/UI/CodeDrivenPage.hpp/cpp`)

Demo page showcasing the system.

**Features:**
- Creates 5 colored rectangular panes
- Demonstrates hierarchical layout
- Shows position and color control
- Fully functional page accessible from Options menu

## Implementation Details

### File Structure

```
PulsarEngine/
├── UI/
│   ├── CodeLayout/
│   │   ├── CodePane.hpp          # Pane abstraction
│   │   ├── CodePane.cpp          # Pane implementation
│   │   ├── CodeLayout.hpp        # Layout manager
│   │   ├── CodeLayout.cpp        # Layout implementation
│   │   └── README.md             # Component documentation
│   └── UI.hpp/cpp                # Updated with CodeDrivenPage
└── Settings/
    └── UI/
        ├── CodeDrivenPage.hpp    # Demo page header
        ├── CodeDrivenPage.cpp    # Demo page implementation
        └── ExpOptionsPage.*      # Updated to add button
```

### Integration Points

#### 1. Page System Integration

Added `PULPAGE_CODEDRIVEN` to the `PulPageId` enum in `UI.hpp`:
```cpp
enum PulPageId {
    PULPAGE_INITIAL = 0x100,
    // ... other pages ...
    PULPAGE_CODEDRIVEN,
    PULPAGE_MAX = PULPAGE_CODEDRIVEN - PULPAGE_INITIAL + 1
};
```

#### 2. Page Creation

Added case in `ExpSection::CreateAndInitPage` (`UI.cpp`):
```cpp
case CodeDrivenPage::id:
    page = new CodeDrivenPage;
    break;
```

#### 3. Section Initialization

Page is created in relevant sections (`UI.cpp`):
```cpp
case SECTION_OPTIONS:
    this->CreateAndInitPage(*this, SettingsPanel::id);
    this->CreateAndInitPage(*this, CodeDrivenPage::id);
```

#### 4. Navigation

Added button in Options menu (`ExpOptionsPage.cpp`) to navigate to demo page:
```cpp
if (pushButton.buttonId == 6) {
    this->nextPageId = static_cast<PageId>(CodeDrivenPage::id);
    this->EndStateAnimated(0, pushButton.GetAnimationFrameSize());
}
```

## Usage Guide

### Creating a Simple Layout

```cpp
void MyPage::BuildLayout() {
    // Create layout manager
    codeLayout = new CodeLayout();
    
    // Create root pane (full screen)
    CodeBasicPane* root = new CodeBasicPane(608.0f, 456.0f);
    root->SetPosition(0.0f, 0.0f, 0.0f);
    codeLayout->SetRootPane(root);
    
    // Create title bar
    CodeBasicPane* titleBar = new CodeBasicPane(600.0f, 50.0f);
    titleBar->SetPosition(0.0f, -200.0f);
    nw4r::ut::Color blue = {50, 100, 200, 255};
    titleBar->GetPane()->SetVtxColor(0, blue);
    root->AddChild(titleBar);
    
    // Create content area
    CodeBasicPane* content = new CodeBasicPane(550.0f, 300.0f);
    content->SetPosition(0.0f, 0.0f);
    nw4r::ut::Color white = {255, 255, 255, 200};
    content->GetPane()->SetVtxColor(0, white);
    root->AddChild(content);
    
    // Build and add to page
    codeLayout->BuildLayout();
    this->AddControl(controlCount++, *codeLayout, 0);
}
```

### Dynamic Layouts

```cpp
// Create a grid of colored boxes
for(int row = 0; row < 3; row++) {
    for(int col = 0; col < 4; col++) {
        CodeBasicPane* box = new CodeBasicPane(50.0f, 50.0f);
        float x = -150.0f + (col * 100.0f);
        float y = -100.0f + (row * 75.0f);
        box->SetPosition(x, y);
        
        // Color based on position
        nw4r::ut::Color color = {
            (u8)(col * 60),
            (u8)(row * 80),
            200,
            255
        };
        box->GetPane()->SetVtxColor(0, color);
        
        root->AddChild(box);
    }
}
```

## Benefits

### 1. **Version Control**
All layout changes are code diffs, making collaboration easier:
```diff
- pane->SetPosition(0.0f, -50.0f);
+ pane->SetPosition(0.0f, -75.0f);
```

### 2. **Type Safety**
C++ compiler catches errors at compile time:
```cpp
pane->SetPosition(100.0f, 50.0f);  // OK
pane->SetPosition("invalid");       // Compile error!
```

### 3. **Dynamic Generation**
Create layouts based on runtime data:
```cpp
for(int i = 0; i < playerCount; i++) {
    CodeBasicPane* slot = new CodeBasicPane(100.0f, 30.0f);
    slot->SetPosition(0.0f, -200.0f + i * 40.0f);
    // ...
}
```

### 4. **No External Dependencies**
No need for BRLYT/BRLAN files in the asset pack.

### 5. **Easier Debugging**
Use standard C++ debugging tools and print statements.

### 6. **Programmatic Control**
Full programmatic control over all layout properties.

## Limitations

### Current Limitations

1. **Text Rendering**: Text panes are simplified; full text rendering requires additional font system integration
2. **Textures**: Material and texture system not fully integrated
3. **Animations**: Would need manual implementation
4. **Complex Shapes**: Currently limited to basic rectangular panes

### When to Use Traditional Files

Traditional BRLYT/BRLAN files are still better for:
- Complex visual designs created by artists
- Layouts with many textures and materials
- Pre-made animation sequences
- Reusable UI components across multiple games

### When to Use Code-Driven System

The code-driven system excels at:
- Programmatically generated layouts
- Dynamic layouts based on runtime data
- Simple geometric layouts
- Prototype and debug UIs
- Layouts that change frequently during development

## Future Enhancements

Potential improvements for the system:

### 1. Text Support
Full integration with font rendering:
```cpp
CodeTextPane* text = new CodeTextPane(300.0f, 40.0f);
text->SetText(L"Hello World");
text->SetFontSize(16.0f);
text->SetTextColor(white);
```

### 2. Material System
Support for textures and materials:
```cpp
CodePicturePane* image = new CodePicturePane(100.0f, 100.0f);
image->SetTexture("common/button.tpl");
image->SetTextureCoords(0, 0, 1, 1);
```

### 3. Animation System
Code-driven animations:
```cpp
pane->AnimateTo(
    Vec3(0, 100, 0),  // target position
    60,                // frames
    EaseOutQuad        // easing function
);
```

### 4. Layout Helpers
Higher-level layout abstractions:
```cpp
GridLayout* grid = new GridLayout(3, 4);  // 3 rows, 4 cols
grid->AddChild(pane1, 0, 0);
grid->AddChild(pane2, 0, 1);
// ...
```

### 5. More Pane Types
Additional pane types:
- Window panes with borders
- Slider/progress bar panes
- Button panes
- List panes

## Testing

To test the demo page:

1. Build the project with `make`
2. Install with `make install`
3. Launch the game
4. Navigate to Options menu
5. Press the "Code-Driven Demo" button
6. You should see 5 colored rectangles:
   - Red title bar
   - Green description area
   - Cyan, blue, and yellow feature boxes

## Conclusion

The code-driven UI layout system provides a powerful alternative to traditional BRLYT files for certain use cases. While it doesn't replace the need for artist-created layouts, it offers significant advantages for programmatic and dynamic UIs. The system integrates seamlessly with the existing Pulsar page infrastructure and can be used alongside traditional layouts as needed.

For questions or contributions, refer to the inline code documentation and the README in `/PulsarEngine/UI/CodeLayout/`.
