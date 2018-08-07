#include "term.h"
#include "../parser/SceneParser.h"

#include "core/math/basics.h"
#include "core/model/Scene.h"
#include "core/common/Buffer.h"
#include "core/realtime/Engine.h"

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <chrono>
#include <fcntl.h>
#include <cstdio>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

using namespace proxo;

char getInput()
{
  fd_set set;
  struct timeval tv;

  FD_ZERO(&set);
  FD_SET(fileno(stdin), &set);

  int res = select(fileno(stdin) + 1, &set, NULL, NULL, NULL);

  if (res > 0) {
    char c;
    read(fileno(stdin), &c, 1);
    return c;
  } else if (res < 0) {
    //perror("select error");
    return 0;
  } else {
    //printf("Select timeout\n");
    return 0;
  }
}

int main(int argc, char** argv)
{
	// Load scene from file
	SceneParser parser;
	Scene scene;
	std::string sceneFileName = argc >= 2 ? argv[1] : "media/parc.scene";
	if(!parser.readScene(sceneFileName, scene)) {
		printf("readScene failed!\n");
		return 2;
	}
	scene.printSize();

	positive screen_width;
	positive screen_height;
  BufferToXTerm::getViewSize(screen_width, screen_height);
  screen_height -= 4;
  printf("Image size: %dx%d\n", screen_width, screen_height);
  scene.camera.setRenderTarget(screen_width, screen_height, 0);
  scene.camera.updateProjection();
	
	Buffer buf(screen_width, screen_height);

	Engine realTimeEngine(&buf, &scene);
	realTimeEngine.createMatchingPool();

	BufferToXTerm btx(buf);
  
  // Init term input
  struct termios oldSettings, newSettings;

  tcgetattr(fileno(stdin), &oldSettings);
  newSettings = oldSettings;
  newSettings.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr(fileno(stdin), TCSANOW, &newSettings);

	// program main loop
	bool done = false;
	while(!done) {
    switch(getInput()) {
    case 'z':
      applyRotate(scene.camera.position, 0.05, -1, 0, 0);
      break;
    case 's':
      applyRotate(scene.camera.position, 0.05, 1, 0, 0);
      break;
    case 'q':
      applyRotate(scene.camera.position, 0.05, 0, -1, 0);
      break;
    case 'd':
      applyRotate(scene.camera.position, 0.05, 0, 1, 0);
      break;
    case 'e':
      done = true;
      break;
    }
      //applyRotate(scene.camera.position, 0.2, 0, -1, 0);

    auto t1 = std::chrono::system_clock::now();
    realTimeEngine.render();
    auto t2 = std::chrono::system_clock::now();
    //std::chrono::duration<double> diff = t2 - t1;
    //fprintf(stderr, "render: %f sec\n", diff.count());
    auto txt = btx.convert2();
    fwrite(txt.get(), strlen(txt.get()), 1, stdout);
    fflush(stdout);

    //usleep(300*1000);
//    break;
	} // end main loop
  
  tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
	return 0;
}
