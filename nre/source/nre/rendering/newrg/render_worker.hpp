#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class NRE_API A_render_worker
    {
    private:
        u8 index_ = 0;
        ED_command_list_type command_list_type_ = ED_command_list_type::DIRECT;

        TU<A_command_queue> command_queue_p_;

    public:
        NCPP_FORCE_INLINE u8 index() const noexcept { return index_; }
        NCPP_FORCE_INLINE ED_command_list_type command_list_type() const noexcept { return command_list_type_; }

        NCPP_FORCE_INLINE TK_valid<A_command_queue> command_queue_p() const noexcept { return NCPP_FOH_VALID(command_queue_p_); }



    protected:
        A_render_worker(u32 index, ED_command_list_type command_list_type = ED_command_list_type::DIRECT);

    public:
        virtual ~A_render_worker();

    public:
        NCPP_OBJECT(A_render_worker);
    };



    template<typename... F_render_workers__>
    class TF_render_worker_list
    {
    private:
        TG_tuple<TU<F_render_workers__>...> render_worker_p_tuple_;



    public:
        TF_render_worker_list()
        {
            using F_render_worker_targs = TF_template_targ_list<F_render_workers__...>;
            T_init_workers_helper<F_render_worker_targs::F_indices>::invoke(render_worker_p_tuple_);
        }

    private:
        template<typename F__>
        struct T_init_workers_helper;
    };

    template<typename... F_render_workers__>
    template<sz... indices__>
    struct TF_render_worker_list<F_render_workers__...>::T_init_workers_helper<TF_template_varg_list<indices__...>>
    {
    public:
        static void invoke(TG_tuple<TU<F_render_workers__>...>& tuple)
        {
            using F_render_worker_targs = TF_template_targ_list<F_render_workers__...>;
            tuple = {
                TU<F_render_worker_targs::template TF_at<indices__>>()(indices__ + 1)...
            };
        }
    };
}