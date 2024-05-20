#include <nre/rendering/material.hpp>
#include <nre/rendering/material_template.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_material::F_material(TSPA<F_material_template> template_p) :
		template_p_(template_p)
	{
		const auto& structure = template_p->structure();

		// setup cache memory
		u8* cache_p = 0;
		{
			sz cache_size = 0;

			// properties and property buffers
			{
				cache_size += structure.property_buffer_structures.size() * sizeof(F_property_buffer_wrapper); // property buffer wrapper
				for (const auto& property_buffer_structure : structure.property_buffer_structures)
				{
					cache_size += property_buffer_structure.size; // data size
					cache_size += property_buffer_structure.property_structures.size() * sizeof(u32); // property offsets
				}
				for (const auto& property_buffer_structure : structure.property_buffer_structures)
				{
					cache_size += property_buffer_structure.size; // data size
					cache_size += property_buffer_structure.property_structures.size() * sizeof(u32); // property offsets
				}
			}

			// pure constant buffers
			{
				cache_size += structure.pure_constant_buffer_structures.size() * sizeof(F_pure_constant_buffer_wrapper); // pure constant buffer wrapper
				for (const auto& pure_constant_buffer_structure : structure.pure_constant_buffer_structures)
				{
					cache_size += pure_constant_buffer_structure.size; // data size
				}
			}

			// passes
			{
				cache_size += structure.pass_structures.size() * sizeof(F_pass_wrapper); // pure constant buffer wrapper
				for (const auto& pass_structure : structure.pass_structures)
				{
					cache_size += pass_structure.property_buffer_structure_indices.size() * sizeof(K_buffer_handle); // property buffers
					cache_size += pass_structure.pure_constant_buffer_structure_indices.size() * sizeof(K_buffer_handle); // pure constant buffers
					cache_size += pass_structure.external_constant_buffer_structure_indices.size() * sizeof(K_buffer_handle); // external constant buffers

					cache_size += pass_structure.srv_structure_indices.size() * sizeof(K_buffer_handle); // srv
					cache_size += pass_structure.uav_structure_indices.size() * sizeof(K_buffer_handle); // uav
				}
			}

			// external constant buffers
			{
				cache_size += structure.external_constant_buffer_linkings.size() * sizeof(K_buffer_handle*); // external constant buffer handle pointer
			}

			// srv
			{
				cache_size += structure.srv_linkings.size() * sizeof(K_srv_handle*); // external constant buffer handle pointer
			}

			// uav
			{
				cache_size += structure.uav_linkings.size() * sizeof(K_uav_handle*); // external constant buffer handle pointer
			}

			cached_memory_.resize(cache_size);
			cache_p = (u8*)(cached_memory_.data());
		}

		// properties and property buffers
		{
			const auto& property_buffer_structures = structure.property_buffer_structures;

			u32 last_property_buffer_wrapper_index = 0;

			property_buffer_wrapper_span_ = TG_span<F_property_buffer_wrapper>(
				(F_property_buffer_wrapper*)cache_p,
				property_buffer_structures.size()
			);
			cache_p += property_buffer_structures.size() * sizeof(F_property_buffer_wrapper);

			for (
				F_material_property_buffer_id property_buffer_id = 0;
			 	property_buffer_id < property_buffer_structures.size();
			 	++property_buffer_id
		 	)
			{
				const auto& property_buffer_structure = property_buffer_structures[property_buffer_id];

				property_buffer_name_to_property_buffer_id_map_[property_buffer_structure.name] = property_buffer_id;

				u8* begin_data_p = (u8*)cache_p;
				cache_p += property_buffer_structure.size;
				u32* begin_property_offset_p = (u32*)cache_p;
				cache_p += property_buffer_structure.property_structures.size() * sizeof(u32);

				u32 local_property_index = 0;
				for (const auto& property_structure : property_buffer_structure.property_structures)
				{
					property_name_to_property_id_map_[property_structure.name] = make_material_property_id(
						local_property_index,
						property_buffer_id
					);

					begin_property_offset_p[local_property_index] = property_structure.offset;// + property_structure.size;
					++local_property_index;
				}

				U_buffer_handle buffer_p = H_buffer::create(
					NRE_RENDER_DEVICE(),
					{},
					property_buffer_structure.size,
					1,
					E_resource_bind_flag::CBV,
					E_resource_heap_type::GREAD_CWRITE
				);

				// to make sure the data is valid
				new(&(property_buffer_wrapper_span_[last_property_buffer_wrapper_index])) F_property_buffer_wrapper();

				property_buffer_wrapper_span_[last_property_buffer_wrapper_index] = {
					.buffer_p = std::move(buffer_p),
					.data = TG_span<u8>(begin_data_p, property_buffer_structure.size),
					.property_offset_span = TG_span<u32>(
						begin_property_offset_p,
						property_buffer_structure.property_structures.size()
					)
				};
				++last_property_buffer_wrapper_index;
			}
		}

		// pure constant buffers
		{
			const auto& pure_constant_buffer_structures = structure.pure_constant_buffer_structures;

			pure_constant_buffer_wrapper_span_ = TG_span<F_pure_constant_buffer_wrapper>(
				(F_pure_constant_buffer_wrapper*)cache_p,
				pure_constant_buffer_structures.size()
			);
			cache_p += pure_constant_buffer_structures.size() * sizeof(F_pure_constant_buffer_wrapper);

			u32 pure_constant_buffer_index = 0;
			for(auto& pure_constant_buffer_wrapper : pure_constant_buffer_wrapper_span_) {

				const auto& pure_constant_buffer_structure = pure_constant_buffer_structures[pure_constant_buffer_index];

				TG_span<u8> data(
					cache_p,
					pure_constant_buffer_structure.size
				);
				cache_p += pure_constant_buffer_structure.size;

				U_buffer_handle buffer_p = H_buffer::create(
					NRE_RENDER_DEVICE(),
					{},
					pure_constant_buffer_structure.size,
					1,
					E_resource_bind_flag::CBV,
					E_resource_heap_type::GREAD_CWRITE
				);

				new(&pure_constant_buffer_wrapper) F_pure_constant_buffer_wrapper();

				pure_constant_buffer_wrapper = {

					.buffer_p = std::move(buffer_p),
					.data = data

				};
				++pure_constant_buffer_index;
			}
		}

		// pass
		{
			pass_wrapper_span_ = TG_span<F_pass_wrapper>(
				(F_pass_wrapper*)cache_p,
				structure.pass_structures.size()
			);
			cache_p += structure.pass_structures.size() * sizeof(F_pass_wrapper);

			u32 pass_index = 0;
			for(auto& pass : pass_wrapper_span_) {

				const auto& pass_structure = structure.pass_structures[pass_index];

				// constant buffer handles
				pass.constant_buffer_p_span = TG_span<K_buffer_handle>(
					(K_buffer_handle*)cache_p,
					pass_structure.property_buffer_structure_indices.size()
					+ pass_structure.pure_constant_buffer_structure_indices.size()
					+ pass_structure.external_constant_buffer_structure_indices.size()
				);
				cache_p += (
					pass_structure.property_buffer_structure_indices.size()
					+ pass_structure.pure_constant_buffer_structure_indices.size()
					+ pass_structure.external_constant_buffer_structure_indices.size()
				) * sizeof(K_buffer_handle);

				u32 constant_buffer_index = pass_structure.external_constant_buffer_structure_indices.size();

				for(auto property_buffer_structure_index : pass_structure.property_buffer_structure_indices) {

					const auto& property_buffer_structure = structure.property_buffer_structures[property_buffer_structure_index];

					const auto& property_buffer_wrapper = property_buffer_wrapper_span_[property_buffer_structure_index];

					pass.constant_buffer_p_span[constant_buffer_index] = property_buffer_wrapper.buffer_p;
					++constant_buffer_index;
				}

				for(auto pure_constant_buffer_structure_index : pass_structure.pure_constant_buffer_structure_indices) {

					const auto& pure_constant_buffer_structure = structure.pure_constant_buffer_structures[pure_constant_buffer_structure_index];

					const auto& pure_constant_buffer_wrapper = pure_constant_buffer_wrapper_span_[pure_constant_buffer_structure_index];

					pass.constant_buffer_p_span[constant_buffer_index] = pure_constant_buffer_wrapper.buffer_p;
					++constant_buffer_index;
				}

				// srv handles
				pass.srv_p_span = TG_span<K_srv_handle>(
					(K_srv_handle*)cache_p,
					pass_structure.srv_structure_indices.size()
				);
				cache_p += pass_structure.srv_structure_indices.size() * sizeof(K_srv_handle);

				// uav handles
				pass.uav_p_span = TG_span<K_uav_handle>(
					(K_uav_handle*)cache_p,
					pass_structure.uav_structure_indices.size()
				);
				cache_p += pass_structure.uav_structure_indices.size() * sizeof(K_uav_handle);

				++pass_index;
			}
		}

		// linkings
		{
			external_constant_buffer_pp_span_ = TG_span<K_buffer_handle*>(
				(K_buffer_handle**)cache_p,
				structure.external_constant_buffer_linkings.size()
			);
			cache_p += structure.external_constant_buffer_linkings.size() * sizeof(K_buffer_handle*);
			for(const auto& linking : structure.external_constant_buffer_linkings) {

				external_constant_buffer_pp_span_[linking.index] = &(
					pass_wrapper_span_[linking.pass_index].constant_buffer_p_span[
						linking.local_index
					]
				);
			}

			srv_pp_span_ = TG_span<K_srv_handle*>(
				(K_srv_handle**)cache_p,
				structure.srv_linkings.size()
			);
			cache_p += structure.srv_linkings.size() * sizeof(K_srv_handle*);
			for(const auto& linking : structure.srv_linkings) {

				srv_pp_span_[linking.index] = &(
					pass_wrapper_span_[linking.pass_index].srv_p_span[
						linking.local_index
					]
				);
			}

			uav_pp_span_ = TG_span<K_uav_handle*>(
				(K_uav_handle**)cache_p,
				structure.uav_linkings.size()
			);
			cache_p += structure.uav_linkings.size() * sizeof(K_uav_handle*);
			for(const auto& linking : structure.uav_linkings) {

				uav_pp_span_[linking.index] = &(
					pass_wrapper_span_[linking.pass_index].uav_p_span[
						linking.local_index
					]
				);
			}
		}
	}
	F_material::~F_material() {
	}

	void F_material::set_property_value(const G_string& name, void* data_p, u32 size) {

		set_property_value(property_id(name), data_p, size);
	}
	void F_material::set_property_value(F_material_property_id id, void* data_p, u32 size) {

		NCPP_ASSERT(size > property_structure(id).size)
		<< "invalid size, the correct size must be greater than or equal to"
		<< T_cout_value(property_structure(id).size);

		const auto& property_buffer_wrapper = property_buffer_wrapper_span_[material_property_id_get_buffer_index(id)];

		memcpy(
			(void*)&(
				property_buffer_wrapper.data[
					property_buffer_wrapper.property_offset_span[material_property_id_get_local_index(id)]
				]
			),
			data_p,
			size
		);
	}
	void F_material::property_value(const G_string& name, void* data_p, u32 size) const {

		property_value(property_id(name), data_p, size);
	}
	void F_material::property_value(F_material_property_id id, void* data_p, u32 size) const {

		NCPP_ASSERT(size > property_structure(id).size)
			<< "invalid size, the correct size must be greater than or equal to"
			<< T_cout_value(property_structure(id).size);

		const auto& property_buffer_wrapper = property_buffer_wrapper_span_[material_property_id_get_buffer_index(id)];

		memcpy(
			data_p,
			(void*)&(
				property_buffer_wrapper.data[
					property_buffer_wrapper.property_offset_span[material_property_id_get_local_index(id)]
				]
			),
			size
		);
	}

	void* F_material::pure_constant_buffer_data_p(const G_string& name) const {

		return pure_constant_buffer_data_p(
			pure_constant_buffer_id(name)
		);
	}
	void* F_material::pure_constant_buffer_data_p(F_material_pure_constant_buffer_id id) const {

		return (void*)(
			pure_constant_buffer_wrapper_span_[
				id
			].data.data()
		);
	}
	const F_material_pure_constant_buffer_structure& F_material::pure_constant_buffer_structure(const G_string& name) const {

		return pure_constant_buffer_structure(
			pure_constant_buffer_id(name)
		);
	}
	const F_material_pure_constant_buffer_structure& F_material::pure_constant_buffer_structure(F_material_pure_constant_buffer_id id) const {

		return structure_.pure_constant_buffer_structures[id];
	}

	void F_material::set_external_constant_buffer_p(const G_string& name, KPA_valid_buffer_handle external_constant_buffer_p) {

		set_external_constant_buffer_p(external_constant_buffer_id(name), external_constant_buffer_p);
	}
	void F_material::set_external_constant_buffer_p(F_material_external_constant_buffer_id id, KPA_valid_buffer_handle external_constant_buffer_p) {

		u32 begin_id = material_external_constant_buffer_id_get_begin_index(id);
		u32 end_id = material_external_constant_buffer_id_get_end_index(id);

		for(u32 i = begin_id; i < end_id; ++begin_id) {

			*(external_constant_buffer_pp_span_[i]) = external_constant_buffer_p.no_requirements();
		}
	}
	K_valid_buffer_handle F_material::external_constant_buffer_p(const G_string& name) const {

		return external_constant_buffer_p(external_constant_buffer_id(name));
	}
	K_valid_buffer_handle F_material::external_constant_buffer_p(F_material_external_constant_buffer_id id) const {

		return NCPP_FOH_VALID(
			*(
				external_constant_buffer_pp_span_[
					material_external_constant_buffer_id_get_begin_index(id)
				]
			)
		);
	}
	const F_material_external_constant_buffer_structure& F_material::external_constant_buffer_structure(const G_string& name) const {

		return external_constant_buffer_structure(
			external_constant_buffer_id(name)
		);
	}
	const F_material_external_constant_buffer_structure& F_material::external_constant_buffer_structure(F_material_pure_constant_buffer_id id) const {

		return structure_.external_constant_buffer_structures[id];
	}

	void F_material::set_srv_p(const G_string& name, KPA_valid_srv_handle srv_p) {

		set_srv_p(srv_id(name), srv_p);
	}
	void F_material::set_srv_p(F_material_srv_id id, KPA_valid_srv_handle srv_p) {

		u32 begin_id = material_srv_id_get_begin_index(id);
		u32 end_id = material_srv_id_get_end_index(id);

		for(u32 i = begin_id; i < end_id; ++begin_id) {

			*(srv_pp_span_[i]) = srv_p.no_requirements();
		}
	}
	K_valid_srv_handle F_material::srv_p(const G_string& name) const {

		return srv_p(srv_id(name));
	}
	K_valid_srv_handle F_material::srv_p(F_material_srv_id id) const {

		return NCPP_FOH_VALID(
			*(
				srv_pp_span_[
					material_srv_id_get_begin_index(id)
				]
			)
		);
	}
	const F_material_srv_structure& F_material::srv_structure(const G_string& name) const {

		return srv_structure(
			srv_id(name)
		);
	}
	const F_material_srv_structure& F_material::srv_structure(F_material_pure_constant_buffer_id id) const {

		return structure_.srv_structures[id];
	}

	void F_material::set_uav_p(const G_string& name, KPA_valid_uav_handle uav_p) {

		set_uav_p(uav_id(name), uav_p);
	}
	void F_material::set_uav_p(F_material_uav_id id, KPA_valid_uav_handle uav_p) {

		u32 begin_id = material_uav_id_get_begin_index(id);
		u32 end_id = material_uav_id_get_end_index(id);

		for(u32 i = begin_id; i < end_id; ++begin_id) {

			*(uav_pp_span_[i]) = uav_p.no_requirements();
		}
	}
	K_valid_uav_handle F_material::uav_p(const G_string& name) const {

		return uav_p(uav_id(name));
	}
	K_valid_uav_handle F_material::uav_p(F_material_uav_id id) const {

		return NCPP_FOH_VALID(
			*(
				uav_pp_span_[
					material_uav_id_get_begin_index(id)
				]
			)
		);
	}
	const F_material_uav_structure& F_material::uav_structure(const G_string& name) const {

		return uav_structure(
			uav_id(name)
		);
	}
	const F_material_uav_structure& F_material::uav_structure(F_material_pure_constant_buffer_id id) const {

		return structure_.uav_structures[id];
	}

	void F_material::bind_pass(
		const G_string& pass_name,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		bind_pass(
			pass_id(pass_name),
			render_command_list_p
		);
	}
	void F_material::bind_pass(
		F_material_pass_id id,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		const auto& pass_wrapper = pass_wrapper_span_[id];

		TK_valid<A_pipeline_state> pso_p = NCPP_FOH_VALID(pass_wrapper.pso_p);

		switch (pso_p->desc().type)
		{
		case E_pipeline_state_type::NONE:
			NCPP_ASSERT(false) << "invalid pso type \"NONE\"";
			break;
		case E_pipeline_state_type::GRAPHICS:
			render_command_list_p->bind_graphics_pipeline_state({ pso_p });
			break;
		case E_pipeline_state_type::COMPUTE:
			render_command_list_p->bind_compute_pipeline_state({ pso_p });
			break;
		}
	}
	void F_material::bind_graphics_pass(
		const G_string& pass_name,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		bind_graphics_pass(
			pass_id(pass_name),
			render_command_list_p
		);
	}
	void F_material::bind_graphics_pass(
		F_material_pass_id id,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		const auto& pass_wrapper = pass_wrapper_span_[id];

		TK_valid<A_pipeline_state> pso_p = NCPP_FOH_VALID(pass_wrapper.pso_p);

		render_command_list_p->bind_graphics_pipeline_state({ pso_p });
	}
	void F_material::bind_compute_pass(
		const G_string& pass_name,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		bind_compute_pass(
			pass_id(pass_name),
			render_command_list_p
		);
	}
	void F_material::bind_compute_pass(
		F_material_pass_id id,
		KPA_valid_render_command_list_handle render_command_list_p
	) {
		const auto& pass_wrapper = pass_wrapper_span_[id];

		TK_valid<A_pipeline_state> pso_p = NCPP_FOH_VALID(pass_wrapper.pso_p);

		render_command_list_p->bind_compute_pipeline_state({ pso_p });
	}
	const F_material_pass_structure& F_material::pass_structure(const G_string& name) const noexcept {

		return pass_structure(
			pass_id(name)
		);
	}
	const F_material_pass_structure& F_material::pass_structure(F_material_pass_id id) const noexcept {

		return structure_.pass_structures[id];
	}

}