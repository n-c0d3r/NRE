#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/general_texture_2d.hpp>



namespace nre {

	class NRE_API F_default_textures {

	private:
		static TK<F_default_textures> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_default_textures> instance_p() {

			return instance_ps;
		}



	private:
		TS<F_general_texture_2d> white_texture_2d_p_;
		TS<F_general_texture_2d> black_texture_2d_p_;
		TS<F_general_texture_2d> red_texture_2d_p_;
		TS<F_general_texture_2d> green_texture_2d_p_;
		TS<F_general_texture_2d> blue_texture_2d_p_;
		TS<F_general_texture_2d> alpha_texture_2d_p_;
		TS<F_general_texture_2d> default_normal_texture_2d_p_;

	public:
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& white_texture_2d_p() const noexcept { return white_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& black_texture_2d_p() const noexcept { return black_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& red_texture_2d_p() const noexcept { return red_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& green_texture_2d_p() const noexcept { return green_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& blue_texture_2d_p() const noexcept { return blue_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& alpha_texture_2d_p() const noexcept { return alpha_texture_2d_p_; }
		NCPP_FORCE_INLINE const TS<F_general_texture_2d>& default_normal_texture_2d_p() const noexcept { return default_normal_texture_2d_p_; }



	public:
		F_default_textures();

	public:
		NCPP_OBJECT(F_default_textures);

	};

}



#define NRE_DEFAULT_TEXTURES() (nre::F_default_textures::instance_p())