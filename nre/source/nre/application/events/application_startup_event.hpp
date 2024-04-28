#pragma once

#include <nre/prerequisites.hpp>
#include <nre/application/events/application_event.hpp>



namespace nre {

	class F_application;

	class F_application_startup_event : public F_application_event {

	public:
		NCPP_FORCE_INLINE F_application_startup_event(TKPA_valid<F_application> application_p) :
			F_application_event(application_p)
		{}
		~F_application_startup_event(){}

	};

}