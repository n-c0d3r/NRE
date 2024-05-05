#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/vertex.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	class A_mesh_buffer;
	class A_mesh;

	template<typename... F_vertex_channel_datas__>
	struct TF_mesh_data;
	template<typename... F_vertex_channel_datas__>
	class TF_mesh_buffer;
	template<typename... F_vertex_channel_datas__>
	class TF_mesh;



	struct F_submesh_header
	{
		u32 vertex_count = 0;
		u32 index_count = 0;
		u32 base_vertex_location = 0;
		u32 base_index_location = 0;
	};



	template<typename... F_vertex_channel_datas__>
	struct TF_mesh_data {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_vertex_channels = TG_tuple<TG_vector<F_vertex_channel_datas__>...>;
		using F_indices = TG_vector<u32>;
		using F_submesh_header_vector = TG_vector<F_submesh_header>;



	public:
		F_vertex_channels vertex_channels;
		F_indices indices;
		F_submesh_header_vector submesh_headers;

	public:
		NCPP_FORCE_INLINE u32 vertex_count() const noexcept
		{
			return eastl::get<0>(vertex_channels).size();
		}

	};

	template<typename... F_vertex_channel_datas__>
	struct TH_mesh_data {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_data = TF_mesh_data<F_vertex_channel_datas__...>;



	public:
		static F_data create(
			const TG_vector<F_vertex_channel_datas__>&... vertex_channels,
			const typename F_data::F_indices& indices
		)
		{
			return {
				vertex_channels...,
				indices,
				{
					F_submesh_header {
						.vertex_count = get_first_vertex_channel_internal(vertex_channels).size(),
						.index_count = indices.size()
					}
				}
			};
		}

	private:
		static const TG_vector<TF_first_template_targ<F_vertex_channel_datas__...>>& get_first_vertex_channel_internal(
			const auto& first_vertex_channel,
			const auto&... rest_vertex_channels
		)
		{
			return first_vertex_channel;
		}

	};



	class A_mesh_buffer
	{

	protected:
		TK_valid<A_mesh> mesh_p_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_mesh> mesh_p() const noexcept { return mesh_p_; }

		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const = 0;
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const = 0;
		virtual K_buffer_handle index_buffer_p() const = 0;



	protected:
		A_mesh_buffer(TKPA_valid<A_mesh> mesh_p);

	public:
		virtual ~A_mesh_buffer() = default;
	};

	template<typename... F_vertex_channel_datas__>
	class TF_mesh_buffer : public A_mesh_buffer {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_data = TF_mesh_data<F_vertex_channel_datas__...>;
		using F_mesh = TF_mesh<F_vertex_channel_datas__...>;



	private:
		TG_array<U_buffer_handle, sizeof...(F_vertex_channel_datas__)> vertex_buffer_p_array_;
		U_buffer_handle index_buffer_p_;

	public:
		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const override { return vertex_buffer_p_array_[index].keyed(); }
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const override { return vertex_buffer_p_array_; }
		virtual K_buffer_handle index_buffer_p() const override { return index_buffer_p_.keyed(); }



	public:
		TF_mesh_buffer(TKPA_valid<F_mesh> mesh_p) :
			A_mesh_buffer(mesh_p)
		{

		}
		~TF_mesh_buffer() {

		}

	public:
		NCPP_DISABLE_COPY(TF_mesh_buffer);

	public:
		void upload() {

			auto& data = mesh_p_.T_cast<F_mesh>()->data;

			using F_vertex_channel_indices = TF_template_targ_list<F_vertex_channel_datas__...>::F_indices;

			for(auto& vertex_buffer_p : vertex_buffer_p_array_)
			{
				if(vertex_buffer_p)
					vertex_buffer_p.reset();
			}
			if(index_buffer_p_)
				index_buffer_p_.reset();

			if(data.vertex_count())
			{
				TF_upload_vertex_buffers_helper<F_vertex_channel_indices>::invoke(*this, data);
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

	private:
		template<typename F__>
		struct TF_upload_vertex_buffers_helper;

	};

	template<typename... F_vertex_channel_datas__>
	template<sz... indices>
	struct TF_mesh_buffer<F_vertex_channel_datas__...>::TF_upload_vertex_buffers_helper<TF_template_varg_list<indices...>>
	{
		static void invoke(TF_mesh_buffer<F_vertex_channel_datas__...>& mesh_buffer, TF_mesh_data<F_vertex_channel_datas__...>& data)
		{
			using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

			mesh_buffer.vertex_buffer_p_array_ = {
				(
					H_buffer::T_create<F_vertex_channel_data_targ_list::template TF_at<indices>>(
						NRE_RENDER_SYSTEM()->device_p(),
						eastl::get<indices>(data.vertex_channels),
						E_resource_bind_flag::VBV
					)
				)...
			};
		}
	};



	class A_mesh
	{

	public:
		virtual u32 vertex_channel_count() const = 0;
		virtual u32 vertex_count() const = 0;
		virtual u32 index_count() const = 0;



	protected:
		A_mesh() = default;

	public:
		virtual ~A_mesh() = default;

	};

	template<typename... F_vertex_channel_datas__>
	class TF_mesh : public A_mesh {

	public:
		using F_data = TF_mesh_data<F_vertex_channel_datas__...>;
		using F_buffer = TF_mesh_buffer<F_vertex_channel_datas__...>;

	public:
		friend F_buffer;



	private:
		G_string name_;

	public:
		F_data data;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(V_string value) noexcept { name_ = value; }
		virtual u32 vertex_channel_count() const override
		{
			return sizeof...(F_vertex_channel_datas__);
		}
		virtual u32 vertex_count() const override
		{
			return data.vertex_count();
		}
		virtual u32 index_count() const override
		{
			return data.indices.size();
		}



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



	using F_mesh_data = TF_mesh_data<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;
	using H_mesh_data = TH_mesh_data<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;
	using F_mesh_buffer = TF_mesh_buffer<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;
	using F_mesh = TF_mesh<
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;

}