﻿#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_render_view;



	class NRE_API F_render_view_system {

	public:
		friend class F_render_view;



	private:
		static TK<F_render_view_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_render_view_system> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TK_valid<F_render_view>> render_view_p_list_;

	public:
		NCPP_FORCE_INLINE const TG_list<TK_valid<F_render_view>>& render_view_p_list() const noexcept { return render_view_p_list_; }



	public:
		F_render_view_system();
		~F_render_view_system();

	public:
		NCPP_DISABLE_COPY(F_render_view_system);

	private:
		void registry(TKPA_valid<F_render_view> render_view_p);
		void deregistry(TKPA_valid<F_render_view> render_view_p);

	};

}



#define NRE_RENDER_VIEW_SYSTEM(...) (nre::F_render_view_system::instance_p())