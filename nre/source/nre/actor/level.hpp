#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_level;
	class F_actor;



	class NRE_API F_level final {

	public:
		friend class F_actor;



	private:
		TG_list<TU<F_actor>> actor_p_list_;
		TG_list<TK_valid<F_actor>> gameplay_tickable_actor_p_list_;
		TG_list<TK_valid<F_actor>> render_tickable_actor_p_list_;

	protected:
		G_string name_;
		typename F_event::F_listener_handle gameplay_tick_event_listener_handle_;
		typename F_event::F_listener_handle render_tick_event_listener_handle_;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(const G_string& value) noexcept { name_ = value; }



	public:
		F_level(const G_string& name = "No name");
		~F_level();

	public:
		NCPP_DISABLE_COPY(F_level);

	public:
		template<typename F_actor__ = F_actor, typename... F_args__>
		requires T_is_object_down_castable<F_actor__, F_actor>
			&& requires(F_args__&&&... args)
			{
				F_actor__(TK_valid<F_level>(), std::forward<F_args__>(args)...);
			}
		TK_valid<F_actor__> T_create_actor(F_args__&&... args)
		{
			auto actor_p = TU<F_actor__>()(
				NCPP_KTHIS(),
				std::forward<F_args__>(args)...
			);

			auto keyed_actor_p = NCPP_FOH_VALID(actor_p);

			actor_p_list_.push_back(std::move(actor_p));
			keyed_actor_p->handle_ = --(actor_p_list_.end());

			return keyed_actor_p;
		}
		void destroy_actor(TKPA_valid<F_actor> actor_p);

	private:
		void gameplay_tick_internal();
		void render_tick_internal();

	private:
		void register_event_listeners();
		void deregister_event_listeners();

	};

}