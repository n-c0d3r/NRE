#pragma once

#include <nre/prerequisites.hpp>
#include <nre/application/events/application_startup_event.hpp>
#include <nre/application/events/application_shutdown_event.hpp>
#include <nre/application/events/application_gameplay_tick_event.hpp>
#include <nre/application/events/application_render_tick_event.hpp>



namespace nre {

	class F_render_system;
	class F_asset_system;



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

		TU<F_render_system> render_system_p_;
		TU<F_asset_system> asset_system_p_;

		eastl::chrono::system_clock::time_point start_;
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

		NCPP_FORCE_INLINE f64 start_seconds() const noexcept {

			u64 nanoseconds = eastl::chrono::duration_cast<eastl::chrono::nanoseconds>(
				frame_start_ - start_
			).count();

			return ((f64)nanoseconds) * 0.000000001;
		}
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



namespace nre::internal
{

	NCPP_FORCE_INLINE b8 tick_by_duration_check(f32& time, f32 duration)
	{

		if(time >= duration)
		{
			time = 0.0f;

			return true;
		}

		return false;
	}
	struct F_tick_by_duration_caller
	{
		template<typename F__>
		NCPP_FORCE_INLINE F_tick_by_duration_caller(F__&& func)
		{
			func();
		}
	};



	struct F_application_startup_caller
	{

	private:
		TK_valid<F_application> app_p_;

	public:
		NCPP_FORCE_INLINE F_application_startup_caller(TKPA_valid<F_application> app_p) :
			app_p_(app_p)
		{}
		template<typename F__>
		NCPP_FORCE_INLINE void operator = (F__&& functor) {

			app_p_->T_get_event<nre::F_application_startup_event>().T_push_back_listener(
				std::forward<F__>(functor)
			);
		}
	};

	struct F_application_shutdown_caller
	{

	private:
		TK_valid<F_application> app_p_;

	public:
		NCPP_FORCE_INLINE F_application_shutdown_caller(TKPA_valid<F_application> app_p) :
			app_p_(app_p)
		{}
		template<typename F__>
		NCPP_FORCE_INLINE void operator = (F__&& functor) {

			app_p_->T_get_event<nre::F_application_shutdown_event>().T_push_back_listener(
				std::forward<F__>(functor)
			);
		}
	};

	struct F_application_gameplay_tick_caller
	{

	private:
		TK_valid<F_application> app_p_;

	public:
		NCPP_FORCE_INLINE F_application_gameplay_tick_caller(TKPA_valid<F_application> app_p) :
			app_p_(app_p)
		{}
		template<typename F__>
		NCPP_FORCE_INLINE void operator = (F__&& functor) {

			app_p_->T_get_event<nre::F_application_gameplay_tick_event>().T_push_back_listener(
				std::forward<F__>(functor)
			);
		}
	};

	struct F_application_render_tick_caller
	{

	private:
		TK_valid<F_application> app_p_;

	public:
		F_application_render_tick_caller(TKPA_valid<F_application> app_p) :
			app_p_(app_p)
		{}
		template<typename F__>
		void operator = (F__&& functor) {

			app_p_->T_get_event<nre::F_application_render_tick_event>().T_push_back_listener(
				std::forward<F__>(functor)
			);
		}
	};

}



#define NRE_APPLICATION(...) (nre::F_application::instance_p())
#define NRE_MAIN_SURFACE(...) (NRE_APPLICATION()->main_surface_p())
#define NRE_APPLICATION_STARTUP(...) \
	nre::internal::F_application_startup_caller ___nre_application_startup___ ## NCPP_LINE ( \
		NCPP_FOREF_VALID(__VA_ARGS__)                        \
	); \
	___nre_application_startup___ ## NCPP_LINE = [&](auto&)
#define NRE_APPLICATION_SHUTDOWN(...) \
	nre::internal::F_application_shutdown_caller ___nre_application_shutdown___ ## NCPP_LINE ( \
		NCPP_FOREF_VALID(__VA_ARGS__)                        \
	); \
	___nre_application_shutdown___ ## NCPP_LINE = [&](auto&)
#define NRE_APPLICATION_GAMEPLAY_TICK(...) \
	nre::internal::F_application_gameplay_tick_caller ___nre_application_gameplay_tick___ ## NCPP_LINE ( \
		NCPP_FOREF_VALID(__VA_ARGS__)                        \
	); \
	___nre_application_gameplay_tick___ ## NCPP_LINE = [&](auto&)
#define NRE_APPLICATION_RENDER_TICK(...) \
	nre::internal::F_application_render_tick_caller ___nre_application_render_tick___ ## NCPP_LINE ( \
		NCPP_FOREF_VALID(__VA_ARGS__)                        \
	); \
	___nre_application_render_tick___ ## NCPP_LINE = [&](auto&)

#define NRE_TICK_BY_DURATION(...) \
	static f32 ___nre_tick_by_duration_time___ ## NCPP_LINE = 0;\
	___nre_tick_by_duration_time___ ## NCPP_LINE += NRE_APPLICATION()->delta_seconds();\
	if(nre::internal::tick_by_duration_check(___nre_tick_by_duration_time___ ## NCPP_LINE, __VA_ARGS__))\
		nre::internal::F_tick_by_duration_caller ___nre_tick_by_duration_caller___ ## NCPP_LINE = [&]()