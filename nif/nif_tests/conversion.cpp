#include "pch.h"
#include "CppUnitTest.h"

#include "Rotation.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace conversion
{
	using namespace math;

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

				Rotation::euler_type in{ degf(20.0f), degf(53.0f), degf (-17.0f), order };
				Rotation r1;
				r1.setEuler(in);

				Niflib::Matrix33 mat = nif_type_conversion<Niflib::Matrix33>::from(r1);
				Rotation r2 = nif_type_conversion<Rotation>::from(mat);

				Rotation::euler_type out = r2.getEuler(order);

				for (size_t i = 0; i < in.size(); i++)
					Assert::AreEqual(in[i].value, out[i].value, in[i].value * m_tolerance);
			}
		}

		float m_tolerance = 1.0e-3f;//relative
	};
}