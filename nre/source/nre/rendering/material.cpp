#include <nre/rendering/material.hpp>
#include <nre/rendering/material_template.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	F_material::F_material(TSPA<F_material_template> template_p) :
		template_p_(template_p)
	{
		const auto& structure = template_p->structure();

		const auto& property_buffer_structures = structure.property_buffer_structures;
		property_buffer_wrappers_.reserve(property_buffer_structures.size());

		for(F_material_property_buffer_id property_buffer_id = 0; property_buffer_id < property_buffer_structures.size(); ++property_buffer_id)
		{
			const auto& property_buffer_structure = property_buffer_structures[property_buffer_id];

			property_buffer_name_to_property_buffer_id_map_[property_buffer_structure.name] = property_buffer_id;

			property_buffer_memory_.resize(
				property_buffer_memory_.size()
				+ property_buffer_structure.size
				+ property_buffer_structure.property_structures.size() * sizeof(u32)
			);
			u32* begin_property_offset_p = (u32*)(
				((u8*)property_buffer_memory_.end())
				- (property_buffer_structure.property_structures.size() * sizeof(u32))
			);
			u8* begin_data_p = (
				((u8*)begin_property_offset_p)
				- property_buffer_structure.size
			);

			u32 local_property_index = 0;
			for(const auto& property_structure : property_buffer_structure.property_structures) {

				property_name_to_property_id_map_[property_structure.name] = make_material_property_id(
					local_property_index,
					property_buffer_id
				);

				begin_property_offset_p[local_property_index] = property_structure.offset + property_structure.size;
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

			property_buffer_wrappers_.push_back({
				.buffer_p = std::move(buffer_p),
				.data = TG_span<u8>(begin_data_p, property_buffer_structure.size),
				.property_offset_span = TG_span<u32>(begin_property_offset_p, property_buffer_structure.property_structures.size())
			});
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

		const auto& property_buffer_wrapper = property_buffer_wrappers_[material_property_id_get_buffer_index(id)];

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

		const auto& property_buffer_wrapper = property_buffer_wrappers_[material_property_id_get_buffer_index(id)];

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

}