#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>



namespace nre {

	class NRE_API A_asset {

	public:
		friend class F_asset_system;



	protected:
		A_asset();

	public:
		virtual ~A_asset();

	public:
		NCPP_DISABLE_COPY(A_asset);

	protected:
		virtual void build(const F_asset_buffer& buffer);
		virtual void manual_build(V_string path);
		virtual b8 use_manual_build();

	};

}