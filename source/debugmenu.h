#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include "plugin.h"
#include "CPad.h"
#include "CCamera.h"
#include "gui/imgui/imgui.h"

enum EntryType : unsigned char {
	ENTRY_MENU,
	ENTRY_TOGGLE,
	ENTRY_CMD,
	ENTRY_VAR,
	ENTRY_COMBO // we need this to identify if lowerbound offset and shiet should be taken into account
};

enum eVarType : unsigned char {
	VAR_INT8,
	VAR_UINT8,
	VAR_INT16,
	VAR_UINT16,
	VAR_INT32,
	VAR_UINT32,
	VAR_INT64,
	VAR_UINT64,
	VAR_FLOAT32,
	VAR_FLOAT64
};

union VarTypes {
	char I8;
	short I16;
	int I32;
	long long I64;

	unsigned char UI8;
	unsigned short UI16;
	unsigned int UI32;
	unsigned long long UI64;

	float F32;
	double F64;
};

class MenuEntry {
public:
	EntryType m_eType;
	std::string m_szName;

	struct SingleEntry
	{
		eVarType m_eVarType;
		char** m_aStrings;
		void(*m_func)();

		VarTypes m_lowerBound;
		VarTypes m_upperBound;
		VarTypes m_step;

		VarTypes* m_pVarPtr;

		bool m_bIsToggled;
		int m_nSelectedItem;
		int m_nLowerBoundOffset;
		bool m_bWrap;
	} m_entry;

	struct MenuPage {
		MenuEntry* m_previousPage;
		std::vector<MenuEntry> m_aEntriesInPage;
	} m_page;

	template<typename T>
	inline void SetVars(eVarType type, T upperBound, T lowerBound, T step, void* ptr)
	{
		m_entry.m_eVarType = type;
		m_entry.m_pVarPtr = (VarTypes*)ptr;
		if (m_eType == ENTRY_COMBO)
			m_entry.m_nLowerBoundOffset = lowerBound;
		switch (type)
		{
		case VAR_INT8:
			m_entry.m_lowerBound.I8 = lowerBound;
			m_entry.m_upperBound.I8 = upperBound;
			m_entry.m_step.I8 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I8;
			break;
		case VAR_INT16:
			m_entry.m_lowerBound.I16 = lowerBound;
			m_entry.m_upperBound.I16 = upperBound;
			m_entry.m_step.I16 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I16;
			break;
		case VAR_INT32:
			m_entry.m_lowerBound.I32 = lowerBound;
			m_entry.m_upperBound.I32 = upperBound;
			m_entry.m_step.I32 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I32;
			break;
		case VAR_INT64:
			m_entry.m_lowerBound.I64 = lowerBound;
			m_entry.m_upperBound.I64 = upperBound;
			m_entry.m_step.I64 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I64;
			break;
		case VAR_UINT8:
			m_entry.m_lowerBound.UI8 = lowerBound;
			m_entry.m_upperBound.UI8 = upperBound;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI8;
			m_entry.m_step.UI8 = step;
			break;
		case VAR_UINT16:
			m_entry.m_lowerBound.UI16 = lowerBound;
			m_entry.m_upperBound.UI16 = upperBound;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI16;
			m_entry.m_step.UI16 = step;
			break;
		case VAR_UINT32:
			m_entry.m_lowerBound.UI32 = lowerBound;
			m_entry.m_upperBound.UI32 = upperBound;
			m_entry.m_step.UI32 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI32;
			break;
		case VAR_UINT64:
			m_entry.m_lowerBound.UI64 = lowerBound;
			m_entry.m_upperBound.UI64 = upperBound;
			m_entry.m_step.UI64 = step;
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI64;
			break;
		case VAR_FLOAT32:
			m_entry.m_lowerBound.F32 = lowerBound;
			m_entry.m_upperBound.F32 = upperBound;
			m_entry.m_step.F32 = step;
			break;
		case VAR_FLOAT64:
			m_entry.m_lowerBound.F64 = lowerBound;
			m_entry.m_upperBound.F64 = upperBound;
			m_entry.m_step.F64 = step;
			break;
		}
	}

#define MenuEntryCtor(typeName, enumType) MenuEntry(std::string_view name, typeName* \
						varPtr, typeName lowerBound, typeName upperBound, typeName step, void(*func)(), const char** strings, std::string_view path) \
	{  \
		m_szName = name; \
		m_szName += "##"; \
		m_szName += path; \
		m_entry.m_func = func; \
		m_entry.m_aStrings = (char**)strings; \
		m_eType = ENTRY_VAR; \
		m_entry.m_bWrap = false; \
		if (strings) \
			{ \
				m_eType = ENTRY_COMBO; \
				if (upperBound - lowerBound == 1) \
					if (std::string_view(strings[0]) == "Off" && std::string_view(strings[1]) == "On") \
						{ \
							m_entry.m_bIsToggled = *varPtr != 0; \
							m_eType = ENTRY_TOGGLE; \
						} \
			} \
		SetVars(enumType, upperBound, lowerBound, step, (void*)varPtr); \
	};

	MenuEntryCtor(char, VAR_INT8)
	MenuEntryCtor(short, VAR_INT16)
	MenuEntryCtor(int, VAR_INT32)
	MenuEntryCtor(long long, VAR_INT64)
	MenuEntryCtor(unsigned char, VAR_UINT8)
	MenuEntryCtor(unsigned short, VAR_UINT16)
	MenuEntryCtor(unsigned int, VAR_UINT32)
	MenuEntryCtor(unsigned long long, VAR_UINT64)
	MenuEntryCtor(float, VAR_FLOAT32)
	MenuEntryCtor(double, VAR_FLOAT64)

	MenuEntry()
	{
		m_eType = ENTRY_MENU;
		m_szName = "";
		m_page.m_previousPage = nullptr;
	}

	MenuEntry(std::string_view path, std::string_view pageName, MenuEntry* prevPage = nullptr)
	{
		m_eType = ENTRY_MENU;
		m_szName = pageName;
		m_szName += "##";
		m_szName += path;
		m_page.m_previousPage = prevPage;
	}

	MenuEntry(std::string_view name, void(*func)(), std::string_view(path))
	{
		m_eType = ENTRY_CMD;
		m_szName = name;
		m_szName += "##";
		m_szName += path;
		m_entry.m_func = func;
	}

	inline void UpdateIsToggled()
	{
		switch (m_entry.m_eVarType)
		{
		case VAR_INT8:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->I8 != 0;
			break;
		case VAR_UINT8:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->UI8 != 0;
			break;
		case VAR_INT16:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->I16 != 0;
			break;
		case VAR_UINT16:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->UI16 != 0;
			break;
		case VAR_INT32:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->I32 != 0;
			break;
		case VAR_UINT32:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->UI32 != 0;
			break;
		case VAR_INT64:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->I64 != 0;
			break;
		case VAR_UINT64:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->UI64 != 0;
			break;
		case VAR_FLOAT32:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->F32 != 0.0f;
			break;
		case VAR_FLOAT64:
			m_entry.m_bIsToggled = m_entry.m_pVarPtr->F64 != 0.0;
			break;
		}
	}

	inline long long GetValueAsInt(const VarTypes& vars)
	{
		switch (m_entry.m_eVarType)
		{
		case VAR_INT8:
			return (vars.I8);
		case VAR_UINT8:
			return (vars.UI8);
		case VAR_INT16:
			return (vars.I16);
		case VAR_UINT16:
			return (vars.UI16);
		case VAR_INT32:
			return (vars.I32);
		case VAR_UINT32:
			return (vars.UI32);
		case VAR_INT64:
			return (vars.I64);
		case VAR_UINT64:
			return (vars.UI64);
		case VAR_FLOAT32:
			return (vars.F32);
		case VAR_FLOAT64:
			return (vars.F64);
		}
	}

	inline void UpdateSelectedItem()
	{
		switch (m_entry.m_eVarType)
		{
		case VAR_INT8:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I8;
			break;
		case VAR_UINT8:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI8;
			break;
		case VAR_INT16:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I16;
			break;
		case VAR_UINT16:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI16;
			break;
		case VAR_INT32:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I32;
			break;
		case VAR_UINT32:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI32;
			break;
		case VAR_INT64:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->I64;
			break;
		case VAR_UINT64:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->UI64;
			break;
		case VAR_FLOAT32:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->F32;
			break;
		case VAR_FLOAT64:
			m_entry.m_nSelectedItem = m_entry.m_pVarPtr->F64;
			break;
		}
		
		m_entry.m_nSelectedItem -= m_entry.m_nLowerBoundOffset;
	}

	inline void ChangeVar(auto val)
	{
		switch (m_entry.m_eVarType)
		{
		case VAR_INT8:
			m_entry.m_pVarPtr->I8 = val;
			break;
		case VAR_UINT8:
			m_entry.m_pVarPtr->UI8 = val;
			break;
		case VAR_INT16:
			m_entry.m_pVarPtr->I16 = val;
			break;
		case VAR_UINT16:
			m_entry.m_pVarPtr->UI16 = val;
			break;
		case VAR_INT32:
			m_entry.m_pVarPtr->I32 = val;
			break;
		case VAR_UINT32:
			m_entry.m_pVarPtr->UI32 = val;
			break;
		case VAR_INT64:
			m_entry.m_pVarPtr->I64 = val;
			break;
		case VAR_UINT64:
			m_entry.m_pVarPtr->UI64 = val;
			break;
		case VAR_FLOAT32:
			m_entry.m_pVarPtr->F32 = val;
			break;
		case VAR_FLOAT64:
			m_entry.m_pVarPtr->F64 = val;
			break;
		}
	}

	inline auto& NewEntry()
	{
		return m_page.m_aEntriesInPage.emplace_back();
	}

	MenuEntry* GetEntryByName(std::string_view name);
	void Process(void);
};
class debugmenu {
public:
	bool m_bIsActive;
	bool m_bCanBeActivated;
	static MenuEntry* m_sMainMenu;

	debugmenu() :
		m_bIsActive(false),
		m_bCanBeActivated(false)
	{
		m_sMainMenu = new MenuEntry();
	}

	static MenuEntry& FindPageByName(std::string_view name);
	void Process();

	inline void OnActivate()
	{
		m_bIsActive ^= 1;
		if (m_bIsActive)
		{
			auto pad = CPad::GetPad(0);
			pad->DisablePlayerControls = 1;
			pad->Clear(false);
		}
		else
		{
			CPad::GetPad(0)->DisablePlayerControls = 0;
		}
	};

};
extern debugmenu TheMenu;