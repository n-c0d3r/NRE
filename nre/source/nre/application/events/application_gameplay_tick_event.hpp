#pragma once

#include <nre/prerequisites.hpp>
#include <nre/application/events/application_tick_event.hpp>



namespace nre {

	class F_application;

	class F_application_gameplay_tick_event : public F_application_tick_event {

	public:
		NCPP_FORCE_INLINE F_application_gameplay_tick_event(TKPA_valid<F_application> application_p) :
			F_application_tick_event(application_p)
		{}
		~F_application_gameplay_tick_event(){}

	};

}