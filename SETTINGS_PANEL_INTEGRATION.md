# Settings Panel Integration Summary

## Overview

The CodeDrivenPage demo is now accessible as the 13th "page" in the Settings Panel navigation cycle.

## Changes Made

### 1. SettingsPanel.cpp
- **Added include**: `#include <Settings/UI/CodeDrivenPage.hpp>`
- **Modified `OnActivate()`**: Added detection for demo page index and automatic redirect
- **Modified `GetNextSheetIdx()`**: Extended page count by 1 to include demo page
- **Modified `GetNextBMGOffset()`**: Added special handling for demo page BMG display

### 2. CodeDrivenPage.cpp  
- **Added include**: `#include <Settings/UI/SettingsPanel.hpp>`
- **Updated constructor**: Changed `prevPageId` from `PAGE_OPTIONS` to `SettingsPanel::id`

### 3. Documentation
- Created `/PulsarEngine/UI/CodeLayout/ACCESS.md` - Detailed access instructions
- Updated `CODE_DRIVEN_UI_SYSTEM.md` - Added Settings Panel integration section

## How It Works

```
User Flow:
Options → Settings → Navigate RIGHT through pages → Auto-redirect to Demo

Technical Flow:
1. User navigates RIGHT past the last settings page (Misc)
2. sheetIdx becomes 12 (Settings::Params::pageCount)
3. OnActivate() detects sheetIdx >= pageCount
4. Page sets nextPageId to CodeDrivenPage::id
5. EndStateAnimated(0, 0) triggers page transition
6. CodeDrivenPage loads and displays colored panes
7. User presses BACK → Returns to SettingsPanel

Navigation Cycle:
0: Race 1
1: Race 2
2: Menu
3: Online
4: Sound
5: Battle
6: Friend Room 1
7: Friend Room 2
8: OTT
9: KO
10: Extended Teams
11: Misc
12: [Virtual Demo Page] → Auto-redirect to CodeDrivenPage
```

## Key Implementation Details

### Virtual Page Approach
Instead of adding CodeDrivenPage as a real settings page with controls, we use a "virtual page" that exists only in the navigation index. When activated, it immediately redirects.

**Benefits:**
- No need to modify Settings page structure
- Clean separation between settings and demo
- Demo page can have completely different layout
- Easy to remove if needed

### Navigation Wrapping
The page index wraps around: Misc → Demo → Race 1 → ... → Misc (cycle continues)

### BMG Handling
The demo page reuses BMG_TEXT for the button label. For custom text, add a BMG entry and update `GetNextBMGOffset()`.

## Testing Checklist

- [ ] Navigate to Options → Settings
- [ ] Press RIGHT through all 12 settings pages
- [ ] Press RIGHT once more - should see redirect to demo
- [ ] Verify 5 colored rectangles appear
- [ ] Press BACK - should return to Settings Panel
- [ ] Verify can navigate to other settings pages normally
- [ ] Test LEFT navigation wraps from Race 1 to Demo
- [ ] Test in different game modes (Options, Froom, WFC)

## Files Modified

```
PulsarEngine/Settings/UI/
├── SettingsPanel.cpp       [Modified - Added redirect logic]
├── CodeDrivenPage.cpp      [Modified - Updated prevPageId]
└── ExpOptionsPage.cpp      [Previously modified - Direct button]

PulsarEngine/UI/CodeLayout/
├── ACCESS.md               [Created - Access documentation]
└── README.md               [Existing - Component documentation]

CODE_DRIVEN_UI_SYSTEM.md     [Updated - Integration section]
SETTINGS_PANEL_INTEGRATION.md [Created - This file]
```

## Future Enhancements

1. **Custom BMG Text**: Add dedicated BMG entry for "Code Demo" label
2. **Multiple Demo Pages**: Extend to include multiple demo/test pages
3. **Settings Context**: Pass settings context to demo page for dynamic content
4. **Dev Tools Menu**: Create a full developer tools submenu accessible this way

## Notes

- The demo page is always accessible, even in voting sections
- No actual settings are stored for the demo page
- The virtual page approach could be reused for other non-settings pages
- This pattern works well for debug/diagnostic pages that don't fit the settings model
