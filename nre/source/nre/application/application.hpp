#pragma once

#include <nre/prerequisites.hpp>
#include <nre/application/events/application_startup_event.hpp>
#include <nre/application/events/application_shutdown_event.hpp>
#include <nre/application/events/application_gameplay_tick_event.hpp>
#include <nre/application/events/application_render_tick_event.hpp>



namespace nre {

	struct F_application_desc {

		F_surface_desc main_surface_desc;

	};



	class NRE_API F_application {

	private:
		static TK<F_application> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_application> instance_p() {

			return instance_ps;
		}



	private:
		F_application_desc desc_;
		F_application_startup_event startup_event_;
		F_application_shutdown_event shutdown_event_;
		F_application_gameplay_tick_event gameplay_tick_event_;
		F_application_render_tick_event render_tick_event_;

		TU<F_surface_manager> surface_manager_p_;
		TK<F_surface> main_surface_p_;

		eastl::chrono::system_clock::time_point frame_start_;
		eastl::chrono::system_clock::time_point frame_end_;

	public:
		NCPP_FORCE_INLINE const F_application_desc& desc() const noexcept { return desc_; }
		NCPP_DECLARE_STATIC_EVENTS(
			startup_event_,
			shutdown_event_,
			gameplay_tick_event_,
			render_tick_event_
		);

		NCPP_FORCE_INLINE TKPA<F_surface> main_surface_p() const noexcept { return main_surface_p_; }

		NCPP_FORCE_INLINE f32 delta_seconds() const noexcept {

			u64 nanoseconds = eastl::chrono::duration_cast<eastl::chrono::nanoseconds>(
				frame_end_ - frame_start_
			).count();

			return ((f32)nanoseconds) * 0.000000001f;
		}
		NCPP_FORCE_INLINE f32 fps() const noexcept {

			return 1.0f / delta_seconds();
		}



	public:
		F_application(const F_application_desc& desc);
		~F_application();

	public:
		void start();

	};

}



#define NRE_APPLICATION_STARTUP(...) \
	__VA_ARGS__->T_get_event<nre::F_application_startup_event>().T_push_back_listener
#define NRE_APPLICATION_SHUTDOWN(...) \
	__VA_ARGS__->T_get_event<nre::F_application_shutdown_event>().T_push_back_listener
#define NRE_APPLICATION_GAMEPLAY_TICK(...) \
	__VA_ARGS__->T_get_event<nre::F_application_gameplay_tick_event>().T_push_back_listener
#define NRE_APPLICATION_RENDER_TICK(...) \
	__VA_ARGS__->T_get_event<nre::F_application_render_tick_event>().T_push_back_listener