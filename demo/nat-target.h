#pragma once

namespace nat {

	namespace target {

		enum reg_class
		{
			rs, /* reserved */
			cr, /* caller saved */
			ce, /* callee saved */
		};

		struct backend
		{
			static backend* get_default();

			virtual size_t get_reg_count() = 0;
			virtual const char** get_reg_name() = 0;
			virtual const reg_class* get_reg_class() = 0;
			virtual const size_t* get_reg_order() = 0;
			virtual node_list emit(compiler*, node*) = 0;
		};

	}

}
