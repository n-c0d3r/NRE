#include <nre/rendering/render_pipeline.hpp>



namespace nre {

	TK<A_render_pipeline> A_render_pipeline::instance_ps;



	A_render_pipeline::A_render_pipeline()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();
	}
	A_render_pipeline::~A_render_pipeline() {
	}

	void A_render_pipeline::begin_setup() {
	}
	void A_render_pipeline::end_setup() {
	}

	void A_render_pipeline::begin_render() {
	}
	void A_render_pipeline::end_render() {
	}

}