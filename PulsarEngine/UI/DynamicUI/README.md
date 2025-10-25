# Dynamic UI Layout System

This module provides a code-based UI layout system for Mario Kart Wii that eliminates the need for external brlyt and brctr files.

## Overview

The Dynamic UI system allows developers to create entire UI layouts programmatically in C++, without requiring external binary layout files. This provides several advantages:

- **No External Files**: All UI is defined in code, reducing asset dependencies
- **Dynamic Generation**: UI elements can be created and modified at runtime
- **Easier Maintenance**: Changes to UI don't require external tooling
- **Version Control**: UI code changes are tracked in the same repository

## Components

### DynamicPaneBuilder
A static utility class providing methods to create UI panes programmatically:

```cpp
// Create a basic pane
nw4r::lyt::Pane* pane = DynamicPaneBuilder::CreatePane("name", x, y, width, height);

// Create a colored rectangle (picture pane)
nw4r::lyt::Picture* rect = DynamicPaneBuilder::CreatePicturePane("rect", x, y, width, height, 0xFF0000FF);

// Create a text pane
nw4r::lyt::TextBox* text = DynamicPaneBuilder::CreateTextPane("text", x, y, width, height, L"Hello!");

// Modify pane properties
DynamicPaneBuilder::SetPanePosition(pane, x, y, z);
DynamicPaneBuilder::SetPaneSize(pane, width, height);
DynamicPaneBuilder::SetPaneAlpha(pane, 200);
DynamicPaneBuilder::SetPictureColor(picture, 0x00FF00FF);

// Add child panes
DynamicPaneBuilder::AddChild(parent, child);
```

### DynamicLayoutControl
A UIControl subclass that manages dynamically created panes:

```cpp
DynamicLayoutControl control;
nw4r::lyt::Pane* root = DynamicPaneBuilder::CreatePane("root", 0, 0, 640, 480);
control.InitWithRootPane(root);
// Add as a control to your page
```

### DynamicUIBuilder
A high-level builder class for common UI patterns:

```cpp
DynamicUIBuilder builder(rootPane);

// Add common UI elements
builder.AddPanel("panel", x, y, width, height, color);
builder.AddText("label", x, y, width, height, L"Text");
builder.AddButton("button", x, y, width, height);
builder.AddTitleBar("title", x, y, width, height);
builder.AddSeparator("sep", x, y, width);
builder.AddListItem("item", x, y, width, height, L"Item Text");
```

## Example: DynamicSettingsPage

The `DynamicSettingsPage` demonstrates a complete settings page built entirely with the dynamic UI system. It includes:

- Title bar
- Main content panel
- Text labels
- List items
- Separator lines
- Decorative borders

### Accessing the Dynamic Settings Page

From the settings panel (accessible via the Options menu), navigate past the last settings category using the right shoulder buttons (or left from the first category). The panel wraps to the **Dynamic Layouts** entry, which opens the dynamic settings demo page.

### Code Example

```cpp
void BuildMyUI() {
    // Create root pane
    nw4r::lyt::Pane* root = DynamicPaneBuilder::CreatePane("root", 0, 0, 640, 480);
    
    // Use the builder for convenience
    DynamicUIBuilder builder(root);
    
    // Build the layout
    builder.AddTitleBar("title", -300, -200, 600, 50);
    builder.AddText("title_text", -250, -185, 500, 30, L"My Custom UI");
    builder.AddPanel("panel", -280, -140, 560, 320, 0x222222FF);
    builder.AddListItem("option", -260, -120, 520, 40, L"Option 1");
    builder.AddListItem("option", -260, -70, 520, 40, L"Option 2");
}
```

## Integration

To integrate dynamic UI into a page:

1. Include the headers:
```cpp
#include <UI/DynamicUI/DynamicLayout.hpp>
#include <UI/DynamicUI/DynamicSettingsPage.hpp>
```

2. Create and initialize a DynamicLayoutControl
3. Build your UI using DynamicPaneBuilder or DynamicUIBuilder
4. Add the control to your page

## Technical Details

### Coordinate System
- Origin is at screen center (320, 240)
- X increases right, Y increases down
- Default screen size: 640x480

### Colors
Colors are specified as 32-bit RGBA values (0xRRGGBBAA):
- `0xFF0000FF` = Red
- `0x00FF00FF` = Green
- `0x0000FFFF` = Blue
- `0xFFFFFFFF` = White
- `0x000000FF` = Black

### Limitations
- Textures and materials require proper setup
- Complex animations require additional work
- Font rendering depends on system fonts

## Future Enhancements

Potential improvements:
- Support for texture loading
- Animation system integration
- Interactive button controls
- Layout serialization/deserialization
- More helper functions for common patterns

## Files

- `DynamicLayout.hpp` - Core pane builder declarations
- `DynamicLayout.cpp` - Pane builder implementations
- `DynamicSettingsPage.hpp` - Example page declaration
- `DynamicSettingsPage.cpp` - Example page implementation
- `README.md` - This documentation
