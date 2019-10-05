.CODE

VBoxExchangeData PROC
			xchg rax, rcx
			mov edx, 5659h
			out dx, rax
			ret
VBoxExchangeData ENDP

END 