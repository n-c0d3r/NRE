#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/material_mask.hpp>
#include <nre/rendering/material.hpp>



namespace nre {

	class A_material;



	class NRE_API F_material_system {

	public:
		friend class A_material;



	private:
		static TK<F_material_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_material_system> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TK_valid<A_material>> material_p_list_;

		u8 last_channel_id_ = 0;
		TG_map<u64, u8> type_hash_code_to_channel_id_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TK_valid<A_material>>& material_p_list() const noexcept { return material_p_list_; }

		NCPP_FORCE_INLINE const TG_map<u64, u8>& type_hash_code_to_channel_id_map() const noexcept { return type_hash_code_to_channel_id_map_; }



	public:
		F_material_system();
		~F_material_system();

	public:
		NCPP_OBJECT(F_material_system);

	private:
		void registry(TKPA_valid<A_material> material_p);
		void deregistry(TKPA_valid<A_material> material_p);

	public:
		NCPP_FORCE_INLINE b8 is_has_channel_id(u64 type_hash_code) const noexcept {

			auto it = type_hash_code_to_channel_id_map_.find(type_hash_code);

			return (it != type_hash_code_to_channel_id_map_.end());
		}
		NCPP_FORCE_INLINE u8 channel_id(u64 type_hash_code) const noexcept {

			auto it = type_hash_code_to_channel_id_map_.find(type_hash_code);

			NCPP_ASSERT(it != type_hash_code_to_channel_id_map_.end()) << "channel " << T_cout_value(u32(type_hash_code)) << " not found";

			return it->second;
		}
		template<typename F__>
		NCPP_FORCE_INLINE u8 T_channel_id() const noexcept {

			constexpr u64 type_hash_code = T_type_hash_code<F__>;

			auto it = type_hash_code_to_channel_id_map_.find(type_hash_code);

			NCPP_ASSERT(it != type_hash_code_to_channel_id_map_.end()) << "channel " << T_cout_value(u32(type_hash_code)) << " not found";

			return it->second;
		}
		NCPP_FORCE_INLINE u8 register_channel_id(u64 type_hash_code) noexcept {

			NCPP_ASSERT(!is_has_channel_id(type_hash_code)) << "channel " << T_cout_value(u32(type_hash_code)) << " already registered";

			u8 id = last_channel_id_;
			type_hash_code_to_channel_id_map_[type_hash_code] = id;
			++last_channel_id_;

			return id;
		}
		template<typename F__>
		NCPP_FORCE_INLINE u8 T_register_channel_id() noexcept {

			constexpr u64 type_hash_code = T_type_hash_code<F__>;

			NCPP_ASSERT(!is_has_channel_id(type_hash_code)) << "channel " << T_cout_value(u32(type_hash_code)) << " already registered";

			u8 id = last_channel_id_;
			type_hash_code_to_channel_id_map_[type_hash_code] = id;
			++last_channel_id_;

			return id;
		}
		NCPP_FORCE_INLINE F_material_mask mask() noexcept {

			return 0;
		}
		NCPP_FORCE_INLINE F_material_mask mask(auto type_hash_code) noexcept
		{
			if(!is_has_channel_id(type_hash_code)) {

				return (1 << register_channel_id(type_hash_code));
			}

			return (1 << channel_id(type_hash_code));
		}
		NCPP_FORCE_INLINE F_material_mask mask(auto type_hash_code_1, auto type_hash_code_2, auto... reset_type_hash_codes) noexcept {

			return flag_combine(
				mask(type_hash_code_1),
				mask(type_hash_code_2),
				(
					mask(reset_type_hash_codes)
				)...
			);
		}
		template<typename... Fs__>
		NCPP_FORCE_INLINE F_material_mask T_mask() noexcept {

			return mask(T_type_hash_code<Fs__>...);
		}

		inline void for_each(auto&& functor) {

			for(const auto& material_p : material_p_list_) {

				functor(material_p);
			}
		}
		template<typename... Fs__>
		inline void T_for_each(auto&& functor) {

			auto search_mask = T_mask<Fs__...>();

			if(search_mask)
			{
				for(auto it = material_p_list_.begin(); it != material_p_list_.end(); ++it) {

					if((*it)->mask() & search_mask)
						functor(*it);
				}
			}
			else for(auto it = material_p_list_.begin(); it != material_p_list_.end(); ++it) {

				functor(*it);
			}
		}

	};

}



#define NRE_MATERIAL_SYSTEM(...) (nre::F_material_system::instance_p())