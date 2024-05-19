#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	struct F_material_property_structure
	{
		G_string name;
		u32 offset = 0;
		u32 size = 0;
		u64 type_id = 0;
	};

	// lower 32 bits: local index
	// higher 32 bits: buffer index
	using F_material_property_id = u64;

	struct F_material_property_buffer_structure
	{
		G_string name;
		TG_vector<F_material_property_structure> property_structures;
		u32 size = 0;
	};

	using F_material_property_buffer_id = u32;

	struct F_material_structure {

		TG_vector<F_material_property_buffer_structure> property_buffer_structures;

	};

	struct F_material_property_info {

		G_string name;
		G_string buffer_name;
		u32 size;
		u32 add_index = 0;
		u64 type_id = 0;

	};

	using F_material_pso_id = u32;



	constexpr F_material_property_buffer_id material_property_id_get_buffer_index(F_material_property_id id) {

		return (
			(id & 0xFFFFFFFF00000000)
			>> 32
		);
	}
	constexpr u32 material_property_id_get_local_index(F_material_property_id id) {

		return (id & 0x00000000FFFFFFFF);
	}
	constexpr F_material_property_id make_material_property_id(
		u32 local_index,
		F_material_property_buffer_id buffer_id
	) {

		return (
			F_material_property_id(local_index)
			| (F_material_property_id(buffer_id) << 32)
		);
	}



	class NRE_API F_material_template {

	private:
		TG_unordered_map<G_string, F_material_property_info> property_info_map_;
		F_material_structure structure_;
		u32 last_add_index_ = 0;

		TG_unordered_map<G_string, F_material_pso_id> pso_id_map_;
		TG_vector<TK_valid<A_pipeline_state>> pso_p_vector_;

	public:
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_property_info>& property_info_map() const noexcept {

			return property_info_map_;
		}
		NCPP_FORCE_INLINE const TG_vector<TK_valid<A_pipeline_state>>& pso_p_vector() const noexcept {

			return pso_p_vector_;
		}
		NCPP_FORCE_INLINE const F_material_structure& structure() const noexcept {

			return structure_;
		}



	public:
		F_material_template();
		virtual ~F_material_template();

	public:
		NCPP_OBJECT(F_material_template);

	public:
		inline b8 is_has_property(const G_string& name) const noexcept {

			return (property_info_map_.find(name) != property_info_map_.end());
		}
		inline const F_material_property_info& property_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property(name)) << "\"" << name << "\" property is not added";

			return property_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_property_info(
			const G_string& name,
			const G_string& buffer_name,
			u32 size,
			u64 type_id
		) {
			NCPP_ASSERT(!is_has_property(name)) << "\"" << name << "\" property already added";

			property_info_map_[name] = {
				name,
				buffer_name,
				size,
				last_add_index_,
				type_id
			};
			++last_add_index_;
		}
		template<typename F__>
		void T_add_property_info(
			const G_string& name,
			const G_string& buffer_name
		) {
			add_property_info(
				name,
				buffer_name,
				sizeof(F__),
				T_type_hash_code<F__>
			);
		}

	public:
		void clear();
		const F_material_structure& build();

	};

}