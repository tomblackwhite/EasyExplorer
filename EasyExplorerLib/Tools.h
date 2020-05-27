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
	
	//alias Function name
	
	using EENtQuerySystemInformation = NtQuerySystemInformation;
	using EENtQueryInformationProcess = NtQueryInformationProcess;

	template<typename Fun>
	auto GetFunctionAddress(HINSTANCE hin,std::string str)->Fun*
	{
		return reinterpret_cast<Fun*>(GetProcAddress(hin, str.c_str()));
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
			nullptr);

		return status;
	}

	//process 
	class NativeProcess
	{
	public:
		NativeProcess(PSYSTEM_PROCESS_INFORMATION info):m_Information(info)
		{}

		bool RefreshOwnedHandles();

		PSYSTEM_PROCESS_INFORMATION m_Information=nullptr;

	private:
		std::unique_ptr<PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX> m_OwnedHandles=nullptr;

		std::unique_ptr<std::byte[]> m_BufferOwnedHandles = nullptr;
		HandleUniquePtr m_ProcessHandle = nullptr;
	};

	class NativeProcessSet
	{
	public:
		NativeProcessSet() 
		{
			//load ntdll and NtQuerySystemInformation address
			HINSTANCE hin = LoadLibraryW(L"Ntdll.dll");
			if (hin == nullptr)
				throw EESYSTEM_ERROR_CODE("loadNt error");
			m_NtDll.reset(hin);
			m_NtQuerySystemInformation = 
				GetFunctionAddress<NtQuerySystemInformation>(m_NtDll.get(),"NtQuerySystemInformation");
			if (m_NtQuerySystemInformation == nullptr)
				throw EESYSTEM_ERROR_CODE("Get NtQuerySystemInformation address error");
			m_NtQueryInformationProcess =
				GetFunctionAddress<EENtQueryInformationProcess>(m_NtDll.get(), "NtQueryInformationProcess");

			if (m_NtQueryInformationProcess == nullptr)
				throw EESYSTEM_ERROR_CODE("Get NtQueryInformationProcess address error");



		}

		//Get New processes and refresh m_Processes;
		bool Refresh();

		//Get process owned handles or refresh
		bool RefreshOwnedHandles();
		

		const std::vector<std::unique_ptr<NativeProcess>>&
			GetProcesses()const
		{
			return m_Processes;
		}
	private:
		std::unique_ptr<std::byte[]> m_Buffer;
		//pSystemProcess information array in m_Buffer
		std::vector<std::unique_ptr<NativeProcess>> m_Processes;
		//owned handle in process
		std::unique_ptr<std::byte[]> m_OwnedHandles;

		//DLL
		DllUniquePtr m_NtDll;
		EENtQuerySystemInformation* m_NtQuerySystemInformation=nullptr;
		EENtQueryInformationProcess* m_NtQueryInformationProcess = nullptr;
	};

}