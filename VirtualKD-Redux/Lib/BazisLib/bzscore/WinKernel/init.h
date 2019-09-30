#pragma once

NTSTATUS _stdcall CPPDriverEntry(
	IN OUT PDRIVER_OBJECT   DriverObject,
	IN PUNICODE_STRING      RegistryPath
	);
