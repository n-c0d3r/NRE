#include <nre/rendering/material_template.hpp>



namespace nre {

	F_material_property_buffer_builder::F_material_property_buffer_builder(
		TKPA_valid<F_material_template> material_template_p,
		const G_string& name
	) :
		material_template_p_(material_template_p),
		name_(name)
	{
	}

	F_material_property_buffer_builder& F_material_property_buffer_builder::add(
		const G_string& name,
		u32 size,
		u64 type_id
	) {
		material_template_p_->add_property(
			name,
			name_,
			size,
			type_id
		);

		return *this;
	}



	F_material_template::F_material_template() {
	}
	F_material_template::~F_material_template() {
	}

	void F_material_template::clear() {

		structure_ = {};
		property_info_map_.clear();
		pass_info_map_.clear();
		pure_constant_buffer_info_map_.clear();
		external_constant_buffer_info_map_.clear();
		srv_info_map_.clear();
		uav_info_map_.clear();
		last_add_index_ = 0;
	}
	const F_material_structure& F_material_template::build() {

		structure_ = {};
		TG_unordered_map<G_string, u32> name_to_property_buffer_index_map;
		TG_unordered_map<G_string, u32> name_to_pure_constant_buffer_index_map;
		TG_unordered_map<G_string, u32> name_to_external_constant_buffer_index_map;
		TG_unordered_map<G_string, u32> name_to_srv_index_map;
		TG_unordered_map<G_string, u32> name_to_uav_index_map;

		// construct and sort property infos
		TG_vector<F_material_property_info> property_infos;
		property_infos.reserve(property_info_map_.size());

		for (const auto& it : property_info_map_)
		{
			property_infos.push_back(it.second);
		}

		eastl::sort(
			property_infos.begin(),
			property_infos.end(),
			[&](const auto& a, const auto& b) -> b8 {

			  	return a.add_index < b.add_index;
			}
		);

		// push property structures into property buffer structures
		for (auto& property_info : property_infos) {

			F_material_property_buffer_id buffer_id = 0;

			auto property_buffer_structure_it = name_to_property_buffer_index_map.find(property_info.buffer_name);

			if (property_buffer_structure_it == name_to_property_buffer_index_map.end()) {

				buffer_id = structure_.property_buffer_structures.size();
				structure_.property_buffer_structures.push_back({
					.name = property_info.buffer_name,
					.property_structures = {},
					.size = 0
				});

				name_to_property_buffer_index_map[property_info.buffer_name] = buffer_id;
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

		// construct and sort pure constant buffer infos
		TG_vector<F_material_pure_constant_buffer_info> pure_constant_buffer_infos;
		pure_constant_buffer_infos.reserve(pure_constant_buffer_info_map_.size());

		for (const auto& it : pure_constant_buffer_info_map_)
		{
			pure_constant_buffer_infos.push_back(it.second);
		}

		eastl::sort(
			pure_constant_buffer_infos.begin(),
			pure_constant_buffer_infos.end(),
			[&](const auto& a, const auto& b) -> b8 {

		  		return a.add_index < b.add_index;
			}
		);

		// build pure constant buffer index and structure
		u32 next_pure_constant_buffer_index = 0;
		for (auto& pure_constant_buffer_info : pure_constant_buffer_infos) {

			name_to_property_buffer_index_map[pure_constant_buffer_info.name] = next_pure_constant_buffer_index;

			structure_.pure_constant_buffer_structures.push_back({

				.name = pure_constant_buffer_info.name,
				.size = pure_constant_buffer_info.size

			});

			++next_pure_constant_buffer_index;
		}

		// construct and sort external_constant_buffer infos
		TG_vector<F_material_external_constant_buffer_info> external_constant_buffer_infos;
		external_constant_buffer_infos.reserve(external_constant_buffer_info_map_.size());

		for (const auto& it : external_constant_buffer_info_map_)
		{
			external_constant_buffer_infos.push_back(it.second);
		}

		eastl::sort(
			external_constant_buffer_infos.begin(),
			external_constant_buffer_infos.end(),
			[&](const auto& a, const auto& b) -> b8 {

			  return a.add_index < b.add_index;
			}
		);

		// build external_constant_buffer index and structure
		u32 next_external_constant_buffer_index = 0;
		for (auto& external_constant_buffer_info : external_constant_buffer_infos) {

			name_to_external_constant_buffer_index_map[external_constant_buffer_info.name] = next_external_constant_buffer_index;

			structure_.external_constant_buffer_structures.push_back({

				.name = external_constant_buffer_info.name

			});

			++next_external_constant_buffer_index;
		}

		// construct and sort srv infos
		TG_vector<F_material_srv_info> srv_infos;
		srv_infos.reserve(srv_info_map_.size());

		for (const auto& it : srv_info_map_)
		{
			srv_infos.push_back(it.second);
		}

		eastl::sort(
			srv_infos.begin(),
			srv_infos.end(),
			[&](const auto& a, const auto& b) -> b8 {

			  return a.add_index < b.add_index;
			}
		);

		// build srv index and structure
		u32 next_srv_index = 0;
		for (auto& srv_info : srv_infos) {

			name_to_srv_index_map[srv_info.name] = next_srv_index;

			structure_.srv_structures.push_back({

				.name = srv_info.name

			});

			++next_srv_index;
		}

		// construct and sort uav infos
		TG_vector<F_material_uav_info> uav_infos;
		uav_infos.reserve(uav_info_map_.size());

		for (const auto& it : uav_info_map_)
		{
			uav_infos.push_back(it.second);
		}

		eastl::sort(
			uav_infos.begin(),
			uav_infos.end(),
			[&](const auto& a, const auto& b) -> b8 {

			  	return a.add_index < b.add_index;
			}
		);

		// build uav index and structure
		u32 next_uav_index = 0;
		for (auto& uav_info : uav_infos) {

			name_to_uav_index_map[uav_info.name] = next_uav_index;

			structure_.uav_structures.push_back({

				.name = uav_info.name

			});

			++next_uav_index;
		}

		// linking maps
		TG_unordered_map<G_string, TG_vector<F_material_external_constant_buffer_linking>> external_constant_buffer_linkings_map;
		TG_unordered_map<G_string, TG_vector<F_material_srv_linking>> srv_linkings_map;
		TG_unordered_map<G_string, TG_vector<F_material_uav_linking>> uav_linkings_map;

		// setup pass structures
		{
			u32 pass_index = 0;

			structure_.pass_structures.reserve(pass_info_map_.size());
			for(auto& it : pass_info_map_) {

				auto& pass_info = it.second;

				TG_vector<u32> property_buffer_structure_indices;
				property_buffer_structure_indices.reserve(pass_info.property_buffer_names.size());

				for(auto& property_buffer_name : pass_info.property_buffer_names) {

					property_buffer_structure_indices.push_back(
						name_to_property_buffer_index_map[property_buffer_name]
					);
				}

				TG_vector<u32> pure_constant_buffer_structure_indices;
				pure_constant_buffer_structure_indices.reserve(pass_info.property_buffer_names.size());

				for(auto& pure_constant_buffer_name : pass_info.pure_constant_buffer_names) {

					pure_constant_buffer_structure_indices.push_back(
						name_to_property_buffer_index_map[pure_constant_buffer_name]
					);
				}

				TG_vector<u32> external_constant_buffer_structure_indices;
				external_constant_buffer_structure_indices.reserve(pass_info.external_constant_buffer_names.size());

				u32 external_constant_buffer_local_index = 0;
				for(auto& external_constant_buffer_name : pass_info.external_constant_buffer_names) {

					u32 external_constant_buffer_index = name_to_external_constant_buffer_index_map[external_constant_buffer_name];

					external_constant_buffer_structure_indices.push_back(external_constant_buffer_index);

					if(external_constant_buffer_linkings_map.find(external_constant_buffer_name) == external_constant_buffer_linkings_map.end()) {

						external_constant_buffer_linkings_map[external_constant_buffer_name] = {};
					}

					external_constant_buffer_linkings_map[external_constant_buffer_name].push_back({
						.index = external_constant_buffer_index,
						.local_index = external_constant_buffer_local_index,
						.pass_index = pass_index
					});

					++external_constant_buffer_local_index;
				}

				TG_vector<u32> srv_structure_indices;
				srv_structure_indices.reserve(pass_info.srv_names.size());

				u32 srv_local_index = 0;
				for(auto& srv_name : pass_info.srv_names) {

					u32 srv_index = name_to_srv_index_map[srv_name];

					srv_structure_indices.push_back(srv_index);

					if(srv_linkings_map.find(srv_name) == srv_linkings_map.end()) {

						srv_linkings_map[srv_name] = {};
					}

					srv_linkings_map[srv_name].push_back({
						.index = srv_index,
						.local_index = srv_local_index,
						.pass_index = pass_index
					});

					++srv_local_index;
				}

				TG_vector<u32> uav_structure_indices;
				uav_structure_indices.reserve(pass_info.uav_names.size());

				u32 uav_local_index = 0;
				for(auto& uav_name : pass_info.uav_names) {

					u32 uav_index = name_to_uav_index_map[uav_name];

					uav_structure_indices.push_back(uav_index);

					if(uav_linkings_map.find(uav_name) == uav_linkings_map.end()) {

						uav_linkings_map[uav_name] = {};
					}

					uav_linkings_map[uav_name].push_back({
						.index = uav_index,
						.local_index = uav_local_index,
						.pass_index = pass_index
					});

					++uav_local_index;
				}

				structure_.pass_structures.push_back({
					.name = pass_info.name,
					.pso_p = pass_info.pso_p,
					.property_buffer_structure_indices = std::move(property_buffer_structure_indices),
					.pure_constant_buffer_structure_indices = std::move(pure_constant_buffer_structure_indices),
					.external_constant_buffer_structure_indices = std::move(external_constant_buffer_structure_indices),
					.srv_structure_indices = std::move(srv_structure_indices),
					.uav_structure_indices = std::move(uav_structure_indices)
				});

				++pass_index;
			}
		}

		// apply linkings maps
		{
			for(const auto& linkings_it : external_constant_buffer_linkings_map) {

				structure_.external_constant_buffer_linkings.insert(
					structure_.external_constant_buffer_linkings.end(),
					linkings_it.second.begin(),
					linkings_it.second.end()
				);
			}
			for(const auto& linkings_it : srv_linkings_map) {

				structure_.srv_linkings.insert(
					structure_.srv_linkings.end(),
					linkings_it.second.begin(),
					linkings_it.second.end()
				);
			}
			for(const auto& linkings_it : uav_linkings_map) {

				structure_.uav_linkings.insert(
					structure_.uav_linkings.end(),
					linkings_it.second.begin(),
					linkings_it.second.end()
				);
			}
		}

		return structure_;
	}

}