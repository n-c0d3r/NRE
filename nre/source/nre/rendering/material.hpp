#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/material_template.hpp>
#include <nre/rendering/render_command_list.hpp>



namespace nre {

	class NRE_API F_material {

	private:
		struct F_property_buffer_wrapper {

			U_buffer_handle buffer_p;
			TG_span<u8> data;
			TG_span<u32> property_offset_span;

		};
		struct F_pure_constant_buffer_wrapper {

			U_buffer_handle buffer_p;
			TG_span<u8> data;

		};
		struct F_pass_wrapper {

			TK<A_pipeline_state> pso_p;
			TG_span<K_buffer_handle> constant_buffer_p_span;
			TG_span<K_srv_handle> srv_p_span;
			TG_span<K_uav_handle> uav_p_span;

		};



	private:
		TS<F_material_template> template_p_;

		F_material_structure structure_;

		TG_vector<u8> cached_memory_;

		TG_span<F_property_buffer_wrapper> property_buffer_wrapper_span_;
		TG_span<F_pure_constant_buffer_wrapper> pure_constant_buffer_wrapper_span_;
		TG_span<F_pass_wrapper> pass_wrapper_span_;
		TG_span<K_buffer_handle*> external_constant_buffer_pp_span_;
		TG_span<K_srv_handle*> srv_pp_span_;
		TG_span<K_uav_handle*> uav_pp_span_;

		TG_unordered_map<G_string, F_material_property_id> property_name_to_property_id_map_;
		TG_unordered_map<G_string, F_material_property_buffer_id> property_buffer_name_to_property_buffer_id_map_;
		TG_unordered_map<G_string, F_material_pure_constant_buffer_id> pure_constant_buffer_name_to_pure_constant_buffer_id_map_;
		TG_unordered_map<G_string, F_material_pass_id > pass_name_to_pass_id_map_;
		TG_unordered_map<G_string, F_material_external_constant_buffer_id> external_constant_buffer_name_to_external_constant_buffer_id_map_;
		TG_unordered_map<G_string, F_material_srv_id> srv_name_to_srv_id_map_;
		TG_unordered_map<G_string, F_material_srv_id> uav_name_to_uav_id_map_;

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
				property_buffer_wrapper_span_[property_buffer_name_to_property_buffer_id_map_.find(name)->second].buffer_p
			);
		}
		NCPP_FORCE_INLINE K_valid_buffer_handle property_buffer_p(F_material_property_buffer_id id) const noexcept {

			return NCPP_FOH_VALID(
				property_buffer_wrapper_span_[id].buffer_p
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

	public:
		NCPP_FORCE_INLINE b8 is_has_pure_constant_buffer(const G_string& name) const noexcept {

			return (pure_constant_buffer_name_to_pure_constant_buffer_id_map_.find(name) != pure_constant_buffer_name_to_pure_constant_buffer_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_external_constant_buffer_id pure_constant_buffer_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_pure_constant_buffer(name)) << "\"" << name << "\" pure_constant_buffer not found";

			return (pure_constant_buffer_name_to_pure_constant_buffer_id_map_.find(name) != pure_constant_buffer_name_to_pure_constant_buffer_id_map_.end());
		}

	public:
		void* pure_constant_buffer_data_p(const G_string& name) const;
		void* pure_constant_buffer_data_p(F_material_pure_constant_buffer_id id) const;
		const F_material_pure_constant_buffer_structure& pure_constant_buffer_structure(const G_string& name) const;
		const F_material_pure_constant_buffer_structure& pure_constant_buffer_structure(F_material_pure_constant_buffer_id id) const;

	public:
		NCPP_FORCE_INLINE b8 is_has_external_constant_buffer(const G_string& name) const noexcept {

			return (external_constant_buffer_name_to_external_constant_buffer_id_map_.find(name) != external_constant_buffer_name_to_external_constant_buffer_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_external_constant_buffer_id external_constant_buffer_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_external_constant_buffer(name)) << "\"" << name << "\" external_constant_buffer not found";

			return (external_constant_buffer_name_to_external_constant_buffer_id_map_.find(name) != external_constant_buffer_name_to_external_constant_buffer_id_map_.end());
		}

	public:
		void set_external_constant_buffer_p(const G_string& name, KPA_valid_buffer_handle external_constant_buffer_p);
		void set_external_constant_buffer_p(F_material_external_constant_buffer_id id, KPA_valid_buffer_handle external_constant_buffer_p);
		K_valid_buffer_handle external_constant_buffer_p(const G_string& name) const;
		K_valid_buffer_handle external_constant_buffer_p(F_material_external_constant_buffer_id id) const;
		const F_material_external_constant_buffer_structure& external_constant_buffer_structure(const G_string& name) const;
		const F_material_external_constant_buffer_structure& external_constant_buffer_structure(F_material_pure_constant_buffer_id id) const;

	public:
		NCPP_FORCE_INLINE b8 is_has_srv(const G_string& name) const noexcept {

			return (srv_name_to_srv_id_map_.find(name) != srv_name_to_srv_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_srv_id srv_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_srv(name)) << "\"" << name << "\" srv not found";

			return (srv_name_to_srv_id_map_.find(name) != srv_name_to_srv_id_map_.end());
		}

	public:
		void set_srv_p(const G_string& name, KPA_valid_srv_handle srv_p);
		void set_srv_p(F_material_srv_id id, KPA_valid_srv_handle srv_p);
		K_valid_srv_handle srv_p(const G_string& name) const;
		K_valid_srv_handle srv_p(F_material_srv_id id) const;
		const F_material_srv_structure& srv_structure(const G_string& name) const;
		const F_material_srv_structure& srv_structure(F_material_pure_constant_buffer_id id) const;

	public:
		NCPP_FORCE_INLINE b8 is_has_uav(const G_string& name) const noexcept {

			return (uav_name_to_uav_id_map_.find(name) != uav_name_to_uav_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_uav_id uav_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_uav(name)) << "\"" << name << "\" uav not found";

			return (uav_name_to_uav_id_map_.find(name) != uav_name_to_uav_id_map_.end());
		}

	public:
		void set_uav_p(const G_string& name, KPA_valid_uav_handle uav_p);
		void set_uav_p(F_material_uav_id id, KPA_valid_uav_handle uav_p);
		K_valid_uav_handle uav_p(const G_string& name) const;
		K_valid_uav_handle uav_p(F_material_uav_id id) const;
		const F_material_uav_structure& uav_structure(const G_string& name) const;
		const F_material_uav_structure& uav_structure(F_material_pure_constant_buffer_id id) const;

	public:
		NCPP_FORCE_INLINE b8 is_has_pass(const G_string& name) const noexcept {

			return (pass_name_to_pass_id_map_.find(name) != pass_name_to_pass_id_map_.end());
		}
		NCPP_FORCE_INLINE F_material_pass_id pass_id(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_pass(name)) << "pass \"" << name << "\" not found";

			return pass_name_to_pass_id_map_.find(name)->second;
		}

	public:
		void bind_pass(
			const G_string& pass_name,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		void bind_pass(
			F_material_pass_id id,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		void bind_graphics_pass(
			const G_string& pass_name,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		void bind_graphics_pass(
			F_material_pass_id id,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		void bind_compute_pass(
			const G_string& pass_name,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		void bind_compute_pass(
			F_material_pass_id id,
			KPA_valid_render_command_list_handle render_command_list_p
		);
		const F_material_pass_structure& pass_structure(const G_string& name) const noexcept;
		const F_material_pass_structure& pass_structure(F_material_pass_id id) const noexcept;

	};

}