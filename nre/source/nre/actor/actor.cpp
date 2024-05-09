#include <nre/actor/actor.hpp>
#include <nre/actor/actor_component.hpp>
#include <nre/actor/level.hpp>



namespace nre {

	F_actor::F_actor(TK_valid<F_level> level_p, const G_string& name) :
		level_p_(level_p),
		name_(name)
	{
	}
	F_actor::~F_actor() {

		set_gameplay_tick(false);
		set_render_tick(false);

		for(i32 i = component_vector_.size() - 1; i >= 0; --i)
		{
			component_vector_[i].reset();
		}
	}

	void F_actor::gameplay_tick() {

		for(auto& component_p : component_vector_)
		{
			component_p->gameplay_tick();
		}
	}
	void F_actor::render_tick() {

		for(auto& component_p : component_vector_)
		{
			component_p->render_tick();
		}
	}

	void F_actor::set_gameplay_tick(b8 value)
	{
		if(value == is_gameplay_tickable_)
			return;

		if(value)
		{
			level_p_->gameplay_tickable_actor_p_list_.push_back(NCPP_KTHIS());
			gameplay_tick_handle_ = --(level_p_->gameplay_tickable_actor_p_list_.end());
		}
		else
		{
			level_p_->gameplay_tickable_actor_p_list_.erase(gameplay_tick_handle_);
		}

		is_gameplay_tickable_ = value;
	}
	void F_actor::set_render_tick(b8 value)
	{
		if(value == is_render_tickable_)
			return;

		if(value)
		{
			level_p_->render_tickable_actor_p_list_.push_back(NCPP_KTHIS());
			render_tick_handle_ = --(level_p_->render_tickable_actor_p_list_.end());
		}
		else
		{
			level_p_->render_tickable_actor_p_list_.erase(render_tick_handle_);
		}

		is_render_tickable_ = value;
	}

}