#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API F_data_stream
    {
    private:
        TG_vector<u8> buffer_;
        ptrd offset_ = 0;

    public:
        NCPP_FORCE_INLINE auto& buffer() noexcept { return buffer_; }
        NCPP_FORCE_INLINE const auto& buffer() const noexcept { return buffer_; }
        NCPP_FORCE_INLINE u8* root_data_p() const noexcept { return (u8*)buffer_.data(); }
        NCPP_FORCE_INLINE u8* current_data_p() const noexcept { return root_data_p() + offset_; }
        NCPP_FORCE_INLINE sz size() const noexcept { return buffer_.size(); }
        NCPP_FORCE_INLINE ptrd offset() const noexcept { return offset_; }



    public:
        F_data_stream(sz size = 0, ptrd offset = 0);
        F_data_stream(const TG_vector<u8>& buffer, ptrd offset = 0);
        F_data_stream(TG_vector<u8>&& buffer, ptrd offset = 0);
        virtual ~F_data_stream();

    public:
        NCPP_OBJECT(F_data_stream);



    public:
        void resize(sz size);
        void jump(ptrd offset);

    public:
        template<typename F_data__>
        F_data__& T_read_shallow()
        {
            sz data_size = sizeof(F_data__);

            offset_ = align_address(offset_, NCPP_ALIGNOF(F_data__));

            NCPP_ASSERT(offset_ + data_size <= size());

            ptrd last_offset = offset_;

            offset_ += data_size;

            return *((F_data__*)(root_data_p() + last_offset));
        }
        template<typename F_data__>
        TG_span<F_data__> T_read_span(sz count)
        {
            sz data_size = count * sizeof(F_data__);

            offset_ = align_address(offset_, NCPP_ALIGNOF(F_data__));

            NCPP_ASSERT(offset_ + data_size <= size());

            ptrd last_offset = offset_;

            offset_ += data_size;

            return {
                ((F_data__*)(root_data_p() + last_offset)),
                count
            };
        }
        template<typename F_data__>
        TG_vector<F_data__> T_read_vector(sz count)
        {
            auto span = T_read_span<F_data__>(count);

            return {
                span.begin(),
                span.end()
            };
        }

    public:
        template<typename F_data__>
        void T_write_shallow(const F_data__& data)
        {
            offset_ = align_address(offset_, NCPP_ALIGNOF(F_data__));

            sz data_size = sizeof(F_data__);
            sz min_buffer_size = offset_ + data_size;

            if(size() < min_buffer_size)
            {
                buffer_.resize(min_buffer_size);
            }

            memcpy(
                root_data_p() + offset_,
                (void*)&data,
                data_size
            );

            offset_ += data_size;
        }
        template<typename F_data__>
        void T_write_span(const TG_span<F_data__>& data_span)
        {
            offset_ = align_address(offset_, NCPP_ALIGNOF(F_data__));

            sz data_size = data_span.size() * sizeof(F_data__);
            sz min_buffer_size = offset_ + data_size;

            if(size() < min_buffer_size)
            {
                buffer_.resize(min_buffer_size);
            }

            memcpy(
                root_data_p() + offset_,
                (void*)data_span.data(),
                data_size
            );

            offset_ += data_size;
        }
        template<typename F_data__>
        void T_write_vector(const TG_vector<F_data__>& data_vector)
        {
            offset_ = align_address(offset_, NCPP_ALIGNOF(F_data__));

            sz data_size = data_vector.size() * sizeof(F_data__);
            sz min_buffer_size = offset_ + data_size;

            if(size() < min_buffer_size)
            {
                buffer_.resize(min_buffer_size);
            }

            memcpy(
                root_data_p() + offset_,
                (void*)data_vector.data(),
                data_size
            );

            offset_ += data_size;
        }

    public:
        NCPP_FORCE_INLINE void write(u8 data)
        {
            T_write_shallow<u8>(data);
        }
        NCPP_FORCE_INLINE void write(u16 data)
        {
            T_write_shallow<u16>(data);
        }
        NCPP_FORCE_INLINE void write(u32 data)
        {
            T_write_shallow<u32>(data);
        }
        NCPP_FORCE_INLINE void write(u64 data)
        {
            T_write_shallow<u64>(data);
        }
        NCPP_FORCE_INLINE void write(i8 data)
        {
            T_write_shallow<i8>(data);
        }
        NCPP_FORCE_INLINE void write(i16 data)
        {
            T_write_shallow<i16>(data);
        }
        NCPP_FORCE_INLINE void write(i32 data)
        {
            T_write_shallow<i32>(data);
        }
        NCPP_FORCE_INLINE void write(i64 data)
        {
            T_write_shallow<i64>(data);
        }

    public:
        NCPP_FORCE_INLINE void write(const G_string& data)
        {
            T_write_span<G_string::value_type>((G_string&)data);
        }
        NCPP_FORCE_INLINE void write(const G_wstring& data)
        {
            T_write_span<G_wstring::value_type>((G_wstring&)data);
        }

    public:
        NCPP_FORCE_INLINE void write(const TG_vector<u8>& data)
        {
            T_write_span<u8>((TG_vector<u8>&)data);
        }
    };
}