#pragma once

#include <nre/actor/actor_component.hpp>



namespace nre {

	class NRE_API F_transform_node : public A_actor_component {

	public:
		using F_child_node_p_list = TG_list<TK_valid<F_transform_node>>;
		using F_handle = typename F_child_node_p_list::iterator;
		using F_node_p_opt = eastl::optional<TK<F_transform_node>>;



	public:
		F_matrix4x4 transform;

	private:
		F_child_node_p_list child_node_p_list_;
		F_handle handle_;
		F_node_p_opt parent_node_p_opt_;

	public:
		NCPP_FORCE_INLINE const F_child_node_p_list& child_node_p_list() const noexcept { return child_node_p_list_; }
		NCPP_FORCE_INLINE const F_node_p_opt& parent_node_p_opt() const noexcept { return parent_node_p_opt_; }



	public:
		F_transform_node(TKPA_valid<F_actor> actor_p, PA_matrix4x4 initial_transform);
		F_transform_node(TKPA_valid<F_actor> actor_p);
		virtual ~F_transform_node();

	public:
		void set_parent_p(const F_node_p_opt& parent_node_p_opt);

	private:
		void add_child_internal(TKPA_valid<F_transform_node> child_node_p);
		void remove_child_internal(TKPA_valid<F_transform_node> child_node_p);

	};

}