#include "debugmenu.h"
#include "gui/imgui/imgui.h"
#include <functional>
#include <algorithm>

debugmenu TheMenu;
MenuEntry* debugmenu::m_sMainMenu;

MenuEntry* MenuEntry::GetEntryByName(std::string_view name)
{
	for (auto& entry : m_page.m_aEntriesInPage)
	{
		auto nameWithoutPath = entry.m_szName.substr(0, entry.m_szName.find_first_of('#'));
		if (nameWithoutPath == name)
			return &entry;
	}
	return nullptr;
}

MenuEntry& debugmenu::FindPageByName(std::string_view name)
{
	std::stringstream stream(name.data());
	std::string currentPageName;
	std::vector<std::string> pageNames;

	while (std::getline(stream, currentPageName, '|'))
	{
		pageNames.push_back(currentPageName);
	}
	MenuEntry* curMenuEntry = m_sMainMenu;
	std::sort(m_sMainMenu->m_page.m_aEntriesInPage.begin(), m_sMainMenu->m_page.m_aEntriesInPage.end(), [](const MenuEntry& a, const MenuEntry& b)
		{
			return a.m_szName < b.m_szName;
		});
	std::string curPath;

	for (size_t i = 0; i < pageNames.size(); ++i)
	{
		if (i == 0)
			curPath = pageNames[i];
		else
		{
			curPath += "|";
			curPath += pageNames[i];
		}
		auto anotherPage = curMenuEntry->GetEntryByName(pageNames[i]);
		if (!anotherPage)
		{
			auto newEntry = &curMenuEntry->NewEntry();
			*newEntry = MenuEntry(curPath, pageNames[i], curMenuEntry);
			anotherPage = newEntry;
		}
		curMenuEntry = anotherPage;
	}
	return *curMenuEntry;
}

template<typename T>
inline void Clamp(T& var, const T& min, const T& max, bool wrap)
{
	if (wrap)
	{
		if (var < min)
			var = max;
		else if (var > max)
			var = min;
	}
	else
	{
		if (var < min)
			var = min;
		else if (var > max)
			var = max;
	}

}

bool InputScalarWithMinMax(const char* label, ImGuiDataType dataType, VarTypes* data, const VarTypes& step,
	const VarTypes& min, const VarTypes& max, bool wrap)
{
	auto prevValue = *data; // we need to handle unsigned cases, these wont go below their min and max values with the imgui func
							// so we make a copy of the previous value and check because it wont wrap otherwise (iv tier workaround)

	auto value = *data; // copy this too so we only process after enter key or such is pressed (nvm)

	bool changed = ImGui::InputScalar(label, dataType, &value, &step, NULL);
	if (changed)
	{
		switch (dataType)
		{
		case VAR_INT8:
			Clamp(value.I8, min.I8, max.I8, wrap);
			value.I8 = round((double)value.I8 / (double)step.I8) * (double)step.I8;
			break;

		case VAR_UINT8:
			Clamp(value.UI8, min.UI8, max.UI8, wrap);
			if (wrap)
			{
				if (prevValue.UI8 == 0 && value.UI8 == 0)
					value.UI8 = max.UI8;
				else if (prevValue.UI8 == UINT8_MAX && value.UI8 == UINT8_MAX)
					value.UI8 = min.UI8;
			}
			value.UI8 = round((double)value.UI8 / (double)step.UI8) * (double)step.UI8;
			break;

		case VAR_INT16:
			Clamp(value.I16, min.I16, max.I16, wrap);
			value.I16 = round((double)value.I16 / (double)step.I16) * (double)step.I16;
			break;

		case VAR_UINT16:
			Clamp(value.UI16, min.UI16, max.UI16, wrap);
			if (wrap)
			{
				if (prevValue.UI16 == 0 && value.UI16 == 0)
					value.UI16 = max.UI16;
				else if (prevValue.UI16 == UINT16_MAX && value.UI16 == UINT16_MAX)
					value.UI16 = min.UI16;
			}
			value.UI16 = round((double)value.UI16 / (double)step.UI16) * (double)step.UI16;
			break;

		case VAR_INT32:
			Clamp(value.I32, min.I32, max.I32, wrap);
			value.I32 = round((double)value.I32 / (double)step.I32) * (double)step.I32;
			break;

		case VAR_UINT32:
			Clamp(value.UI32, min.UI32, max.UI32, wrap);
			if (wrap)
			{
				if (prevValue.UI32 == 0 && value.UI32 == 0)
					value.UI32 = max.UI32;
				else if (prevValue.UI32 == UINT32_MAX && value.UI32 == UINT32_MAX)
					value.UI32 = min.UI32;
			}
			value.UI32 = round((double)value.UI32 / (double)step.UI32) * (double)step.UI32;
			break;

		case VAR_INT64:
			Clamp(value.I64, min.I64, max.I64, wrap);
			value.I64 = round((double)value.I64 / (double)step.I64) * (double)step.I64;
			break;

		case VAR_UINT64:
			Clamp(value.UI64, min.UI64, max.UI64, wrap);
			if (wrap)
			{
				if (prevValue.UI64 == 0 && value.UI64 == 0)
					value.UI64 = max.UI64;
				else if (prevValue.UI64 == UINT64_MAX && value.UI64 == UINT64_MAX)
					value.UI64 = min.UI64;
			}
			value.UI64 = round((double)value.UI64 / (double)step.UI64) * (double)step.UI64;
			break;

		case VAR_FLOAT32:
			Clamp(value.F32, min.F32, max.F32, wrap);
			value.F32 = round((double)value.F32 / (double)step.F32) * (double)step.F32;
			break;

		case VAR_FLOAT64:
			Clamp(value.F64, min.F64, max.F64, wrap);
			value.F64 = round((double)value.F64 / (double)step.F64) * (double)step.F64;
			break;
		}
		*data = value;
	}

	return changed;
}

void MenuEntry::Process()
{
	switch (m_eType)
	{
	case ENTRY_MENU:
		if (m_page.m_previousPage == debugmenu::m_sMainMenu)
		{
			if (ImGui::CollapsingHeader(m_szName.c_str()))
			{
				ImGui::Indent(10.0f);
				for (auto& entry : m_page.m_aEntriesInPage)
				{
					entry.Process();
				}
				ImGui::Unindent(10.0f);
			}
		}
		else
		{
			if (ImGui::BeginMenu(m_szName.c_str()))
			{
				for (auto& entry : m_page.m_aEntriesInPage)
				{
					entry.Process();
				}
				ImGui::EndMenu();
			}
		}
		break;
	case ENTRY_CMD:
		if (ImGui::Button(m_szName.c_str()))
			if (m_entry.m_func)
				m_entry.m_func();
		break;
	case ENTRY_VAR:
		switch (m_entry.m_eVarType)
		{
		case VAR_FLOAT32:
			if (m_entry.m_step.F32 == 0.0f)
			{
				ImGui::Text("%s: %.3f", m_szName.c_str(), m_entry.m_pVarPtr->F32);
				break;
			}
		case VAR_FLOAT64:
			if (m_entry.m_step.F64 == 0.0f)
			{
				ImGui::Text("%s: %.3lf", m_szName.c_str(), m_entry.m_pVarPtr->F64);
				break;
			}
			if(InputScalarWithMinMax(m_szName.c_str(), m_entry.m_eVarType, m_entry.m_pVarPtr, 
				m_entry.m_step, m_entry.m_lowerBound, m_entry.m_upperBound, m_entry.m_bWrap))
				if (m_entry.m_func)
					m_entry.m_func();
			break;
		default:
			if (GetValueAsInt(m_entry.m_step) == 0)
			{
				ImGui::Text("%s: %lld", GetValueAsInt(*m_entry.m_pVarPtr));
				break;
			}

			if (InputScalarWithMinMax(m_szName.c_str(), m_entry.m_eVarType, m_entry.m_pVarPtr, 
				m_entry.m_step, m_entry.m_lowerBound, m_entry.m_upperBound, m_entry.m_bWrap))
			{
				if (m_entry.m_func)
					m_entry.m_func();
			}
			break;
		}
		break;
	case ENTRY_TOGGLE:
		UpdateIsToggled();
		if (ImGui::Checkbox(m_szName.c_str(), &m_entry.m_bIsToggled))
		{
			ChangeVar(m_entry.m_bIsToggled);
			if (m_entry.m_func)
				m_entry.m_func();
		}
		break;
	case ENTRY_COMBO:
		UpdateSelectedItem();
		if (ImGui::Combo(m_szName.c_str(), &m_entry.m_nSelectedItem, (const char**)m_entry.m_aStrings, 
			GetValueAsInt(m_entry.m_upperBound) - m_entry.m_nLowerBoundOffset + 1))
		{
			ChangeVar(m_entry.m_nSelectedItem + m_entry.m_nLowerBoundOffset);
			if (m_entry.m_func)
				m_entry.m_func();
		}
		break;
	}
	return;
}

void debugmenu::Process()
{
	if (!m_bIsActive)
		return;

	ImGui::GetIO().MouseDrawCursor = true;
	ImGui::Begin("Debugmenu", &m_bIsActive, ImGuiWindowFlags_None);

	for (auto& entry : m_sMainMenu->m_page.m_aEntriesInPage)
		entry.Process();

	ImGui::End();
}


#define EXPORT extern "C" __declspec(dllexport)

#define INTTYPES \
	X(Int8, char) \
	X(Int16, short) \
	X(Int32, int) \
	X(Int64, long long) \
	X(UInt8, unsigned char) \
	X(UInt16, unsigned short) \
	X(UInt32, unsigned int) \
	X(UInt64, unsigned long long)
#define FLOATTYPES \
	X(Float32, float) \
	X(Float64, double)

extern "C" {

#define X(NAME, TYPE) \
EXPORT std::pair<std::string, std::string>* \
	DebugMenuAdd##NAME(const char* path, const char* name, TYPE* ptr, void(*triggerFunc)(), TYPE step, TYPE lowerBound, TYPE upperBound, const char** strings) \
{ \
	auto& newEntry = debugmenu::FindPageByName(path).NewEntry(); \
	newEntry = MenuEntry(name, ptr, lowerBound, upperBound, step, triggerFunc, strings, path);			\
	auto id = new std::pair<std::string, std::string>(path, name); \
	return id; \
}
	INTTYPES
#undef X

#define X(NAME, TYPE) \
EXPORT std::pair<std::string, std::string>* \
	DebugMenuAdd##NAME(const char* path, const char* name, TYPE* ptr, void(*triggerFunc)(), TYPE step, TYPE lowerBound, TYPE upperBound) \
{ \
	auto& newEntry = debugmenu::FindPageByName(path).NewEntry(); \
	newEntry = MenuEntry(name, ptr, lowerBound, upperBound, step, triggerFunc, nullptr, path);			\
	auto id = new std::pair<std::string, std::string>(path, name); \
	return id; \
}
	FLOATTYPES
#undef X
		
	EXPORT std::pair<std::string, std::string>* DebugMenuAddCmd(const char* path, const char* name, void(*triggerFunc)())
	{
		auto& entry = debugmenu::FindPageByName(path).NewEntry();
		entry = MenuEntry(name, triggerFunc, path);
		auto id = new std::pair<std::string, std::string>(path, name);
		return id;
	}

	EXPORT void DebugMenuEntrySetWrap(std::pair<std::string, std::string>* id, bool wrap)
	{
		if (id)
		{
			auto entry = debugmenu::FindPageByName(id->first).GetEntryByName(id->second);
			if (entry->m_eType == ENTRY_VAR)
				entry->m_entry.m_bWrap = wrap;
		}
	}

	EXPORT void DebugMenuEntrySetStrings(std::pair<std::string, std::string>* id, const char** strings)
	{
		if (id)
		{
			auto entry = debugmenu::FindPageByName(id->first).GetEntryByName(id->second);
			if (entry->m_eType == ENTRY_VAR)
				entry->m_entry.m_aStrings = (char**)strings;
		}
	}

	EXPORT void DebugMenuEntrySetAddress(std::pair<std::string, std::string>* id, VarTypes* addr)
	{
		if (id)
		{
			auto entry = debugmenu::FindPageByName(id->first).GetEntryByName(id->second);
			if (entry->m_eType == ENTRY_VAR)
				entry->m_entry.m_pVarPtr = addr;
		}
	}

}