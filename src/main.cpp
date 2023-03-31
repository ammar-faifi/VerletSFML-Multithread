#include <iostream>

#include "engine/common/color_utils.hpp"
#include "engine/common/event_manager.hpp"
#include "engine/common/number_generator.hpp"
#include "engine/window_context_handler.hpp"

#include "physics/physics.hpp"
#include "renderer/renderer.hpp"
#include "thread_pool/thread_pool.hpp"

int main() {
  const uint32_t window_width = 4400;
  const uint32_t window_height = 2500;
  bool gravity_on = false;
  WindowContextHandler app("Verlet-MultiThread",
                           sf::Vector2u(window_width, window_height),
                           sf::Style::Default);
  RenderContext &render_context = app.getRenderContext();
  // Initialize solver and renderer

  /* The number of threads should be adjusted so that it divides the world size
     or some of the grid's cells won't be processed */
  tp::ThreadPool thread_pool(10);
  const IVec2 world_size{300, 300};
  PhysicSolver solver{world_size, thread_pool};
  Renderer renderer(solver, thread_pool);

  const float margin = 20.0f;
  const auto zoom = static_cast<float>(window_height - margin) /
                    static_cast<float>(world_size.y);
  render_context.setZoom(zoom);
  render_context.setFocus({world_size.x * 0.5f, world_size.y * 0.5f});

  bool emit = true;
  app.getEventManager().addKeyPressedCallback(
      sf::Keyboard::Space, [&](sfev::CstEv) { emit = !emit; });

  // Toggle gravity
  app.getEventManager().addKeyPressedCallback(sf::Keyboard::G,
                                              [&](sfev::CstEv) {
                                                if (gravity_on) {
                                                  solver.gravity = {0.f, 0.f};
                                                  gravity_on = false;
                                                } else {
                                                  solver.gravity = {0.f, 20.f};
                                                  gravity_on = true;
                                                }
                                              });

  constexpr uint32_t fps_cap = 60;
  int32_t target_fps = fps_cap;
  app.getEventManager().addKeyPressedCallback(
      sf::Keyboard::S, [&](sfev::CstEv) {
        target_fps = target_fps ? 0 : fps_cap;
        app.setFramerateLimit(target_fps);
      });

  // Main loop
  const float dt = 1.0f / static_cast<float>(fps_cap);
  while (app.run()) {
    if (solver.objects.size() < 20 && emit) {
      for (uint32_t i{20}; i--;) {
        const auto id = solver.createObject({2.0f, 10.0f + 1.1f * i});
        solver.objects[id].last_position.x -= 0.2f;
        solver.objects[id].color = ColorUtils::getRainbow(id * 0.0001f);
      }
    }

    solver.update(dt);

    render_context.clear();
    renderer.render(render_context);
    render_context.display();
  }

  return 0;
}
