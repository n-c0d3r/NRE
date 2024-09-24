#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API F_bind_table
    {
    private:
        G_hash_table hash_table_;
        TG_vector<G_string> names_;

    public:
        NCPP_FORCE_INLINE const auto& hash_table() const noexcept { return hash_table_; }
        NCPP_FORCE_INLINE const auto& names() const noexcept { return names_; }



    public:
        F_bind_table() = default;
        F_bind_table(const TG_unordered_set<G_string>& name_set);
        F_bind_table(const G_hash_table& hash_table, const TG_vector<G_string>& names);
        F_bind_table(TG_unordered_set<G_string>&& name_set);
        F_bind_table(G_hash_table&& hash_table, TG_vector<G_string>&& names);

        F_bind_table(const F_bind_table& x) = default;
        F_bind_table& operator = (const F_bind_table& x) = default;

        F_bind_table(F_bind_table&& x) noexcept = default;
        F_bind_table& operator = (F_bind_table&& x) noexcept = default;

    public:
        sz find_index(const G_string& name) const;
        NCPP_FORCE_INLINE const G_string& at(sz index) const
        {
            return names_[index];
        }
        NCPP_FORCE_INLINE const G_string& operator [] (sz index) const
        {
            return at(index);
        }
        NCPP_FORCE_INLINE sz operator [] (const G_string& name) const
        {
            return find_index(name);
        }
        NCPP_FORCE_INLINE sz size() const noexcept
        {
            return names_.size();
        }

    public:
        operator TG_unordered_set<G_string> () const;
        NCPP_FORCE_INLINE operator b8 () const
        {
            return (size() != 0);
        }
    };
}