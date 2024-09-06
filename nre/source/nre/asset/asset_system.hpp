#pragma once

#include <nre/prerequisites.hpp>
#include <nre/asset/asset_buffer.hpp>
#include <nre/asset/path.hpp>



namespace nre {

	class A_asset;
	class A_asset_factory;



	class NRE_API F_asset_system {

	private:
		static TK<F_asset_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_asset_system> instance_p() {

			return instance_ps;
		}



	private:
		TG_vector<TU<A_asset_factory>> asset_factory_p_vector_;
		TG_unordered_map<G_string, TK<A_asset_factory>> asset_factory_map_;



	public:
		F_asset_system();
		~F_asset_system();

	public:
		NCPP_OBJECT(F_asset_system);

	public:
		template<typename F_asset_factory__>
		requires T_is_object_down_castable<F_asset_factory__, A_asset_factory>
		TK_valid<F_asset_factory__> T_registry_asset_factory()
		{
			auto asset_factory_p = TU<F_asset_factory__>()();
			auto keyed_asset_factory_p = NCPP_FOH_VALID(asset_factory_p);

			asset_factory_p_vector_.push_back(
				std::move(asset_factory_p)
			);

			for(const auto& file_extension : keyed_asset_factory_p->file_extensions())
				asset_factory_map_[file_extension] = keyed_asset_factory_p.no_requirements();

			return keyed_asset_factory_p;
		}
		TK<A_asset_factory> find_asset_factory(const G_string& file_extension);

	public:
		TS<A_asset> load_asset(const G_string& path, const TG_span<G_string>& external_base_paths = {});
		TS<A_asset> load_asset(const G_string& path, const G_string& overrided_file_extension, const TG_span<G_string>& external_base_paths = {});
		TS<A_asset> load_asset(const G_string& path, TK_valid<A_asset_factory> asset_factory_p, const TG_span<G_string>& external_base_paths = {});

	public:
		TS<A_asset> load_asset_from_abs_path(const G_string& abs_path);
		TS<A_asset> load_asset_from_abs_path(const G_string& abs_path, const G_string& overrided_file_extension);
		TS<A_asset> load_asset_from_abs_path(const G_string& abs_path, TK_valid<A_asset_factory> asset_factory_p);

	};

}



#define NRE_ASSET_SYSTEM(...) (nre::F_asset_system::instance_p())