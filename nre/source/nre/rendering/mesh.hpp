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



	template<typename F_vertex__ = F_vertex>
	struct TF_mesh_data {

	public:
		using F_vertex = F_vertex__;

		using F_vertex_vector = TG_vector<F_vertex>;
		using F_index_vector = TG_vector<u32>;



	public:
		F_vertex_vector vertices;
		F_index_vector indices;

	};



	template<typename F_vertex__ = F_vertex>
	class TF_mesh_buffer {

	public:
		using F_vertex = F_vertex__;

		using F_data = TF_mesh_data<F_vertex>;
		using TF_mesh = TF_mesh<F_vertex>;



	private:
		TK_valid<TF_mesh> mesh_p_;
		U_buffer_handle vertex_buffer_p_;
		U_buffer_handle index_buffer_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<TF_mesh> mesh_p() const noexcept { return mesh_p_; }
		NCPP_FORCE_INLINE K_buffer_handle vertex_buffer_p() const noexcept { return NCPP_FOREF_VALID(vertex_buffer_p_); }
		NCPP_FORCE_INLINE K_buffer_handle index_buffer_p() const noexcept { return NCPP_FOREF_VALID(index_buffer_p_); }



	public:
		TF_mesh_buffer(TKPA_valid<TF_mesh> mesh_p) :
			mesh_p_(mesh_p)
		{

		}
		~TF_mesh_buffer() {

		}

	public:
		void upload() {

			auto& data = mesh_p_->data_;

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
		F_data data_;
		TU<F_buffer> buffer_p_;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE const F_data& data() const noexcept { return data_; }
		NCPP_FORCE_INLINE TKPA_valid<F_buffer> buffer_p() const noexcept { return NCPP_FOREF_VALID(buffer_p_); }



	public:
		TF_mesh(const G_string& name, const F_data& data = {}) :
			name_(name),
			data_(data),
			buffer_p_(TU<F_buffer>()(NCPP_KTHIS()))
		{
			buffer_p_->upload();
		}
		~TF_mesh() {

		}

	public:
		void update(const F_data& data)
		{
			data_ = data;
			buffer_p_->upload();
		}

	};



	using F_mesh_data = TF_mesh_data<>;
	using F_mesh_buffer = TF_mesh_buffer<>;
	using F_mesh = TF_mesh<>;

}