#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>
#include <nre/rendering/static_mesh.hpp>



namespace nre {

	class NRE_API F_obj_mesh_asset : public A_asset {

	private:
		TU<F_static_mesh> mesh_p_;

	public:
		NCPP_FORCE_INLINE TK_valid<F_static_mesh> mesh_p() const noexcept { return NCPP_FOH_VALID(mesh_p_); }



	public:
		F_obj_mesh_asset();
		~F_obj_mesh_asset();

	public:
		NCPP_DISABLE_COPY(F_obj_mesh_asset);

	public:
		virtual void manual_build(const G_string& path) override;
		virtual b8 use_manual_build() override ;

	};

}