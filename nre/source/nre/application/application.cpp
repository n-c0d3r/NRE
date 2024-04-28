#include <nre/application/application.hpp>



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
	}
	F_application::~F_application() {
	}

	void F_application::start() {

		frame_start_ = eastl::chrono::high_resolution_clock::now();
		frame_end_ = eastl::chrono::high_resolution_clock::now();

		startup_event_.invoke();

		surface_manager_p_->T_run([this](F_surface_manager& surface_manager){

		  	gameplay_tick_event_.invoke();
		  	render_tick_event_.invoke();

		  	frame_start_ = frame_end_;
		  	frame_end_ = eastl::chrono::high_resolution_clock::now();

		});

		shutdown_event_.invoke();

	}

}