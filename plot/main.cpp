#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <ostream>
#include <random>

constexpr int BINS = 10;
constexpr int WIDTH = 2100;
constexpr int HEIGHT = 1500;
constexpr int CIR_NUMBERS = 50;
constexpr float CIR_RADIUS = 10;
constexpr float SCALE = 50.0f;
constexpr float MAX_SPEED = 5;
const sf::Vector2f ORIGIN = {200, 350};

sf::RectangleShape rect_shapes[BINS];

void plot_dist(sf::RenderWindow &window, float velocities[],
               bool re_calculate = false) {
  float bin_range = MAX_SPEED / BINS;
  int counts[BINS] = {}; // number of particle fall into this speed's range
  const float scale_width = 50;
  const float scale_height = 300;
  const float spacing = 1;
  const float bin_width = bin_range * scale_width;

  if (re_calculate) {
    for (int i = 0; i < BINS; i++) {
      // loop through all velo and increment each group of bin
      for (int j = 0; j < CIR_NUMBERS; j++) {
        float speed = abs(velocities[j]);
        if (speed >= i * bin_range && speed < i * bin_range + bin_range)
          counts[i]++;
      }
    }

    // Setup rect as bins
    for (int i = 0; i < BINS; i++) {
      float bin_height = scale_height * counts[i] / CIR_NUMBERS;
      // stretch the size
      rect_shapes[i].setSize({bin_width, bin_height});
      rect_shapes[i].setPosition(
          {(bin_width + spacing) * i + ORIGIN.x, ORIGIN.y - bin_height});
      rect_shapes[i].setFillColor(sf::Color::Magenta);
    }
  }

  // Draw hist
  for (int i = 0; i < BINS; i++) {
    window.draw(rect_shapes[i]);
  }
  // Draw axes
  sf::RectangleShape vertical({2, scale_height + spacing});
  sf::RectangleShape horizontal({(bin_width + spacing) * BINS + spacing, 2});
  vertical.setPosition({ORIGIN.x, ORIGIN.y - scale_height});
  horizontal.setPosition(ORIGIN);
  window.draw(vertical);
  window.draw(horizontal);
}

int main() {

  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Test random kicks");
  window.setFramerateLimit(60);

  sf::VertexArray sin_graph(sf::LinesStrip, WIDTH);
  for (int x = 0; x < WIDTH; x++) {
    float y = HEIGHT / 2.0f - std::sin(x / SCALE) * SCALE;
    sin_graph[x].position = sf::Vector2f(x, y);
    sin_graph[x].color = sf::Color::White;
  }

  // Create a random number generator
  std::random_device rd;
  std::mt19937 eng(rd());
  // Create a uniform distribution in the specified range
  std::uniform_real_distribution<float> rand_move(-MAX_SPEED, MAX_SPEED);

  // Array of circles
  sf::CircleShape circles[CIR_NUMBERS];
  float velocities[CIR_NUMBERS] = {};
  // set all init positions
  for (int i = 0; i < CIR_NUMBERS; i++) {
    circles[i].setRadius(CIR_RADIUS);
    circles[i].setPosition({2 * CIR_RADIUS * i + 50, HEIGHT - 500});
  }

  //
  constexpr uint32_t fps_cap = 60;
  int32_t target_fps = fps_cap;
  int counter = 0;

  while (window.isOpen()) {
    // catch key pressed
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();

      // FIXME use event manager
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        target_fps = target_fps ? 0 : fps_cap;
        window.setFramerateLimit(target_fps);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
        // 0 to disable limit
        window.setFramerateLimit(0);
    }

    // Clear the window with black color
    window.clear(sf::Color::Black);

    // Draw the sin(x) graph
    // window.draw(sin_graph);

    for (int i = 0; i < CIR_NUMBERS; i++) {
      velocities[i] = rand_move(eng);
      circles[i].move({0, velocities[i]});
      // Draw
      window.draw(circles[i]);
    }

    // Draw Distribution each 100 frames
    if (counter == 50) {
      counter = 0;
      plot_dist(window, velocities, true);
    } else {
      counter++;
      plot_dist(window, velocities, false);
    }

    // Display the result on the screen
    window.display();
  }

  return 0;
}
