#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/vertex.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre {

	class A_static_mesh_buffer;
	class A_static_mesh;

	template<class F_vertex_channel_format_varg_list__, typename... F_vertex_channel_datas__>
	class TF_static_mesh_buffer;
	template<class F_vertex_channel_format_varg_list__, typename... F_vertex_channel_datas__>
	class TF_static_mesh;



	struct F_static_submesh_header
	{
		u32 vertex_count = 0;
		u32 index_count = 0;
		u32 base_vertex_location = 0;
		u32 base_index_location = 0;
	};



	class NRE_API A_static_mesh_buffer
	{

	protected:
		TK_valid<A_static_mesh> mesh_p_;
		U_buffer_handle index_buffer_p_;
		U_srv_handle index_srv_p_;
		U_uav_handle index_uav_p_;
		b8 is_has_srv_ = false;
		b8 is_has_uav_ = false;

	public:
		NCPP_FORCE_INLINE TKPA_valid<A_static_mesh> mesh_p() const noexcept { return mesh_p_; }

		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const = 0;
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const = 0;
		virtual u32 vertex_buffer_count() const = 0;
		virtual K_srv_handle vertex_srv_p(u32 index = 0) const { return {}; }
		virtual TG_span<K_srv_handle> vertex_srv_p_span() const { return {}; }
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const { return {}; }
		virtual TG_span<K_uav_handle> vertex_uav_p_span() const { return {}; }
		NCPP_FORCE_INLINE b8 is_has_srv() const noexcept { return is_has_srv_; }
		NCPP_FORCE_INLINE b8 is_has_uav() const noexcept { return is_has_uav_; }

		NCPP_FORCE_INLINE K_buffer_handle index_buffer_p() const noexcept { return index_buffer_p_; }
		NCPP_FORCE_INLINE K_srv_handle index_srv_p() const noexcept { return index_srv_p_; }
		NCPP_FORCE_INLINE K_uav_handle index_uav_p() const noexcept { return index_uav_p_; }

		NCPP_FORCE_INLINE u32 uploaded_vertex_buffer_count() const noexcept {

			return vertex_buffer_p_span().size();
		}
		NCPP_FORCE_INLINE u32 uploaded_vertex_count() const noexcept {

			auto vs = vertex_buffer_p_span();

			if(vs.size())
				return vs[0]->desc().element_count;

			return 0;
		}
		NCPP_FORCE_INLINE u32 uploaded_index_count() const noexcept {

			if(index_buffer_p_)
				return index_buffer_p_->desc().element_count;

			return 0;
		}



	protected:
		A_static_mesh_buffer(TKPA_valid<A_static_mesh> static_mesh_p);

	public:
		virtual ~A_static_mesh_buffer() = default;

	public:
		NCPP_OBJECT(A_static_mesh_buffer);
	};

	template<class F_vertex_channel_format_varg_list__, typename... F_vertex_channel_datas__>
	class TF_static_mesh_buffer : public A_static_mesh_buffer {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_mesh = TF_static_mesh<F_vertex_channel_format_varg_list__, F_vertex_channel_datas__...>;

	public:
		friend F_mesh;



	private:
		TG_array<U_buffer_handle, sizeof...(F_vertex_channel_datas__)> vertex_buffer_p_array_;
		TG_array<K_buffer_handle, sizeof...(F_vertex_channel_datas__)> keyed_vertex_buffer_p_array_;
		TG_array<U_srv_handle, sizeof...(F_vertex_channel_datas__)> vertex_srv_p_array_;
		TG_array<K_srv_handle, sizeof...(F_vertex_channel_datas__)> keyed_vertex_srv_p_array_;
		TG_array<U_uav_handle, sizeof...(F_vertex_channel_datas__)> vertex_uav_p_array_;
		TG_array<K_uav_handle, sizeof...(F_vertex_channel_datas__)> keyed_vertex_uav_p_array_;

	public:
		virtual K_buffer_handle vertex_buffer_p(u32 index = 0) const override { return vertex_buffer_p_array_[index].keyed(); }
		virtual TG_span<K_buffer_handle> vertex_buffer_p_span() const override
		{
			return (TG_array<K_buffer_handle, sizeof...(F_vertex_channel_datas__)>&)keyed_vertex_buffer_p_array_;
		}
		virtual u32 vertex_buffer_count() const override
		{
			return sizeof...(F_vertex_channel_datas__);
		}
		virtual K_srv_handle vertex_srv_p(u32 index = 0) const override { return vertex_srv_p_array_[index].keyed(); }
		virtual TG_span<K_srv_handle> vertex_srv_p_span() const override
		{
			return (TG_array<K_srv_handle, sizeof...(F_vertex_channel_datas__)>&)keyed_vertex_srv_p_array_;
		}
		virtual K_uav_handle vertex_uav_p(u32 index = 0) const override { return vertex_uav_p_array_[index].keyed(); }
		virtual TG_span<K_uav_handle> vertex_uav_p_span() const override
		{
			return (TG_array<K_uav_handle, sizeof...(F_vertex_channel_datas__)>&)keyed_vertex_uav_p_array_;
		}



	public:
		TF_static_mesh_buffer(TKPA_valid<F_mesh> static_mesh_p) :
			A_static_mesh_buffer(static_mesh_p)
		{
		}
		~TF_static_mesh_buffer() {
		}

	public:
		NCPP_OBJECT(TF_static_mesh_buffer);

	private:
		void upload_internal(
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		) {

			auto mesh_p = mesh_p_.T_cast<F_mesh>();

			is_has_srv_ = flag_is_has(
				additional_bind_flag,
				E_resource_bind_flag::SRV
			);
			is_has_uav_ = flag_is_has(
				additional_bind_flag,
				E_resource_bind_flag::UAV
			);

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
				TF_upload_vertex_buffers_helper<F_vertex_channel_indices>::invoke(
					*this,
					mesh_p,
					additional_bind_flag,
					heap_type
				);
			}

			auto& indices = (TG_vector<u32>&)mesh_p->indices();
			if(indices.size()) {

				index_buffer_p_ = H_buffer::T_create<u32>(
					NRE_RENDER_SYSTEM()->device_p(),
					indices,
					E_format::R32_UINT,
					flag_combine(
						E_resource_bind_flag::IBV,
						additional_bind_flag
					),
					heap_type
				);

				if(is_has_srv_)
					index_srv_p_ = H_resource_view::create_srv(
						NCPP_FOH_VALID(index_buffer_p_)
					);
				if(is_has_uav_)
					index_uav_p_ = H_resource_view::create_uav(
						NCPP_FOH_VALID(index_buffer_p_)
					);
			}
		}

	private:
		template<typename F__>
		struct TF_upload_vertex_buffers_helper;

	};

	template<class F_vertex_channel_format_varg_list__, typename... F_vertex_channel_datas__>
	template<sz... indices>
	struct TF_static_mesh_buffer<F_vertex_channel_format_varg_list__, F_vertex_channel_datas__...>::TF_upload_vertex_buffers_helper<TF_template_varg_list<indices...>>
	{

	private:
		static NCPP_FORCE_INLINE auto& inject_non_const(const auto& x)
		{
			return (std::remove_const_t<std::remove_reference_t<decltype(x)>>&)x;
		}

	public:
		static void invoke(
			TF_static_mesh_buffer<F_vertex_channel_format_varg_list__, F_vertex_channel_datas__...>& static_mesh_buffer,
			TKPA_valid<TF_static_mesh<F_vertex_channel_format_varg_list__, F_vertex_channel_datas__...>> mesh_p,
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
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
						flag_combine(
							E_resource_bind_flag::VBV,
							additional_bind_flag
						),
						heap_type
					)
				)...
			};

			if(static_mesh_buffer.is_has_srv_) {
				static_mesh_buffer.vertex_srv_p_array_ = {
					(
						H_resource_view::create_srv(
							NRE_RENDER_SYSTEM()->device_p(),
							{
								.resource_p = NCPP_FOH_VALID(static_mesh_buffer.vertex_buffer_p_array_[indices]),
								.overrided_format = F_vertex_channel_format_varg_list__::template T_at<indices>
							}
						)
					)...
				};
				for(u32 i = 0; i < sizeof...(F_vertex_channel_datas__); ++i)
				{
					static_mesh_buffer.keyed_vertex_srv_p_array_[i] = static_mesh_buffer.vertex_srv_p_array_[i].keyed();
				}
			}

			if(static_mesh_buffer.is_has_uav_) {
				static_mesh_buffer.vertex_uav_p_array_ = {
					(
						H_resource_view::create_uav(
							NRE_RENDER_SYSTEM()->device_p(),
							{
								.resource_p = NCPP_FOH_VALID(static_mesh_buffer.vertex_buffer_p_array_[indices]),
								.overrided_format = F_vertex_channel_format_varg_list__::template T_at<indices>
							}
						)
					)...
				};
				for(u32 i = 0; i < sizeof...(F_vertex_channel_datas__); ++i)
				{
					static_mesh_buffer.keyed_vertex_uav_p_array_[i] = static_mesh_buffer.vertex_uav_p_array_[i].keyed();
				}
			}

			for(u32 i = 0; i < sizeof...(F_vertex_channel_datas__); ++i)
			{
				static_mesh_buffer.keyed_vertex_buffer_p_array_[i] = static_mesh_buffer.vertex_buffer_p_array_[i].keyed();
			}
		}
	};



	class NRE_API A_static_mesh
	{

	public:
		using F_indices = TG_vector<u32>;
		using F_static_submesh_headers = TG_vector<F_static_submesh_header>;



	private:
		G_string name_;

	protected:
		F_indices indices_;
		F_static_submesh_headers submesh_headers_;
		u32 vertex_count_;
		u32 vertex_channel_count_;
		TU<A_static_mesh_buffer> buffer_p_;

	public:
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(const G_string& value) noexcept { name_ = value; }
		NCPP_FORCE_INLINE const F_indices& indices() const noexcept { return indices_; }
		NCPP_FORCE_INLINE const F_static_submesh_headers& submesh_headers() const noexcept { return submesh_headers_; }
		NCPP_FORCE_INLINE u32 vertex_count() const noexcept { return vertex_count_; }
		NCPP_FORCE_INLINE u32 vertex_channel_count() const noexcept { return vertex_channel_count_; }
		NCPP_FORCE_INLINE TK<A_static_mesh_buffer> buffer_p() const noexcept { return buffer_p_; }



	protected:
		A_static_mesh(
			const G_string& name,
			F_indices indices,
			F_static_submesh_headers submesh_headers
		);

	public:
		virtual ~A_static_mesh() = default;

	public:
		NCPP_OBJECT(A_static_mesh);

	public:
		virtual void release_data();

	};

	template<class F_vertex_channel_format_varg_list__, typename... F_vertex_channel_datas__>
	class TF_static_mesh : public A_static_mesh {

	public:
		using F_vertex_channel_data_targ_list = TF_template_targ_list<F_vertex_channel_datas__...>;

		using F_vertex_channels = TG_tuple<TG_vector<F_vertex_channel_datas__>...>;

		using F_buffer = TF_static_mesh_buffer<F_vertex_channel_format_varg_list__, F_vertex_channel_datas__...>;

	public:
		friend F_buffer;



	private:
		F_vertex_channels vertex_channels_;

	public:
		NCPP_FORCE_INLINE const F_vertex_channels& vertex_channels() const noexcept { return vertex_channels_; }



	public:
		TF_static_mesh(
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE,
			const G_string& name = ""
		) :
			A_static_mesh(
				name,
				{},
				{}
			)
		{
			vertex_count_ = 0;
			vertex_channel_count_ = sizeof...(F_vertex_channel_datas__);

			buffer_p_ = TU<F_buffer>()(NCPP_KTHIS());
			NCPP_FOH_VALID(buffer_p_).T_cast<F_buffer>()->upload_internal(
				additional_bind_flag,
				heap_type
			);
		}
		TF_static_mesh(
			const F_vertex_channels& vertex_channels,
			const F_indices& indices,
			const F_static_submesh_headers& submesh_headers,
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE,
			const G_string& name = ""
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

			buffer_p_ = TU<F_buffer>()(NCPP_KTHIS());
			NCPP_FOH_VALID(buffer_p_).T_cast<F_buffer>()->upload_internal(
				additional_bind_flag,
				heap_type
			);
		}
		~TF_static_mesh() {
		}

	public:
		NCPP_OBJECT(TF_static_mesh);

	public:
		void rebuild(
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		)
		{
			vertex_channels_ = F_vertex_channels();
			indices_ = {};
			submesh_headers_ = {};

			vertex_count_ = 0;

			NCPP_FOH_VALID(buffer_p_).T_cast<F_buffer>()->upload_internal(
				additional_bind_flag,
				heap_type
			);
		}
		void rebuild(
			const F_vertex_channels& vertex_channels,
			const F_indices& indices,
			const F_static_submesh_headers& submesh_headers,
			E_resource_bind_flag additional_bind_flag = E_resource_bind_flag::SRV,
			E_resource_heap_type heap_type = E_resource_heap_type::GREAD_GWRITE
		)
		{
			vertex_channels_ = vertex_channels;
			indices_ = indices;
			submesh_headers_ = submesh_headers;

			vertex_count_ = eastl::get<0>(vertex_channels).size();

			NCPP_FOH_VALID(buffer_p_).T_cast<F_buffer>()->upload_internal(
				additional_bind_flag,
				heap_type
			);
		}

	public:
		virtual void release_data() override {

			indices_.clear();
			submesh_headers_.clear();
			vertex_channels_ = {};
		}

	};



	using F_static_mesh_buffer = TF_static_mesh_buffer<
	    TF_template_varg_list<
	    	E_format::R32G32B32A32_FLOAT,
	    	E_format::R32G32B32A32_FLOAT,
	    	E_format::R32G32B32A32_FLOAT,
	    	E_format::R32G32_FLOAT
		>,
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;
	using F_static_mesh = TF_static_mesh<
		TF_template_varg_list<
			E_format::R32G32B32A32_FLOAT,
			E_format::R32G32B32A32_FLOAT,
			E_format::R32G32B32A32_FLOAT,
			E_format::R32G32_FLOAT
		>,
		F_vertex_position,
		F_vertex_normal,
		F_vertex_tangent,
		F_vertex_uv
	>;

}