#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset.hpp>



namespace nre {

	class NRE_API F_texture_2d_asset : public A_asset {

	private:
		U_texture_2d_handle texture_2d_p_;

	public:
		NCPP_FORCE_INLINE K_valid_texture_2d_handle texture_2d_p() const noexcept { return NCPP_FOH_VALID(texture_2d_p_); }



	public:
		F_texture_2d_asset();
		~F_texture_2d_asset();

	public:
		NCPP_DISABLE_COPY(F_texture_2d_asset);

	public:
		virtual void manual_build(const G_string& path) override;
		virtual b8 use_manual_build() override ;

	};

}