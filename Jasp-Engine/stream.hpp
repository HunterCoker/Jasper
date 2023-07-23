#pragma once

namespace jasp {

	struct stream {
		stream();
		stream(const stream& other);
		~stream();

		static void create_depth_stream(stream& s);
		static void create_color_stream(stream& s);
		static void create_threshold_stream(stream& s);

		int width;
		int height;
		int stride;
		void* data;
		unsigned int* pixel_data;
	};


	// struct threshold_stream : private stream {
	// 	threshold_stream();
	// 	threshold_stream(const threshold_stream& other);
	// 	~threshold_stream();
		
	// 	// inline void set_threshold_value(float threshold);
	// 	inline bool threshold_pixel(int x, int y);
	// private:
	// 	float threshold;
	// };

}