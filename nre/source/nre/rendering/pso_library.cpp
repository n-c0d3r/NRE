#include <nre/rendering/pso_library.hpp>



namespace nre {

	TK<F_pso_library> F_pso_library::instance_ps;



	F_pso_library::F_pso_library() {

		instance_ps = NCPP_KTHIS().no_requirements();
	}
	F_pso_library::~F_pso_library() {
	}

}