#include <nre/application/application.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/asset/asset_system.hpp>



namespace nre {

	TK<F_application> F_application::instance_ps;



	F_application::F_application(const F_application_desc& desc) :
		desc_(desc),
		startup_event_(NCPP_KTHIS()),
		shutdown_event_(NCPP_KTHIS()),
		gameplay_tick_event_(NCPP_KTHIS()),
		render_tick_event_(NCPP_KTHIS()),
		surface_manager_p_(TU<F_surface_manager>()()),
		main_surface_p_(
			surface_manager_p_->create_surface(desc.main_surface_desc).no_requirements()
		)
	{
		instance_ps = NCPP_KTHIS().no_requirements();

		FreeImage_Initialise();

		nrhi::initialize_system();

		render_system_p_ = TU<F_render_system>()();
		asset_system_p_ = TU<F_asset_system>()();
	}
	F_application::~F_application() {

		nrhi::release_system();

		FreeImage_DeInitialise();
	}

	void F_application::start() {

		start_ = eastl::chrono::high_resolution_clock::now();
		frame_start_ = start_;
		frame_end_ = start_;

		main_surface_p_->T_get_event<F_surface_destroy_event>().T_push_back_listener(
			[this](auto& e){
			  	shutdown_event_.invoke();
			  	render_system_p_.reset();
			}
		);

		startup_event_.invoke();

		surface_manager_p_->T_run([this](F_surface_manager& surface_manager){

		  	if(main_surface_p_) {

				render_system_p_->main_frame_buffer_p()->update_viewport();

				gameplay_tick_event_.invoke();
				render_tick_event_.invoke();

				render_system_p_->main_swapchain_p()->present();

		  	}

		  	frame_start_ = frame_end_;
		  	frame_end_ = eastl::chrono::high_resolution_clock::now();

		});

	}

}