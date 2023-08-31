#pragma once

#include <librealsense2/rs.hpp>

namespace Jasp {

    namespace Internal {

		void Init();

        inline rs2::config rs_cfg;
        inline rs2::pipeline rs_pipe;
        inline rs2::frameset rs_fset;

    }

}
