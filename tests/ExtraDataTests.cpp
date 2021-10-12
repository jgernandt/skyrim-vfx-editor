#include "pch.h"
#include "CppUnitTest.h"
#include "CommonTests.h"
#include "Mocks.h"
#include "nodes.h"

namespace nif
{
	TEST_CLASS(ExtraDataTests)
	{
	public:

		TEST_METHOD(Name)
		{
			nif::NiStringExtraData obj;
			nif::NiExtraData& objref = obj;
			StringPropertyTest(objref.name());
		}
	};

	TEST_CLASS(StringExtraDataTests)
	{
	public:

		TEST_METHOD(Value)
		{
			nif::NiStringExtraData obj;
			StringPropertyTest(obj.value());
		}
	};
}


namespace node
{
	TEST_CLASS(ExtraDataTests)
	{
	public:

		//Target should receive ISet<NiExtraData> (multi)
		TEST_METHOD(Target)
		{
			std::unique_ptr<ExtraData> node = std::make_unique<StringData>();
			nif::NiExtraData& obj = node->object();
			SetReceiverTest(ExtraData::TARGET, true, std::move(node), obj);
		}
	};
}
