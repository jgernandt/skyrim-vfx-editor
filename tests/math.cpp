#include "pch.h"
#include "CppUnitTest.h"

#include "Rotation.h"
#include "nif_types.h"
#include "nif_math.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace math
{
	TEST_CLASS(RotationTests)
	{
	public:

		//Test the consistency of our conversion functions to and from Niflib::Matrix33
		TEST_METHOD(Niflib_converters)
		{
			EulerOrder orders[6]{
				EulerOrder::XYZ,
				EulerOrder::YZX,
				EulerOrder::ZXY,
				EulerOrder::XZY,
				EulerOrder::YXZ,
				EulerOrder::ZYX, };

			for (auto order : orders) {

				Rotation::euler_type in{ 20.0f, 53.0f, -17.0f, order };
				Rotation r1;
				r1.setEuler(in);

				Niflib::Matrix33 mat = nif::nif_type_conversion<Niflib::Matrix33>::from(r1);
				Rotation r2 = nif::nif_type_conversion<Rotation>::from(mat);

				Rotation::euler_type out = r2.getEuler(order);

				for (size_t i = 0; i < in.size(); i++)
					Assert::AreEqual(in[i].value, out[i].value, in[i].value * m_tolerance);
			}
		}

		//Test the validity of our conversions between euler and the native quaternion format
		TEST_METHOD(get_set_Euler)
		{
			//general case
			{
				std::vector<math::Rotation::euler_type> rots{
					{ 20.0f, 53.0f, -17.0f, EulerOrder::XYZ },
					{ 20.0f, 53.0f, -17.0f, EulerOrder::YZX },
					{ 20.0f, 53.0f, -17.0f, EulerOrder::ZXY },
					{ 20.0f, 53.0f, -17.0f, EulerOrder::XZY },
					{ 20.0f, 53.0f, -17.0f, EulerOrder::YXZ },
					{ 20.0f, 53.0f, -17.0f, EulerOrder::ZYX },
				};

				std::array<Quaternion, 6> blender{
					Quaternion{ 0.860205f, 0.218647f, 0.411622f, -0.2069f },
					Quaternion{ 0.860205f, 0.218647f, 0.457563f, -0.05364f },
					Quaternion{ 0.860205f, 0.088746f, 0.457563f, -0.2069f },
					Quaternion{ 0.88311f, 0.088746f, 0.411622f, -0.2069f },
					Quaternion{ 0.88311f, 0.218647f, 0.411622f, -0.05364f },
					Quaternion{ 0.88311f, 0.088746f, 0.457563f, -0.05364f },
				};

				EulerTest(rots, blender);
			}

			//singular case, single angle
			{
				std::vector<math::Rotation::euler_type> rots{
					{ 47.0f, 90.0f, 0.0f, EulerOrder::XYZ },
					{ 0.0f, 47.0f, 90.0f, EulerOrder::YZX },
					{ 90.0f, 0.0f, 47.0f, EulerOrder::ZXY },
					{ 47.0f, 0.0f, 90.0f, EulerOrder::XZY },
					{ 90.0f, 47.0f, 0.0f, EulerOrder::YXZ },
					{ 0.0f, 90.0f, 47.0f, EulerOrder::ZYX },
				};

				std::array<Quaternion, 6> blender{
					Quaternion{ 0.648459f, 0.281958f, 0.648459f, -0.281958f },
					Quaternion{ 0.648459f, -0.281958f, 0.281958f, 0.648459f },
					Quaternion{ 0.648459f, 0.648459f, -0.281958f, 0.281958f },
					Quaternion{ 0.648459f, 0.281958f, 0.281958f, 0.648459f },
					Quaternion{ 0.648459f, 0.648459f, 0.281958f, 0.281958f },
					Quaternion{ 0.648459f, 0.281958f, 0.648459f, 0.281958f },
				};

				EulerTest(rots, blender);
			}
			{
				std::vector<math::Rotation::euler_type> rots{
					{ 47.0f, -90.0f, 0.0f, EulerOrder::XYZ },
					{ 0.0f, 47.0f, -90.0f, EulerOrder::YZX },
					{ -90.0f, 0.0f, 47.0f, EulerOrder::ZXY },
					{ 47.0f, 0.0f, -90.0f, EulerOrder::XZY },
					{ -90.0f, 47.0f, 0.0f, EulerOrder::YXZ },
					{ 0.0f, -90.0f, 47.0f, EulerOrder::ZYX },
				};

				std::array<Quaternion, 6> blender{
					Quaternion{ 0.648459f, 0.281958f, -0.648459f, 0.281958f },
					Quaternion{ 0.648459f, 0.281958f, 0.281958f, -0.648459f },
					Quaternion{ 0.648459f, -0.648459f, 0.281958f, 0.281958f },
					Quaternion{ 0.648459f, 0.281958f, -0.281958f, -0.648459f },
					Quaternion{ 0.648459f, -0.648459f, 0.281958f, -0.281958f },
					Quaternion{ 0.648459f, -0.281958f, -0.648459f, 0.281958f },
				};

				EulerTest(rots, blender);
			}

			//singular case, two angles
			{
				std::vector<math::Rotation::euler_type> in{
					{ 47.0f, 90.0f, -21.0f, EulerOrder::XYZ },
					{ -21.0f, 47.0f, 90.0f, EulerOrder::YZX },
					{ 90.0f, -21.0f, 47.0f, EulerOrder::ZXY },
					{ 47.0f, -21.0f, 90.0f, EulerOrder::XZY },
					{ 90.0f, 47.0f, -21.0f, EulerOrder::YXZ },
					{ -21.0f, 90.0f, 47.0f, EulerOrder::ZYX },
				};
				//The expected outcome threw me off for a while. The first and third axes will be antiparallel
				//in a right-handed order, but parallel in a left-handed order (reverse at -90).
				std::vector<math::Rotation::euler_type> expected{
					{ 68.0f, 90.0f, 0.0f, EulerOrder::XYZ },
					{ 0.0f, 68.0f, 90.0f, EulerOrder::YZX },
					{ 90.0f, 0.0f, 68.0f, EulerOrder::ZXY },
					{ 26.0f, 0.0f, 90.0f, EulerOrder::XZY },
					{ 90.0f, 26.0f, 0.0f, EulerOrder::YXZ },
					{ 0.0f, 90.0f, 26.0f, EulerOrder::ZYX },
				};

				std::array<Quaternion, 6> blender{
					Quaternion{ 0.586218f, 0.395409f, 0.586218f, -0.395409f },
					Quaternion{ 0.586218f, -0.395409f, 0.395409f, 0.586218f },
					Quaternion{ 0.586218f, 0.586218f, -0.395409f, 0.395409f },
					Quaternion{ 0.688984f, 0.159064f, 0.159064f, 0.688984f },
					Quaternion{ 0.688984f, 0.688984f, 0.159064f, 0.159064f },
					Quaternion{ 0.688984f, 0.159064f, 0.688984f, 0.159064f },
				};

				EulerTest(in, blender, expected);
			}
			{
				std::vector<math::Rotation::euler_type> in{
					{ 47.0f, -90.0f, -21.0f, EulerOrder::XYZ },
					{ -21.0f, 47.0f, -90.0f, EulerOrder::YZX },
					{ -90.0f, -21.0f, 47.0f, EulerOrder::ZXY },
					{ 47.0f, -21.0f, -90.0f, EulerOrder::XZY },
					{ -90.0f, 47.0f, -21.0f, EulerOrder::YXZ },
					{ -21.0f, -90.0f, 47.0f, EulerOrder::ZYX },
				};
				std::vector<math::Rotation::euler_type> expected{
					{ 26.0f, -90.0f, 0.0f, EulerOrder::XYZ },
					{ 0.0f, 26.0f, -90.0f, EulerOrder::YZX },
					{ -90.0f, 0.0f, 26.0f, EulerOrder::ZXY },
					{ 68.0f, 0.0f, -90.0f, EulerOrder::XZY },
					{ -90.0f, 68.0f, 0.0f, EulerOrder::YXZ },
					{ 0.0f, -90.0f, 68.0f, EulerOrder::ZYX },
				};

				std::array<Quaternion, 6> blender{
					Quaternion{ 0.688984f, 0.159064f, -0.688984f, 0.159064f },
					Quaternion{ 0.688984f, 0.159064f, 0.159064f, -0.688984f },
					Quaternion{ 0.688984f, -0.688984f, 0.159064f, 0.159064f },
					Quaternion{ 0.586218f, 0.395409f, -0.395409f, -0.586218f },
					Quaternion{ 0.586218f, -0.586218f, 0.395409f, -0.395409f },
					Quaternion{ 0.586218f, -0.395409f, -0.586218f, 0.395409f },
				};

				EulerTest(in, blender, expected);
			}
		}

	private:
		void EulerTest(const std::vector<Rotation::euler_type>& in, 
			const std::array<Quaternion, 6>& ref, 
			const std::vector<Rotation::euler_type>& expected = std::vector<Rotation::euler_type>())
		{
			Assert::IsTrue(in.size() == 6 && (expected.size() == 6 || expected.empty()) && ref.size() == 6);

			for (size_t i = 0; i < in.size(); i++) {
				Assert::IsTrue(expected.empty() || in[i].order == expected[i].order);//or we messed up the values

				Rotation R;
				R.setEuler(in[i]);

				Quaternion q = R.getQuaternion();
				AssertQuats(ref[i], q, m_tolerance);

				Rotation::euler_type out = R.getEuler(in[i].order);
				for (int j = 0; j < 3; j++) {
					if (expected.empty())
						Assert::AreEqual(in[i][j].value, out[j].value, in[i][j].value * m_tolerance);
					else
						Assert::AreEqual(expected[i][j].value, out[j].value, expected[i][j].value * m_tolerance);
				}

				if (!expected.empty() && in[i] != expected[i]) {
					//set Euler and check the quat again, since we have merged the angles
					R.setEuler(out);
					q = R.getQuaternion();
					AssertQuats(ref[i], q, m_tolerance);
				}
			}
		}

		void AssertQuats(const Quaternion& expected, const Quaternion& actual, float relTol)
		{
			Assert::AreEqual(expected.s, actual.s, expected.s * relTol);
			Assert::AreEqual(expected.v[0], actual.v[0], expected.v[0] * relTol);
			Assert::AreEqual(expected.v[1], actual.v[1], expected.v[1] * relTol);
			Assert::AreEqual(expected.v[2], actual.v[2], expected.v[2] * relTol);
		}

		float m_tolerance = 1.0e-3f;//relative
	};
}