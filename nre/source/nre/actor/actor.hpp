#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_level;
	class F_actor;
	class A_actor_component;



	class NRE_API F_actor : public A_object
	{
	public:
		friend class F_level;
	
	private:
		using F_handle = typename TG_list<TU<F_actor>>::iterator;
		using F_gameplay_tick_handle = typename TG_list<TK_valid<F_actor>>::iterator;
		using F_render_tick_handle = typename TG_list<TK_valid<F_actor>>::iterator;
		using F_component_multimap = TG_unordered_multimap<u64, TK<A_actor_component>>;
		using F_component_vector = TG_vector<TU<A_actor_component>>;



	private:
		TK_valid<F_level> level_p_;
		F_handle handle_;
		F_component_multimap component_multimap_;
		F_component_vector component_vector_;

		F_gameplay_tick_handle gameplay_tick_handle_;
		F_render_tick_handle render_tick_handle_;
		b8 is_gameplay_tickable_ = false;
		b8 is_render_tickable_ = false;

	protected:
		G_string name_;

	public:
		NCPP_FORCE_INLINE TKPA_valid<F_level> level_p() const noexcept { return level_p_; }
		NCPP_FORCE_INLINE const G_string& name() const noexcept { return name_; }
		NCPP_FORCE_INLINE void set_name(const G_string& value) noexcept { name_ = value; }
		NCPP_FORCE_INLINE b8 is_gameplay_tickable() const noexcept { return is_gameplay_tickable_; }
		NCPP_FORCE_INLINE b8 is_render_tickable() const noexcept { return is_render_tickable_; }



	public:
		F_actor(TK_valid<F_level> level_p, const G_string& name = "No name");
		virtual ~F_actor();

	public:
		NCPP_OBJECT(F_actor);

	public:
		template<typename F_component__>
		b8 T_is_has_component() const noexcept
		{
			return (component_multimap_.find(T_type_hash_code<F_component__>) != component_multimap_.end());
		}
		template<typename F_component__, typename... F_args__>
		requires requires(F_args__&&... args) {
			F_component__(TK_valid<F_actor>(), std::forward<F_args__>(args)...);
		}
		TK_valid<F_component__> T_add_component(F_args__&&... args)
		{
			auto component_p = TU<F_component__>()(
				NCPP_KTHIS(),
				std::forward<F_args__>(args)...
			);

			auto keyed_component_p = NCPP_FOH_VALID(component_p);

			for(auto type_id : keyed_component_p->type_ids())
			{
				component_multimap_.insert({
					type_id,
					keyed_component_p.no_requirements()
				});
			}

			component_vector_.push_back(std::move(component_p));

			keyed_component_p.T_cast<A_actor_component>()->ready();

			return keyed_component_p;
		}
		template<typename F_component__>
		TK_valid<F_component__> T_component() const
		{
			NCPP_ASSERT(T_is_has_component<F_component__>())
				<< "component of type "
				<< T_cout_value(T_type_fullname<F_component__>())
				<< " was not added";

			return NCPP_FOH_VALID(
				component_multimap_.find(T_type_hash_code<F_component__>)->second
				.T_cast<F_component__>()
			);
		}
		template<typename F_component__>
		auto T_components() const
		{
			return component_multimap_.equal_range(T_type_hash_code<F_component__>);
		}
		template<typename F_component__>
		TK<F_component__> T_try_component() const
		{
			auto it = component_multimap_.find(T_type_hash_code<F_component__>);

			if(it == component_multimap_.end())
				return null;

			return it->second.T_cast<F_component__>();
		}
		template<typename F_search_component__, typename F_new_component__ = F_search_component__, typename... F_args__>
		requires requires(F_args__&&... args) {
//			F_component__(TK_valid<F_actor>(), std::forward<F_args__>(args)...);
			F_new_component__(TK_valid<F_actor>(), std::forward<F_args__>(args)...);
		}
		TK_valid<F_search_component__> T_guarantee_component(F_args__&&... args)
		{
			if(T_is_has_component<F_search_component__>())
				return T_component<F_search_component__>();

			return T_add_component<F_new_component__>(
				std::forward<F_args__>(args)...
			);
		}
		template<typename F_search_component__>
		TK_valid<F_search_component__> T_guarantee_component(auto&& create_component_functor)
		{
			if(T_is_has_component<F_search_component__>())
				return T_component<F_search_component__>();

			return create_component_functor(NCPP_KTHIS());
		}

	public:
		void set_gameplay_tick(b8 value);
		void set_render_tick(b8 value);

	protected:
		virtual void gameplay_tick();
		virtual void render_tick();

	};

}