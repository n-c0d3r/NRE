#include <nre/rendering/newrg/unified_mesh_system.hpp>
#include <nre/rendering/newrg/unified_mesh_stream.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_loader_system.hpp>



namespace nre::newrg
{
    TK<F_unified_mesh_system> F_unified_mesh_system::instance_p_;



    F_unified_mesh_system::F_unified_mesh_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        NRE_ASSET_SYSTEM()->T_registry_asset_factory<F_unified_mesh_asset_factory>();

        NRE_FILE_SAVER_SYSTEM()->T_registry_saver<F_compressed_unified_mesh_data_file_saver>();
        NRE_FILE_LOADER_SYSTEM()->T_registry_loader<F_compressed_unified_mesh_data_file_loader>();

        stream_p_ = TU<F_unified_mesh_stream>()();
    }
    F_unified_mesh_system::~F_unified_mesh_system()
    {
    }



    void F_unified_mesh_system::RG_begin_register()
    {
        stream_p_->RG_begin_register();

        // process queue
        {
            NCPP_SCOPED_LOCK(lock_);

            if(command_queue_.size())
            {
                auto command = command_queue_.front();
                command_queue_.pop();

                switch (command.type)
                {
                case E_unified_mesh_command_type::UPLOAD_MESH:
                    stream_p_->upload_mesh(command.mesh_p, command.compressed_mesh_data);
                    break;
                case E_unified_mesh_command_type::TRY_FLUSH_MESH:
                    stream_p_->try_flush_mesh(command.mesh_p);
                    break;
                }
            }
        }
    }
    void F_unified_mesh_system::RG_end_register()
    {
        stream_p_->RG_end_register();
    }



    void F_unified_mesh_system::enqueue_command(const F_unified_mesh_command& command)
    {
        NCPP_SCOPED_LOCK(lock_);

        command_queue_.push(command);
    }
    void F_unified_mesh_system::enqueue_command(F_unified_mesh_command&& command)
    {
        NCPP_SCOPED_LOCK(lock_);

        command_queue_.push(eastl::move(command));
    }

    void F_unified_mesh_system::enqueue_upload(TSPA<F_unified_mesh> mesh_p, const F_compressed_unified_mesh_data& compressed_mesh_data)
    {
        enqueue_command({
            .type = E_unified_mesh_command_type::UPLOAD_MESH,
            .mesh_p = mesh_p,
            .compressed_mesh_data = compressed_mesh_data
        });
    }
    void F_unified_mesh_system::enqueue_upload(TSPA<F_unified_mesh> mesh_p, F_compressed_unified_mesh_data&& compressed_mesh_data)
    {
        enqueue_command({
            .type = E_unified_mesh_command_type::UPLOAD_MESH,
            .mesh_p = mesh_p,
            .compressed_mesh_data = eastl::move(compressed_mesh_data)
        });
    }
    void F_unified_mesh_system::enqueue_try_flush(TSPA<F_unified_mesh> mesh_p)
    {
        enqueue_command({
            .type = E_unified_mesh_command_type::TRY_FLUSH_MESH,
            .mesh_p = mesh_p
        });
    }
}