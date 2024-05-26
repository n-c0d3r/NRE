#pragma once

#include <nre/prerequisites.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class A_render_view;
	class A_material;
	class A_material_proxy;



//	class NRE_API A_material_binding_group {
//
//	public:
//		friend class A_material;
//		friend class A_material_proxy;
//
//
//
//	private:
//		TK_valid<A_material_proxy> proxy_p_;
//
//	public:
//		NCPP_FORCE_INLINE TKPA_valid<A_material_proxy> proxy_p() const noexcept { return proxy_p_; }
//
//
//
//	protected:
//		A_material_binding_group(TKPA_valid<A_material_proxy> proxy_p)
//
//	public:
//		virtual ~A_material_binding_group();
//
//	public:
//		NCPP_OBJECT(A_material_binding_group);
//
//	protected:
//		virtual void bind() = 0;
//		virtual void update() = 0;
//
//	};



	class NRE_API A_material_proxy {

	public:
		friend class A_material;



	private:
		TK_valid<A_material> material_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_material> material_p() const noexcept { return material_p_; }



	protected:
		A_material_proxy(TKPA_valid<A_material> material_p);

	public:
		virtual ~A_material_proxy();

	public:
		NCPP_OBJECT(A_material_proxy);

	protected:
		virtual void update();

	protected:
		virtual void bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) = 0;

	};



	class NRE_API A_material : public A_actor_component {

	private:
		TU<A_material_proxy> proxy_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<A_material_proxy> proxy_p() const noexcept { return NCPP_FOH_VALID(proxy_p_); }



	protected:
		A_material(TKPA_valid<F_actor> actor_p, TU<A_material_proxy>&& proxy_p);

	public:
		virtual ~A_material();

	public:
		NCPP_OBJECT(A_material);

	protected:
		virtual void ready() override;
		virtual void render_tick() override;

	public:
		NCPP_FORCE_INLINE void bind(
			KPA_valid_render_command_list_handle render_command_list_p,
			TKPA_valid<A_render_view> render_view_p,
			TKPA_valid<A_frame_buffer> frame_buffer_p
		) {
			proxy_p_->bind(
				render_command_list_p,
				render_view_p,
				frame_buffer_p
			);
		}

	};

}