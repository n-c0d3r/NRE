#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_renderable;



	class NRE_API F_renderable_system {

	public:
		friend class F_renderable;



	private:
		static TK<F_renderable_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_renderable_system> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TK_valid<F_renderable>> renderable_p_list_;

	public:
		NCPP_FORCE_INLINE const TG_list<TK_valid<F_renderable>>& renderable_p_list() const noexcept { return renderable_p_list_; }



	public:
		F_renderable_system();
		~F_renderable_system();

	public:
		NCPP_OBJECT(F_renderable_system);

	private:
		void registry(TKPA_valid<F_renderable> renderable_p);
		void deregistry(TKPA_valid<F_renderable> renderable_p);

	};

}



#define NRE_RENDERABLE_SYSTEM(...) (nre::F_renderable_system::instance_p())