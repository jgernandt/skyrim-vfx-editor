//Copyright 2021 Jonas Gernandt
//
//This file is part of SVFX Editor, a program for creating visual effects
//in the NetImmerse format.
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
#include "Editor.h"
#include "Constructor.h"
#include "CallWrapper.h"

#include "widget_types.h"
#include "nodes.h"
#include "CompositionActions.h"

#include <fstream>
#include <sstream>

constexpr const char* DOC_FILE_NAME = "block types.txt";

class HelpWindow final : public gui::Window
{
public:
	HelpWindow();
};

template<typename T>
void node::Editor::WorkArea::addNode()
{
	try {
		asyncInvoke<gui::AddChild>(std::make_unique<T>(), this, true);
	}
	catch (const std::exception&) {
		//Could be bad_alloc or anything thrown by the constructor.
		//We should display an error here.
		asyncInvoke<gui::AddChild>(std::make_unique<gui::MessageBox>("Error", "Failed to add object"), this, false);
	}
}

node::Editor::Editor() : m_niVersion{ nif::File::Version::UNKNOWN }
{
}

node::Editor::Editor(const nif::File& file) : m_niVersion{ file.getVersion() }
{
	std::unique_ptr<Constructor> c;//assign an object of the right version (there is only one so far)

	if (file.getVersion() == nif::File::Version::SKYRIM || file.getVersion() == nif::File::Version::SKYRIM_SE)
		c = std::make_unique<Constructor>();

	if (c) {
		try {
			auto workArea = newChild<WorkArea>();

			//Nodes
			c->makeRoot(file.getRoot());
			c->extractNodes(*workArea);

			for (auto&& warning : c->warnings())
				newChild<gui::MessageBox>("Warning", std::move(warning));

			//Context menu
			auto panel = newChild<gui::Panel>();
			auto context = std::make_unique<gui::Popup>();
			context->addChild(workArea->createAddMenu());
			panel->setContextMenu(std::move(context));

			//Main menu additions
			//Strange to add to main menu from here, no? Strictly speaking, we don't even know that there is a main menu.
			auto main = std::make_unique<gui::MainMenu>("Add");
			main->addChild(workArea->createAddMenu());
			addChild(std::move(main));
		}
		catch (const std::exception& e) {
			clearChildren();
			auto workArea = newChild<WorkArea>();
			newChild<gui::MessageBox>("Error", e.what());

			//Now we have the appearance we should have, but no way to add nodes. It wouldn't make sense to. 
			//better to force the user to open another file.
			//Still, I can't shake the feeling that this whole setup is a bit stupid.
		}
	}
	else {
		newChild<gui::MessageBox>("Error", "File version not supported");
	}

	//Main menu additions
	auto help = std::make_unique<gui::MainMenu>("Help");
	help->newChild<gui::MenuItem>("Block info",
		[this]() { asyncInvoke<gui::AddChild>(std::make_unique<HelpWindow>(), this, false); });
	addChild(std::move(help));
}

node::Editor::~Editor()
{
}

void node::Editor::frame(gui::FrameDrawer& fd)
{
	//pan
	/*if (fd.isMouseDown(gui::MouseButton::MIDDLE)) {
		m_workAreaT += fd.getMouseMove();
	}

	gui::Drawer drawer;//do we just use the FrameDrawer instead?
	drawer.setTargetLayer(gui::Layer::BACKGROUND);
	drawer.begin();
	//Rectangle should always go from (0, 0) to size (actually from fd.toGlobal(m_translation) to fd.toGlobal(m_translation + m_scale * m_size))
	gui::Floats<2> rectTL = fd.toGlobal(m_translation);
	gui::Floats<2> rectBR = fd.toGlobal(m_translation + m_scale * m_size);
	drawer.rectangle(rectTL, rectBR, { 0.2f, 0.2f, 0.2f, 1.0f });
	//But if we add panning, the grid lines might have a different origin (and scale, if we add zoom):
	//Work-space coords:
	gui::Floats<2> tl = m_workAreaT;
	gui::Floats<2> br = tl + m_size / m_workAreaS;
	//gui::Floats<2> br{ tl[0] + m_size[0] / m_workAreaS[0], tl[1] + m_size[1] / m_workAreaS[1] };
	float step = 64.0f;
	for (float x = std::fmodf(tl[0], step); x < br[0]; x += step)
		drawer.line({ rectTL[0] + x, rectTL[1] }, { rectTL[0] + x, rectBR[1] }, { 0.3f, 0.3f, 0.3f, 1.0f });
	for (float y = std::fmodf(tl[1], step); y < br[1]; y += step)
		drawer.line({ rectTL[0], rectTL[1] + y }, { rectBR[0], rectTL[1] + y }, { 0.3f, 0.3f, 0.3f, 1.0f });
	drawer.end();*/

	Composite::frame(fd);
}

void node::Editor::setProjectName(const std::string& name)
{
	if (Root* root = findRootNode())
		root->object().name().set(name);
}

std::unique_ptr<gui::IComponent> node::Editor::WorkArea::createAddMenu()
{
	auto root = std::make_unique<gui::Composite>();
	auto group = root->newChild<gui::Menu>("Grouping");
	group->newChild<gui::MenuItem>("Node", std::bind(&Editor::WorkArea::addNode<Node>, this));

	auto part = root->newChild<gui::Menu>("Particles");
	part->newChild<gui::MenuItem>("Particle system", std::bind(&Editor::WorkArea::addNode<ParticleSystem>, this));
	part->newChild<gui::Separator>();
	part->newChild<gui::MenuItem>("Emitter (box)", std::bind(&Editor::WorkArea::addNode<BoxEmitter>, this));
	part->newChild<gui::MenuItem>("Emitter (cylinder)", std::bind(&Editor::WorkArea::addNode<CylinderEmitter>, this));
	part->newChild<gui::MenuItem>("Emitter (sphere)", std::bind(&Editor::WorkArea::addNode<SphereEmitter>, this));
	part->newChild<gui::Separator>();
	part->newChild<gui::MenuItem>("Force field (planar)", std::bind(&Editor::WorkArea::addNode<PlanarForceField>, this));
	part->newChild<gui::MenuItem>("Force field (spherical)", std::bind(&Editor::WorkArea::addNode<SphericalForceField>, this));
	part->newChild<gui::Separator>();
	part->newChild<gui::MenuItem>("Colour modifier (simple)", std::bind(&Editor::WorkArea::addNode<SimpleColourModifier>, this));
	part->newChild<gui::MenuItem>("Rotation modifier", std::bind(&Editor::WorkArea::addNode<RotationModifier>, this));
	part->newChild<gui::MenuItem>("Scale modifier", std::bind(&Editor::WorkArea::addNode<ScaleModifier>, this));

	auto shad = root->newChild<gui::Menu>("Shaders");
	shad->newChild<gui::MenuItem>("Effect shader", std::bind(&Editor::WorkArea::addNode<EffectShader>, this));

	auto extra = root->newChild<gui::Menu>("Extra data");
	extra->newChild<gui::MenuItem>("String data", std::bind(&Editor::WorkArea::addNode<StringData>, this));
	extra->newChild<gui::MenuItem>("Weapon type", std::bind(&Editor::WorkArea::addNode<WeaponTypeData>, this));

	return root;
}

node::Root* node::Editor::findRootNode() const
{
	for (auto&& child : getChildren()) {
		if (Root* root = dynamic_cast<Root*>(child.get()))
			return root;
	}
	return nullptr;
}

HelpWindow::HelpWindow()
{
	setClosable();
	setTitle("Block types");
	setSize({ 640.0f, 400.0f });

	std::ifstream file(DOC_FILE_NAME);
	if (file.is_open()) {
		std::stringstream s;
		s << file.rdbuf();

		auto text = newChild<gui::Text>(s.str());
		text->setWrap();
	}
	else
		newChild<gui::Text>(std::string("Failed to load ") + DOC_FILE_NAME);
}

node::Editor::WorkArea::WorkArea()
{
}

void node::Editor::WorkArea::frame(gui::FrameDrawer& fd)
{
	//pan
	if (fd.isMouseDown(gui::MouseButton::MIDDLE))
		m_translation += fd.getMouseMove();

	//They should set our size, but let's go with this for now
	assert(getParent());
	m_size = getParent()->getSize();
	//Hmm, this means we are using size inconsistently.
	//Normally components occupy the area from m_translation to m_translation + m_scale * m_size, in parent space
	// ((0, 0) to m_size in local space).
	//We occupy the area from (0, 0) to parent_size in parent space.
	//Again, this setup feels a little stupid.

	gui::Drawer drawer;//rework to use the FrameDrawer instead
	drawer.setTargetLayer(gui::Layer::BACKGROUND);
	drawer.begin();
	//Background always covers (0,0) to size
	drawer.rectangle(fd.toGlobal({ 0.0f, 0.0f }), fd.toGlobal(m_size), { 0.2f, 0.2f, 0.2f, 1.0f });
	//But if we add panning, the grid lines might have a different origin (and scale, if we add zoom):
	float step = 64.0f;
	float x = std::fmodf(m_translation[0], step);
	if (x < 0.0f)
		x += step;
	for (; x < m_size[0]; x += step)
		drawer.line(fd.toGlobal({ x, 0.0f }), fd.toGlobal({ x, m_size[1] }), { 0.3f, 0.3f, 0.3f, 1.0f });
	float y = std::fmodf(m_translation[1], step);
	if (y < 0.0f)
		y += step;
	for (; y < m_size[1]; y += step)
		drawer.line(fd.toGlobal({ 0.0f, y }), fd.toGlobal({ m_size[0], y }), { 0.3f, 0.3f, 0.3f, 1.0f });
	drawer.end();

	ConnectionHandler::frame(fd);
}
