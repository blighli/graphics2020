#pragma once
#include <Runtime/Math.hpp>

namespace Luna
{
	enum class CameraType : u32
	{
		perspective = 0,
		orthographic = 1,
	};

	struct Camera
	{
		Tranform3D transform = Tranform3D(Float3(0.0f, 0.0f, 0.0f), Quaternion::identity(), Float3(1.0f, 1.0f, 1.0f));
		CameraType type = CameraType::perspective;
		f32 fov = pi / 2.0f;
		f32 size = 5.0f;
		f32 near_clipping_plane = 0.3f;
		f32 far_clipping_plane = 100.0f;
		f32 aspect_ratio = 8.0f / 6.0f;

		Float4x4 projection_matrix() const
		{
			if (type == CameraType::perspective)
			{
				return Float4x4::make_perspective_field_of_view(fov, aspect_ratio, 
					near_clipping_plane, far_clipping_plane);
			}
			else
			{
				return Float4x4::make_orthographic(size, 
					size / aspect_ratio, near_clipping_plane, far_clipping_plane);
			}
		}

		Float4x4 view_matrix() const
		{
			return inverse(Float4x4::make_affine_position_rotation_scale(transform.position, transform.rotation, transform.scale));
		}
	};
}