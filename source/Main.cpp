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

void CPad__UpdatePadsHook()
{
	if (TheMenu.m_bIsActive)
	{
		auto pad = CPad::GetPad(0);
		pad->OldMouseControllerState.x = 0.0f;
		pad->OldMouseControllerState.y = 0.0f;
		pad->NewMouseControllerState.x = 0.0f;
		pad->NewMouseControllerState.y = 0.0f;
		pad->PCTempMouseControllerState.x = 0.0f;
		pad->PCTempMouseControllerState.y = 0.0f;
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

		CPadUpdateCall = injector::ReadRelativeOffset(0x48C850 + 1);

		injector::MakeCALL(0x48C850, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48AE15, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48DE2F, CPad__UpdatePadsHook);
		injector::MakeCALL(0x48E717, CPad__UpdatePadsHook);
		injector::MakeCALL(0x582AA9, CPad__UpdatePadsHook);
		injector::MakeCALL(0x582C3C, CPad__UpdatePadsHook);
		injector::MakeCALL(0x592C0C, CPad__UpdatePadsHook);

		patch::SetPointer(0x61D4E4, SetCursorPosHook);

		Events::initGameEvent.after += []
			{
				TheMenu.m_bCanBeActivated = true;
			};

		CdeclEvent<AddressList<0x48E6DF, H_CALL>, PRIORITY_AFTER, ArgPickNone, void(void)> CCreditsRenderEvent;

		// we use ermaccer's hooks except endscene one as skygfx radiosity makes menu look awkward
		CCreditsRenderEvent.after += [](){
			GUIImplementationDX9::OnEndScene((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
		};
	};
} debugmenuimguiPlugin;
