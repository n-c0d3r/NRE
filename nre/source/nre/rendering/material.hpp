#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>
#include <nre/rendering/material_mask.hpp>



namespace nre {

	class NRE_API A_material : public A_actor_component {

	public:
		friend class F_material_system;



	private:
		typename TG_list<TK_valid<A_material>>::iterator handle_;
		F_material_mask mask_ = 0;

	public:
		NCPP_FORCE_INLINE F_material_mask mask() const noexcept { return mask_; }



	protected:
		A_material(TKPA_valid<F_actor> actor_p, F_material_mask mask = 0);

	public:
		virtual ~A_material();

	public:
		NCPP_OBJECT(A_material);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	protected:
		void update_mask(F_material_mask value);
	};
}