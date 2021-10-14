//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor.
//
//SVFX Editor is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SVFX Editor is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SVFX Editor. If not, see <https://www.gnu.org/licenses/>.

#include "pch.h"
#include "gui_types.h"
#include "Connector.h"
#include "ConnectionHandler.h"
#include "ConnectorActions.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"//for ImVec operators and GImGui

constexpr float g_connectorWidth = 15.0f;

class HandlerFinder final :
	public gui::AscendingVisitor
{
public:
	virtual void visit(gui::ConnectionHandler& c)
	{
		result = &c;
	}

	gui::ConnectionHandler* result{ nullptr };
};

void gui::merge(Connector::StateMap& target, Connector::StateMap&& source)
{
	target.merge(source);
	for (auto& element : source) {
		auto it = target.find(element.first);
		assert(it != target.end());//sanity check
		if (it->second != element.second)
			target.erase(it);
	}
}

gui::Connector::Connector(ISender& sender, IReceiver& receiver) :
	m_sender{ sender }, m_receiver{ receiver }
{}

void gui::Connector::accept(Visitor& v)
{
	v.visit(*this);
}

//from imgui.cpp
static inline bool IsWindowContentHoverable(ImGuiWindow* window, ImGuiHoveredFlags flags)
{
	// An active popup disable hovering on other windows (apart from its own children)
	// FIXME-OPT: This could be cached/stored within the window.
	ImGuiContext& g = *GImGui;
	if (g.NavWindow)
		if (ImGuiWindow* focused_root_window = g.NavWindow->RootWindow)
			if (focused_root_window->WasActive && focused_root_window != window->RootWindow)
			{
				// For the purpose of those flags we differentiate "standard popup" from "modal popup"
				// NB: The order of those two tests is important because Modal windows are also Popups.
				if (focused_root_window->Flags & ImGuiWindowFlags_Modal)
					return false;
				if ((focused_root_window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiHoveredFlags_AllowWhenBlockedByPopup))
					return false;
			}
	return true;
}

void gui::Connector::frame()
{
	using namespace ImGui;

	ImVec2 cursorPos = GetCursorScreenPos();
	float frameH = ImGui::GetFrameHeight();
	Floats<2> hint = { cursorPos.x, cursorPos.y + 0.5f * frameH };
	m_position = m_ctlr ? m_ctlr->place(hint) : hint;
	ImVec2 imPos = gui_type_conversion<ImVec2>::from(m_position);

	ImVec2 size = ImVec2(GetFrameHeight(), GetFrameHeight());
	ImRect bb(imPos - size * 0.5f, imPos + size * 0.5f);

	ImDrawList* dl = GetWindowDrawList();
	assert(dl);
	dl->PushClipRect(bb.Min, bb.Max);
	//dl->AddRectFilled(bb.Min, bb.Max, 0x60ffffff);

	const float radius = 4.0f;
	dl->AddCircleFilled(imPos, radius, 0xffffffff);
	dl->AddCircle(imPos, radius, 0xff000000);

	//highlight (could be a separate decorator)
	if (ConnectionHandler* handler = getHandler(); handler && handler->query(this))
		dl->AddCircle(imPos, radius * 1.5f, { 0xff00d7ff }, 0, 2.0f);

	//Get the imgui item id for this connector (using our address as name)
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();
#ifdef WIN32
	char buf[8];
	std::snprintf(buf, sizeof(buf), "%8lx", reinterpret_cast<uintptr_t>(this));
	const ImGuiID id = window->GetID(buf);
#else
	char buf[16];
	std::snprintf(buf, sizeof(buf), "%16llx", reinterpret_cast<uintptr_t>(this));
	const ImGuiID id = window->GetID(buf);
#endif

	if (!window->SkipItems) {
		assert(size.x > 0.0f && size.y > 0.0f);

		//Boiled-down ItemAdd
		g.LastItemData.ID = id;
		g.LastItemData.Rect = bb;
		g.LastItemData.StatusFlags = ImGuiItemStatusFlags_None;
		g.NextItemData.Flags = ImGuiNextItemDataFlags_None;//don't know why they do this

		if (bb.Contains(GetIO().MousePos))
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredRect;
		//---

		//Boiled-down ButtonBehavior
		bool hovered = false;
		bool otherActive =
			g.ActiveId &&
			g.ActiveId != id &&
			!g.ActiveIdAllowOverlap &&
			g.ActiveId != window->MoveId;//??

		if (!g.HoveredId || g.HoveredId == id || g.HoveredIdAllowOverlap)
			if (!g.HoveredWindow || g.HoveredWindow == window)
				//if (g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdAllowOverlap)
					if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect)
						if (IsWindowContentHoverable(window, ImGuiHoveredFlags_None)) {
							g.HoveredIdDisabled = true;
							hovered = true;
						}

		if (hovered) {
			//Click on us
			if (g.IO.MouseClicked[ImGuiMouseButton_Left] && !g.ActiveId) {
				SetActiveID(id, window);
				g.ActiveIdMouseButton = ImGuiMouseButton_Left;
				//SetFocusID(id, window);//Keyboard focus, do we care?
				FocusWindow(window);

				onClick();
			}

			//Release any item on us (including ourselves)
			if (g.IO.MouseReleased[ImGuiMouseButton_Left]) {
				onRelease();
			}
		}

		//Release us anywhere
		if (g.ActiveId == id) {
			if (g.ActiveIdSource == ImGuiInputSource_Mouse) {
				const int button = g.ActiveIdMouseButton;
				IM_ASSERT(button >= 0 && button < ImGuiMouseButton_COUNT);
				if (g.IO.MouseReleased[button])
					ClearActiveID();
			}
		}

		//---
	}

	/*ImVec2 pos = GetCursorScreenPos();
	ImVec2 size = { g_connectorWidth, ImGui::GetFrameHeight() };
	m_position = { pos.x + size.x / 2.0f, pos.y + size.y / 2.0f };

	Button(("##" + std::to_string(reinterpret_cast<uintptr_t>(this))).c_str(), size);
	if (IsItemHovered() && IsMouseClicked(ImGuiMouseButton_Left))
		onClick();

	//We need the allow when blocked flag here or this won't return true on items before us in 
	//the execution order. The call to InvisibleButton sets/unsets active state, so only 
	//buttons that are executed after us would report as hovered otherwise.
	if (IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && IsMouseReleased(ImGuiMouseButton_Left))
		onRelease();*/

	dl->PopClipRect();
}

gui::Floats<2> gui::Connector::getSizeHint() const
{
	return { g_connectorWidth, ImGui::GetFrameHeight() };
}

void gui::Connector::onRelease()
{
	if (ConnectionHandler* handler = getHandler())
		handler->connect(this);
}

gui::ConnectionHandler* gui::Connector::getHandler() const
{
	HandlerFinder finder;
	if (IComponent* parent = getParent())
		parent->accept(finder);
	return finder.result;
}

gui::SingleConnector::~SingleConnector()
{
	assert(!m_connected);
}

void gui::SingleConnector::onClick()
{
	//If we are connected, disconnect them and let THEM start seeking.
	Connector* active;
	StateMap changes;
	if (m_connected) {
		active = m_connected;
		changes = m_connected->queryStateChange(this, false);
		merge(changes, queryStateChange(m_connected, false));
	}
	else
		active = this;

	asyncInvoke<SetConnectionState>(changes, false);

	if (ConnectionHandler* handler = getHandler())
		handler->startSearch(active, std::move(changes));
}

std::vector<gui::Connector*> gui::SingleConnector::getConnected() const
{
	if (m_connected)
		return std::vector<Connector*>{ m_connected };
	else
		return std::vector<Connector*>();
}

gui::Connector::StateMap gui::SingleConnector::queryStateChange(Connector* c, bool state)
{
	StateMap ret;
	if (c && (c == m_connected) != state) {
		if (m_connected) {
			//disconnect it
			ret.insert({ { this, m_connected }, false });
			if (state)//short for (m_connected != c) - disconnect a third part
				merge(ret, m_connected->queryStateChange(this, false));
		}
		if (state) {
			//connect c
			ret.insert({ { this, c }, true });
		}
	}
	return std::move(ret);
}

void gui::SingleConnector::setConnectionState(Connector* c, bool state)
{
	if (c && (c == m_connected) != state) {
		ConnectionHandler* handler = getHandler();
		if (m_connected) {
			//disconnect it
			if (handler)
				handler->setIncidence(this, m_connected, false);
			m_receiver.removeSender(m_connected->getSender());

			//not necessary anymore
			//if (state)//short for (m_connected != c) - disconnect a third part
			//	m_connected->setConnectionState(this, false);

			m_connected = nullptr;
		}
		if (state) {
			//connect c
			if (handler)
				handler->setIncidence(this, c, true);
			m_receiver.addSender(c->getSender());
			m_connected = c;
		}
	}
}

gui::Connector::StateMap gui::SingleConnector::disconnect()
{
	StateMap result;
	if (m_connected) {
		merge(result, m_connected->queryStateChange(this, false));
		merge(result, queryStateChange(m_connected, false));

		m_connected->setConnectionState(this, false);
		setConnectionState(m_connected, false);
	}
	return result;
}

gui::MultiConnector::~MultiConnector()
{
	assert(m_connected.empty());
}

void gui::MultiConnector::onClick()
{
	if (ConnectionHandler* handler = getHandler())
		handler->startSearch(this, StateMap());
}

std::vector<gui::Connector*> gui::MultiConnector::getConnected() const
{
	std::vector<Connector*> ret(m_connected.size());
	int i = 0;
	for (auto c : m_connected)
		ret[i++] = c;
	return std::move(ret);
}

gui::Connector::StateMap gui::MultiConnector::queryStateChange(Connector* c, bool state)
{
	StateMap ret;
	if (c) {
		if (auto it = m_connected.find(c); (it != m_connected.end()) != state) {
			if (state)
				ret.insert({ { this, c }, true });
			else
				ret.insert({ { this, c }, false });
		}
	}
	return std::move(ret);
}

void gui::MultiConnector::setConnectionState(Connector* c, bool state)
{
	if (c) {
		if (auto it = m_connected.find(c); (it != m_connected.end()) != state) {
			if (ConnectionHandler* handler = getHandler())
				handler->setIncidence(this, c, state);
			if (state) {
				m_connected.insert(c);
				m_receiver.addSender(c->getSender());
			}
			else {
				m_receiver.removeSender(c->getSender());
				m_connected.erase(it);
			}
		}
	}
}

gui::Connector::StateMap gui::MultiConnector::disconnect()
{
	StateMap result;
	for (auto&& c : m_connected) {
		assert(c);
		merge(result, c->queryStateChange(this, false));
		merge(result, queryStateChange(c, false));
	}
	for (auto&& item : result)
		item.first.first->setConnectionState(item.first.second, item.second);

	return result;
}

void gui::Disconnector::visit(Connector& c)
{
	for (auto&& connected : c.getConnected()) {
		assert(connected);
		merge(m_stateChanges, connected->queryStateChange(&c, false));
		merge(m_stateChanges, c.queryStateChange(connected, false));
	}
}

void gui::Disconnector::execute()
{
	for (auto&& item : m_stateChanges) {
		assert(item.first.first);
		item.first.first->setConnectionState(item.first.second, item.second);
	}
}
