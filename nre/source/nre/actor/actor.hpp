#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_level;
	class F_actor;
	class A_actor_component;



	class NRE_API F_actor {

	public:
		friend class F_level;



	private:
		using F_handle = typename TG_list<TU<F_actor>>::iterator;
		using F_component_map = TG_unordered_map<u64, TK<A_actor_component>>;
		using F_component_vector = TG_vector<TU<A_actor_component>>;



	private:
		TK_valid<F_level> level_p_;
		F_handle handle_;
		F_component_map component_map_;
		F_component_vector component_vector_;

	protected:
		G_string name_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_level> level_p() const noexcept { return level_p_; }
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(const G_string& value) noexcept { name_ = value; }



	public:
		F_actor(TK_valid<F_level> level_p, const G_string& name = "No name");
		virtual ~F_actor();

	public:
		NCPP_DISABLE_COPY(F_actor);

	public:
		template<typename F_component__>
		b8 T_is_has_component() const noexcept
		{
			return (component_map_.find(T_type_hash_code<F_component__>) != component_map_.end());
		}
		template<typename F_component__, typename... F_args__>
		requires requires(F_args__&&... args) {
			F_component__(TK_valid<F_actor>(), std::forward<F_args__>(args)...);
		}
		TK_valid<F_component__> T_add_component(F_args__&&... args)
		{
			NCPP_ASSERT(!T_is_has_component<F_component__>())
				<< "component of type "
				<< T_cout_value(T_type_fullname<F_component__>())
				<< " already added";

			auto component_p = TU<F_component__>()(
				NCPP_KTHIS(),
				std::forward<F_args__>(args)...
			);

			auto keyed_component_p = NCPP_FOH_VALID(component_p);

			component_map_[T_type_hash_code<F_component__>] = keyed_component_p.no_requirements();
			component_vector_.push_back(std::move(component_p));

			return keyed_component_p;
		}
		template<typename F_component__>
		TK_valid<F_component__> T_get_component() const
		{
			NCPP_ASSERT(T_is_has_component<F_component__>())
				<< "component of type "
				<< T_cout_value(T_type_fullname<F_component__>())
				<< " was not added";

			return NCPP_FOH_VALID(
				component_map_.find(T_type_hash_code<F_component__>)->second
				.T_cast<F_component__>()
			);
		}
		template<typename F_component__>
		eastl::optional<TK<F_component__>> T_try_get_component() const
		{
			auto it = component_map_.find(T_type_hash_code<F_component__>);

			if (it == component_map_.end())
			{
				return eastl::nullopt;
			}

			return NCPP_FOH_VALID(
				it->second
				.T_cast<F_component__>()
			);
		}

	protected:
		virtual void gameplay_tick();
		virtual void render_tick();

	};

}