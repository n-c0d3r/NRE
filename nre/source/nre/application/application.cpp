#include <nre/application/application.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/asset/asset_system.hpp>



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

		// init imgui
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

#ifdef EA_PLATFORM_WINDOWS
			ImGui_ImplWin32_Init(main_surface_p_->handle());
			main_surface_p_->registry_custom_window_proc_handler(
				ImGui_ImplWin32_WndProcHandler
			);
#endif
		}

		render_system_p_ = TU<F_render_system>()();
		asset_system_p_ = TU<F_asset_system>()();
	}
	F_application::~F_application() {

		render_system_p_.reset();

		// release imgui
		{
#ifdef EA_PLATFORM_WINDOWS
			ImGui_ImplWin32_Shutdown();
#endif

			ImGui::DestroyContext();
		}

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

				// begin imgui frame
				{
#ifdef NRHI_DRIVER_DIRECTX_11
					if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
						ImGui_ImplDX11_NewFrame();
#endif

#ifdef EA_PLATFORM_WINDOWS
					ImGui_ImplWin32_NewFrame();
#endif

					ImGui::NewFrame();
				}

				ImGui::ShowDemoWindow();

				gameplay_tick_event_.invoke();

				render_tick_event_.invoke();

				// end imgui frame
				{
					ImGui::Render();

#ifdef NRHI_DRIVER_DIRECTX_11
					if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
					{
						ID3D11RenderTargetView* d3d11_rtv_p = (ID3D11RenderTargetView*)(
							render_system_p_->main_swapchain_p()->back_rtv_p().T_cast<F_directx11_resource_view>()->d3d11_view_p()
						);
						ID3D11DeviceContext* d3d11_ctx_p = render_system_p_->command_queue_p().T_cast<F_directx11_command_queue>()->d3d11_device_context_p();
						d3d11_ctx_p->OMSetRenderTargets(
							1,
							&d3d11_rtv_p,
							0
						);

						ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
					}
#endif
				}

				render_system_p_->main_swapchain_p()->present();
		  	}

		  	frame_start_ = frame_end_;
		  	frame_end_ = eastl::chrono::high_resolution_clock::now();

		});

	}

}