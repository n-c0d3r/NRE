#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	F_transform_node::F_transform_node(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform(T_identity<F_matrix4x4>())
	{
	}
	F_transform_node::~F_transform_node() {

		set_parent_p(eastl::nullopt);
	}

	void F_transform_node::set_parent_p(const F_node_p_opt& parent_node_p_opt)
	{
		if(parent_node_p_opt_)
		{
			auto parent_node_p = parent_node_p_opt_.value();

			if(parent_node_p)
				parent_node_p_opt_.value()->remove_child_internal(NCPP_KTHIS());
		}

		if(parent_node_p_opt)
			parent_node_p_opt.value()->add_child_internal(NCPP_KTHIS());

		parent_node_p_opt_ = parent_node_p_opt;
	}

	void F_transform_node::add_child_internal(TKPA_valid<F_transform_node> child_node_p)
	{
		child_node_p_list_.push_back(child_node_p);
		child_node_p->handle_ = --(child_node_p_list_.end());
	}
	void F_transform_node::remove_child_internal(TKPA_valid<F_transform_node> child_node_p)
	{
		child_node_p_list_.erase(child_node_p->handle_);
	}

}