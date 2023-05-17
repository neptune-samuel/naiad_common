
#include <iostream>
#include <uv.h>
#include <common/libuv_utils.h>

// uv_loop_t* loop;

// void on_signal(uv_signal_t* handle, int signum) {
//   std::cout << "Received signal: " << signum << std::endl;
//   uv_stop(loop);
// }

int main() {
  //loop = uv_default_loop();

//   uv_signal_t signal;
//   uv_signal_init(loop, &signal);
//   uv_signal_start(&signal, on_signal, SIGINT);

//   std::cout << "Running event loop..." << std::endl;
//   uv_run(loop, UV_RUN_DEFAULT);

//   uv_signal_stop(&signal);

  nos::libuv::RunLoop mloop(true);
//   loop = mloop.get_loop();
// //   uv_signal_t signal;
// //   uv_signal_init(loop, &signal);
// //   uv_signal_start(&signal, on_signal, SIGINT);


  mloop.spin();  


  return 0;
}
