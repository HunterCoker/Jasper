#include "stream.hpp"
#include "jasp_internal.hpp"

#include <cstring>

namespace jasp {

	stream::stream() {
		width = 0;
		height = 0;
		stride = 0;
		data = nullptr;
		pixel_data = nullptr;
	}

	stream::stream(const stream& other) {
		this->width = other.width;
		this->height = other.height;
		this->stride = other.stride;

		size_t n = this->width * this->height * this->stride;
		this->data = new uint8_t[n];
		std::memcpy(this->data, other.data, n);

		if (other.pixel_data) {
			size_t n = this->width * this->height;
			this->pixel_data = new uint32_t[n];
			std::memcpy(this->pixel_data, other.pixel_data, n);
		}
		else
			this->pixel_data = nullptr;
	}

	stream::~stream() {
		
	}

	void stream::create_depth_stream(stream& s) {
		rs2::depth_frame rs_fdep = internal::rs_fset.get_depth_frame();
		s.width = rs_fdep.get_width();
		s.height = rs_fdep.get_height();
		s.stride = rs_fdep.get_bytes_per_pixel();
		s.data = const_cast<void*>(rs_fdep.get_data());
	}

	void stream::create_color_stream(stream& s) {
		rs2::video_frame rs_fcol = internal::rs_fset.get_color_frame();
		s.width = rs_fcol.get_width();
		s.height = rs_fcol.get_height();
		s.stride = rs_fcol.get_bytes_per_pixel();
		s.data = const_cast<void*>(rs_fcol.get_data());
	}

	void stream::create_threshold_stream(stream& s) {
		rs2::video_frame rs_fcol = internal::rs_fset.get_color_frame();
		s.width = rs_fcol.get_width();
		s.height = rs_fcol.get_height();
		s.stride = 1;
		s.data = new uint8_t[s.width * s.height * s.stride];
	}

}