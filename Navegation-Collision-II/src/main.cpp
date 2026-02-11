#include <GLFW/glfw3.h>
#include <chrono>

#include "Math/Vector.hpp"
#include "engine.hpp"
#include "window.hpp"

#include "Actions/Invoker.hpp"

#include "Cells/Factory.hpp"

#include "Grid/Manager.hpp"

#include "Actions/AddCell.hpp"
#include "Actions/RemoveCell.hpp"

#include "Chain/GridInit/Chain.hpp"
#include "Chain/GridInit/Config.hpp"
#include "Chain/GridInit/Factory.hpp"
#include "Chain/GridInit/Manager.hpp"

#include "Observer/GridChanged.hpp"

#include "Animation/Manager.hpp"

#include "Path/Manager.hpp"

#include "Simulation/Manager.hpp"

#include "Simulation/Collision/Factory/Direct.hpp"
#include "Simulation/Collision/Factory/Grid.hpp"
#include "Simulation/Collision/Factory/Rvo.hpp"

#include "imgui.h"
#include "implot.h"

#include "Tracy.hpp"

struct MyWindow : public Engine::Window {
  MyWindow(int argc, const char **argv)
      : simManager(Simulation::Manager::get()) {

    ImGui::SetCurrentContext(getImGuiContext());
    ImPlot::SetCurrentContext(getImPlotContext());
    glfwSwapInterval(1);

    initLogger();
    setupStartup();
    animationSetup();

    simManager.init(std::make_unique<Simulation::Collision::RvoFactory>());
    m_param.factoryIndex = 0;
  }

  void update(double dt) override {
    ZoneScoped;
    Invoker::get().execute();

    if (restore) {
      Invoker::get().restore(restore);
      restore = 0;
    }

    if (anim.getState() == decltype(anim)::PlayerState::PLAYING) {
      refresh = true;
    }

    if (refresh) {
      refresh = false;
      clearEngine();

      if (GridManager::get().allocated()) {
        Vec2 start = GridManager::get().start();
        Vec2 end = GridManager::get().end();

        m_engine->createPoint(end, {1, 1, 0}, 8);
        m_engine->createPoint(start, {0, 1, 1}, 8);

        anim.loop(dt);
      }
    }
  }

  void uiUpdate() {
    ZoneScoped;

    ImGui::Begin("Simulation Controls");

    {
      const char *items[] = {"Square", "Hexa"};

      if (ImGui::Combo("Choose Grid Type", &m_param.factoryIndex, items,
                       IM_ARRAYSIZE(items))) {
        if (GridManager::get().allocated()) {
          GridManager::get().setGridFactory(
              Chain::GridFactory::FACTORIES[m_param.factoryIndex]());
        }
      }
    }

    ImGui::Separator();
    ImGui::Spacing();

    {

      static int stageRes[2] = {10, 10};
      Vec2u currRes = GridManager::get().resolution();

      ImGui::Text("Current Size: %llu x %llu", currRes[0], currRes[1]);

      ImGui::InputInt2("Grid Resolution:", stageRes);

      for (size_t i = 0; i < 2; i++)
        if (stageRes[i] < 1)
          stageRes[i] = 1;

      bool isChanged =
          (stageRes[0] != (int)currRes[0] || stageRes[1] != (int)currRes[1]);

      if (!isChanged) {
        ImGui::BeginDisabled();
      }

      if (ImGui::Button("Apply Resize", ImVec2(-1, 0))) {
        if (GridManager::get().allocated()) {
          GridManager::get().resize(stageRes[1], stageRes[0]);
        }
      }
      m_param.gridConfig = Vec2u{(uint32_t)stageRes[0], (uint32_t)stageRes[1]};

      if (!isChanged) {
        ImGui::EndDisabled();
      }

      static bool regularGrid = std::get<bool>(sm.get("regularGrid").value());

      ImGui::Checkbox("regularGrid", &regularGrid);
      sm.set("regularGrid", regularGrid);
    }

    ImGui::Separator();
    ImGui::Spacing();

    {

      const char *items[] = {"RVO (No Comm)", "Grid (Indirect)",
                             "Direct (Geometric)"};

      static int currentItem = 0;

      if (ImGui::Combo("Choose Strategy", &currentItem, items,
                       IM_ARRAYSIZE(items))) {

        if (currentItem == 0) {
          Simulation::Manager::get().init(
              std::make_unique<Simulation::Collision::RvoFactory>());
        } else if (currentItem == 1) {
          Simulation::Manager::get().init(
              std::make_unique<Simulation::Collision::GridFactory>());
        } else if (currentItem == 2) {
          Simulation::Manager::get().init(
              std::make_unique<Simulation::Collision::DirectFactory>());
        }
      }

      ImGui::Text("Current Mode: %s", items[currentItem]);
      ImGui::Separator();

      static double fps = simManager.getFPS();

      ImGui::PushItemWidth(120.0f);
      ImGui::InputDouble("Simulation FPS (0=unlimited)", &fps);
      ImGui::PopItemWidth();

      bool isChanged = std::abs(simManager.getFPS() - fps) > 1e-6;

      if (!isChanged) {
        ImGui::BeginDisabled();
      }

      if (ImGui::Button("Apply FPS", ImVec2(-1, 0))) {
        simManager.setFPS(fps);
      }

      if (!isChanged) {
        ImGui::EndDisabled();
      }
    }
    ImGui::Spacing();
    ImGui::Separator();

    {
      ImGui::Text("Agents Config:");
      static double radius = std::get<double>(sm.get("agentsRadius").value());
      static double speed = std::get<double>(sm.get("agentsSpeed").value());
      static bool midPoints = std::get<bool>(sm.get("midPointPath").value());

      ImGui::Checkbox("midPoints", &midPoints);
      ImGui::PushItemWidth(120.0f);
      ImGui::InputDouble("radius", &radius);
      if (radius <= 10) {
        radius = 10;
      }

      ImGui::SameLine();

      ImGui::InputDouble("speed", &speed);
      if (speed <= 10) {
        speed = 10;
      }
      ImGui::PopItemWidth();

      sm.set("agentsRadius", radius);
      sm.set("agentsSpeed", speed);
      sm.set("midPointPath", midPoints);
    }

    ImGui::Spacing();
    ImGui::Separator();

    {
      static float data[1000] = {0};
      static size_t size = IM_ARRAYSIZE(data);
      static int offset = 0;

      data[offset] = (float)std::get<double>(sm.get("fps").value());
      offset = (offset + 1) % size;

      if (ImPlot::BeginPlot("FPS History", ImVec2(-1, 0))) {

        ImPlot::SetupAxes("Time", "FPS", ImPlotAxisFlags_AutoFit,
                          ImPlotAxisFlags_AutoFit);

        ImPlot::PlotLine("FPS", data, size, 1.0, 0.0, 0, offset);

        ImPlot::EndPlot();
      }
    }
    {
      float availWidth = ImGui::GetContentRegionAvail().x;
      float spacing = ImGui::GetStyle().ItemSpacing.x;
      float plotWidth = (availWidth - spacing) / 2.0f;
      ImVec2 plotSize(plotWidth, 0);

      {
        static float simData[500] = {0};
        static size_t size = IM_ARRAYSIZE(simData);
        static int simOffset = 0;

        if (sm.get("fpsSim").has_value()) {
          simData[simOffset] =
              (float)std::get<double>(sm.get("fpsSim").value());
          simOffset = (simOffset + 1) % size;
        }

        if (ImPlot::BeginPlot("FPS Simulation", plotSize)) {
          ImPlot::SetupAxes("Time", "FPS", ImPlotAxisFlags_AutoFit,
                            ImPlotAxisFlags_AutoFit);
          ImPlot::PlotLine("Sim", simData, size, 1.0, 0.0, 0, simOffset);
          ImPlot::EndPlot();
        }
      }

      ImGui::SameLine();

      {
        static float iterData[500] = {0};
        static size_t size = IM_ARRAYSIZE(iterData);
        static int iterOffset = 0;

        if (sm.get("iterSim").has_value()) {
          iterData[iterOffset] =
              (float)std::get<size_t>(sm.get("iterSim").value());
          iterOffset = (iterOffset + 1) % size;
        }

        if (ImPlot::BeginPlot("Iter", plotSize)) {
          ImPlot::SetupAxes("Iterations", "Frame", ImPlotAxisFlags_AutoFit,
                            ImPlotAxisFlags_AutoFit);
          ImPlot::PlotLine("Iterations", iterData, size, 1.0, 0.0, 0,
                           iterOffset);
          ImPlot::EndPlot();
        }
      }
    }

    ImGui::End();
  }

private:
  Vec2 start;
  Vec2 end;
  int mouseHolding = -1;

  Subscribers::CallbackSubscriber displayRefreshSubscriber;
  Subscribers::CallbackSubscriber pathRefreshSubscriber;
  Subscribers::CallbackSubscriber animationState;
  Simulation::Manager &simManager;

  AnimationManager<std::chrono::high_resolution_clock> anim;
  Chain::GridAllocatorChain initChain;
  FILE *out = stdout;
  bool refresh = false;
  bool dirty = false;
  int restore = 0;
  std::optional<Clock::time_point> m_tickTimeStamp;
  std::vector<Vec2> m_collisionPoints;

  GridManager::AllocationParam m_param;

  void mouseButtonCallback(int button, int action, int mods) override {
    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    if (action == GLFW_PRESS) {
      auto uid = m_engine->lookupObjectUUID(glx, gly);
      int type = m_engine->getType(uid);

      mouseHolding = button;
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!GridManager::get().allocated()) {
          start = {glx, gly};
        } else {
          cursorPosCallback(0, 0);
        }
      }
    } else if (action == GLFW_RELEASE) {
      mouseHolding = -1;

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (!GridManager::get().allocated()) {
          end = {glx, gly};
          m_param.start = start;
          m_param.end = end;

          if (initChain.handle(m_param))
            refresh = true;
        } else {
          cursorPosCallback(0, 0);
        }
      }
    }
  }

  void clear() {
    GridManager::get().deallocate();
    refresh = true;
  }

  bool m_enableTick = false;

  void keyCallback(int key, int scancode, int action, int mode) override {
    static std::optional<Vec2u> m_startGrid = std::nullopt;
    if (!GridManager::get().allocated())
      return;

    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    auto uuid = m_engine->lookupObjectUUID(glx, gly);

    Vec2u gridPos = GridManager::get().getCoord(Vec2{glx, gly});
    Grid::IGrid *grid = GridManager::get().get(uuid);

    if (action == GLFW_PRESS) {
      if (!grid)
        return;

      switch (key) {
      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_ESCAPE:
        terminate();
        break;

      case GLFW_KEY_SPACE:
        m_enableTick = !m_enableTick;
        if (anim.getState() == decltype(anim)::PlayerState::PLAYING)
          anim.pause();
        else
          anim.start();
        break;

      case GLFW_KEY_F1:
        if (!m_startGrid && GridManager::get().allocated()) {
          Vec2u pos = GridManager::get().getCoord(Vec2{glx, gly});
          if (!GridManager::get().cget(pos)->empty()) {
            break;
          }
          m_startGrid = pos;

          Invoker::get().addCommand(new Commands::AddCell(
              CellFactory::get().Create(CellFactory::CellType::ORIGIN, gridPos),
              gridPos));
        }
        break;

      case GLFW_KEY_F2:
        grid->clear();
        refresh = true;
        break;

      case GLFW_KEY_F3: {
        GridManager::get().setup();

        refresh = true;
      }

      break;

      case GLFW_KEY_LEFT_SHIFT:
        break;

      default:
        break;
      }

    } else if (action == GLFW_RELEASE) {
      switch (key) {
      case GLFW_KEY_F1:
        if (m_startGrid) {
          Vec2 end = {glx, gly};
          Vec2u endGrid = GridManager::get().getCoord(end);

          if (endGrid == *m_startGrid ||
              !GridManager::get().cget(endGrid)->empty()) {
            Invoker::get().addCommand(new Commands::RemoveCell(*m_startGrid));
            std::cout << "short\n";
          } else {
            Invoker::get().addCommand(new Commands::AddCell(
                CellFactory::get().Create(CellFactory::CellType::ORIGIN,
                                          *m_startGrid, endGrid),
                *m_startGrid));

            Invoker::get().addCommand(new Commands::AddCell(
                CellFactory::get().Create(CellFactory::CellType::DESTINATION,
                                          endGrid, *m_startGrid),
                endGrid));
          }

          m_startGrid = std::nullopt;
        }
        break;

      default:
        break;
      }
    }
  }

  void cursorPosCallback(double xpos, double ypos) override {
    if (!GridManager::get().allocated())
      return;

    float glx = m_mouse[0];
    float gly = m_windowSize[1] - m_mouse[1];

    auto uuid = m_engine->lookupObjectUUID(glx, gly);

    if (uuid == m_engine->NONE_UUID)
      return;

    Vec2u gridPos = GridManager::get().getCoord(Vec2{glx, gly});
    Grid::IGrid *grid = GridManager::get().get(uuid);

    if (!grid)
      return;

    if (mouseHolding != -1) {
      if (mouseHolding == GLFW_MOUSE_BUTTON_LEFT) {

        Invoker::get().addCommand(new Commands::AddCell(
            CellFactory::get().Create(CellFactory::CellType::OBS, gridPos),
            grid));

      } else if (mouseHolding == GLFW_MOUSE_BUTTON_RIGHT) {
        Invoker::get().addCommand(new Commands::RemoveCell(grid));
      }
      refresh = true;
    }
  }

  void initLogger() {
    sm.set("obsAmount", 0ull);
    sm.set("pathAmount", 0ull);

    sm.set("rows", 0ull);
    sm.set("cols", 0ull);
    sm.set("regularGrid", false);

    sm.set("pathTime", 0.0);
    sm.set("pathDist", 0.0);

    sm.set("fpsSim", 0.0);
    sm.set("iterSim", 1ull);

    sm.set("agentsSpeed", 150.0);
    sm.set("agentsRadius", 16.0);
    sm.set("midPointPath", false);

    sm.set("sample", 1ull);
    sm.set("run", 1ull);
  }

  void setupStartup() {
    initChain.setNext(new ::Chain::GridConfig())
        ->setNext(new ::Chain::GridFactory())
        ->setNext(new ::Chain::GridManager());
  }

  void animationSetup() {
    displayRefreshSubscriber.setOnChange([this]() { refresh = true; });
    pathRefreshSubscriber.setOnChange([this]() {
      dirty = true;
      anim.pause();
    });

    GridManager::get()
        .subscribeOnCellChange(&displayRefreshSubscriber)
        .subscribeOnCellChange(&pathRefreshSubscriber)
        .subscribeOnGridChange(&displayRefreshSubscriber);

    anim.setPlayFunction([this](double dt, double elapsedTime) {
      if (dt == 0) {
        GridManager::get().setup();
        if (dirty) {
          dirty = false;
          PathManager::get().update();
        }
      }
      simManager.update(dt);
      bool res = GridManager::get().update(*m_engine, elapsedTime);

      return res;
    });

    anim.setIdleFunction([this]() {
      sm.set("fpsSim", 0.0);
      return GridManager::get().update(*m_engine);
    });

    animationState.setOnChange([this]() {
      switch (anim.getState()) {
      case decltype(anim)::PlayerState::INIT:
        break;

      case decltype(anim)::PlayerState::PLAYING:
        break;

      case decltype(anim)::PlayerState::PAUSED:
        GridManager::get().setup();
        Simulation::Manager::get().reset();
        break;

      case decltype(anim)::PlayerState::ENDED:
        refresh = true;
        break;

      default:
        break;
      }
    });
    anim.registerOnStateChanged(&animationState);
  }
};

int main(int argc, const char **argv) {
  MyWindow win(argc, argv);
  while (win.isActivate()) {
    win.gameloop();
    FrameMark;
  }

  return 0;
}
