#pragma once
#include"pch.h"
namespace EasyExplorerLib {
#pragma unmanaged

	//Windows error code exception
#define EESYSTEM_ERROR_CODE(str)\
	std::system_error(GetLastError(),std::system_category(),str)

	
	
	//freelibary
	struct FreeLibDeleter
	{
		using pointer = HINSTANCE;
		bool operator()(HINSTANCE h)
		{
			return FreeLibrary(h);
		}
	};
	using DllUniquePtr = std::unique_ptr<HINSTANCE, FreeLibDeleter>;

	//Close handle
	struct HandleDeleter
	{
		using pointer = HANDLE;
		bool operator()(HANDLE h)
		{
			return CloseHandle(h);
		}
	};
	using HandleUniquePtr = std::unique_ptr<HANDLE, HandleDeleter>;


	//Macro Fun Name
#define EEFUN_NAME_TYPE(fun)\
	&fun,#fun
	
	
	
	
	template<typename Fun>
	auto GetFunctionAddress(HINSTANCE hin,std::string str)->Fun*
	{
		return reinterpret_cast<Fun*>(GetProcAddress(hin, str.c_str()));
	};
	

	//DLL and Function address
	struct DllFunction
	{
		//alias Function name
		using EENtQuerySystemInformation = NtQuerySystemInformation;
		using EENtQueryInformationProcess = NtQueryInformationProcess;
		static void init();
		//DLL
		inline static DllUniquePtr m_NtDll;
		inline static EENtQuerySystemInformation* m_NtQuerySystemInformation;
		inline static EENtQueryInformationProcess* m_NtQueryInformationProcess;
	};

	
	/*template GetAllInformation*/
	
	template<typename Fun,typename ...Args>
	NTSTATUS GetSystemInformation( 
		std::unique_ptr<std::byte[]> &buffer,
		Fun&& fun,
		Args&& ...args
		) noexcept
	{
		ULONG bufferSize = 0;
		NTSTATUS status =
			std::forward<Fun>(fun)(
				std::forward<Args>(args)...,
				buffer.get(),
				0,
				&bufferSize);

		//get length information
		

		if (!NT_SUCCESS(status) && status != STATUS_BUFFER_TOO_SMALL && status != STATUS_INFO_LENGTH_MISMATCH)
			return status;

		buffer.reset(new(std::nothrow) std::byte[bufferSize]);
		if (buffer == nullptr)
			return status;

		status = std::forward<Fun>(fun)(
			std::forward<Args>(args)...,
			buffer.get(),
			bufferSize,
			&bufferSize);

		return status;
	}

	/*
	set Privilege in acess token 
	require TOKEN_ADJUST_PRIVILEGES*/
	void SetPrivilege(
		HANDLE hToken,
		std::wstring privilegeName,
		bool enablePrivilege
	);


	//for EasyLibaryLib 
	//Get Debug Privilege
	void SetDebugPrivilege();

	

	//process 
	class NativeProcess
	{
	public:
		NativeProcess(PSYSTEM_PROCESS_INFORMATION info):m_Information(info)
		{		
		}

		//bool RefreshOwnedHandles();
		bool GetHandleInformation(HANDLE handle);
		PSYSTEM_PROCESS_INFORMATION m_Information=nullptr;
		

	private:
		void GetMemberProcessHandle()
		{
			auto tempHandle = OpenProcess(PROCESS_ALL_ACCESS, false,
				reinterpret_cast<DWORD>(m_Information->UniqueProcessId));
			if (tempHandle == nullptr)
				throw EESYSTEM_ERROR_CODE("Get process handle error");
			m_ProcessHandle.reset(tempHandle);
		}
		
		
		HandleUniquePtr m_ProcessHandle = nullptr;

	};

	class NativeProcessSet
	{
	public:
		NativeProcessSet() 
		{
		}

		//Get New processes and refresh m_Processes;
		bool Refresh();

		//Get process owned handles or refresh
		//all handles in system 
		void RefreshOwnedHandles();
		

		const std::vector<std::unique_ptr<NativeProcess>>&
			GetProcesses()const
		{
			return m_Processes;
		}
	private:
		std::unique_ptr<std::byte[]> m_Buffer;
		//pSystemProcess information array in m_Buffer
		std::vector<std::unique_ptr<NativeProcess>> m_Processes;
		

		//handles in system
		std::vector<PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX> m_OwnedHandles;
		std::unique_ptr<std::byte[]> m_BufferOwnedHandles = nullptr;
	};

}