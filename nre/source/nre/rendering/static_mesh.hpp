#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/vertex.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	class A_static_mesh_buffer;
	class A_static_mesh;

	template<typename... F_vertex_channel_datas__>
	struct TF_static_mesh_data;
	template<typename... F_vertex_channel_datas__>
	class TF_static_mesh_buffer;
	template<typename... F_vertex_channel_datas__>
	class TF_static_mesh;



	struct F_static_submesh_header
	{
		u32 vertex_count = 0;
		u32 index_count = 0;
		u32 base_vertex_location = 0;
		u32 base_index_location = 0;
	};



	class A_static_mesh_buffer
	{

	protected:
		TK_valid<A_static_mesh> mesh_p_;
		U_buffer_handle index_buffer_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_static_mesh> mesh_p() const noexcept { return mesh_p_; }

		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const = 0;
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const = 0;
		NCPP_FORCE_INLINE K_buffer_handle index_buffer_p() const noexcept { return index_buffer_p_.keyed(); }



	protected:
		A_static_mesh_buffer(TKPA_valid<A_static_mesh> static_mesh_p);

	public:
		virtual ~A_static_mesh_buffer() = default;
	};

	template<typename... F_vertex_channel_datas__>
	class TF_static_mesh_buffer : public A_static_mesh_buffer {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_data = TF_static_mesh_data<F_vertex_channel_datas__...>;
		using F_static_mesh = TF_static_mesh<F_vertex_channel_datas__...>;



	private:
		TG_array<U_buffer_handle, sizeof...(F_vertex_channel_datas__)> vertex_buffer_p_array_;
		TG_array<K_buffer_handle, sizeof...(F_vertex_channel_datas__)> keyed_vertex_buffer_p_array_;

	public:
		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const override { return vertex_buffer_p_array_[index].keyed(); }
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const override
		{
			return (TG_array<K_buffer_handle, sizeof...(F_vertex_channel_datas__)>&)keyed_vertex_buffer_p_array_;
		}



	public:
		TF_static_mesh_buffer(TKPA_valid<F_static_mesh> static_mesh_p) :
			A_static_mesh_buffer(static_mesh_p)
		{
		}
		~TF_static_mesh_buffer() {
		}

	public:
		NCPP_DISABLE_COPY(TF_static_mesh_buffer);

	public:
		void upload() {

			auto mesh_p = mesh_p_.T_cast<F_static_mesh>();

			using F_vertex_channel_indices = typename TF_template_targ_list<F_vertex_channel_datas__...>::F_indices;

			for(auto& vertex_buffer_p : vertex_buffer_p_array_)
			{
				if(vertex_buffer_p)
					vertex_buffer_p.reset();
			}
			if(index_buffer_p_)
				index_buffer_p_.reset();

			if(mesh_p->vertex_count())
			{
				TF_upload_vertex_buffers_helper<F_vertex_channel_indices>::invoke(*this, mesh_p);
			}

			auto& indices = (TG_vector<u32>&)mesh_p->indices();
			if(indices.size()) {

				index_buffer_p_ = H_buffer::T_create<u32>(
					NRE_RENDER_SYSTEM()->device_p(),
					indices,
					E_format::R32_UINT,
					E_resource_bind_flag::IBV
				);
			}
		}

	private:
		template<typename F__>
		struct TF_upload_vertex_buffers_helper;

	};

	template<typename... F_vertex_channel_datas__>
	template<sz... indices>
	struct TF_static_mesh_buffer<F_vertex_channel_datas__...>::TF_upload_vertex_buffers_helper<TF_template_varg_list<indices...>>
	{

	private:
		static NCPP_FORCE_INLINE auto& inject_non_const(const auto& x)
		{
			return (std::remove_const_t<std::remove_reference_t<decltype(x)>>&)x;
		}

	public:
		static void invoke(
			TF_static_mesh_buffer<F_vertex_channel_datas__...>& static_mesh_buffer,
			TKPA_valid<TF_static_mesh<F_vertex_channel_datas__...>> mesh_p
		)
		{
			using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

			static_mesh_buffer.vertex_buffer_p_array_ = {
				(
					H_buffer::T_create<F_vertex_channel_data_targ_list::template TF_at<indices>>(
						NRE_RENDER_SYSTEM()->device_p(),
						inject_non_const(
							eastl::get<indices>(mesh_p->vertex_channels())
						),
						E_resource_bind_flag::VBV
					)
				)...
			};

			for(u32 i = 0; i < sizeof...(F_vertex_channel_datas__); ++i)
			{
				static_mesh_buffer.keyed_vertex_buffer_p_array_[i] = static_mesh_buffer.vertex_buffer_p_array_[i].keyed();
			}
		}
	};



	class A_static_mesh
	{

	public:
		using F_indices = TG_vector<u32>;
		using F_static_submesh_headers = TG_vector<F_static_submesh_header>;



	private:
		G_string name_;
		F_indices indices_;
		F_static_submesh_headers submesh_headers_;

	protected:
		u32 vertex_count_;
		u32 vertex_channel_count_;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(V_string value) noexcept { name_ = value; }
		NCPP_FORCE_INLINE const F_indices& indices() const noexcept { return indices_; }
		NCPP_FORCE_INLINE const F_static_submesh_headers& submesh_headers() const noexcept { return submesh_headers_; }
		NCPP_FORCE_INLINE u32 vertex_count() const noexcept { return vertex_count_; }
		NCPP_FORCE_INLINE u32 vertex_channel_count() const noexcept { return vertex_channel_count_; }



	protected:
		A_static_mesh(
			V_string name,
			F_indices indices,
			F_static_submesh_headers submesh_headers
		);

	public:
		virtual ~A_static_mesh() = default;

	};

	template<typename... F_vertex_channel_datas__>
	class TF_static_mesh : public A_static_mesh {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_vertex_channels = TG_tuple<TG_vector<F_vertex_channel_datas__>...>;

		using F_buffer = TF_static_mesh_buffer<F_vertex_channel_datas__...>;

	public:
		friend F_buffer;



	private:
		F_vertex_channels vertex_channels_;

	public:
		NCPP_FORCE_INLINE const F_vertex_channels& vertex_channels() const noexcept { return vertex_channels_; }



	public:
		TF_static_mesh(V_string name = "") :
			A_static_mesh(
				name,
				{},
				{}
			)
		{
			vertex_count_ = 0;
			vertex_channel_count_ = sizeof...(F_vertex_channel_datas__);
		}
		TF_static_mesh(
			const F_vertex_channels& vertex_channels,
			const F_indices& indices,
			const F_static_submesh_headers& submesh_headers,
			V_string name = ""
		) :
			A_static_mesh(
				name,
				indices,
				submesh_headers
			),
			vertex_channels_(vertex_channels)
		{
			vertex_count_ = eastl::get<0>(vertex_channels).size();
			vertex_channel_count_ = sizeof...(F_vertex_channel_datas__);
		}
		~TF_static_mesh() {

		}

	public:
		NCPP_DISABLE_COPY(TF_static_mesh);

	};



	using F_static_mesh_buffer = TF_static_mesh_buffer<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;
	using F_static_mesh = TF_static_mesh<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;

}