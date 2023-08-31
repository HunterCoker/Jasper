#pragma once

#include "../Util.hpp"

namespace Jasp {

	struct Stream {
		Stream();
		Stream(const Stream& other);
		~Stream();

		virtual void RetrieveCameraData() = 0;
        virtual void ConstructPixelData() = 0;

		int width;
		int height;
		int stride;
		void* data;
		unsigned int* pixel_data;
	};

    struct DepthStream : Stream {
    public:
        DepthStream();
        DepthStream(const DepthStream& other);

		void RetrieveCameraData();
        void ConstructPixelData();

		[[nodiscard]] inline float GetDepthAtPixel(const Vec2& coord) const;
    };

    struct ColorStream : Stream {
        ColorStream();
        ColorStream(const ColorStream& other);

		void RetrieveCameraData();
		void ConstructPixelData();
    };

    struct ThreshStream : Stream {
        ThreshStream();
		ThreshStream(const ThreshStream& other);

		void Threshold(const ColorStream& color_stream);
		void RetrieveCameraData();
		void ConstructPixelData();

		float threshold;
	};

}