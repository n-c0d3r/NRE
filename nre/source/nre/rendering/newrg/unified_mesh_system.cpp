#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_loader_system.hpp>


namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system() :
        mesh_header_table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_UNIFIED_MESH_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.mesh_header_table")
        ),
        subpage_header_table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_UNIFIED_MESH_SUBPAGE_HEADER_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.subpage_header_table")
        ),
        cluster_table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_UNIFIED_MESH_CLUSTER_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.cluster_table")
        ),
        dag_table_(
            ED_resource_flag::SHADER_RESOURCE,
            ED_resource_heap_type::DEFAULT,
            ED_resource_state::COMMON,
            NRE_NEWRG_UNIFIED_MESH_DAG_TABLE_PAGE_CAPACITY_IN_ELEMENTS,
            0
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_system.dag_table")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_ASSET_SYSTEM()->T_registry_asset_factory<F_unified_mesh_asset_factory>();

        NRE_FILE_SAVER_SYSTEM()->T_registry_saver<F_compressed_unified_mesh_data_file_saver>();
        NRE_FILE_LOADER_SYSTEM()->T_registry_loader<F_compressed_unified_mesh_data_file_loader>();
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }



    void F_unified_mesh_system::update_internal(TSPA<F_unified_mesh> mesh_p)
    {
        register_mesh_header_queue_.push(mesh_p);
        upload_mesh_header_queue_.push(mesh_p);

        register_cluster_queue_.push(mesh_p);
        upload_cluster_queue_.push(mesh_p);

        register_dag_queue_.push(mesh_p);
        upload_dag_queue_.push(mesh_p);

        if(mesh_p->need_to_make_resident_)
            make_resident_internal(mesh_p);
    }
    void F_unified_mesh_system::make_resident_internal(TSPA<F_unified_mesh> mesh_p)
    {
        register_subpage_header_queue_.push(mesh_p);
        upload_subpage_header_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::evict_internal(u32 mesh_header_id)
    {
        auto& mesh_header = mesh_header_table_.T_element(mesh_header_id);

        deregister_subpage_header_queue_.push(mesh_header.subpage_offset);
    }
    void F_unified_mesh_system::flush_internal(u32 mesh_header_id)
    {
        auto& mesh_header = mesh_header_table_.T_element(mesh_header_id);

        deregister_mesh_header_queue_.push(mesh_header_id);
        deregister_cluster_queue_.push(mesh_header.subpage_offset);
        deregister_dag_queue_.push(mesh_header.subpage_offset);
    }



    void F_unified_mesh_system::RG_begin_register()
    {
    }
    void F_unified_mesh_system::RG_end_register()
    {
        auto render_graph_p = F_render_graph::instance_p();

        // external queues
        {
            // process evict_queue_
            {
                NCPP_SCOPED_LOCK(evict_lock_);

                while(evict_queue_.size())
                {
                    auto mesh_header_id = evict_queue_.front();
                    evict_queue_.pop();

                    evict_internal(mesh_header_id);
                }
            }

            // process flush_queue_
            {
                NCPP_SCOPED_LOCK(flush_lock_);

                while(flush_queue_.size())
                {
                    auto mesh_header_id = flush_queue_.front();
                    flush_queue_.pop();

                    flush_internal(mesh_header_id);
                }
            }

            // process update_queue_
            {
                NCPP_SCOPED_LOCK(update_lock_);

                while(update_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = update_queue_.front();
                    update_queue_.pop();

                    if(mesh_p->need_to_evict_)
                        evict_internal(mesh_p->last_frame_subpage_header_id_);

                    if(mesh_p->need_to_flush_)
                        flush_internal(mesh_p->last_frame_header_id_);

                    update_internal(mesh_p);
                }
            }
        }

        // internal queues
        {
            // process deregister_subpage_header_queue_
            {
                subpage_header_table_.RG_begin_register();

                while(deregister_subpage_header_queue_.size())
                {
                    u32 subpage_header_id = deregister_subpage_header_queue_.front();
                    deregister_subpage_header_queue_.pop();

                    subpage_header_table_.deregister_id(subpage_header_id);
                }

                subpage_header_table_.RG_end_register();
            }

            // process deregister_mesh_header_queue_
            {
                mesh_header_table_.RG_begin_register();

                while(deregister_mesh_header_queue_.size())
                {
                    u32 mesh_header_id = deregister_mesh_header_queue_.front();
                    deregister_mesh_header_queue_.pop();

                    mesh_header_table_.deregister_id(mesh_header_id);
                }

                mesh_header_table_.RG_end_register();
            }

            // process register_mesh_header_queue_
            {
                mesh_header_table_.RG_begin_register();

                while(register_mesh_header_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = register_mesh_header_queue_.front();
                    register_mesh_header_queue_.pop();

                    mesh_p->last_frame_header_id_ = mesh_header_table_.register_id();
                }

                mesh_header_table_.RG_end_register();
            }

            // process register_subpage_header_queue_
            {
                subpage_header_table_.RG_begin_register();

                while(register_subpage_header_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = register_subpage_header_queue_.front();
                    register_subpage_header_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 subpage_count = compressed_data.subpage_vertex_counts.size();

                    mesh_p->last_frame_subpage_header_id_ = subpage_header_table_.register_id(subpage_count);
                    mesh_p->temp_subpage_header_span_ = H_render_frame_allocator::T_create_array<F_unified_mesh_subpage_header>(
                        subpage_count
                    );

                    auto& mesh_header = mesh_header_table_.T_element(mesh_p->last_frame_header_id_);
                    mesh_header.subpage_offset = mesh_p->last_frame_subpage_header_id_;
                    mesh_header.subpage_count = subpage_count;
                }

                subpage_header_table_.RG_end_register();
            }

            // process upload_subpage_header_queue_
            {
                while(upload_subpage_header_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = upload_subpage_header_queue_.front();
                    upload_subpage_header_queue_.pop();

                    subpage_header_table_.T_upload(
                        mesh_p->last_frame_subpage_header_id_,
                        mesh_p->temp_subpage_header_span_
                    );
                }
            }

            // process register_cluster_queue_
            {
                cluster_table_.RG_begin_register();

                while(register_cluster_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = register_cluster_queue_.front();
                    register_cluster_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 cluster_count = compressed_data.cluster_headers.size();

                    mesh_p->last_frame_cluster_id_ = cluster_table_.register_id(cluster_count);

                    mesh_p->temp_cluster_header_span_ = H_render_frame_allocator::T_create_array<F_cluster_header>(
                        cluster_count,
                        (TG_vector<F_cluster_header>&)compressed_data.cluster_headers
                    );
                    mesh_p->temp_cluster_culling_data_span_ = H_render_frame_allocator::T_create_array<F_cluster_culling_data>(
                        cluster_count,
                        (TG_vector<F_cluster_culling_data>&)compressed_data.cluster_culling_datas
                    );

                    auto& mesh_header = mesh_header_table_.T_element(mesh_p->last_frame_header_id_);
                    mesh_header.cluster_offset = mesh_p->last_frame_cluster_id_;
                    mesh_header.cluster_count = cluster_count;
                }

                cluster_table_.RG_end_register();
            }

            // process upload_cluster_queue_
            {
                while(upload_cluster_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = upload_cluster_queue_.front();
                    upload_cluster_queue_.pop();

                    cluster_table_.T_upload<0>(
                        mesh_p->last_frame_cluster_id_,
                        mesh_p->temp_cluster_header_span_
                    );
                    cluster_table_.T_upload<1>(
                        mesh_p->last_frame_cluster_id_,
                        mesh_p->temp_cluster_culling_data_span_
                    );
                }
            }

            // process register_dag_queue_
            {
                dag_table_.RG_begin_register();

                while(register_dag_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = register_dag_queue_.front();
                    register_dag_queue_.pop();

                    auto& compressed_data = mesh_p->compressed_data();
                    u32 dag_node_count = compressed_data.dag_node_headers.size();

                    mesh_p->last_frame_dag_node_id_ = dag_table_.register_id(dag_node_count);

                    mesh_p->temp_dag_node_header_span_ = H_render_frame_allocator::T_create_array<F_dag_node_header>(
                        dag_node_count,
                        (TG_vector<F_dag_node_header>&)compressed_data.dag_node_headers
                    );
                    mesh_p->temp_dag_node_culling_data_span_ = H_render_frame_allocator::T_create_array<F_dag_node_culling_data>(
                        dag_node_count,
                        (TG_vector<F_dag_node_culling_data>&)compressed_data.dag_node_culling_datas
                    );
                    mesh_p->temp_dag_cluster_id_range_span_ = H_render_frame_allocator::T_create_array<F_cluster_id_range>(
                        dag_node_count,
                        (TG_vector<F_cluster_id_range>&)compressed_data.dag_cluster_id_ranges
                    );

                    auto& mesh_header = mesh_header_table_.T_element(mesh_p->last_frame_header_id_);
                    mesh_header.dag_node_offset = mesh_p->last_frame_dag_node_id_;
                    mesh_header.dag_node_count = dag_node_count;
                }

                dag_table_.RG_end_register();
            }

            // process upload_dag_queue_
            {
                while(upload_dag_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = upload_dag_queue_.front();
                    upload_dag_queue_.pop();

                    dag_table_.T_upload<0>(
                        mesh_p->last_frame_dag_node_id_,
                        mesh_p->temp_dag_node_header_span_
                    );
                    dag_table_.T_upload<1>(
                        mesh_p->last_frame_dag_node_id_,
                        mesh_p->temp_dag_node_culling_data_span_
                    );
                    dag_table_.T_upload<2>(
                        mesh_p->last_frame_dag_node_id_,
                        mesh_p->temp_dag_cluster_id_range_span_
                    );
                }
            }

            // process upload_mesh_header_queue_
            {
                mesh_header_table_.RG_begin_register_upload();

                while(upload_mesh_header_queue_.size())
                {
                    TS<F_unified_mesh> mesh_p = upload_mesh_header_queue_.front();
                    upload_mesh_header_queue_.pop();

                    mesh_header_table_.T_enqueue_upload(
                        mesh_p->last_frame_header_id_
                    );
                }

                mesh_header_table_.RG_end_register_upload();
            }
        }
    }



    void F_unified_mesh_system::enqueue_update(TSPA<F_unified_mesh> mesh_p)
    {
        NCPP_SCOPED_LOCK(update_lock_);

        update_queue_.push(mesh_p);
    }
    void F_unified_mesh_system::enqueue_flush(u32 mesh_header_id)
    {
        NCPP_SCOPED_LOCK(flush_lock_);

        flush_queue_.push(mesh_header_id);
    }
    void F_unified_mesh_system::enqueue_evict(u32 mesh_header_id)
    {
        NCPP_SCOPED_LOCK(evict_lock_);

        evict_queue_.push(mesh_header_id);
    }
}
