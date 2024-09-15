#include <nre/rendering/newrg/unified_mesh_asset.hpp>
#include <nre/rendering/newrg/unified_mesh_asset_factory.hpp>
#include <nre/rendering/newrg/unified_mesh.hpp>
#include <nre/asset/asset_system.hpp>
#include <nre/asset/static_mesh_asset.hpp>
#include <nre/rendering/static_mesh.hpp>
#include <nre/io/path.hpp>


namespace nre::newrg
{
    F_unified_mesh_asset::F_unified_mesh_asset(
        const G_string& abs_path,
        const F_compressed_unified_mesh_data& compressed_mesh_data
    ) :
        A_asset(abs_path),
        mesh_p_(TS<F_unified_mesh>()(compressed_mesh_data))
    {
    }
    F_unified_mesh_asset::~F_unified_mesh_asset()
    {
    }



    TS<F_unified_mesh_asset> H_unified_mesh_asset::load(
        const G_string& path,
        const F_unified_mesh_build_options& options,
        const TG_span<G_string>& external_base_paths
    )
    {
        //
        auto file_extension = H_path::extension(path);
        if(file_extension == NRE_NEWRG_UNIFIED_MESH_ASSET_FACTORY_FILE_EXTENSION)
        {
            return NRE_ASSET_SYSTEM()->load_asset(file_extension).T_cast<F_unified_mesh_asset>();
        }

        //
        G_string asset_name_without_extension = H_path::remove_extension(
            H_path::file_name(path)
        );
        G_string new_path = H_path::base_name(path) + "/" + asset_name_without_extension + "." + NRE_NEWRG_UNIFIED_MESH_ASSET_FACTORY_FILE_EXTENSION;

        //
        if(auto pre_built_asset_p = NRE_ASSET_SYSTEM()->load_asset(new_path))
        {
            return pre_built_asset_p.T_cast<F_unified_mesh_asset>();
        }

        //
        auto original_mesh_asset_p = NRE_ASSET_SYSTEM()->load_asset(path);

        TG_vector<F_vector3_f32> positions;
        TG_vector<F_vector3_f32> normals;
        TG_vector<F_vector3_f32> tangents;
        TG_vector<F_vector2_f32> texcoords;
        TG_vector<u32> indices;

        // static mesh asset
        {
            TK<F_static_mesh_asset> static_mesh_asset_p;
            if(original_mesh_asset_p.T_try_interface<F_static_mesh_asset>(static_mesh_asset_p))
            {
                auto& mesh_p = static_mesh_asset_p->mesh_p;

                // default static mesh
                {
                    TK<F_static_mesh> static_mesh_p;
                    if(mesh_p.T_try_interface<F_static_mesh>(static_mesh_p))
                    {
                        positions = eastl::get<NRE_STATIC_MESH_DEFAULT_VERTEX_CHANNEL_INDEX_POSITION>(
                            static_mesh_p->vertex_channels()
                        );
                        normals = eastl::get<NRE_STATIC_MESH_DEFAULT_VERTEX_CHANNEL_INDEX_NORMAL>(
                            static_mesh_p->vertex_channels()
                        );
                        tangents = eastl::get<NRE_STATIC_MESH_DEFAULT_VERTEX_CHANNEL_INDEX_TANGENT>(
                            static_mesh_p->vertex_channels()
                        );
                        texcoords = eastl::get<NRE_STATIC_MESH_DEFAULT_VERTEX_CHANNEL_INDEX_TEXCOORD>(
                            static_mesh_p->vertex_channels()
                        );
                        indices = static_mesh_p->indices();
                        goto finalize_loading;
                    }
                }
            }
        }

        //
        return null;

        //
        finalize_loading:

        F_raw_unified_mesh_data raw_data = H_unified_mesh_builder::build(
            positions,
            normals,
            tangents,
            texcoords,
            indices,
            options
        );

        F_compressed_unified_mesh_data compressed_data = H_unified_mesh_builder::compress(raw_data);

        return null;
        // return TS<F_unified_mesh_asset>()(abs_path, compressed_data);
    }
}
