#include <nre/application/application.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/ui/imgui.hpp>


#ifdef EA_PLATFORM_WINDOWS
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif



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

		asset_system_p_ = TU<F_asset_system>()();
		imgui_p_ = TU<F_imgui>()();
		render_system_p_ = TU<F_render_system>()();

#ifdef NRE_ENABLE_TASK_SYSTEM
		task_system_p_ = TU<F_task_system>()(
			NRE_RENDER_PIPELINE()->task_system_desc()
		);
#endif

		NRE_RENDER_PIPELINE()->install();

#ifdef NRE_ENABLE_TASK_SYSTEM
		task_system_p_->start();
#endif

		NRE_RENDER_PIPELINE()->begin_setup();
	}
	F_application::~F_application() {

		render_system_p_.reset();
		imgui_p_.reset();

		nrhi::release_system();

		FreeImage_DeInitialise();
	}

	void F_application::start() {

		is_started_ = true;

		start_ = eastl::chrono::high_resolution_clock::now();
		frame_start_ = start_;
		frame_end_ = start_;

		main_surface_p_->T_get_event<F_surface_destroy_event>().T_push_back_listener(
			[this](auto& e){
				shutdown_event_.invoke();

#ifdef NRE_ENABLE_TASK_SYSTEM
				task_system_p_->join();
#endif
			  	render_system_p_.reset();
			}
		);

		startup_event_.invoke();

		NRE_RENDER_PIPELINE()->end_setup();

		surface_manager_p_->T_run([this](F_surface_manager& surface_manager){

		  	if(main_surface_p_) {

				NRE_RENDER_PIPELINE()->begin_render();

				gameplay_tick_event_.invoke();

				render_tick_event_.invoke();

				NRE_RENDER_PIPELINE()->end_render();
		  	}

		  	frame_start_ = frame_end_;
		  	frame_end_ = eastl::chrono::high_resolution_clock::now();

		});
	}

}