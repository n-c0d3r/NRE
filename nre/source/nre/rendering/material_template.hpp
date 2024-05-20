#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_material_template;



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



	struct F_material_pure_constant_buffer_structure
	{
		G_string name;
		u32 size = 0;
	};

	using F_material_pure_constant_buffer_id = u32;



	struct F_material_external_constant_buffer_structure
	{
		G_string name;
	};

	using F_material_external_constant_buffer_id = u64;



	struct F_material_srv_structure {

		G_string name;

	};

	// lower 32 bits: begin index
	// higher 32 bits: end index
	using F_material_srv_id = u64;



	struct F_material_uav_structure {

		G_string name;

	};

	// lower 32 bits: begin index
	// higher 32 bits: end index
	using F_material_uav_id = u64;



	struct F_material_pass_structure {

		G_string name;
		TK<A_pipeline_state> pso_p;
		TG_vector<u32> property_buffer_structure_indices;
		TG_vector<u32> pure_constant_buffer_structure_indices;
		TG_vector<u32> external_constant_buffer_structure_indices;
		TG_vector<u32> srv_structure_indices;
		TG_vector<u32> uav_structure_indices;

	};

	using F_material_pass_id = u32;



	struct F_material_external_constant_buffer_linking {

		u32 index = 0;
		u32 local_index = 0;
		u32 pass_index = 0;

	};
	struct F_material_srv_linking {

		u32 index = 0;
		u32 local_index = 0;
		u32 pass_index = 0;

	};
	struct F_material_uav_linking {

		u32 index = 0;
		u32 local_index = 0;
		u32 pass_index = 0;

	};



	struct F_material_structure {

		TG_vector<F_material_property_buffer_structure> property_buffer_structures;
		TG_vector<F_material_pure_constant_buffer_structure> pure_constant_buffer_structures;
		TG_vector<F_material_external_constant_buffer_structure> external_constant_buffer_structures;
		TG_vector<F_material_srv_structure> srv_structures;
		TG_vector<F_material_uav_structure> uav_structures;
		TG_vector<F_material_pass_structure> pass_structures;

		TG_vector<F_material_external_constant_buffer_linking> external_constant_buffer_linkings;
		TG_vector<F_material_srv_linking> srv_linkings;
		TG_vector<F_material_uav_linking> uav_linkings;

	};



	struct F_material_property_info {

		G_string name;
		G_string buffer_name;
		u32 size;
		u32 add_index = 0;
		u64 type_id = 0;

	};

	struct F_material_pure_constant_buffer_info {

		G_string name;
		u32 size = 0;
		u32 add_index = 0;

	};

	struct F_material_external_constant_buffer_info {

		G_string name;
		u32 add_index = 0;

	};

	struct F_material_srv_info {

		G_string name;
		u32 add_index = 0;

	};

	struct F_material_uav_info {

		G_string name;
		u32 add_index = 0;

	};

	struct F_material_pass_info {

		G_string name;
		TK<A_pipeline_state> pso_p;
		TG_vector<G_string> property_buffer_names;
		TG_vector<G_string> pure_constant_buffer_names;
		TG_vector<G_string> external_constant_buffer_names;
		TG_vector<G_string> srv_names;
		TG_vector<G_string> uav_names;

	};



	constexpr u32 material_property_id_get_local_index(F_material_property_id id) {

		return (id & 0x00000000FFFFFFFF);
	}
	constexpr F_material_property_buffer_id material_property_id_get_buffer_index(F_material_property_id id) {

		return (
			(id & 0xFFFFFFFF00000000)
			>> 32
		);
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



	constexpr u32 material_external_constant_buffer_id_get_begin_index(F_material_external_constant_buffer_id id) {

		return (id & 0x00000000FFFFFFFF);
	}
	constexpr u32 material_external_constant_buffer_id_get_end_index(F_material_external_constant_buffer_id id) {

		return (
			(id & 0xFFFFFFFF00000000)
			>> 32
		);
	}
	constexpr F_material_external_constant_buffer_id make_material_external_constant_buffer_id(
		u32 begin_index,
		u32 end_index
	) {

		return (
			F_material_external_constant_buffer_id(begin_index)
			| (F_material_external_constant_buffer_id(end_index) << 32)
		);
	}



	constexpr u32 material_srv_id_get_begin_index(F_material_srv_id id) {

		return (id & 0x00000000FFFFFFFF);
	}
	constexpr u32 material_srv_id_get_end_index(F_material_srv_id id) {

		return (
			(id & 0xFFFFFFFF00000000)
			>> 32
		);
	}
	constexpr F_material_srv_id make_material_srv_id(
		u32 begin_index,
		u32 end_index
	) {

		return (
			F_material_srv_id(begin_index)
			| (F_material_srv_id(end_index) << 32)
		);
	}



	constexpr u32 material_uav_id_get_begin_index(F_material_uav_id id) {

		return (id & 0x00000000FFFFFFFF);
	}
	constexpr u32 material_uav_id_get_end_index(F_material_uav_id id) {

		return (
			(id & 0xFFFFFFFF00000000)
			>> 32
		);
	}
	constexpr F_material_uav_id make_material_uav_id(
		u32 begin_index,
		u32 end_index
	) {

		return (
			F_material_uav_id(begin_index)
			| (F_material_uav_id(end_index) << 32)
		);
	}



	class NRE_API F_material_property_buffer_builder {

	private:
		TK_valid<F_material_template> material_template_p_;
		G_string name_;



	public:
		F_material_property_buffer_builder(
			TKPA_valid<F_material_template> material_template_p,
			const G_string& name
		);

	public:
		F_material_property_buffer_builder& add(
			const G_string& name,
			u32 size,
			u64 type_id
		);
		template<typename F__>
		NCPP_FORCE_INLINE F_material_property_buffer_builder& T_add(const G_string& name) {

			return add(
				name,
				sizeof(F__),
				T_type_hash_code<F__>
			);
		}

	};



	class NRE_API F_material_template {

	private:
		TG_unordered_map<G_string, F_material_property_info> property_info_map_;
		TG_unordered_map<G_string, F_material_pure_constant_buffer_info> pure_constant_buffer_info_map_;
		TG_unordered_map<G_string, F_material_external_constant_buffer_info> external_constant_buffer_info_map_;
		TG_unordered_map<G_string, F_material_srv_info> srv_info_map_;
		TG_unordered_map<G_string, F_material_uav_info> uav_info_map_;
		TG_unordered_map<G_string, F_material_pass_info> pass_info_map_;
		u32 last_add_index_ = 0;

		F_material_structure structure_;

	public:
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_property_info>& property_info_map() const noexcept {

			return property_info_map_;
		}
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_pure_constant_buffer_info>& pure_constant_buffer_info_map() const noexcept {

			return pure_constant_buffer_info_map_;
		}
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_external_constant_buffer_info>& external_constant_buffer_info_map() const noexcept {

			return external_constant_buffer_info_map_;
		}
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_srv_info>& srv_info_map() const noexcept {

			return srv_info_map_;
		}
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_uav_info>& uav_info_map() const noexcept {

			return uav_info_map_;
		}
		NCPP_FORCE_INLINE const TG_unordered_map<G_string, F_material_pass_info>& pass_info_map() const noexcept {

			return pass_info_map_;
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
		NCPP_FORCE_INLINE void add_property(
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
		void T_add_property(
			const G_string& name,
			const G_string& buffer_name
		) {
			add_property(
				name,
				buffer_name,
				sizeof(F__),
				T_type_hash_code<F__>
			);
		}
		NCPP_FORCE_INLINE F_material_property_buffer_builder property_buffer_builder(
			const G_string& name
		) {
			return F_material_property_buffer_builder(
				NCPP_KTHIS(),
				name
			);
		}

	public:
		NCPP_FORCE_INLINE b8 is_has_pure_constant_buffer(const G_string& name) const noexcept {

			return (pure_constant_buffer_info_map_.find(name) != pure_constant_buffer_info_map_.end());
		}
		inline const F_material_pure_constant_buffer_info& pure_constant_buffer_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_pure_constant_buffer(name)) << "\"" << name << "\" pure constant buffer is not added";

			return pure_constant_buffer_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_pure_constant_buffer(
			const G_string& name,
			u32 size
		) {
			NCPP_ASSERT(!is_has_pure_constant_buffer(name)) << "\"" << name << "\" pure constant buffer already added";

			pure_constant_buffer_info_map_[name] = {
				name,
				size,
				last_add_index_
			};
			++last_add_index_;
		}
		template<typename F__>
		void T_add_pure_constant_buffer(
			const G_string& name
		) {
			add_pure_constant_buffer(
				name,
				sizeof(F__)
			);
		}

	public:
		NCPP_FORCE_INLINE b8 is_has_external_constant_buffer(const G_string& name) const noexcept {

			return (external_constant_buffer_info_map_.find(name) != external_constant_buffer_info_map_.end());
		}
		inline const F_material_external_constant_buffer_info& external_constant_buffer_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_external_constant_buffer(name)) << "\"" << name << "\" external constant buffer is not added";

			return external_constant_buffer_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_external_constant_buffer(
			const G_string& name
		) {
			NCPP_ASSERT(!is_has_external_constant_buffer(name)) << "\"" << name << "\" external constant buffer already added";

			external_constant_buffer_info_map_[name] = {
				name,
				last_add_index_
			};
			++last_add_index_;
		}

	public:
		NCPP_FORCE_INLINE b8 is_has_srv(const G_string& name) const noexcept {

			return (srv_info_map_.find(name) != srv_info_map_.end());
		}
		inline const F_material_srv_info& srv_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_srv(name)) << "\"" << name << "\" srv is not added";

			return srv_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_srv(
			const G_string& name
		) {
			NCPP_ASSERT(!is_has_srv(name)) << "\"" << name << "\" srv already added";

			srv_info_map_[name] = {
				name,
				last_add_index_
			};
			++last_add_index_;
		}

	public:
		NCPP_FORCE_INLINE b8 is_has_uav(const G_string& name) const noexcept {

			return (uav_info_map_.find(name) != uav_info_map_.end());
		}
		inline const F_material_uav_info& uav_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_uav(name)) << "\"" << name << "\" uav is not added";

			return uav_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_uav(
			const G_string& name
		) {
			NCPP_ASSERT(!is_has_uav(name)) << "\"" << name << "\" uav already added";

			uav_info_map_[name] = {
				name,
				last_add_index_
			};
			++last_add_index_;
		}

	public:
		NCPP_FORCE_INLINE b8 is_has_pass(const G_string& name) const noexcept {

			return (pass_info_map_.find(name) != pass_info_map_.end());
		}
		NCPP_FORCE_INLINE const F_material_pass_info& pass_info(const G_string& name) const noexcept {

			NCPP_ASSERT(is_has_pass(name)) << "pass \"" << name << "\" not found";

			return pass_info_map_.find(name)->second;
		}
		NCPP_FORCE_INLINE void add_pass(
			const G_string& name,
			TK_valid<A_pipeline_state> pso_p,
			const TG_vector<G_string>& property_buffer_names,
			const TG_vector<G_string>& pure_constant_buffer_names,
			const TG_vector<G_string>& srv_names,
			const TG_vector<G_string>& uav_names
		) noexcept {

			NCPP_ASSERT(!is_has_pass(name)) << "pass \"" << name << "\" already added";

			pass_info_map_[name] = {
				.name = name,
				.pso_p = pso_p.no_requirements(),
				.property_buffer_names = property_buffer_names,
				.pure_constant_buffer_names = pure_constant_buffer_names,
				.srv_names = srv_names,
				.uav_names = uav_names
			};
		}

	public:
		void clear();
		const F_material_structure& build();

	};

}