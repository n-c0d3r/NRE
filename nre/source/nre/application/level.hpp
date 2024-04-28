#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_level;
	class F_actor;



	class F_level {

	public:
		friend class F_actor;



	private:
		TG_list<TK_valid<F_actor>> actor_p_list_;

	public:
		NCPP_FORCE_INLINE const TG_list<TK_valid<F_actor>>& actor_p_list() const noexcept { return actor_p_list_; }



	public:
		F_level();
		~F_level();

	private:
		typename TG_list<TK_valid<F_actor>>::iterator push_actor(TK_valid<F_actor> actor_p);
		void pop_actor(TK_valid<F_actor> actor_p);

	};

}