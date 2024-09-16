#include <nre/io/data_stream.hpp>



namespace nre
{
    F_data_stream::F_data_stream(sz size, ptrd offset) :
        buffer_(size),
        offset_(offset)
    {
    }
    F_data_stream::F_data_stream(const TG_vector<u8>& buffer, ptrd offset) :
        buffer_(buffer),
        offset_(offset)
    {
    }
    F_data_stream::F_data_stream(TG_vector<u8>&& buffer, ptrd offset) :
        buffer_(eastl::move(buffer)),
        offset_(offset)
    {
    }
    F_data_stream::~F_data_stream()
    {
    }



    void F_data_stream::resize(sz size)
    {
        buffer_.resize(size);
    }
    void F_data_stream::jump(ptrd offset)
    {
        NCPP_ASSERT(offset <= size());
        offset_ = offset;
    }
}