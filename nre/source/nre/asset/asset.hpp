#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>



namespace nre {

	class A_asset {

	public:
		friend class F_asset_system;



	protected:
		A_asset();

	public:
		virtual ~A_asset();

	protected:
		virtual void build(const F_asset_buffer& buffer) = 0;

	};

}