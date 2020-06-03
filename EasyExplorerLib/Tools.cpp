#include "pch.h"
#include "Tools.h"
namespace EasyExplorerLib {
	


	bool NativeProcessSet::Refresh()
	{
		m_Buffer.reset();
		auto status = GetSystemInformation(m_Buffer,DllFunction::m_NtQuerySystemInformation,SystemProcessInformation);
		if (m_Buffer == nullptr || (!NT_SUCCESS(status)))
			return false;
		auto buffer = m_Buffer.get();
		m_Processes.clear();

		PSYSTEM_PROCESS_INFORMATION current=nullptr;
		do
		{
			current = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(buffer);
			m_Processes.push_back(std::make_unique<NativeProcess>(current));
		} while ((current->NextEntryOffset) && (buffer += current->NextEntryOffset, true));

		return true;
	}


	void NativeProcessSet::RefreshOwnedHandles()
	{
		std::unique_ptr<std::byte[]> tempHandlesBuffer;
		//Get SYSTEM_HANDLE_INFORMATION_EX all handles in system
		auto status = GetSystemInformation(tempHandlesBuffer,DllFunction::m_NtQuerySystemInformation,SystemExtendedHandleInformation);

		if (!NT_SUCCESS(status))
		{
			throw EESYSTEM_ERROR_CODE("Get all handles information ex error");
		}
	

		m_BufferOwnedHandles= std::move(tempHandlesBuffer);
		
		auto handlesInformation = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION_EX>(m_BufferOwnedHandles.get());
		m_OwnedHandles.clear();
		for (int i = 0; i < handlesInformation->NumberOfHandles; ++i)
		{
			m_OwnedHandles.push_back(handlesInformation->Handles + i);
		}

		
	}

	////NativeProcess
	//bool NativeProcess::RefreshOwnedHandles()
	//{
	//	if (m_ProcessHandle == nullptr)
	//	{
	//		NativeProcess::GetMemberProcessHandle();
	//	}
	//	auto status = GetSystemInformation(
	//		m_BufferOwnedHandles,
	//		DllFunction::m_NtQueryInformationProcess,
	//		m_ProcessHandle,
	//		ProcessHandleInformation
	//	);

	//	auto handlesInfo=reinterpret_cast<PPROCESS_HANDLE_SNAPSHOT_INFORMATION>(m_BufferOwnedHandles.get());
	//	
	//	auto count = handlesInfo->NumberOfHandles;
	//	m_OwnedHandles.clear();
	//	for (int i = 0; i < count; ++i)
	//	{
	//		m_OwnedHandles.push_back(handlesInfo->Handles + i);
	//	}

	//	return false;
	//}


	//TODO implement GetHandleInformation in class NativeProcess
	bool NativeProcess::GetHandleInformation(HANDLE handle)
	{
		return false;
	}


	void SetPrivilege(HANDLE hToken, std::wstring privilegeName, bool enablePrivilege)
	{
		LUID luid;
		TOKEN_PRIVILEGES privileges;


		if (!LookupPrivilegeValueW(
			nullptr,
			privilegeName.c_str(),
			&luid
		))
		{
			throw EESYSTEM_ERROR_CODE("LookupPrivilege Value error");
		}
		privileges.PrivilegeCount = 1;
		privileges.Privileges[0].Luid = luid;

		//set 0 when false is because 0 is default number
		//Get the number from microsoft case 
		privileges.Privileges[0].Attributes =
			enablePrivilege ?
			SE_PRIVILEGE_ENABLED :
			0;


		//require TOKEN_ADJUST_PRIVILEGES
		auto result = AdjustTokenPrivileges(
			hToken,
			false,
			&privileges,
			0,
			nullptr,
			nullptr
		);
		if (result)
		{
			auto errorCode = GetLastError();
			if (errorCode == ERROR_NOT_ALL_ASSIGNED)
			{
				throw EESYSTEM_ERROR_CODE("don't have the privilege");
			}
		}
		else
		{
			auto errorCode = GetLastError();
			throw EESYSTEM_ERROR_CODE("adjust Token Privileges error");
		}
		
	}

	void SetDebugPrivilege()
	{
		HANDLE processHandle = GetCurrentProcess();
		
		HANDLE tokenHandle=nullptr;
		auto result = OpenProcessToken(processHandle, TOKEN_ADJUST_PRIVILEGES, &tokenHandle);
		if (!result)
			throw EESYSTEM_ERROR_CODE("openProcessToken error");
		HandleUniquePtr tokenUPtrHandle(tokenHandle);

#ifdef DEBUG
		auto debugHandle = tokenUPtrHandle.get();
#endif // DEBUG

		
		SetPrivilege(tokenHandle, SE_DEBUG_NAME, true);

	}
		
	
	void DllFunction::init()
	{

		//load ntdll and NtQuerySystemInformation address
		HINSTANCE hin = LoadLibraryW(L"Ntdll.dll");
		if (hin == nullptr)
			throw EESYSTEM_ERROR_CODE("loadNt error");
		m_NtDll.reset(hin);
		m_NtQuerySystemInformation =
			GetFunctionAddress<NtQuerySystemInformation>(m_NtDll.get(), "NtQuerySystemInformation");
		if (m_NtQuerySystemInformation == nullptr)
			throw EESYSTEM_ERROR_CODE("Get NtQuerySystemInformation address error");

		m_NtQueryInformationProcess =
			GetFunctionAddress<EENtQueryInformationProcess>(m_NtDll.get(), "NtQueryInformationProcess");

		if (m_NtQueryInformationProcess == nullptr)
			throw EESYSTEM_ERROR_CODE("Get NtQueryInformationProcess address error");
	}

}