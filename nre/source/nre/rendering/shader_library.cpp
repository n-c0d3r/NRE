#include <nre/rendering/shader_library.hpp>



namespace nre {

	TK<F_shader_library> F_shader_library::instance_ps;



	F_shader_library::F_shader_library() {

		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_shader_library::~F_shader_library() {
	}

}