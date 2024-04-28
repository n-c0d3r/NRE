#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_level;
	class F_actor;



	class F_actor {

	public:
		friend class F_level;



	private:
		TK_valid<F_level> level_p_;
		G_string name_;
		typename TG_list<TK_valid<F_actor>>::iterator handle_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_level> level_p() const noexcept { return level_p_; }
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }



	public:
		F_actor(TK_valid<F_level> level_p, const G_string& name = "No name");
		~F_actor();

	};

}