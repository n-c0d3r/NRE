#include <nre/rendering/render_pipeline.hpp>



namespace nre {

	TK<A_render_pipeline> A_render_pipeline::instance_ps;



	A_render_pipeline::A_render_pipeline()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();
	}
	A_render_pipeline::~A_render_pipeline() {
	}

	void A_render_pipeline::install()
	{
	}

	void A_render_pipeline::begin_main_command_list() {
	}
	void A_render_pipeline::submit_main_command_list() {
	}
	void A_render_pipeline::async_begin_main_command_list() {
	}
	void A_render_pipeline::async_submit_main_command_list() {
	}

	void A_render_pipeline::begin_infrequent_upload_command_list() {
	}
	void A_render_pipeline::submit_infrequent_upload_command_list() {
	}
	void A_render_pipeline::async_begin_infrequent_upload_command_list() {
	}
	void A_render_pipeline::async_submit_infrequent_upload_command_list() {
	}

	void A_render_pipeline::begin_infrequent_compute_command_list() {
	}
	void A_render_pipeline::submit_infrequent_compute_command_list() {
	}
	void A_render_pipeline::async_begin_infrequent_compute_command_list() {
	}
	void A_render_pipeline::async_submit_infrequent_compute_command_list() {
	}

	void A_render_pipeline::begin_frame_upload_command_list() {
	}
	void A_render_pipeline::submit_frame_upload_command_list() {
	}
	void A_render_pipeline::async_begin_frame_upload_command_list() {
	}
	void A_render_pipeline::async_submit_frame_upload_command_list() {
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