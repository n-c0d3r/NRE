#include <nre/rendering/bind_table.hpp>



namespace nre
{
    F_bind_table::F_bind_table(const TG_unordered_set<G_string>& name_set) :
        hash_table_(name_set.size(), name_set.size())
    {
        names_.reserve(name_set.size());
        for(const auto& name : name_set)
        {
            hash_table_.add(TF_hash<G_string>()(name), names_.size());
            names_.push_back(name);
        }
    }
    F_bind_table::F_bind_table(const G_hash_table& hash_table, const TG_vector<G_string>& names) :
        hash_table_(hash_table),
        names_(names)
    {
    }
    F_bind_table::F_bind_table(TG_unordered_set<G_string>&& name_set) :
        hash_table_(name_set.size(), name_set.size())
    {
        names_.reserve(name_set.size());
        for(auto& name : name_set)
        {
            hash_table_.add(
                TF_hash<G_string>()(name),
                names_.size()
            );
            names_.push_back(
                eastl::move(name)
            );
        }
    }
    F_bind_table::F_bind_table(G_hash_table&& hash_table, TG_vector<G_string>&& names) :
        hash_table_(eastl::move(hash_table)),
        names_(eastl::move(names))
    {
    }

    sz F_bind_table::find_index(const G_string& name) const
    {
        for(
            auto index = hash_table_.first(TF_hash<G_string>()(name));
            hash_table_.is_valid(index);
            index = hash_table_.next(index)
        )
        {
            if(names_[index] == name)
            {
                return index;
            }
        }

        return sz(-1);
    }

    F_bind_table::operator TG_unordered_set<G_string> () const
    {
        TG_unordered_set<G_string> result;

        for(const auto& name : names_)
        {
            result.insert(name);
        }

        return eastl::move(result);
    }
}