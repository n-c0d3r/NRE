#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/shadow_mask.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class A_render_view;
	class F_transform_node;



	class NRE_API A_shadow : public A_actor_component {

	public:
		friend class F_shadow_system;



	private:
		TK_valid<F_transform_node> transform_node_p_;
		typename TG_list<TK_valid<A_shadow>>::iterator handle_;
		F_shadow_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_transform_node> transform_node_p() const noexcept { return transform_node_p_; }
		NCPP_FORCE_INLINE F_shadow_mask mask() const noexcept { return mask_; }



	protected:
		A_shadow(TKPA_valid<F_actor> actor_p, F_shadow_mask mask = 0);

	public:
		virtual ~A_shadow();

	public:
		NCPP_OBJECT(A_shadow);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	public:
		void update_mask(F_shadow_mask value);
	};
}