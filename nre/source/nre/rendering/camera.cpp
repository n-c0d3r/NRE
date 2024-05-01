#include <nre/rendering/camera.hpp>



namespace nre {

	F_camera::F_camera(const G_string& name) {

	}
	F_camera::~F_camera() {

	}



	F_camera_actor::F_camera_actor(TK_valid<F_level> level_p, const G_string& name) :
		F_actor(level_p, name),
		camera_p_(TU<F_camera>()(name))
	{

	}
	F_camera_actor::~F_camera_actor() {

	}

}