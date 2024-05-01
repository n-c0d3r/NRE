#pragma once

#include <nre/application/actor.hpp>



namespace nre {

	class F_camera {

	private:
		G_string name_;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }



	public:
		F_camera(const G_string& name = "No name");
		~F_camera();

	};

	class F_camera_actor : public F_actor {

	private:
		TU<F_camera> camera_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<F_camera> camera_p() const noexcept { return NCPP_FOREF_VALID(camera_p_); }



	public:
		F_camera_actor(TK_valid<F_level> level_p, const G_string& name = "No name");
		~F_camera_actor();

	};

}