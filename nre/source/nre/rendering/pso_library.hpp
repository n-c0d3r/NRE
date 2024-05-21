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
		TG_unordered_map<G_string, typename TG_list<TU<A_pipeline_state>>::iterator> pso_iterator_handle_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TU<A_pipeline_state>>& pso_p_list() const noexcept { return pso_p_list_; }



	public:
		F_pso_library();
		~F_pso_library();

	public:
		NCPP_OBJECT(F_pso_library);

	public:
		NCPP_FORCE_INLINE b8 is_owned(const G_string& name) const noexcept {

			auto it = pso_iterator_handle_map_.find(name);

			return (it != pso_iterator_handle_map_.end());
		}
		NCPP_FORCE_INLINE TK_valid<A_pipeline_state> add(const G_string& name, TU<A_pipeline_state>&& pso_p) noexcept {

			NCPP_ASSERT(
				!is_owned(name)
			) << "pipeline_state is already added";

			auto keyed_pso_p = NCPP_FOH_VALID(pso_p);

			pso_p_list_.push_back(
				std::move(pso_p)
			);

			pso_iterator_handle_map_[name] = --(pso_p_list_.end());

			return keyed_pso_p;
		}
		NCPP_FORCE_INLINE void remove(const G_string& name) noexcept {

			NCPP_ASSERT(
				is_owned(name)
			) << "pipeline_state is not added";

			auto it = pso_iterator_handle_map_.find(name);

			pso_p_list_.erase(it->second);

			pso_iterator_handle_map_.erase(it);
		}

	};

}



#define NRE_PSO_LIBRARY(...) (nre::F_pso_library::instance_p())