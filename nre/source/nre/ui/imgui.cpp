#include <nre/ui/imgui.hpp>
#include <nre/application/application.hpp>
#include <nre/rendering/render_system.hpp>
#include <nre/rendering/render_pipeline.hpp>



#ifdef EA_PLATFORM_WINDOWS
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif



namespace nre {

	TK<F_imgui> F_imgui::instance_ps;



	F_imgui::F_imgui()
	{
		instance_ps = NCPP_KTHIS().no_requirements();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

#ifdef EA_PLATFORM_WINDOWS
		ImGui_ImplWin32_Init(NRE_MAIN_SURFACE()->handle());
		NRE_MAIN_SURFACE()->registry_custom_window_proc_handler(
			ImGui_ImplWin32_WndProcHandler
		);
#endif
	}
	F_imgui::~F_imgui() {

#ifdef EA_PLATFORM_WINDOWS
		ImGui_ImplWin32_Shutdown();
#endif

		ImGui::DestroyContext();
	}

	void F_imgui::init_renderer()
	{
#ifdef NRHI_DRIVER_DIRECTX_11
		if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			ImGui_ImplDX11_Init(
				NRE_MAIN_DEVICE().T_cast<F_directx11_device>()->d3d11_device_p(),
				NRE_MAIN_COMMAND_QUEUE().T_cast<F_directx11_command_queue>()->d3d11_device_context_p()
			);
#endif

#ifdef NRHI_DRIVER_DIRECTX_12
		// if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
		// 	ImGui_ImplDX12_Init(
		// 		NCPP_FOH_VALID(device_p_).T_cast<F_directx12_device>()->d3d12_device_p(),
		// 		,
		// 		DXGI_FORMAT(
		// 			NRE_MAIN_SWAPCHAIN()->desc().format
		// 		)
		// 	);
#endif
	}
	void F_imgui::deinit_renderer()
	{
#ifdef NRHI_DRIVER_DIRECTX_11
		if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			ImGui_ImplDX11_Shutdown();
#endif

#ifdef NRHI_DRIVER_DIRECTX_12
		if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
			ImGui_ImplDX12_Shutdown();
#endif
	}
	void F_imgui::begin_frame() {
#ifdef NRHI_DRIVER_DIRECTX_11
		if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			ImGui_ImplDX11_NewFrame();
#endif

#ifdef NRHI_DRIVER_DIRECTX_12
		if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
			ImGui_ImplDX12_NewFrame();
#endif

#ifdef EA_PLATFORM_WINDOWS
		ImGui_ImplWin32_NewFrame();
#endif

		ImGui::NewFrame();
	}
	void F_imgui::end_frame() {

		// end imgui frame
		{
			ImGui::EndFrame();
			is_focused_ = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
		}

		// render imgui
		{
			ImGui::Render();

#ifdef NRHI_DRIVER_DIRECTX_11
			if (driver_index() == NRHI_DRIVER_INDEX_DIRECTX_11)
			{
				ID3D11RenderTargetView* d3d11_rtv_p = (ID3D11RenderTargetView*)(
					NRE_MAIN_SWAPCHAIN()->back_rtv_p().T_cast<F_directx11_resource_view>()->d3d11_view_p()
				);
				ID3D11DeviceContext* d3d11_ctx_p = NRE_MAIN_COMMAND_QUEUE().T_cast<F_directx11_command_queue>()->d3d11_device_context_p();
				d3d11_ctx_p->OMSetRenderTargets(
					1,
					&d3d11_rtv_p,
					0
				);

				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}
#endif

#ifdef NRHI_DRIVER_DIRECTX_12
			if(driver_index() == NRHI_DRIVER_INDEX_DIRECTX_12)
			{
			}
#endif
		}
	}
}