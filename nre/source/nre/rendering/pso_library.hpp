#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class NRE_API F_pso_library {

	private:
		static TK<F_pso_library> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_pso_library> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TU<A_pipeline_state>> pso_p_list_;
		TG_unordered_map<sz, typename TG_list<TU<A_pipeline_state>>::iterator> pso_p_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TU<A_pipeline_state>>& pso_p_list() const noexcept { return pso_p_list_; }



	public:
		F_pso_library();
		~F_pso_library();

	public:
		NCPP_OBJECT(F_pso_library);

	public:
		NCPP_FORCE_INLINE b8 is_owned(TKPA_valid<A_pipeline_state> pso_p) const noexcept {

			auto it = pso_p_map_.find(sz(pso_p.object_p()));

			return (it != pso_p_map_.end());
		}
		NCPP_FORCE_INLINE TK_valid<A_pipeline_state> add(TU<A_pipeline_state>&& pso_p) noexcept {

			NCPP_ASSERT(
				!is_owned(
					NCPP_FOH_VALID(pso_p)
				)
			) << "pso is already added";

			auto keyed_pso_p = NCPP_FOH_VALID(pso_p);

			pso_p_list_.push_back(
				std::move(pso_p)
			);

			pso_p_map_[sz(pso_p.object_p())] = --(pso_p_list_.end());

			return keyed_pso_p;
		}
		NCPP_FORCE_INLINE void remove(TKPA_valid<A_pipeline_state> pso_p) noexcept {

			NCPP_ASSERT(
				is_owned(
					NCPP_FOH_VALID(pso_p)
				)
			) << "pso is not added";

			auto it = pso_p_map_.find(
				sz(pso_p.object_p())
			);

			pso_p_list_.erase(it->second);

			pso_p_map_.erase(it);
		}

	};

}



#define NRE_PSO_LIBRARY(...) (nre::F_pso_library::instance_p())