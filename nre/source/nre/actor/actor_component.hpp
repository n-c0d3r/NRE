#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_actor;
	class A_actor_component;



	class NRE_API A_actor_component : public A_object
	{
	public:
		friend class F_actor;

	private:
		TK_valid<F_actor> actor_p_;
		b8 is_active_ = false;
		b8 is_active_next_frame_ = true;

	protected:
		TG_vector<u64> type_ids_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_actor> actor_p() const noexcept { return actor_p_; }
		NCPP_FORCE_INLINE b8 is_active() const noexcept { return is_active_; }

	public:
		NCPP_FORCE_INLINE const TG_vector<u64>& type_ids() const noexcept { return type_ids_; }



	public:
		A_actor_component(TK_valid<F_actor> actor_p);
		virtual ~A_actor_component();

	public:
		NCPP_OBJECT(A_actor_component);

	protected:
		virtual void ready();
		virtual void destroy();
		virtual void gameplay_tick();
		virtual void render_tick();
		virtual void active();
		virtual void deactive();

	public:
		void set_active(b8 value);

	};

}



#define NRE_ACTOR_COMPONENT_REGISTER(...) type_ids_.push_back(ncpp::T_type_hash_code<__VA_ARGS__>);
