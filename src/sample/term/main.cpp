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

using namespace proxo;

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

	// program main loop
	bool done = false;
	while(!done) {
    auto t1 = std::chrono::system_clock::now();
    realTimeEngine.render();
    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    fprintf(stderr, "render: %f sec\n", diff.count());
    auto txt = btx.convert2();
    fwrite(txt.get(), strlen(txt.get()), 1, stdout);
    fflush(stdout);
    //usleep(300*1000);
//    break;
	} // end main loop

	return 0;
}
