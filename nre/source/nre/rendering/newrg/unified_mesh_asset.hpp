#pragma once

#include <nre/prerequisites.hpp>

#include <nre/asset/asset.hpp>
#include <nre/rendering/newrg/unified_mesh_data.hpp>
#include <nre/rendering/newrg/unified_mesh_builder.hpp>



namespace nre::newrg
{
    class F_unified_mesh;



    class NRE_API F_unified_mesh_asset : public A_asset
    {
    private:
        TS<F_unified_mesh> mesh_p_;

    public:
        NCPP_FORCE_INLINE const auto& mesh_p() const noexcept { return mesh_p_; }



    public:
        F_unified_mesh_asset(
            const G_string& abs_path,
            const F_compressed_unified_mesh_data& compressed_mesh_data
        );
        virtual ~F_unified_mesh_asset();

    public:
        NCPP_OBJECT(F_unified_mesh_asset);
    };



    class H_unified_mesh_asset
    {
    public:
        static TS<F_unified_mesh_asset> load(
            const G_string& path,
            const F_unified_mesh_build_options& options = H_unified_mesh_build_options::create_default(),
            const TG_span<G_string>& external_base_paths = {}
        );
    };
}
