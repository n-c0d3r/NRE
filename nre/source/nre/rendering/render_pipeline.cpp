#include <nre/rendering/render_pipeline.hpp>



namespace nre {

	TK<A_render_pipeline> A_render_pipeline::instance_ps;



	A_render_pipeline::A_render_pipeline()
	{
		instance_ps = NCPP_KTHIS_UNSAFE();
	}
	A_render_pipeline::~A_render_pipeline() {
	}

	void A_render_pipeline::begin_main_command_list() {
	}
	void A_render_pipeline::submit_main_command_list() {
	}

	void A_render_pipeline::begin_infrequent_upload_command_list() {

		begin_main_command_list();
	}
	void A_render_pipeline::submit_infrequent_upload_command_list() {

		submit_main_command_list();
	}

	void A_render_pipeline::begin_infrequent_direct_command_list() {

		begin_main_command_list();
	}
	void A_render_pipeline::submit_infrequent_direct_command_list() {

		submit_main_command_list();
	}

	void A_render_pipeline::begin_frame_upload_command_list() {

		begin_main_command_list();
	}
	void A_render_pipeline::submit_frame_upload_command_list() {

		submit_main_command_list();
	}

	void A_render_pipeline::begin_setup() {

		begin_main_command_list();
	}
	void A_render_pipeline::end_setup() {

		submit_main_command_list();
	}

	void A_render_pipeline::begin_render() {

		begin_main_command_list();
	}
	void A_render_pipeline::end_render() {

		submit_main_command_list();
	}

}