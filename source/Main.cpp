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

// SA func (Missing from plugin-sdk, so we add it ourselves)
void __cdecl RsMouseSetPos(RwV2d* pos) {
	Call<0x6194A0, RwV2d*>(pos);
}

void __cdecl SetCursorPosHook(RwV2d* pos) // this is so game doesnt keep centering mouse pointer
{
	if (TheMenu.m_bIsActive)
		return;
	else
		RsMouseSetPos(pos);
}

class debugmenuimgui
{
public:
	debugmenuimgui()
	{
		auto h = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(DX9Hook_Thread), 0, NULL, 0);

		if (!(h == nullptr)) CloseHandle(h);

		CPadUpdateCall = injector::ReadRelativeOffset(0x53BEE6 + 1); // update pads in cgame proccess

		injector::MakeCALL(0x53BEE6, CPad__UpdatePadsHook);
		injector::MakeCALL(0x53E78B, CPad__UpdatePadsHook);
		injector::MakeCALL(0x57C607, CPad__UpdatePadsHook);
		injector::MakeCALL(0x57D7C5, CPad__UpdatePadsHook);
		injector::MakeCALL(0x731540, CPad__UpdatePadsHook);
		injector::MakeCALL(0x748B17, CPad__UpdatePadsHook);

		injector::MakeCALL(0x53E9F1, SetCursorPosHook);
		Events::initGameEvent.after += []
			{
				TheMenu.m_bCanBeActivated = true;
			};
		CdeclEvent<AddressList<0x53EBB1, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CFontRenderEvent;

		// we use ermaccer's hooks except endscene one as skygfx radiosity makes menu look awkward
		CFontRenderEvent += [](){
			GUIImplementationDX9::OnEndScene((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		};
	};
} debugmenuimguiPlugin;
