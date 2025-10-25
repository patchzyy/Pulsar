# Code-Driven UI Layout System

This directory contains a new system for creating UI layouts entirely in C++ code, without needing BRLYT or BRLAN files.

## Components

### CodePane (`CodePane.hpp/cpp`)
Base class system for creating UI panes programmatically:
- **CodePane**: Abstract base class for all code-driven panes
- **CodeBasicPane**: Basic rectangular pane with position, size, and color
- **CodeTextPane**: Text pane (currently simplified to basic pane)
- **CodePicturePane**: Picture pane (currently simplified to basic pane)

Features:
- Set position (x, y, z)
- Set size (width, height)
- Set scale
- Set rotation
- Set alpha transparency
- Set visibility
- Add child panes for hierarchical layouts

### CodeLayout (`CodeLayout.hpp/cpp`)
Main layout management class that extends `LayoutUIControl`:
- Manages a collection of CodePanes
- Handles drawing and updates
- Provides a root pane system for organizing UI hierarchy
- Can be added as a UIControl to any Page

## Demo Page

`CodeDrivenPage` in `/PulsarEngine/Settings/UI/` demonstrates the system:
- Creates 5 colored rectangular panes programmatically
- Shows hierarchical pane relationships
- Demonstrates position and color control
- Accessible from the Options menu (page ID: PULPAGE_CODEDRIVEN)

## Usage Example

```cpp
// Create the layout
CodeLayout* layout = new CodeLayout();

// Create root pane
CodeBasicPane* root = new CodeBasicPane(608.0f, 456.0f);
root->SetPosition(0.0f, 0.0f, 0.0f);
layout->SetRootPane(root);

// Create a colored child pane
CodeBasicPane* colorPane = new CodeBasicPane(200.0f, 50.0f);
colorPane->SetPosition(0.0f, -100.0f);
nw4r::ut::Color red = {255, 0, 0, 255};
colorPane->GetPane()->SetVtxColor(0, red);
root->AddChild(colorPane);

// Build and add to page
layout->BuildLayout();
this->AddControl(controlCount++, *layout, 0);
```

## Benefits

1. **No external files needed**: All layout defined in code
2. **Dynamic layouts**: Can change based on runtime conditions
3. **Version control friendly**: Layout changes are code diffs
4. **Type safety**: C++ compiler catches errors
5. **Easier to understand**: No need to learn BRLYT format
6. **Programmatic generation**: Can create layouts algorithmically

## Limitations

Current implementation is simplified:
- Text rendering requires additional integration with font system
- Texture/material system not fully integrated
- Animations would need manual implementation
- Best suited for simple geometric layouts currently

## Future Enhancements

Potential improvements:
- Full TextBox integration with font rendering
- Material and texture support for CodePicturePane
- Animation system for code-driven animations
- More pane types (windows, sliders, etc.)
- Layout helpers (grid, flex, etc.)

## Integration

The system is fully integrated into the Pulsar page system:
1. Added PULPAGE_CODEDRIVEN to PulPageId enum
2. CodeDrivenPage registered in CreateAndInitPage
3. Automatically created in OPTIONS and related sections
4. Can be navigated to programmatically via nextPageId
