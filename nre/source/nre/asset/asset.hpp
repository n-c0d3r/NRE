#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>



namespace nre {

	class A_asset_factory;



	class NRE_API A_asset : public A_object
	{
	public:
		friend class F_asset_system;
		friend class A_asset_factory;
	
	private:
		G_string abs_path_;

	public:
		NCPP_FORCE_INLINE const G_string& abs_path() const noexcept { return abs_path_; }



	protected:
		A_asset(const G_string& abs_path);

	public:
		virtual ~A_asset();

	public:
		NCPP_OBJECT(A_asset);

	};

}