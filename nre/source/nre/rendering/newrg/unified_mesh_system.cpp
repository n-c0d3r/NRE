#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_loader_system.hpp>


namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system() :
        mesh_table_(
            {
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED
            },
            {
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT
            },
            {
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE
            },
            NRE_NEWRG_UNIFIED_MESH_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0,
            NRE_NEWRG_UNIFIED_MESH_TABLE_SUBPAGE_COUNT_PER_PAGE
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.mesh_table")
        ),
        subpage_header_table_(
            { ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED },
            { ED_resource_heap_type::DEFAULT },
            { ED_resource_state::ALL_SHADER_RESOURCE },
            NRE_NEWRG_UNIFIED_MESH_SUBPAGE_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.subpage_header_table")
        ),
        cluster_table_(
            {
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED,
                ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED
            },
            {
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT,
                ED_resource_heap_type::DEFAULT
            },
            {
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE,
                ED_resource_state::ALL_SHADER_RESOURCE
            },
            NRE_NEWRG_UNIFIED_MESH_CLUSTER_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.cluster_table")
        ),
        vertex_data_table_(
            { ED_resource_flag::SHADER_RESOURCE | ED_resource_flag::STRUCTURED },
            { ED_resource_heap_type::DEFAULT },
            { ED_resource_state::ALL_SHADER_RESOURCE },
            NRE_NEWRG_UNIFIED_MESH_VERTEX_DATA_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.vertex_data_table")
        ),
        triangle_vertex_id_table_(
            { ED_resource_flag::SHADER_RESOURCE },
            { ED_resource_heap_type::DEFAULT },
            { ED_resource_state::ALL_SHADER_RESOURCE },
            NRE_NEWRG_UNIFIED_MESH_TRIANGLE_VERTEX_ID_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.triangle_vertex_id_table")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_ASSET_SYSTEM()->T_registry_asset_factory<F_unified_mesh_asset_factory>();

        NRE_FILE_SAVER_SYSTEM()->T_register_saver<F_compressed_unified_mesh_data_file_saver>();
        NRE_FILE_LOADER_SYSTEM()->T_register_loader<F_compressed_unified_mesh_data_file_loader>();

        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_TABLE_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_TABLE_PAGE_CAPACITY_IN_ELEMENTS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_SUBPAGE_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_CLUSTER_TABLE_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_CLUSTER_TABLE_PAGE_CAPACITY_IN_ELEMENTS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_MAX_VERTEX_COUNT_PER_CLUSTER)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_MAX_TRIANGLE_COUNT_PER_CLUSTER)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_VERTEX_DATA_TABLE_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_VERTEX_DATA_TABLE_PAGE_CAPACITY_IN_ELEMENTS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_TRIANGLE_VERTEX_ID_TABLE_PAGE_CAPACITY_IN_ELEMENTS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_TRIANGLE_VERTEX_ID_TABLE_PAGE_CAPACITY_IN_ELEMENTS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_SUBPAGE_CAPACITY_IN_CLUSTERS)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_MAX_CLUSTER_CHILD_COUNT)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_CLUSTER_CHILD_ID_PACK_COUNT",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_CLUSTER_CHILD_ID_PACK_COUNT)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "NRE_NEWRG_UNIFIED_MESH_CLUSTER_GROUP_LEVEL_COUNT",
            G_to_string(NRE_NEWRG_UNIFIED_MESH_CLUSTER_GROUP_LEVEL_COUNT)
        });
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }



    void F_unified_mesh_system::update_internal(TKPA<F_unified_mesh> mesh_p)
    {
        register_mesh_queue_.push(mesh_p);
        upload_mesh_queue_.push(mesh_p);

        register_cluster_queue_.push(mesh_p);
        upload_cluster_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::make_subpages_resident_internal(TKPA<F_unified_mesh> mesh_p)
    {
        register_subpage_header_queue_.push(mesh_p);
        upload_subpage_header_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::evict_subpages_internal(const F_unified_mesh_evict_subpages_params& input)
    {
        deregister_subpage_header_queue_.push(input);
    }
    void F_unified_mesh_system::flush_internal(u32 mesh_header_id)
    {
        auto& mesh_header = mesh_table_.T_element(mesh_header_id);

        deregister_mesh_queue_.push(mesh_header_id);
        deregister_cluster_queue_.push(mesh_header.subpage_offset);
    }



    void F_unified_mesh_system::RG_begin_register()
    {
        mesh_table_render_bind_list_p_ = 0;
        subpage_header_table_render_bind_list_p_ = 0;
        cluster_table_render_bind_list_p_ = 0;
        vertex_data_table_render_bind_list_p_ = 0;
        triangle_vertex_id_table_render_bind_list_p_ = 0;
    }
    void F_unified_mesh_system::RG_end_register()
    {
        auto render_graph_p = F_render_graph::instance_p();

        // external queues
        {
            // process evict_subpages_queue_
            {
                NCPP_SCOPED_LOCK(evict_subpages_lock_);

                while(evict_subpages_queue_.size())
                {
                    auto subpage_id_and_subpage_headers = evict_subpages_queue_.front();
                    evict_subpages_queue_.pop();

                    evict_subpages_internal(subpage_id_and_subpage_headers);
                }
            }

            // process flush_queue_
            {
                NCPP_SCOPED_LOCK(flush_lock_);

                while(flush_queue_.size())
                {
                    auto mesh_id = flush_queue_.front();
                    flush_queue_.pop();

                    flush_internal(mesh_id);
                }
            }

            // process update_queue_
            {
                NCPP_SCOPED_LOCK(update_lock_);

                while(update_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = update_queue_.front();
                    update_queue_.pop();

                    if(!mesh_p)
                        continue;

                    if(mesh_p->need_to_evict_subpages_)
                    {
                        evict_subpages_internal({
                            mesh_p->last_frame_id_,
                            mesh_p->last_frame_subpage_header_id_,
                            mesh_p->last_frame_subpage_headers_
                        });
                        final_evict_subpages_queue_.push(mesh_p);
                    }

                    if(mesh_p->need_to_flush_)
                    {
                        flush_internal(mesh_p->last_frame_id_);
                        final_flush_queue_.push(mesh_p);
                    }

                    if(mesh_p->need_to_upload_)
                    {
                        update_internal(mesh_p);
                    }

                    if(mesh_p->need_to_make_subpages_resident_)
                    {
                        make_subpages_resident_internal(mesh_p);
                    }
                }
            }
        }

        // internal queues
        {
            // process deregister_subpage_header_queue_
            {
                subpage_header_table_.RG_begin_register();
                vertex_data_table_.RG_begin_register();
                triangle_vertex_id_table_.RG_begin_register();

                while(deregister_subpage_header_queue_.size())
                {
                    auto& subpage_header_id_and_subpage_headers = deregister_subpage_header_queue_.front();

                    subpage_header_table_.deregister_id(subpage_header_id_and_subpage_headers.subpage_id);

                    for(auto& subpage_header : subpage_header_id_and_subpage_headers.subpage_headers)
                    {
                        vertex_data_table_.deregister_id(subpage_header.vertex_offset);
                        triangle_vertex_id_table_.deregister_id(subpage_header.local_cluster_triangle_vertex_id_offset);
                    }

                    deregister_subpage_header_queue_.pop();
                }

                triangle_vertex_id_table_.RG_end_register();
                vertex_data_table_.RG_end_register();
                subpage_header_table_.RG_end_register();
            }

            // process deregister_mesh_header_queue_
            {
                mesh_table_.RG_begin_register();

                while(deregister_mesh_queue_.size())
                {
                    u32 mesh_id = deregister_mesh_queue_.front();
                    deregister_mesh_queue_.pop();

                    mesh_table_.deregister_id(mesh_id);
                }

                mesh_table_.RG_end_register();
            }

            // process register_mesh_queue_
            {
                mesh_table_.RG_begin_register();

                while(register_mesh_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = register_mesh_queue_.front();
                    register_mesh_queue_.pop();

                    mesh_p->last_frame_id_ = mesh_table_.register_id();
                }

                mesh_table_.RG_end_register();
            }

            // process register_subpage_header_queue_
            {
                subpage_header_table_.RG_begin_register();
                vertex_data_table_.RG_begin_register();
                triangle_vertex_id_table_.RG_begin_register();

                while(register_subpage_header_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = register_subpage_header_queue_.front();
                    register_subpage_header_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 subpage_count = compressed_data.subpage_vertex_counts.size();

                    mesh_p->last_frame_subpage_header_id_ = subpage_header_table_.register_id(subpage_count);
                    mesh_p->last_frame_subpage_headers_.resize(subpage_count);

                    //
                    for(u32 subpage_index = 0; subpage_index < subpage_count; ++subpage_index)
                    {
                        auto& subpage_header = mesh_p->last_frame_subpage_headers_[subpage_index];

                        subpage_header.vertex_count = compressed_data.subpage_vertex_counts[subpage_index];
                        subpage_header.vertex_offset = vertex_data_table_.register_id(
                            subpage_header.vertex_count
                        );

                        subpage_header.local_cluster_triangle_vertex_id_count = compressed_data.subpage_local_cluster_triangle_vertex_id_counts[subpage_index];
                        subpage_header.local_cluster_triangle_vertex_id_offset = triangle_vertex_id_table_.register_id(
                            subpage_header.local_cluster_triangle_vertex_id_count
                        );
                    }

                    auto& mesh_header = mesh_table_.T_element(mesh_p->last_frame_id_);
                    mesh_header.subpage_offset = mesh_p->last_frame_subpage_header_id_;
                    mesh_header.subpage_count = subpage_count;
                }

                triangle_vertex_id_table_.RG_end_register();
                vertex_data_table_.RG_end_register();
                subpage_header_table_.RG_end_register();
            }

            // process upload_subpage_header_queue_
            {
                while(upload_subpage_header_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = upload_subpage_header_queue_.front();
                    upload_subpage_header_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 subpage_count = compressed_data.subpage_vertex_counts.size();

                    //
                    sz local_vertex_offset = 0;
                    sz local_triangle_vertex_id_offset = 0;
                    for(u32 subpage_index = 0; subpage_index < subpage_count; ++subpage_index)
                    {
                        auto& subpage_header = mesh_p->last_frame_subpage_headers_[subpage_index];

                        vertex_data_table_.T_upload(
                            subpage_header.vertex_offset,
                            {
                                (F_compressed_vertex_data*)(compressed_data.vertex_datas.data() + local_vertex_offset),
                                sz(subpage_header.vertex_count)
                            }
                        );
                        triangle_vertex_id_table_.T_upload(
                            subpage_header.local_cluster_triangle_vertex_id_offset,
                            {
                                (F_compressed_local_cluster_vertex_id*)(compressed_data.local_cluster_triangle_vertex_ids.data() + local_triangle_vertex_id_offset),
                                sz(subpage_header.local_cluster_triangle_vertex_id_count)
                            }
                        );

                        local_vertex_offset += subpage_header.vertex_count;
                        local_triangle_vertex_id_offset += subpage_header.local_cluster_triangle_vertex_id_count;
                    }

                    subpage_header_table_.T_upload(
                        mesh_p->last_frame_subpage_header_id_,
                        mesh_p->last_frame_subpage_headers_
                    );
                }
            }

            // process register_cluster_queue_
            {
                cluster_table_.RG_begin_register();

                while(register_cluster_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = register_cluster_queue_.front();
                    register_cluster_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 cluster_count = compressed_data.cluster_headers.size();

                    mesh_p->last_frame_cluster_id_ = cluster_table_.register_id(cluster_count);

                    auto& last_cluster_level_header = compressed_data.cluster_level_headers.back();

                    auto& mesh_header = mesh_table_.T_element(mesh_p->last_frame_id_);
                    mesh_header.cluster_offset = mesh_p->last_frame_cluster_id_;
                    mesh_header.cluster_count = cluster_count;
                    mesh_header.root_cluster_offset = mesh_header.cluster_offset + last_cluster_level_header.begin;
                    mesh_header.root_cluster_count = last_cluster_level_header.end - last_cluster_level_header.begin;
                }

                cluster_table_.RG_end_register();
            }

            // process upload_cluster_queue_
            {
                while(upload_cluster_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = upload_cluster_queue_.front();
                    upload_cluster_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();

                    cluster_table_.T_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HEADER>(
                        mesh_p->last_frame_cluster_id_,
                        (TG_vector<F_cluster_header>&)compressed_data.cluster_headers
                    );
                    cluster_table_.T_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_BBOX>(
                        mesh_p->last_frame_cluster_id_,
                        (TG_vector<F_box_f32>&)compressed_data.cluster_bboxes
                    );
                    cluster_table_.T_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_HIERARCHICAL_CULLING_DATA>(
                        mesh_p->last_frame_cluster_id_,
                        (TG_vector<F_cluster_hierarchical_culling_data>&)compressed_data.cluster_hierarchical_culling_datas
                    );
                    cluster_table_.T_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_CLUSTER_TABLE_ROW_INDEX_NODE_HEADER>(
                        mesh_p->last_frame_cluster_id_,
                        (TG_vector<F_cluster_node_header>&)compressed_data.cluster_node_headers
                    );
                }
            }

            // process upload_mesh_queue_
            {
                mesh_table_.RG_begin_register_upload();

                while(upload_mesh_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = upload_mesh_queue_.front();
                    upload_mesh_queue_.pop();

                    mesh_table_.T_enqueue_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_HEADER>(
                        mesh_p->last_frame_id_
                    );
                    mesh_table_.T_enqueue_upload<NRE_NEWRG_UNIFIED_MESH_SYSTEM_MESH_TABLE_ROW_INDEX_CULLING_DATA>(
                        mesh_p->last_frame_id_,
                        mesh_p->compressed_data().culling_data
                    );
                }

                mesh_table_.RG_end_register_upload();
            }

            // process final_evict_subpages_queue_
            {
                while(final_evict_subpages_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = final_evict_subpages_queue_.front();
                    final_evict_subpages_queue_.pop();

                    mesh_p->last_frame_subpage_header_id_ = NCPP_U32_MAX;
                }
            }

            // process final_flush_queue_
            {
                while(final_flush_queue_.size())
                {
                    TK<F_unified_mesh> mesh_p = final_flush_queue_.front();
                    final_flush_queue_.pop();

                    mesh_p->last_frame_id_ = NCPP_U32_MAX;
                    mesh_p->last_frame_cluster_id_ = NCPP_U32_MAX;
                    mesh_p->last_frame_cluster_id_ = NCPP_U32_MAX;
                }
            }
        }

        // create render bind lists
        {
            mesh_table_render_bind_list_p_ = render_graph_p->T_create<F_mesh_table_render_bind_list>(
                &mesh_table_,
                TG_array<ED_resource_view_type, 2>({
                    ED_resource_view_type::SHADER_RESOURCE,
                    ED_resource_view_type::SHADER_RESOURCE
                }),
                TG_array<ED_resource_flag, 2>({
                    ED_resource_flag::NONE,
                    ED_resource_flag::NONE
                }),
                TG_array<ED_format, 2>({
                    ED_format::NONE,
                    ED_format::NONE
                })
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.mesh_table_render_bind_list")
            );
            subpage_header_table_render_bind_list_p_ = render_graph_p->T_create<F_subpage_header_table_render_bind_list>(
                &subpage_header_table_,
                TG_array<ED_resource_view_type, 1>({ ED_resource_view_type::SHADER_RESOURCE }),
                TG_array<ED_resource_flag, 1>({ ED_resource_flag::NONE }),
                TG_array<ED_format, 1>({ ED_format::NONE })
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.subpage_header_table_render_bind_list")
            );
            cluster_table_render_bind_list_p_ = render_graph_p->T_create<F_cluster_table_render_bind_list>(
                &cluster_table_,
                TG_array<ED_resource_view_type, 4>({
                    ED_resource_view_type::SHADER_RESOURCE,
                    ED_resource_view_type::SHADER_RESOURCE,
                    ED_resource_view_type::SHADER_RESOURCE,
                    ED_resource_view_type::SHADER_RESOURCE
                }),
                TG_array<ED_resource_flag, 4>({
                    ED_resource_flag::NONE,
                    ED_resource_flag::NONE,
                    ED_resource_flag::NONE,
                    ED_resource_flag::NONE
                }),
                TG_array<ED_format, 4>({
                    ED_format::NONE,
                    ED_format::NONE,
                    ED_format::NONE,
                    ED_format::NONE
                })
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.cluster_table_render_bind_list")
            );
            vertex_data_table_render_bind_list_p_ = render_graph_p->T_create<F_vertex_data_table_render_bind_list>(
                &vertex_data_table_,
                TG_array<ED_resource_view_type, 1>({ ED_resource_view_type::SHADER_RESOURCE }),
                TG_array<ED_resource_flag, 1>({ ED_resource_flag::NONE }),
                TG_array<ED_format, 1>({ ED_format::NONE })
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.vertex_data_table_render_bind_list")
            );
            triangle_vertex_id_table_render_bind_list_p_ = render_graph_p->T_create<F_triangle_vertex_id_table_render_bind_list>(
                &triangle_vertex_id_table_,
                TG_array<ED_resource_view_type, 1>({ ED_resource_view_type::SHADER_RESOURCE }),
                TG_array<ED_resource_flag, 1>({ ED_resource_flag::NONE }),
                TG_array<ED_format, 1>({ ED_format::R8_UINT })
                NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.triangle_vertex_id_table_render_bind_list")
            );
        }
    }



    void F_unified_mesh_system::enqueue_update(TKPA<F_unified_mesh> mesh_p)
    {
        NCPP_SCOPED_LOCK(update_lock_);

        update_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::enqueue_flush(u32 mesh_id)
    {
        NCPP_SCOPED_LOCK(flush_lock_);

        flush_queue_.push(mesh_id);
    }
    void F_unified_mesh_system::enqueue_evict_subpages(const F_unified_mesh_evict_subpages_params& params)
    {
        NCPP_SCOPED_LOCK(evict_subpages_lock_);

        evict_subpages_queue_.push(params);
    }
}
