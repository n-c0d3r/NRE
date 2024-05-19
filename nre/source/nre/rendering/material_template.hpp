#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	struct F_material_property_structure
	{
		G_string name;
		u32 offset = 0;
		u32 size = 0;
	};

	// lower 32 bits: local index
	// higher 32 bits: buffer index
	using F_material_property_id = u64;

	struct F_material_property_buffer_structure
	{
		G_string name;
		TG_vector<F_material_property_structure> property_structures;
	};

	using F_material_property_buffer_id = u32;

	struct F_material_structure {

		TG_vector<F_material_property_buffer_structure> buffer_structures;

		u32 size = 0;

	};

	struct F_material_property_info {

		G_string name;
		G_string buffer_name;
		u32 size;

	};



	class NRE_API F_material_template {

	private:
		TG_unordered_map<G_string, F_material_property_info> property_info_map_;
		F_material_structure structure_;

	public:
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_property_info>& property_info_map() const noexcept {

			return property_info_map_;
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
		inline void add_property_info(
			const F_material_property_info& info
		) {
			NCPP_ASSERT(!is_has_property(info.name)) << "\"" << info.name << "\" property already added";

			property_info_map_[info.name] = info;
		}
		NCPP_FORCE_INLINE void add_property_info(
			const G_string& name,
			const G_string& buffer_name,
			u32 size
		) {
			add_property_info({
				name,
				buffer_name,
				size
			});
		}
		template<typename F__>
		void T_add_property_info(
			const G_string& name,
			const G_string& buffer_name
		) {
			add_property_info(
				name,
				buffer_name,
				sizeof(F__)
			);
		}

	public:
		void apply();

	};

}