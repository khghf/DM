#pragma once
#include <imgui.h>
#include <Core/Reflection/Mirror/include/mirror.h>

namespace mirror::Serialization
{
	// ============ Json 特化声明 ============
	template <>
	struct JsonSerializer<ImVec2>
	{
		static void Serialize(rapidjson::Value* jsonVal, const ImVec2* inVal, RapidJsonAllocator* allocator);
		static void Deserialize(rapidjson::Value* jsonVal, ImVec2* outVal);
	};
	template <>
	struct JsonSerializer<ImVec4>
	{
		static void Serialize(rapidjson::Value* jsonVal, const ImVec4* inVal, RapidJsonAllocator* allocator);
		static void Deserialize(rapidjson::Value* jsonVal, ImVec4* outVal);
	};
	template <>
	struct JsonSerializer<ImColor>
	{
		static void Serialize(rapidjson::Value* jsonVal, const ImColor* inVal, RapidJsonAllocator* allocator);
		static void Deserialize(rapidjson::Value* jsonVal, ImColor* outVal);
	};

	// ============ Binary 特化声明 ============
	template <>
	struct BinarySerializer<ImVec2>
	{
		static void Serialize(std::ostream* stream, const ImVec2* inVal);
		static void Deserialize(std::istream* stream, ImVec2* outVal);
	};
	template <>
	struct BinarySerializer<ImVec4>
	{
		static void Serialize(std::ostream* stream, const ImVec4* inVal);
		static void Deserialize(std::istream* stream, ImVec4* outVal);
	};
	template <>
	struct BinarySerializer<ImColor>
	{
		static void Serialize(std::ostream* stream, const ImColor* inVal);
		static void Deserialize(std::istream* stream, ImColor* outVal);
	};

	// ============ Json 特化实现 ============
	inline void JsonSerializer<ImVec2>::Serialize(rapidjson::Value* jsonVal, const ImVec2* inVal, RapidJsonAllocator* allocator)
	{
		jsonVal->SetArray();
		rapidjson::Value x{ rapidjson::kObjectType };
		rapidjson::Value y{ rapidjson::kObjectType };
		SerializeJson(&x, &inVal->x, allocator);
		SerializeJson(&y, &inVal->y, allocator);
		jsonVal->PushBack(x, *allocator);
		jsonVal->PushBack(y, *allocator);
	}

	inline void JsonSerializer<ImVec2>::Deserialize(rapidjson::Value* jsonVal, ImVec2* outVal)
	{
		if (!jsonVal->IsArray()) return;
		const auto& array = jsonVal->GetArray();
		if (array.Size() > 0) DeserializeJson(&array[0], &outVal->x);
		if (array.Size() > 1) DeserializeJson(&array[1], &outVal->y);
	}

	inline void JsonSerializer<ImVec4>::Serialize(rapidjson::Value* jsonVal, const ImVec4* inVal, RapidJsonAllocator* allocator)
	{
		jsonVal->SetArray();
		rapidjson::Value x{ rapidjson::kObjectType };
		rapidjson::Value y{ rapidjson::kObjectType };
		rapidjson::Value z{ rapidjson::kObjectType };
		rapidjson::Value w{ rapidjson::kObjectType };
		SerializeJson(&x, &inVal->x, allocator);
		SerializeJson(&y, &inVal->y, allocator);
		SerializeJson(&z, &inVal->z, allocator);
		SerializeJson(&w, &inVal->w, allocator);
		jsonVal->PushBack(x, *allocator);
		jsonVal->PushBack(y, *allocator);
		jsonVal->PushBack(z, *allocator);
		jsonVal->PushBack(w, *allocator);
	}

	inline void JsonSerializer<ImVec4>::Deserialize(rapidjson::Value* jsonVal, ImVec4* outVal)
	{
		if (!jsonVal->IsArray()) return;
		const auto& array = jsonVal->GetArray();
		if (array.Size() > 0) DeserializeJson(&array[0], &outVal->x);
		if (array.Size() > 1) DeserializeJson(&array[1], &outVal->y);
		if (array.Size() > 2) DeserializeJson(&array[2], &outVal->z);
		if (array.Size() > 3) DeserializeJson(&array[3], &outVal->w);
	}

	inline void JsonSerializer<ImColor>::Serialize(rapidjson::Value* jsonVal, const ImColor* inVal, RapidJsonAllocator* allocator)
	{
		JsonSerializer<ImVec4>::Serialize(jsonVal, &inVal->Value, allocator);
	}

	inline void JsonSerializer<ImColor>::Deserialize(rapidjson::Value* jsonVal, ImColor* outVal)
	{
		JsonSerializer<ImVec4>::Deserialize(jsonVal, &outVal->Value);
	}

	// ============ Binary 特化实现 ============
	inline void BinarySerializer<ImVec2>::Serialize(std::ostream* stream, const ImVec2* inVal)
	{
		WriteStream(stream, &inVal->x);
		WriteStream(stream, &inVal->y);
	}

	inline void BinarySerializer<ImVec2>::Deserialize(std::istream* stream, ImVec2* outVal)
	{
		outVal->x = ReadStream<decltype(outVal->x)>(stream);
		outVal->y = ReadStream<decltype(outVal->y)>(stream);
	}

	inline void BinarySerializer<ImVec4>::Serialize(std::ostream* stream, const ImVec4* inVal)
	{
		WriteStream(stream, &inVal->x);
		WriteStream(stream, &inVal->y);
		WriteStream(stream, &inVal->z);
		WriteStream(stream, &inVal->w);
	}

	inline void BinarySerializer<ImVec4>::Deserialize(std::istream* stream, ImVec4* outVal)
	{
		outVal->x = ReadStream<decltype(outVal->x)>(stream);
		outVal->y = ReadStream<decltype(outVal->y)>(stream);
		outVal->z = ReadStream<decltype(outVal->z)>(stream);
		outVal->w = ReadStream<decltype(outVal->w)>(stream);
	}

	inline void BinarySerializer<ImColor>::Serialize(std::ostream* stream, const ImColor* inVal)
	{
		BinarySerializer<ImVec4>::Serialize(stream, &inVal->Value);
	}

	inline void BinarySerializer<ImColor>::Deserialize(std::istream* stream, ImColor* outVal)
	{
		BinarySerializer<ImVec4>::Deserialize(stream, &outVal->Value);
	}
}
