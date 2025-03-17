# debugmenu-imgui
Dear ImGui implementation for aap's debugmenu

## Supported games:
GTA 3 1.0 US, GTA VC 1.0 US, GTA SA 1.0 US

For 3 and VC, you need d3d8to9 and rwd3d9 (included with SkyGFX)

## How to use:
Download the latest release from the releases page and copy debugmenu.dll to your game folder (next to the exe). Make sure you have a plugin that uses it. 
Plugins that used aap's debugmenu are fully compatible with this as this is intended to be a replacement for it.

## How to build:
Setup plugin-sdk and build using the solution in the project-files folder

## Thanks to:

aap (for the original debugmenu)

ermaccer (for his d3d9 hook and ImGui implementation)

TsudaKageyu (for MinHook)

Plugin-SDK

wuzimu7171 (for VC and SA port)
