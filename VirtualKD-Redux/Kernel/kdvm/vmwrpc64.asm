.CODE

extrn HalInitSystem:PROC

DllEntryPoint proc
	jmp HalInitSystem
DllEntryPoint endp

VMWareRPCData struc
	m_Cookie1 dd 0;
	m_Cookie2 dd 0;
	m_ChannelNumber dd 0;
	
	m_ReplyId dd 0;
	m_CachedReplySize dd 0;
VMWareRPCData ends 

VMWareOpenChannel PROC
			push rbx
			push rsi
			push rdi
			mov eax, 564D5868h
			mov ebx, 0C9435052h
			mov r8,rcx
			mov ecx, 0000001Eh
			mov edx, 5658h
			out dx, eax
			test ecx, 00010000h
			jz vmware_rpc_open_channel_error
			mov [r8+VMWareRPCData.m_Cookie1], esi
			mov [r8+VMWareRPCData.m_Cookie2], edi
			mov [r8+VMWareRPCData.m_ChannelNumber], edx
			pop rdi
			pop rsi
			pop rbx
			xor rax, rax
			inc rax
			ret
vmware_rpc_open_channel_error:
			mov edx, 5658h
			in eax, dx
			cmp eax, 'XOBV'	;'VBOX' signature is returned, if this call was made under VirtualBox
			jz vmware_rpc_open_channel_vbox
			pop rdi
			pop rsi
			pop rbx
			xor rax, rax
			ret
vmware_rpc_open_channel_vbox:
			pop rdi
			pop rsi
			pop rbx
			xor rax, rax
			add rax, 2
			ret
VMWareOpenChannel ENDP

VMWareCloseChannel PROC
			push rsi
			push rdi
			mov eax, 564D5868h
			mov edx, [rcx+VMWareRPCData.m_ChannelNumber]
			or edx, 5658h
			mov esi, [rcx+VMWareRPCData.m_Cookie1]
			mov edi, [rcx+VMWareRPCData.m_Cookie2]
			mov ecx, 0006001Eh
			out dx, eax
			cmp ecx, 00010000h
			pop rdi
			pop rsi
			setz al
			and rax,1
			ret
VMWareCloseChannel ENDP

VMWareSendRequestLength PROC
			push rbx
			push rsi
			push rdi
			mov eax, 564D5868h
			mov ebx, edx
			mov edx, [rcx+VMWareRPCData.m_ChannelNumber]
			or edx, 5658h
			mov esi, [rcx+VMWareRPCData.m_Cookie1]
			mov edi, [rcx+VMWareRPCData.m_Cookie2]
			mov ecx, 0001001Eh
			out dx, eax
			cmp ecx, 00810000h
			jnz vmware_rpc_send_size_error

			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			inc rax
			ret

vmware_rpc_send_size_error:
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			ret
VMWareSendRequestLength ENDP

VMWareSendPartialData PROC
			push rbx
			push rsi
			push rdi
			push rbp
			mov eax, 564D5868h
			;ecx <- byte count to send (minus null)
			xchg rcx, rdx
			mov ebx, 00010000h
			mov rsi, r8	;pData
			mov edi, [rdx+VMWareRPCData.m_Cookie2]
			mov ebp, [rdx+VMWareRPCData.m_Cookie1]
			mov edx, [rdx+VMWareRPCData.m_ChannelNumber]
			or edx, 5659h
			rep outsb

			cmp ebx, 00010000h
			jnz vmware_rpc_send_partial_error

			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			inc rax
			ret

vmware_rpc_send_partial_error:
			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			ret
VMWareSendPartialData ENDP

VMWareSendData PROC
			push rbx
			push rsi
			push rdi
			push rbp
			mov eax, 564D5868h
			mov ebx, edx
			mov edx, [rcx+VMWareRPCData.m_ChannelNumber]
			or edx, 5658h
			mov esi, [rcx+VMWareRPCData.m_Cookie1]
			mov edi, [rcx+VMWareRPCData.m_Cookie2]
			; r9 <- class pointer
			mov r9, rcx
			mov ecx, 0001001Eh
			out dx, eax
			cmp ecx, 00810000h
			jnz vmware_rpc_send_data_error

			;ecx <- byte count to send (minus null)
			xchg ecx, ebx
			mov ebx, 00010000h
			mov rsi, r8	;pData
			mov edx, [r9+VMWareRPCData.m_ChannelNumber]
			or edx, 5659h
			mov edi, [r9+VMWareRPCData.m_Cookie2]
			mov ebp, [r9+VMWareRPCData.m_Cookie1]
			rep outsb

			cmp ebx, 00010000h
			jnz vmware_rpc_send_data_error

			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			inc rax
			ret

vmware_rpc_send_data_error:
			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			ret
VMWareSendData ENDP

VMWareGetReplySize PROC
			push rbx
			push rsi
			push rdi
			mov eax, 564D5868h
			mov edx, [rcx+VMWareRPCData.m_ChannelNumber]
			or edx, 5658h
			mov esi, [rcx+VMWareRPCData.m_Cookie1]
			mov edi, [rcx+VMWareRPCData.m_Cookie2]
			mov r8, rcx
			mov ecx, 0003001Eh
			out dx, eax
			mov [r8+VMWareRPCData.m_ReplyId], edx
			cmp ecx, 00830000h
			jz return_from_get_reply_size
			or rbx, -1
return_from_get_reply_size:			
			xchg eax,ebx
			pop rdi
			pop rsi
			pop rbx
			ret
VMWareGetReplySize ENDP

VMWareRecvData PROC
			push rbx
			push rsi
			push rdi
			push rbp
			mov eax, 564D5868h
			mov ebx, 00010000h
			xchg rcx, rdx
			mov rdi, r8	;pData
			mov esi, [rdx+VMWareRPCData.m_Cookie1]
			mov ebp, [rdx+VMWareRPCData.m_Cookie2]
			mov edx, [rdx+VMWareRPCData.m_ChannelNumber]
			or edx, 5659h
			rep insb

			cmp ebx, 00010000h
			jnz vmware_rpc_recv_data_error

			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			inc rax
			ret

vmware_rpc_recv_data_error:
			pop rbp
			pop rdi
			pop rsi
			pop rbx
			xor rax,rax
			ret
VMWareRecvData ENDP

VMWareEndReceive PROC
			push rbx
			push rsi
			push rdi
			mov eax, 564D5868h
			mov ebx, [rcx+VMWareRPCData.m_ReplyId]
			mov edx, [rcx+VMWareRPCData.m_ChannelNumber]
			or edx, 5658h
			mov esi, [rcx+VMWareRPCData.m_Cookie1]
			mov edi, [rcx+VMWareRPCData.m_Cookie2]
			mov ecx, 0005001Eh
			out dx, eax
			cmp ecx, 00010000h
			pop rdi
			pop rsi
			pop rbx
			setz al
			and eax,1
			ret
VMWareEndReceive ENDP
 
END 