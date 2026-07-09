#include "Serialization/Serialization.h"
#include "Serialization/Deserialization.h"

#define CHECK_SUCCESS(expr) do { std::cout << "[OK] " #expr << std::endl; } while(0)
#define CHECK_FAILURE(expr) do { std::cout << "[FAILURE] " #expr << std::endl; } while(0)
#define CHECK(expr) do { if (expr) { CHECK_SUCCESS(expr); } else { CHECK_FAILURE(expr); } } while(0)

int main(int argc, char **argv)
{
	{
		Serialization::Serializer serializer;

		const int8 origin_i8 = 0x12;
		const int16 origin_i16 = 0x3456;
		const int32 origin_i32 = 0x789ABCDE;

		CHECK(serializer.write(origin_i8));
		CHECK(serializer.write(origin_i16));
		CHECK(serializer.write(origin_i32));
		CHECK(serializer.bufferSize() == 7);
		Serialization::Deserializeration deserializer(serializer.buffer(), serializer.bufferSize());
		CHECK(deserializer.remainingBytes() == 7);
		int8 i8;
		CHECK(deserializer.read(i8));
		CHECK(i8 == origin_i8);
		int16 i16;
		CHECK(deserializer.read(i16));
		CHECK(i16 == origin_i16);
		int32 i32;
		CHECK(deserializer.read(i32));
		CHECK(i32 == origin_i32);
		CHECK(deserializer.remainingBytes() == 0);
	}
	{
		struct MyFirstMessage
		{
			int8 value1;
			uint32 value2;
			float32 value3;
			std::vector<uint16> vector;
			std::string string;
			std::vector<std::string> vecStr;
			bool serialize(Serialization::Serializer& serializer) const
			{
				return serializer.write(value1)
					&& serializer.write(value2)
					&& serializer.write(value3)
					&& serializer.write(vector)
					&& serializer.write(string)
					&& serializer.write(vecStr);
			}
			bool deserialize(Serialization::Deserializeration& deserializer)
			{
				return deserializer.read(value1)
					&& deserializer.read(value2)
					&& deserializer.read(value3)
					&& deserializer.read(vector)
					&& deserializer.read(string)
					&& deserializer.read(vecStr);
			}
			bool operator==(const MyFirstMessage& other) const
			{
				bool ok1 = value1 == other.value1;
				bool ok2 = value2 == other.value2;
				bool ok3 = value3 == other.value3;
				bool ok4 = vector == other.vector;
				bool ok5 = string == other.string;
				bool ok6 = vecStr == other.vecStr;

				return ok1 && ok2 && ok3 && ok4 && ok5 && ok6;
			}
		};

		const MyFirstMessage origin_msg = { -37, 123456, 13.589f, {42, 349, 2895, 16578}, "toto va se baigner", {"toto", "mange", "un", "bonbon"} };

		Serialization::Serializer serializer;
		CHECK(origin_msg.serialize(serializer));

		MyFirstMessage msg;
		Serialization::Deserializeration deserializer(serializer.buffer(), serializer.bufferSize());
		CHECK(msg.deserialize(deserializer));

		CHECK(origin_msg == msg);
	}
}