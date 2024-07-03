#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/texel.hpp>



namespace nre {

	struct F_texture_cube_builder {

	public:
		struct F_no_texels {};
		static constexpr F_no_texels no_texels;



	private:
		u32 width_ = 0;
		F_texels texels_;

	public:
		NCPP_FORCE_INLINE u32 width() const noexcept { return width_; }
		NCPP_FORCE_INLINE const F_texels& texels() const noexcept { return texels_; }



	public:
		F_texture_cube_builder() = default;
		F_texture_cube_builder(u32 width, F_no_texels) :
			width_(width)
		{
		}
		F_texture_cube_builder(u32 width) :
			width_(width),
			texels_(width * width * 6)
		{
		}
		F_texture_cube_builder(u32 width, const F_texels& texels) :
			width_(width),
			texels_(texels)
		{
			NCPP_ASSERT(texels_.size() == (width_ * width_ * 6)) << "invalid texels";
		}
		F_texture_cube_builder(u32 width, F_texels&& texels) :
			width_(width),
			texels_(std::move(texels))
		{
			NCPP_ASSERT(texels_.size() == (width_ * width * 6)) << "invalid texels";
		}
		F_texture_cube_builder(u32 width, const TG_span<u8>& data, ED_format format) :
			width_(width),
			texels_(width * width * 6)
		{
			if(data.size())
				H_texel::store(data, texels_, format);
		}
		F_texture_cube_builder(const F_texture_cube_builder& x) :
			width_(x.width_),
			texels_(x.texels_)
		{
		}
		F_texture_cube_builder& operator = (const F_texture_cube_builder& x) {

			width_ = x.width_;
			texels_ = x.texels_;

			return *this;
		}
		F_texture_cube_builder(F_texture_cube_builder&& x) :
			width_(x.width_),
			texels_(std::move(x.texels_))
		{
			x.width_ = 0;
		}
		F_texture_cube_builder& operator = (F_texture_cube_builder&& x) {

			width_ = x.width_;
			texels_ = std::move(x.texels_);

			x.width_ = 0;

			return *this;
		}

	public:
		NCPP_FORCE_INLINE void resize(u32 width) {

			width_ = width;
			texels_.resize(width_ * width_ * 6);
		}
		NCPP_FORCE_INLINE void resize(u32 width, F_no_texels) {

			width_ = width;
			texels_.resize(0);
		}
		NCPP_FORCE_INLINE b8 is_valid() const noexcept {

			return (width_ != 0);
		}
		NCPP_FORCE_INLINE F_texel& at(PA_vector3_u coord) {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < width_) << "coord.y out of bound";
			NCPP_ASSERT(coord.z < 6) << "coord.z out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.z * (width_ * width_) + coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE const F_texel& at(PA_vector3_u coord) const {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < width_) << "coord.y out of bound";
			NCPP_ASSERT(coord.z < 6) << "coord.z out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.z * (width_ * width_) + coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE F_texel& operator [] (PA_vector3_u coord) {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < width_) << "coord.y out of bound";
			NCPP_ASSERT(coord.z < 6) << "coord.z out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.z * (width_ * width_) + coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE const F_texel& operator [] (PA_vector3_u coord) const {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < width_) << "coord.y out of bound";
			NCPP_ASSERT(coord.z < 6) << "coord.z out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.z * (width_ * width_) + coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE TG_vector<u8> data(ED_format format) const {

			NCPP_ASSERT(is_texels_available()) << "texels not available";

			TG_vector<u8> result;

			H_texel::load(result, (F_texels&)texels_, format);

			return std::move(result);
		}
		NCPP_FORCE_INLINE b8 is_texels_available() const noexcept {

			return (texels_.size() != 0);
		}
		NCPP_FORCE_INLINE void release_texels() {

			texels_.clear();
		}
		NCPP_FORCE_INLINE F_texture_cube_builder move_texels() {

			return {
				width_,
				std::move(texels_)
			};
		}

	};

}