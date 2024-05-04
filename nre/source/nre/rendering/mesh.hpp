#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/vertex.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	template<typename F_vertex__>
	struct TF_mesh_data;
	template<typename F_vertex__>
	class TF_mesh_buffer;
	template<typename F_vertex__>
	class TF_mesh;



	struct F_submesh_header
	{
		u32 vertex_count = 0;
		u32 index_count = 0;
		u32 base_vertex_location = 0;
		u32 base_index_location = 0;
	};



	template<typename F_vertex__ = F_vertex>
	struct TF_mesh_data {

	public:
		using F_vertex = F_vertex__;

		using F_vertex_vector = TG_vector<F_vertex>;
		using F_index_vector = TG_vector<u32>;
		using F_submesh_header_vector = TG_vector<F_submesh_header>;



	public:
		F_vertex_vector vertices;
		F_index_vector indices;
		F_submesh_header_vector submesh_headers;

	};

	template<typename F_vertex__ = F_vertex>
	struct TH_mesh_data {

	public:
		using F_data = TF_mesh_data<F_vertex__>;



	public:
		static F_data create(
			const TG_vector<F_vertex__>& vertices,
			const TG_vector<u32>& indices
		)
		{
			return {
				vertices,
				indices,
				{
					F_submesh_header {
						.vertex_count = vertices.size(),
						.index_count = indices.size()
					}
				}
			};
		}

	};



	template<typename F_vertex__ = F_vertex>
	class TF_mesh_buffer {

	public:
		using F_vertex = F_vertex__;

		using F_data = TF_mesh_data<F_vertex>;
		using F_mesh = TF_mesh<F_vertex>;



	private:
		TK_valid<F_mesh> mesh_p_;
		U_buffer_handle vertex_buffer_p_;
		U_buffer_handle index_buffer_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_mesh> mesh_p() const noexcept { return mesh_p_; }
		NCPP_FORCE_INLINE K_buffer_handle vertex_buffer_p() const noexcept { return vertex_buffer_p_.keyed(); }
		NCPP_FORCE_INLINE K_buffer_handle index_buffer_p() const noexcept { return index_buffer_p_.keyed(); }



	public:
		TF_mesh_buffer(TKPA_valid<F_mesh> mesh_p) :
			mesh_p_(mesh_p)
		{

		}
		~TF_mesh_buffer() {

		}

	public:
		NCPP_DISABLE_COPY(TF_mesh_buffer);

	public:
		void upload() {

			auto& data = mesh_p_->data;

			if(vertex_buffer_p_)
				vertex_buffer_p_.reset();
			if(index_buffer_p_)
				index_buffer_p_.reset();

			if(data.vertices.size()) {

				vertex_buffer_p_ = H_buffer::T_create<F_vertex>(
					NRE_RENDER_SYSTEM()->device_p(),
					data.vertices,
					E_resource_bind_flag::VBV
				);
			}

			if(data.indices.size()) {

				index_buffer_p_ = H_buffer::T_create<u32>(
					NRE_RENDER_SYSTEM()->device_p(),
					data.indices,
					E_format::R32_UINT,
					E_resource_bind_flag::IBV
				);
			}
		}

	};



	template<typename F_vertex__ = F_vertex>
	class TF_mesh {

	public:
		using F_vertex = F_vertex__;

		using F_data = TF_mesh_data<F_vertex>;
		using F_buffer = TF_mesh_buffer<F_vertex>;

	public:
		friend F_buffer;



	private:
		G_string name_;

	public:
		F_data data;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(V_string value) noexcept { name_ = value; }



	public:
		TF_mesh(const F_data& data_in = {}, const G_string& name = "") :
			name_(name),
			data(data_in)
		{
		}
		~TF_mesh() {

		}

	public:
		NCPP_DISABLE_COPY(TF_mesh);

	};



	using F_mesh_data = TF_mesh_data<>;
	using H_mesh_data = TH_mesh_data<>;
	using F_mesh_buffer = TF_mesh_buffer<>;
	using F_mesh = TF_mesh<>;

}