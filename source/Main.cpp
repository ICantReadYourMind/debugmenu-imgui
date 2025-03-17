/*
	Dear ImGui wrapper for debugmenu by aap

	This aims to provide an alternative for debugmenu that is a little more modern while being compatible with existing plugins.
*/



#include "plugin.h"
#include <d3d9.h>
#ifndef GTASA
#include "rwd3d9/rwd3d9.h"
#endif
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
		pad->NewMouseControllerState.x = 0.0f;
		pad->NewMouseControllerState.y = 0.0f;
#ifdef GTA3
		pad->ClearMouseHistory();
#else
		CPad::ClearMouseHistory();
#endif

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

#ifdef GTASA
// SA func (Missing from plugin-sdk, so we add it ourselves)
void __cdecl RsMouseSetPos(RwV2d* pos) {
	Call<0x6194A0, RwV2d*>(pos);
}

void __cdecl RsMouseSetPosHook(RwV2d* pos) // this is so game doesnt keep centering mouse pointer
{
	if (TheMenu.m_bIsActive)
		return;
	else
		RsMouseSetPos(pos);
}
#else
BOOL __stdcall SetCursorPosHook(int x, int y) // this is so game doesnt keep centering mouse pointer
{
	if (TheMenu.m_bIsActive)
		return false;
	else
		return SetCursorPos(x, y);
}
#endif

class debugmenuimgui
{
public:
	debugmenuimgui()
	{
		auto h = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(DX9Hook_Thread), 0, NULL, 0);

		if (!(h == nullptr)) CloseHandle(h);

#ifdef GTA3
		CPadUpdateCall = injector::ReadRelativeOffset(0x48C850 + 1);

		injector::MakeCALL(0x48C850, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48AE15, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48DE2F, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48E717, CPad__UpdatePadsHook);
		injector::MakeCALL(0x582AA9, CPad__UpdatePadsHook);
		injector::MakeCALL(0x582C3C, CPad__UpdatePadsHook);
		injector::MakeCALL(0x592C0C, CPad__UpdatePadsHook);

		patch::SetPointer(0x61D4E4, SetCursorPosHook);
#elif GTAVC
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
#elif GTASA
		CPadUpdateCall = injector::ReadRelativeOffset(0x53BEE6 + 1); // update pads in cgame proccess

		injector::MakeCALL(0x53BEE6, CPad__UpdatePadsHook);
		injector::MakeCALL(0x53E78B, CPad__UpdatePadsHook);
		injector::MakeCALL(0x57C607, CPad__UpdatePadsHook);
		injector::MakeCALL(0x57D7C5, CPad__UpdatePadsHook);
		injector::MakeCALL(0x731540, CPad__UpdatePadsHook);
		injector::MakeCALL(0x748B17, CPad__UpdatePadsHook);

		injector::MakeCALL(0x53E9F1, RsMouseSetPosHook);
#endif

		Events::initGameEvent.after += []
			{
				TheMenu.m_bCanBeActivated = true;
			};

		// we use ermaccer's hooks except endscene one as skygfx radiosity makes menu look awkward
#ifdef GTA3
		CdeclEvent<AddressList<0x48E6DF, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CCreditsRenderEvent;
		CCreditsRenderEvent.after += []() {
#elif GTAVC
		CdeclEvent<AddressList<0x4A613D, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CFontRenderEvent;
		CFontRenderEvent += []() {
#elif GTASA
		CdeclEvent<AddressList<0x53EBB1, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CFontRenderEvent;
		CFontRenderEvent.after += [] {
#endif
			GUIImplementationDX9::OnEndScene((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
			};
		};
			} debugmenuimguiPlugin;
