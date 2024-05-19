#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

	class F_material_template;



	class NRE_API F_material {

	private:
		TS<F_material_template> template_p_;

	public:
		 NCPP_FORCE_INLINE TK_valid<F_material_template> template_p() const noexcept { return NCPP_FOH_VALID(template_p_); }



	public:
		F_material(TSPA<F_material_template> template_p);
		virtual ~F_material();

	public:
		NCPP_OBJECT(F_material);

	};

}