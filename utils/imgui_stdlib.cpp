// dear imgui: wrappers for C++ standard library (STL) types (std::string, etc.)
// This is also an example of how you may wrap your own similar types.

// Changelog:
// - v0.10: Initial version. Added InputText() / InputTextMultiline() calls with std::string

// See more C++ related extension (fmt, RAII, syntaxis sugar) on Wiki:
//   https://github.com/ocornut/imgui/wiki/Useful-Extensions#cness

#ifndef IMGUI_DISABLE
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#endif

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextWithHint(label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}


static bool sortbysec_desc(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return (b.second < a.second);
}

static int index_of_key(const std::vector<std::pair<int, int>>& pair_list, int key)
{
	for (int i = 0; i < (int)pair_list.size(); ++i)
	{
		if (pair_list[i].first == key)
			return i;
	}
	return -1;
}

// Copied from imgui_widgets.cpp (internal)
static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

static const char* ICON_FA_SEARCH = (const char*)u8"*";        // U+E935

// ComboWithFilter: Combo box with fuzzy filtering support.
// items: vector of strings to choose from
// current_item: pointer to selected index
// popup_max_height_in_items: max height of popup in items (default 5)
bool ImGui::ComboWithFilter(const char* label, int* current_item, const std::vector<std::string>& items, int popup_max_height_in_items)
{
	using namespace fts;
	const bool    allow_repeat = ImGuiInputFlags_Repeat;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	int               items_count = (int)items.size();

	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		preview_value = items[*current_item].c_str();

	static int  focus_idx = -1;
	static char pattern_buffer[256] = { 0 };

	bool value_changed = false;

	const ImGuiID id = window->GetID(label);
	const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
	const bool    is_already_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
	const bool    is_filtering = is_already_open && pattern_buffer[0] != '\0';

	int                              show_count = items_count;
	std::vector<std::pair<int, int>> itemScoreVector;

	if (is_filtering)
	{
		for (int i = 0; i < items_count; i++)
		{
			int  score = 0;
			bool matched = fuzzy_match(pattern_buffer, items[i].c_str(), score);
			if (matched)
				itemScoreVector.push_back(std::make_pair(i, score));
		}
		std::sort(itemScoreVector.begin(), itemScoreVector.end(), sortbysec_desc);
		int current_score_idx = index_of_key(itemScoreVector, focus_idx);
		if (current_score_idx < 0 && !itemScoreVector.empty())
			focus_idx = itemScoreVector[0].first;

		show_count = (int)itemScoreVector.size();
	}

	if (popup_max_height_in_items == -1)
		popup_max_height_in_items = 5;
	popup_max_height_in_items = ImMin(popup_max_height_in_items, show_count);

	// TODO: migrate to a newer one and fix this.

#	if IMGUI_VERSION_NUM <= 19045
	// HACK: for imgui pre 1.91 
	if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
#else
	if (!(g.NextWindowData.HasFlags & ImGuiNextWindowDataFlags_HasSizeConstraint))
#endif
	{
		int items_for_height = popup_max_height_in_items + 3;        // extra for search bar
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(items_for_height)));
	}

	if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
		return false;

	if (!is_already_open)
	{
		focus_idx = *current_item;
		memset(pattern_buffer, 0, sizeof(pattern_buffer));
	}

	PushStyleColor(ImGuiCol_FrameBg, ImVec4(240 / 255.f, 240 / 255.f, 240 / 255.f, 1.0f));
	PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
	PushItemWidth(-FLT_MIN);

	if (!is_already_open)
		SetKeyboardFocusHere();

	InputText("##ComboWithFilter_inputText", pattern_buffer, sizeof(pattern_buffer), ImGuiInputTextFlags_AutoSelectAll);

	const ImVec2 label_size = CalcTextSize(ICON_FA_SEARCH, NULL, true);
	const ImVec2 search_icon_pos(
		ImGui::GetItemRectMax().x - label_size.x - style.ItemInnerSpacing.x * 2,
		window->DC.CursorPos.y + style.FramePadding.y + g.FontSize * 0.3f);
	RenderText(search_icon_pos, ICON_FA_SEARCH);

	PopStyleColor(2);

	int move_delta = 0;

#	if IMGUI_VERSION_NUM <= 19045
	if (Shortcut(ImGuiKey_UpArrow, 0, allow_repeat))
		--move_delta;
	else if (Shortcut(ImGuiKey_DownArrow, 0, allow_repeat))
		++move_delta;
#	else
	if (Shortcut(ImGuiKey_UpArrow, allow_repeat))
		--move_delta;
	else if (Shortcut(ImGuiKey_DownArrow, allow_repeat))
		++move_delta;
#	endif
	else if (Shortcut(ImGuiKey_PageUp, ImGuiInputFlags_None))
		move_delta -= popup_max_height_in_items;
	else if (Shortcut(ImGuiKey_PageDown, ImGuiInputFlags_None))
		move_delta += popup_max_height_in_items;

	if (move_delta != 0)
	{
		if (is_filtering)
		{
			int current_score_idx = index_of_key(itemScoreVector, focus_idx);
			if (current_score_idx >= 0)
			{
				const int count = (int)itemScoreVector.size();
				current_score_idx = ImClamp(current_score_idx + move_delta, 0, count - 1);
				focus_idx = itemScoreVector[current_score_idx].first;
			}
		}
		else
		{
			focus_idx = ImClamp(focus_idx + move_delta, 0, items_count - 1);
		}
	}

	float  height_in_items_f = (popup_max_height_in_items < 0 ? ImMin(items_count, 7) : popup_max_height_in_items) + 0.25f;
	ImVec2 size(0.0f, GetTextLineHeightWithSpacing() * height_in_items_f + g.Style.FramePadding.y * 2.0f);

	if (BeginListBox("##ComboWithFilter_itemList", size))
	{
		for (int i = 0; i < show_count; i++)
		{
			int idx = is_filtering ? itemScoreVector[i].first : i;
			PushID((void*)(intptr_t)idx);
			const bool  item_selected = (idx == focus_idx);
			const char* item_text = items[idx].c_str();

			if (Selectable(item_text, item_selected))
			{
				value_changed = true;
				*current_item = idx;
				CloseCurrentPopup();
			}

			if (item_selected)
			{
				SetItemDefaultFocus();
				if (move_delta != 0 || IsWindowAppearing())
					SetScrollHereY();
			}
			PopID();
		}
		EndListBox();

		const bool repeat = false;
		if (IsKeyPressed(ImGuiKey_Enter, repeat))
		{
			value_changed = true;
			*current_item = focus_idx;
			CloseCurrentPopup();
		}
		else if (IsKeyPressed(ImGuiKey_Escape, repeat))
		{
			value_changed = false;
			CloseCurrentPopup();
		}
	}

	PopItemWidth();
	EndCombo();

	if (value_changed)
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}


#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // #ifndef IMGUI_DISABLE
