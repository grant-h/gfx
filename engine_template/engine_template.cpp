#include "Log.hpp"
#include "Window.hpp"

int main(int argc, char * argv[])
{
  log_set_level(LOG_LEVEL_TRACE);

  if (argc > 0)
    LOG_INFO("Starting %s", argv[0]);

  Window * window = new Window("EngineTemplate");

  if (!window->create(640, 480)) {
    return 1;
  }

  window->process();
}
