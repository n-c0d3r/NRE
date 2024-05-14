#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_actor;
	class A_actor_component;



	class NRE_API A_actor_component {

	public:
		friend class F_actor;



	private:
		TK_valid<F_actor> actor_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_actor> actor_p() const noexcept { return actor_p_; }



	public:
		A_actor_component(TK_valid<F_actor> actor_p);
		virtual ~A_actor_component();

	public:
		NCPP_OBJECT(A_actor_component);

	protected:
		virtual void gameplay_tick();
		virtual void render_tick();

	};

}