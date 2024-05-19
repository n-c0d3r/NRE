#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/material_template.hpp>



namespace nre {

	class NRE_API F_material {

	private:
		struct F_property_buffer_wrapper {

			U_buffer_handle buffer_p;
			TG_span<u8> data;
			TG_span<u32> property_offset_span;

		};



	private:
		TS<F_material_template> template_p_;

		F_material_structure structure_;

		TG_vector<u8> property_buffer_memory_;

		TG_vector<F_property_buffer_wrapper> property_buffer_wrappers_;

		TG_unordered_map<G_string, F_material_property_id> property_name_to_property_id_map_;
		TG_unordered_map<G_string, F_material_property_buffer_id> property_buffer_name_to_property_buffer_id_map_;

	public:
		 NCPP_FORCE_INLINE TK_valid<F_material_template> template_p() const noexcept { return NCPP_FOH_VALID(template_p_); }

		 NCPP_FORCE_INLINE const F_material_structure& structure() const noexcept { return structure_; }



	public:
		F_material(TSPA<F_material_template> template_p);
		virtual ~F_material();

	public:
		NCPP_OBJECT(F_material);

	public:
		NCPP_FORCE_INLINE b8 is_has_property(const G_string& name) const noexcept {

			return (property_name_to_property_id_map_.find(name) != property_name_to_property_id_map_.end());
		}
		NCPP_FORCE_INLINE b8 is_has_property_buffer(const G_string& name) const noexcept {

			return (property_buffer_name_to_property_buffer_id_map_.find(name) != property_buffer_name_to_property_buffer_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_property_id property_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property(name)) << "\"" << name << "\" property not found";

			return (property_name_to_property_id_map_.find(name) != property_name_to_property_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_property_buffer_id property_buffer_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property_buffer(name)) << "\"" << name << "\" property buffer not found";

			return (property_buffer_name_to_property_buffer_id_map_.find(name) != property_buffer_name_to_property_buffer_id_map_.end());
		}
		NCPP_FORCE_INLINE K_valid_buffer_handle property_buffer_p(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property_buffer(name)) << "\"" << name << "\" property buffer not found";

			return NCPP_FOH_VALID(
				property_buffer_wrappers_[property_buffer_name_to_property_buffer_id_map_.find(name)->second].buffer_p
			);
		}
		NCPP_FORCE_INLINE K_valid_buffer_handle property_buffer_p(F_material_property_buffer_id id) const noexcept {

			return NCPP_FOH_VALID(
				property_buffer_wrappers_[id].buffer_p
			);
		}
		NCPP_FORCE_INLINE const F_material_property_structure& property_structure(F_material_property_id id) const noexcept {

			return property_buffer_structure(
				material_property_id_get_buffer_index(id)
			).property_structures[
				material_property_id_get_local_index(id)
			];
		}
		NCPP_FORCE_INLINE const F_material_property_structure& property_structure(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property(name)) << "\"" << name << "\" property not found";

			return property_structure(
				property_id(name)
			);
		}
		NCPP_FORCE_INLINE const F_material_property_buffer_structure& property_buffer_structure(F_material_property_buffer_id id) const noexcept {

			return structure_.property_buffer_structures[id];
		}
		NCPP_FORCE_INLINE const F_material_property_buffer_structure& property_buffer_structure(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_property_buffer(name)) << "\"" << name << "\" property buffer not found";

			return property_buffer_structure(
				property_buffer_id(name)
			);
		}

	public:
		void set_property_value(const G_string& name, void* data_p, u32 size);
		void set_property_value(F_material_property_id id, void* data_p, u32 size);
		void property_value(const G_string& name, void* data_p, u32 size) const;
		void property_value(F_material_property_id id, void* data_p, u32 size) const;
		template<typename F__>
		NCPP_FORCE_INLINE F__ T_property_value(const G_string& name) const {

			F__ value;
			property_value(name, (void*)&value, sizeof(F__));

			return value;
		}
		template<typename F__>
		NCPP_FORCE_INLINE F__ T_property_value(F_material_property_id id) const {

			F__ value;
			property_value(id, (void*)&value, sizeof(F__));

			return value;
		}

	};

}