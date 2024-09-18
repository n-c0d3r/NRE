#include <nre/rendering/newrg/unified_mesh_stream.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>



namespace nre::newrg
{
    TK<F_unified_mesh_geometry_stream> F_unified_mesh_geometry_stream::instance_p_;



    F_unified_mesh_geometry_stream::F_unified_mesh_geometry_stream()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_unified_mesh_geometry_stream::~F_unified_mesh_geometry_stream()
    {
    }



    TK<F_unified_mesh_header_pool> F_unified_mesh_header_pool::instance_p_;



    F_unified_mesh_header_pool::F_unified_mesh_header_pool() :
        TF_cpu_gpu_data_pool<F_unified_mesh_header>(
            ED_resource_flag::SHADER_RESOURCE,
            NRE_NEWRG_UNIFIED_MESH_HEADER_POOL_PAGE_CAPACITY_IN_ELEMENTS
            NRE_OPTIONAL_DEBUG_PARAM("nre.newrg.unified_mesh_header_pool")
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_unified_mesh_header_pool::~F_unified_mesh_header_pool()
    {
    }



    TK<F_unified_mesh_stream> F_unified_mesh_stream::instance_p_;



    F_unified_mesh_stream::F_unified_mesh_stream()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        geometry_stream_p_ = TU<F_unified_mesh_geometry_stream>()();
        mesh_header_pool_p_ = TU<F_unified_mesh_header_pool>()();
    }
    F_unified_mesh_stream::~F_unified_mesh_stream()
    {
    }



    void F_unified_mesh_stream::RG_begin_register()
    {
    }
    void F_unified_mesh_stream::RG_end_register()
    {
        mesh_header_pool_p_->RG_begin_register_data();

        // process deregister_mesh_header_queue_
        {
            u32 mesh_header_id = NCPP_U32_MAX;
            while(deregister_mesh_header_queue_.size())
            {
                mesh_header_id = deregister_mesh_header_queue_.front();
                deregister_mesh_header_queue_.pop();

                NCPP_ASSERT(mesh_header_id != NCPP_U32_MAX);

                mesh_header_pool_p_->deregister_data(mesh_header_id);
            }
        }

        // process register_mesh_header_queue_
        {
            TS<F_unified_mesh> mesh_p;
            while(register_mesh_header_queue_.size())
            {
                mesh_p = register_mesh_header_queue_.front();
                register_mesh_header_queue_.pop();

                NCPP_ASSERT(mesh_p->last_frame_header_id_ == NCPP_U32_MAX);

                mesh_p->last_frame_header_id_ = mesh_header_pool_p_->register_data();
            }
        }

        mesh_header_pool_p_->RG_end_register_data();

        mesh_header_pool_p_->RG_begin_register_upload();

        // process upload_mesh_header_queue_
        {
            TS<F_unified_mesh> mesh_p;
            while(upload_mesh_header_queue_.size())
            {
                mesh_p = upload_mesh_header_queue_.front();
                upload_mesh_header_queue_.pop();

                NCPP_ASSERT(mesh_p->last_frame_header_id_ != NCPP_U32_MAX);

                F_unified_mesh_header mesh_header;

                mesh_header_pool_p_->register_upload(
                    mesh_p->last_frame_header_id_,
                    &mesh_header
                );
            }
        }

        mesh_header_pool_p_->RG_end_register_upload();
    }

    void F_unified_mesh_stream::enqueue_upload(TSPA<F_unified_mesh> mesh_p)
    {
        NCPP_ASSERT(mesh_p);

        register_mesh_header_queue_.push(mesh_p);
        upload_mesh_header_queue_.push(mesh_p);
    }
    void F_unified_mesh_stream::enqueue_flush(u32 mesh_header_id)
    {
        NCPP_ASSERT(mesh_header_id != NCPP_U32_MAX);

        deregister_mesh_header_queue_.push(mesh_header_id);
    }
}