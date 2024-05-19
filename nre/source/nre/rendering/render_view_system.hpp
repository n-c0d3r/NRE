#pragma once

#include <nre/prerequisites.hpp>
#include <nre/rendering/render_view_mask.hpp>



namespace nre {

	class F_render_view;



	class NRE_API F_render_view_system {

	public:
		friend class A_render_view;



	private:
		static TK<F_render_view_system> instance_ps;

	public:
		static NCPP_FORCE_INLINE TK<F_render_view_system> instance_p() {

			return instance_ps;
		}



	private:
		TG_list<TK_valid<A_render_view>> render_view_p_list_;

		u8 last_channel_id_ = 0;
		TG_map<u64, u8> type_hash_code_to_channel_id_map_;

	public:
		NCPP_FORCE_INLINE const TG_list<TK_valid<A_render_view>>& render_view_p_list() const noexcept { return render_view_p_list_; }

		NCPP_FORCE_INLINE const TG_map<u64, u8>& type_hash_code_to_channel_id_map() const noexcept { return type_hash_code_to_channel_id_map_; }



	public:
		F_render_view_system();
		~F_render_view_system();

	public:
		NCPP_OBJECT(F_render_view_system);

	private:
		void registry(TKPA_valid<A_render_view> render_view_p);
		void deregistry(TKPA_valid<A_render_view> render_view_p);

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
		NCPP_FORCE_INLINE u8 channel_id(u64 type_hash_code) noexcept {

			auto it = type_hash_code_to_channel_id_map_.find(type_hash_code);

			if(it != type_hash_code_to_channel_id_map_.end())
				return it->second;

			return it->second;
		}
		template<typename F__>
		NCPP_FORCE_INLINE u8 T_channel_id() noexcept {

			constexpr u64 type_hash_code = T_type_hash_code<F__>;

			auto it = type_hash_code_to_channel_id_map_.find(type_hash_code);

			if(it != type_hash_code_to_channel_id_map_.end())
				return it->second;

			return it->second;
		}
		NCPP_FORCE_INLINE void register_channel_id(u64 type_hash_code) noexcept {

			NCPP_ASSERT(is_has_channel_id(type_hash_code)) << "channel " << T_cout_value(u32(type_hash_code)) << " already registered";

			type_hash_code_to_channel_id_map_[type_hash_code] = last_channel_id_;
			++last_channel_id_;
		}
		template<typename F__>
		NCPP_FORCE_INLINE void T_register_channel_id() noexcept {

			constexpr u64 type_hash_code = T_type_hash_code<F__>;

			NCPP_ASSERT(is_has_channel_id(type_hash_code)) << "channel " << T_cout_value(u32(type_hash_code)) << " already registered";

			type_hash_code_to_channel_id_map_[type_hash_code] = last_channel_id_;
			++last_channel_id_;
		}
		NCPP_FORCE_INLINE F_render_view_mask mask(auto... type_hash_codes) noexcept {

			return flag_combine(
				0,
				(
					1
					<< u64(channel_id(type_hash_codes))
				)...
			);
		}
		template<typename... Fs__>
		NCPP_FORCE_INLINE F_render_view_mask T_mask() noexcept {

			return flag_combine(
				0,
				(
					1
					<< u64(T_channel_id<Fs__>())
				)...
			);
		}

		template<typename F_functor__, typename... Fs__>
		inline void T_for_each(F_functor__&& functor) const {

			auto search_mask = T_mask<Fs__...>();

			if(search_mask)
			{
				for (const auto& render_view_p : render_view_p_list_)
				{

					if (render_view_p->mask() & search_mask)
						functor(render_view_p);
				}
			}

			for(const auto& render_view_p : render_view_p_list_) {

				functor(render_view_p);
			}
		}

	};

}



#define NRE_RENDER_VIEW_SYSTEM(...) (nre::F_render_view_system::instance_p())