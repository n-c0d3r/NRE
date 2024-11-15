#include <nre/rendering/newrg/abytek_opaque_drawable_material_shader_asset.hpp>
#include <nre/rendering/nsl_shader_system.hpp>
#include <nre/io/path.hpp>



namespace nre::newrg
{
    F_abytek_opaque_drawable_material_shader_asset::F_abytek_opaque_drawable_material_shader_asset(
        const G_string& abs_path,
        E_abytek_drawable_material_flag flags,
        const G_string& src_content
    ) :
        F_abytek_drawable_material_shader_asset(abs_path, flags)
    {
    }
    F_abytek_opaque_drawable_material_shader_asset::~F_abytek_opaque_drawable_material_shader_asset()
    {
    }
}
