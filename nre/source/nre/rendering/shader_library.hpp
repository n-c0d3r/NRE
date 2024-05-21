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
		TG_list<TU<A_shader>> shader_p_list_;
		TG_unordered_map<G_string, typename TG_list<TU<A_shader>>::iterator> shader_iterator_handle_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TU<A_shader>>& shader_p_list() const noexcept { return shader_p_list_; }



	public:
		F_shader_library();
		~F_shader_library();

	public:
		NCPP_OBJECT(F_shader_library);

	public:
		NCPP_FORCE_INLINE b8 is_owned(const G_string& name) const noexcept {

			auto it = shader_iterator_handle_map_.find(name);

			return (it != shader_iterator_handle_map_.end());
		}
		NCPP_FORCE_INLINE TK_valid<A_shader> add(const G_string& name, TU<A_shader>&& shader_p) noexcept {

			NCPP_ASSERT(
				!is_owned(name)
			) << "shader is already added";

			auto keyed_shader_p = NCPP_FOH_VALID(shader_p);

			shader_p_list_.push_back(
				std::move(shader_p)
			);

			shader_iterator_handle_map_[name] = --(shader_p_list_.end());

			return keyed_shader_p;
		}
		NCPP_FORCE_INLINE void remove(const G_string& name) noexcept {

			NCPP_ASSERT(
				is_owned(name)
			) << "shader is not added";

			auto it = shader_iterator_handle_map_.find(name);

			shader_p_list_.erase(it->second);

			shader_iterator_handle_map_.erase(it);
		}

	};

}



#define NRE_SHADER_LIBRARY(...) (nre::F_shader_library::instance_p())