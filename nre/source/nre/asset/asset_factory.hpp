#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>



namespace nre {

	class A_asset;



	enum class E_asset_build_mode
	{
		FROM_MEMORY,
		FROM_FILE
	};



	class NRE_API A_asset_factory {

	public:
		friend class F_asset_system;



	private:
		TG_vector<G_string> file_extensions_;

	public:
		const TG_vector<G_string> file_extensions() const noexcept { return file_extensions_; }



	protected:
		A_asset_factory(const TG_vector<G_string>& file_extensions);

	public:
		virtual ~A_asset_factory();

	public:
		NCPP_OBJECT(A_asset_factory);

	protected:
		virtual TS<A_asset> build_from_memory(const G_string& abs_path, const F_asset_buffer& buffer);
		virtual TS<A_asset> build_from_file(const G_string& abs_path);
		virtual E_asset_build_mode build_mode();

	};

}