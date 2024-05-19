#include <nre/rendering/material.hpp>
#include <nre/rendering/material_template.hpp>



namespace nre {

	F_material::F_material(TSPA<F_material_template> template_p) :
		template_p_(template_p)
	{
	}
	F_material::~F_material() {
	}

}