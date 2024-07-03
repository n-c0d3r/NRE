#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/texel.hpp>



namespace nre {

	struct F_texture_2d_builder {

	public:
		struct F_no_texels {};
		static constexpr F_no_texels no_texels;



	private:
		u32 width_ = 0;
		u32 height_ = 0;
		F_texels texels_;

	public:
		NCPP_FORCE_INLINE u32 width() const noexcept { return width_; }
		NCPP_FORCE_INLINE u32 height() const noexcept { return height_; }
		NCPP_FORCE_INLINE const F_texels& texels() const noexcept { return texels_; }



	public:
		F_texture_2d_builder() = default;
		F_texture_2d_builder(u32 width, u32 height, F_no_texels) :
			width_(width),
			height_(height)
		{
		}
		F_texture_2d_builder(u32 width, u32 height) :
			width_(width),
			height_(height),
			texels_(width * height)
		{
		}
		F_texture_2d_builder(u32 width, u32 height, const F_texels& texels) :
			width_(width),
			height_(height),
			texels_(texels)
		{
			NCPP_ASSERT(texels_.size() == (width_ * height_)) << "invalid texels";
		}
		F_texture_2d_builder(u32 width, u32 height, F_texels&& texels) :
			width_(width),
			height_(height),
			texels_(std::move(texels))
		{
			NCPP_ASSERT(texels_.size() == (width_ * height_)) << "invalid texels";
		}
		F_texture_2d_builder(u32 width, u32 height, const TG_span<u8>& data, ED_format format) :
			width_(width),
			height_(height),
			texels_(width * height)
		{
			NCPP_ASSERT(data.size() >= (width_ * height_)) << "invalid texels";

			H_texel::store(data, texels_, format);
		}
		F_texture_2d_builder(PA_vector2_u size) :
			width_(size.x),
			height_(size.y),
			texels_(size.x * size.y)
		{
		}
		F_texture_2d_builder(PA_vector2_u size, F_no_texels) :
			width_(size.x),
			height_(size.y)
		{
		}
		F_texture_2d_builder(PA_vector2_u size, const F_texels& texels) :
			width_(size.x),
			height_(size.y),
			texels_(texels)
		{
			NCPP_ASSERT(texels_.size() == (width_ * height_)) << "invalid texels";
		}
		F_texture_2d_builder(PA_vector2_u size, F_texels&& texels) :
			width_(size.x),
			height_(size.y),
			texels_(std::move(texels))
		{
			NCPP_ASSERT(texels_.size() == (width_ * height_)) << "invalid texels";
		}
		F_texture_2d_builder(PA_vector2_u size, const TG_span<u8>& data, ED_format format) :
			width_(size.x),
			height_(size.y),
			texels_(size.x * size.y)
		{
			NCPP_ASSERT(data.size() >= (width_ * height_)) << "invalid texels";

			H_texel::store(data, texels_, format);
		}
		F_texture_2d_builder(const F_texture_2d_builder& x) :
			width_(x.width_),
			height_(x.height_),
			texels_(x.texels_)
		{
		}
		F_texture_2d_builder& operator = (const F_texture_2d_builder& x) {

			width_ = x.width_;
			height_ = x.height_;
			texels_ = x.texels_;

			return *this;
		}
		F_texture_2d_builder(F_texture_2d_builder&& x) :
			width_(x.width_),
			height_(x.height_),
			texels_(std::move(x.texels_))
		{
			x.width_ = 0;
			x.height_ = 0;
		}
		F_texture_2d_builder& operator = (F_texture_2d_builder&& x) {

			width_ = x.width_;
			height_ = x.height_;
			texels_ = std::move(x.texels_);

			x.width_ = 0;
			x.height_ = 0;

			return *this;
		}

	public:
		NCPP_FORCE_INLINE void resize(PA_vector2_u width_height) {

			width_ = width_height.x;
			height_ = width_height.y;
			texels_.resize(width_ * height_);
		}
		NCPP_FORCE_INLINE void resize(u32 w, u32 h) {

			width_ = w;
			height_ = h;
			texels_.resize(width_ * height_);
		}
		NCPP_FORCE_INLINE void resize(PA_vector2_u width_height, F_no_texels) {

			width_ = width_height.x;
			height_ = width_height.y;
			texels_.resize(0);
		}
		NCPP_FORCE_INLINE void resize(u32 w, u32 h, F_no_texels) {

			width_ = w;
			height_ = h;
			texels_.resize(0);
		}
		NCPP_FORCE_INLINE b8 is_valid() const noexcept {

			return (
				(width_ != 0)
				&& (height_ != 0)
			);
		}
		NCPP_FORCE_INLINE F_texel& at(PA_vector2_u coord) {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < height_) << "coord.y out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE const F_texel& at(PA_vector2_u coord) const {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < height_) << "coord.y out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE F_texel& operator [] (PA_vector2_u coord) {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < height_) << "coord.y out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.y * width_ + coord.x;

			return texels_[index];
		}
		NCPP_FORCE_INLINE const F_texel& operator [] (PA_vector2_u coord) const {

			NCPP_ASSERT(coord.x < width_) << "coord.x out of bound";
			NCPP_ASSERT(coord.y < height_) << "coord.y out of bound";
			NCPP_ASSERT(is_texels_available()) << "texels not available";

			u32 index = coord.y * width_ + coord.x;

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
		NCPP_FORCE_INLINE F_texture_2d_builder move_texels() {

			return {
				width_,
				height_,
				std::move(texels_)
			};
		}

	};

}