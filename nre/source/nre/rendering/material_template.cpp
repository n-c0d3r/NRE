#include <nre/rendering/material_template.hpp>



namespace nre {

	F_material_template::F_material_template() {
	}
	F_material_template::~F_material_template() {
	}

	void F_material_template::clear() {

		structure_ = {};
		property_info_map_.clear();
		last_add_index_ = 0;
	}
	const F_material_structure& F_material_template::build() {

		structure_ = {};
		TG_unordered_map<G_string, F_material_property_buffer_id> name_to_property_buffer_id_map;

		// push property structures into property buffer structures
		for (const auto& it : property_info_map_)
		{
			const F_material_property_info& property_info = it.second;

			F_material_property_buffer_id buffer_id = 0;

			auto property_buffer_structure_it = name_to_property_buffer_id_map.find(property_info.buffer_name);

			if (property_buffer_structure_it == name_to_property_buffer_id_map.end()) {

				buffer_id = structure_.property_buffer_structures.size();
				structure_.property_buffer_structures.push_back({
					.name = property_info.buffer_name,
					.property_structures = {},
					.size = 0
				});

				name_to_property_buffer_id_map[property_info.buffer_name] = buffer_id;
			}

			structure_.property_buffer_structures[buffer_id].property_structures.push_back({
				.name = property_info.name,
				.offset = 0,
				.size = property_info.size,
				.type_id = property_info.type_id
			});
		}

		// build property buffer structure
		for(auto& property_buffer_structure : structure_.property_buffer_structures) {

			auto& property_structures = property_buffer_structure.property_structures;

			// sort property structures
			auto comparator = [&](const auto& a, const auto& b) -> b8 {

				return property_info_map_[a.name].add_index < property_info_map_[b.name].add_index;
			};
			eastl::sort(property_structures.begin(), property_structures.end(), comparator);

			// compute property offsets and buffer size
			u32 offset = 0;
			constexpr u8 property_buffer_alignment = sizeof(f32) * 4;
			for(auto& property_structure : property_structures) {

				u32 property_size = property_structure.size;
				property_size = eastl::max<u32>(property_size, sizeof(f32));
				property_size = pow(
					2,
					ceil(
						log(f32(property_size))
						/ log(2.0f)
					)
				);

				u32 last_mod_alignment = offset % property_buffer_alignment;
				if(
					(property_buffer_alignment - (last_mod_alignment + property_size)) > property_buffer_alignment
				) {
					offset = align_size(offset, eastl::max<u32>(property_buffer_alignment, property_size));
				}

				property_structure.offset = offset;
				offset += property_size;
			}
			property_buffer_structure.size = align_size(offset, property_buffer_alignment);
		}

		return structure_;
	}

}