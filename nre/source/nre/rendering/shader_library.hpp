#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class NRE_API F_shader_library {

	private:
		static TK<F_shader_library> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_shader_library> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TU<A_pipeline_state>> shader_p_list_;
		TG_unordered_map<sz, typename TG_list<TU<A_pipeline_state>>::iterator> shader_p_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TU<A_pipeline_state>>& shader_p_list() const noexcept { return shader_p_list_; }



	public:
		F_shader_library();
		~F_shader_library();

	public:
		NCPP_OBJECT(F_shader_library);

	public:
		NCPP_FORCE_INLINE b8 is_owned(TKPA_valid<A_pipeline_state> shader_p) const noexcept {

			auto it = shader_p_map_.find(sz(shader_p.object_p()));

			return (it != shader_p_map_.end());
		}
		NCPP_FORCE_INLINE TK_valid<A_pipeline_state> add(TU<A_pipeline_state>&& shader_p) noexcept {

			NCPP_ASSERT(
				!is_owned(
					NCPP_FOH_VALID(shader_p)
				)
			) << "shader is already added";

			auto keyed_shader_p = NCPP_FOH_VALID(shader_p);

			shader_p_list_.push_back(
				std::move(shader_p)
			);

			shader_p_map_[sz(shader_p.object_p())] = --(shader_p_list_.end());

			return keyed_shader_p;
		}
		NCPP_FORCE_INLINE void remove(TKPA_valid<A_pipeline_state> shader_p) noexcept {

			NCPP_ASSERT(
				is_owned(
					NCPP_FOH_VALID(shader_p)
				)
			) << "shader is not added";

			auto it = shader_p_map_.find(
				sz(shader_p.object_p())
			);

			shader_p_list_.erase(it->second);

			shader_p_map_.erase(it);
		}

	};

}



#define NRE_SHADER_LIBRARY(...) (nre::F_shader_library::instance_p())