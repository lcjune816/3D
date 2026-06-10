#ifndef Engine_Function_h__
#define Engine_Function_h__

#include "Engine_Struct.h"
#include "Engine_Typedef.h"
namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}
	template<typename T>
	void Save_Binary_Data_SizeT(T& Data, const string path)
	{
		ofstream Write(path, std::ios::binary | std::ios::app);
		Write.write((char*)(&Data), sizeof(Data));
		Write.close();

	}
	template<typename T>
	void	Save_Binary_Data_String(T& Data, const string path)
	{
		ofstream Write(path, std::ios::binary | std::ios::app);

		string strName = Data;
		size_t j = strName.length();
		Write.write((char*)(&j), sizeof(size_t));
		Write.write((char*)(strName.data()), sizeof(char) * j);
		Write.close();
	}


	template<typename T>
	void	Save_Binary_Data_NoArray(T& Data,const string path)
	{
		ofstream file(path, std::ios::binary | std::ios::app);
		if (!file.is_open())
		{
			MSG_BOX(" 크아악");
			return;
		}
		Data.Save_Data(file);

		file.close();
	}
	template<typename T>
	void	Save_Binary_Data_Array(std::vector<T>& Data, ofstream& fWrite)
	{
		size_t iArraySize = Data.size();
		fWrite.write((char*)(&iArraySize), sizeof(size_t));

		fWrite.write((char*)(Data.data()), sizeof(T) * iArraySize);
	}


	template<typename T>
	void	Save_Binary_Data_Map(T& Data, const string path)
	{
		ofstream file(path, std::ios::binary | std::ios::app);
		if (!file.is_open())
		{
			MSG_BOX("맵 크아악");
			return ;
		}

		size_t iArray = Data.size();
		file.write((char*)(&iArray), sizeof(size_t));
		
		for (auto iter = Data.begin(); iter != Data.end(); ++iter)
		{
			string name = iter->first;
			size_t iNameSize = name.length();
			file.write((char*)(&iNameSize), sizeof(size_t));

			file.write((char*)(name.data()), sizeof(char) * iNameSize);
			iter->second.Save_Data(file);

		}
		file.close();
	}



	template<typename T1>
	void	Load_Binary_Data_Array(std::vector<T1>& Array,ifstream& fRead)
	{
		size_t iArraySize = 0;
		fRead.read((char*)(&iArraySize), sizeof(size_t));
		Array.resize(iArraySize);
		fRead.read((char*)(Array.data()), sizeof(T1) * iArraySize);
	}



	class JsonSaveLoadManager
	{
	public:
		static void  SaveJsonTypeFloat4(json& jsonFile, const string& TypeName, _float4& Float4)
		{
			jsonFile[TypeName] = { Float4.x,Float4.y,Float4.z,Float4.w };
		}

		static void  SaveJsonTypeFloat3(json& jsonFile, const string& TypeName, _float3& Float3)
		{
			jsonFile[TypeName] = { Float3.x,Float3.y,Float3.z };
		}
		static void  SaveJsonTypeFloat2(json& jsonFile, const string& TypeName, _float2& Float2)
		{
			jsonFile[TypeName] = { Float2.x,Float2.y};
		}
		static void  LoadJsonTypeFloat4(json& jsonFile, const string& TypeName, _float4& Float4)
		{
			if (jsonFile.contains(TypeName))
				Float4 = _float4(jsonFile[TypeName][0], jsonFile[TypeName][1], jsonFile[TypeName][2], jsonFile[TypeName][3]);
		}

		static void  LoadJsonTypeFloat3(json& jsonFile, const string& TypeName, _float3& Float3)
		{
			if (jsonFile.contains(TypeName))
			Float3 = _float3(jsonFile[TypeName][0], jsonFile[TypeName][1], jsonFile[TypeName][2]);
		}
		static void  LoadJsonTypeFloat(json& jsonFile, const string& TypeName, _float& Float)
		{
			if (jsonFile.contains(TypeName))
				Float = jsonFile[TypeName];
		}
		static void  LoadJsonTypeUINT(json& jsonFile, const string& TypeName, uint32_t & iUlnt)
		{
			if (jsonFile.contains(TypeName))
				iUlnt = jsonFile[TypeName];
		}
		static void  LoadJsonTypeFloat2(json& jsonFile, const string& TypeName, _float2& Float2)
		{
			if (jsonFile.contains(TypeName))
				Float2 = _float2(jsonFile[TypeName][0], jsonFile[TypeName][1]);
		}
	};
	
	template<typename ENUM>
	void		SavejsonEnum(json& jsonFile, const string& TypeName, ENUM& eNum)
	{
		uint32_t iType = static_cast<uint32_t>(eNum);
		jsonFile[TypeName] = iType;
	}
	template<typename ENUM>
	void		LoadjsonEnum(json& jsonFile, const string& TypeName, ENUM& eNum)
	{
		if (jsonFile.contains(TypeName))
		{
			uint32_t iType = jsonFile[TypeName];
			eNum = static_cast<ENUM>(iType);
		}
	}


	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int	iRefCnt = 0;

		if (nullptr != pInstance)
			iRefCnt = pInstance->AddRef();
		return iRefCnt;
	}


	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int	iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if (0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}




}

#endif // Engine_Function_h__
