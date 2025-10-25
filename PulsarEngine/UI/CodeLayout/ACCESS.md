# Accessing the Code-Driven Demo Page

The code-driven UI demo page is now accessible through the Settings Panel!

## How to Access

1. **From the Main Menu**: 
   - Go to Options
   - Click the Settings button
   
2. **Navigate to Demo Page**:
   - In the Settings Panel, use the LEFT/RIGHT navigation buttons
   - Cycle through all the settings pages (Race 1, Race 2, Menu, Online, Sound, Battle, Friend Room 1, Friend Room 2, OTT, KO, Extended Teams, Misc)
   - After the last settings page (Misc), press RIGHT once more
   - The page will automatically redirect to the Code-Driven Demo Page

3. **From the Demo Page**:
   - Press BACK or click OK to return to the Settings Panel
   - The Settings Panel will remember you were on the last page and will stay there

## Alternative Access

The demo page is also accessible directly from the Options menu via the second settings button (button ID 6) if you modified ExpOptionsPage.

## Navigation Flow

```
Options Menu
    ↓
Settings Panel → (navigate with LEFT/RIGHT)
    ↓
    ├─ Race 1
    ├─ Race 2
    ├─ Menu
    ├─ Online
    ├─ Sound
    ├─ Battle
    ├─ Friend Room 1
    ├─ Friend Room 2
    ├─ OTT
    ├─ KO
    ├─ Extended Teams
    ├─ Misc
    └─ [DEMO PAGE] → Code-Driven Demo
         ↑
         └─ (auto-redirect on activate)
```

## Implementation Details

The demo page works by:
1. Adding a virtual "13th page" to the Settings Panel (pageCount + 1)
2. When that page index is activated, `OnActivate()` detects it's >= pageCount
3. The page immediately redirects to CodeDrivenPage using `EndStateAnimated()`
4. The navigation buttons show this as part of the normal page cycle

This creates a seamless integration where the demo page appears as just another section in the settings flow.

## BMG Messages

The LEFT/RIGHT button text for the demo page uses a calculated BMG offset. If you want custom text:
- Add a BMG entry for "Code Demo" or similar
- Update the GetNextBMGOffset() method to return the correct BMG ID for the demo page

## Testing

To test the integration:
1. Build the project
2. Navigate to Options → Settings
3. Press RIGHT through all pages
4. Verify the page redirects to the colorful demo layout
5. Press BACK to return to Settings Panel
6. Verify you can navigate to other settings pages normally
