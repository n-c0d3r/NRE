
import(newrg/abytek/prerequisites.nsh)



define F_primitive_id(u32)

struct F_primitive_id_range(
    begin(F_primitive_id)
    end(F_primitive_id)
)

struct F2_primitive_id_range(
    offset(F_primitive_id)
    count(F_primitive_id)
)