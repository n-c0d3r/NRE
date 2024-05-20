#include <nre/hierarchy/transform_node.hpp>



namespace nre {

	F_transform_node::F_transform_node(TKPA_valid<F_actor> actor_p, PA_matrix4x4 initial_transform) :
		A_actor_component(actor_p),
		transform(initial_transform)
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_transform_node);
	}
	F_transform_node::F_transform_node(TKPA_valid<F_actor> actor_p) :
		A_actor_component(actor_p),
		transform(T_identity<F_matrix4x4>())
	{
		NRE_ACTOR_COMPONENT_REGISTER(F_transform_node);
	}
	F_transform_node::~F_transform_node() {

		set_parent_p(null);
	}

	void F_transform_node::set_parent_p(F_null)
	{
		if(parent_node_p_)
		{
			parent_node_p_->remove_child_internal(NCPP_KTHIS());
		}
		parent_node_p_ = null;
	}
	void F_transform_node::set_parent_p(TKPA<F_transform_node> parent_node_p)
	{
		if(parent_node_p_)
		{
			parent_node_p_->remove_child_internal(NCPP_KTHIS());
		}

		if(parent_node_p)
			parent_node_p->add_child_internal(NCPP_KTHIS());

		parent_node_p_ = parent_node_p;
	}
	void F_transform_node::set_parent_p(TKPA_valid<F_transform_node> parent_node_p)
	{
		if(parent_node_p_)
		{
			parent_node_p_->remove_child_internal(NCPP_KTHIS());
		}

		parent_node_p->add_child_internal(NCPP_KTHIS());

		parent_node_p_ = parent_node_p.no_requirements();
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