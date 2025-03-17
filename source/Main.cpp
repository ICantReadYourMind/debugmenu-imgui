/*
	Dear ImGui wrapper for debugmenu by aap

	This aims to provide an alternative for debugmenu that is a little more modern while being compatible with existing plugins.
*/

#include "plugin.h"
#include <d3d9.h>
#include "rwd3d9/rwd3d9.h"
#include "gui/dx9hook.h"
#include "gui/imgui/imgui.h"
#include "debugmenu.h"
#include "CMenuManager.h"

using namespace plugin;
injector::memory_pointer_raw CPadUpdateCall;
void __cdecl CPad__UpdatePadsHook()
{
	if (TheMenu.m_bIsActive) {
		// Clear mouse data so the camera won't move when closing the debug menu
		CPad* pad = CPad::GetPad(0);
		CPad::UpdatePads();
		CPad::NewMouseControllerState.x = 0.0f;
		CPad::NewMouseControllerState.y = 0.0f;
		CPad::ClearMouseHistory();
		pad->NewState.DPadUp = 0;
		pad->OldState.DPadUp = 0;
		pad->NewState.DPadDown = 0;
		pad->OldState.DPadDown = 0;
		pad->DisablePlayerControls = true;
		return;
	}
	else
		reinterpret_cast<void(*)()>(CPadUpdateCall.as_int())();
}

BOOL __stdcall SetCursorPosHook(int x, int y) // this is so game doesnt keep centering mouse pointer
{
	if (TheMenu.m_bIsActive)
		return false;
	else
		return SetCursorPos(x, y);
}
class debugmenuimgui
{
public:
	debugmenuimgui()
	{
		auto h = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(DX9Hook_Thread), 0, NULL, 0);

		if (!(h == nullptr)) CloseHandle(h);

		CPadUpdateCall = injector::ReadRelativeOffset(0x4A4412 + 1); // update pads in cgame proccess

		injector::MakeCALL(0x4A4412, CPad__UpdatePadsHook);
		injector::MakeCALL(0x490476, CPad__UpdatePadsHook);
		injector::MakeCALL(0x4A5C7E, CPad__UpdatePadsHook);
		injector::MakeCALL(0x4A669F, CPad__UpdatePadsHook);
		injector::MakeCALL(0x4AB0A0, CPad__UpdatePadsHook);
		injector::MakeCALL(0x54460C, CPad__UpdatePadsHook);
		injector::MakeCALL(0x5FFFD9, CPad__UpdatePadsHook);
		injector::MakeCALL(0x60018F, CPad__UpdatePadsHook);
		injector::MakeCALL(0x61D9F4, CPad__UpdatePadsHook);
		injector::MakeCALL(0x61DBB6, CPad__UpdatePadsHook);
	 	injector::MakeCALL(0x61DD47, CPad__UpdatePadsHook);

		injector::MakeCALL(0x602115, SetCursorPosHook);
		Events::initGameEvent += []
			{
				TheMenu.m_bCanBeActivated = true;
			};

		CdeclEvent<AddressList<0x4A613D, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CFontRenderEvent;

		CFontRenderEvent += [](){
		GUIImplementationDX9::OnEndScene((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		};

	};
} debugmenuimguiPlugin;
