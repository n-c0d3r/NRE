#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_application;

	class F_application_event : public F_event {

	private:
		TK_valid<F_application> application_p_;

	public:
		inline TKPA_valid<F_application> application_p() const noexcept { return application_p_; }



	public:
		NCPP_FORCE_INLINE F_application_event(TKPA_valid<F_application> application_p) :
			application_p_(application_p)
		{}
		~F_application_event(){}

	};

}